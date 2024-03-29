#include "fsm.h"


int main()
{
    // Массив эскизов фигур - 7 шт.
    const sketch_figure figures[7] = {
        {"0XXXX0", 3, 2}, // S-образная
        {"XX00XX", 3, 2}, // Z-образная
        {"0X0XXX", 3, 2}, // T-образная
        {"00XXXX", 3, 2}, // L-образная
        {"X00XXX", 3, 2}, // зеркальная L
        {"XXXX", 2, 2}, // квадрат
        {"XXXX", 4, 1}, // прямая
    };
    
    init_ncurses(); // Инициализация ncurses и цвета
    game_process(figures);
    endwin(); // Завершение работы с ncurses

    return 0;
}


// Основной игровой процесс, который идет пока не нажали 'Q' или 'q'
void game_process(const sketch_figure figures[]) {
    
	struct timeval before;
    gettimeofday(&before, NULL);
  
    state_of_game state = START;
    UserAction_t key_now = No_enter;

    bool quit_flag = true; // Флаг выхода
    
    // Играем пока не нажали на q или Q
    while(quit_flag) {
        FILE *high_score_file = NULL;
        GameInfo_t game = init_game(&high_score_file, figures); 
        draw_main_field(game.field);
        state = START;
        // Играем пока не дошли до game_over
        while (game.game_over) {

            if(state == START || state == GAME || state == GAME_OVER) {
                key_now = userInput(getch(), true);
            }

            if (key_now == Exit) {
                delete_game(&game, high_score_file);
                quit_flag = false;
                break;
            }
            updateCurrentState(&state, key_now, &game, figures, &before, high_score_file);

            draw_all(game, state, figures[game.number_next_figure], game.number_next_figure); 
        } 
    } 
}



void updateCurrentState(state_of_game *state, UserAction_t key_now, GameInfo_t *game, const sketch_figure figures[], struct timeval *before, FILE *file) {
    double timer = game->speed;
	struct timeval after;


    if (key_now == Exit){
        return;
    }

    switch (*state)
    {
        // Состояние старта
        case START:

            while(getch() != 10){
            draw_next_field(figures[game->number_next_figure], game->number_next_figure);
            draw_field_score(game->high_score, game->score);
            draw_field_level(game->level);
            draw_service_field("   Press ENTER\n     for start", 18, 4);
            }
            *state = GAME;
            game->number_next_figure = get_random_shape(); 

        break;

        // Состояние игры
        case GAME:
            
            gettimeofday(&after, NULL);
            if (((double)after.tv_sec * 1000000 + (double)after.tv_usec)-((double)before->tv_sec * 1000000 + (double)before->tv_usec) > timer) { 
                *before = after;
                if (check_down_position(game->figure, game->field) == 0) game->figure.current_y++;
                else {
                    figure_down(game, figures);
                }
                if(check_сreate_position(game->field) == 1) {
                    *state = GAME_OVER;
                    break;
                }
            }

            switch (key_now)
            {
                case Up:
                    if(check_rotate_position(game->figure, game->field) == 0) game->figure = rotate_figure(game->figure);
                    break;
                case Right:
                    if (check_right_position(game->figure, game->field) == 0) game->figure.current_x += 2;
                    break;
                case Left:
                    if (check_left_position(game->figure, game->field) == 0) game->figure.current_x -= 2;
                    break;
                case Down:
                    if (check_down_position(game->figure, game->field) == 0) game->figure.current_y++;
                    else {
                        figure_down(game, figures);
                    }
                    if(check_сreate_position(game->field) == 1) {
                        *state = GAME_OVER;
                        break;
                    }

                    *state = GAME;
                    break;
                case Pause:
                    *state = PAUSE;
                    break;
                default:
                    break;
            } 
        break;

        // Состояние паузы
        case PAUSE:
            while (getch() != ' ') { 
                draw_service_field("     PAUSE", 18, 3);
            }
            *state = GAME;
            break;

        // Состояние Game Over
        case GAME_OVER:
            game->game_over = 0;
            while (getch() != 10) { 
                draw_service_field("   GAME OVER!", 18, 3);
            }

            delete_game(game, file);

            *state = START;

        break;
    
        default:


            break;
    }
}


/*

Perhaps you used a tool such as dmalloc or valgrind to check for memory leaks. It will normally report a lot of memory still in use. That is normal.

The ncurses configure script has an option, --disable-leaks, which you can use to continue the analysis. It tells ncurses to free memory if possible. However, most of the in-use memory is “permanent” (normally not freed).

Any implementation of curses must not free the memory associated with a screen, since (even after calling endwin()), it must be available for use in the next call to refresh(). There are also chunks of memory held for performance reasons. That makes it hard to analyze curses applications for memory leaks. To work around this, build a debugging version of the ncurses library which frees those chunks which it can, and provides the exit_curses() function to free the remainder on exit. The ncurses utility and test programs use this feature, e.g., via the ExitProgram() macro.

*/


// gcc -Wall -Wextra -Werror -std=c11 fsm_tetris.c fsm_functions.c -lncurses

