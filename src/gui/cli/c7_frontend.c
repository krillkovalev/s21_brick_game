#include "../../brick_game/tetris/c7_tetris.h"

/**
 * @brief Запускаем игру ТЕТРИС в консоли
 */

int main() {
  // Массив эскизов фигур - 7 шт.
  const sketch_figure figures[7] = FIGURES_INIT;

  init_ncurses();  // Инициализация ncurses и цвета
  game_process(figures);
  endwin();  // Завершение работы с ncurses

  return 0;
}

/**
 * @brief Конечные автоматы, реализованные через SWITCH
 *
 * @param state Состояние игры
 * @param key_now Нажатая клаивиша
 * @param game Описание игры в текущий момент
 * @param figures массив шаблонов фигур
 * @param befor Старт времени для свободного падения фигуры
 * @param file Файл, в котором хранится рекордный результат
 */
void updateCurrentState(state_of_game *state, UserAction_t key_now,
                        GameInfo_t *game, const sketch_figure figures[],
                        struct timeval *before, FILE *file) {
  if (key_now == Exit) {
    return;
  }

  switch (*state) {
    // Состояние старта
    case START:
      while (getch() != 10) {
        draw_next_field(figures[game->number_next_figure],
                        game->number_next_figure);
        draw_field_score(game->high_score, game->score);
        draw_field_level(game->level);
        draw_service_field("   Press ENTER\n     for start", 18, 4);
      }
      *state = GAME;
      game->number_next_figure = get_random_shape();
      break;

    // Состояние игры
    case GAME:
      moving(state, key_now, game, figures, before);
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

/**
 * @brief Реализация движения фигуры на поле
 *
 * @param state Состояние игры
 * @param key_now Нажатая клаивиша
 * @param game Описание игры в текущий момент
 * @param figures массив шаблонов фигур
 * @param befor Старт времени для свободного падения фигуры
 */
void moving(state_of_game *state, UserAction_t key_now, GameInfo_t *game,
            const sketch_figure figures[], struct timeval *before) {
  double timer = game->speed;
  struct timeval after;
  gettimeofday(&after, NULL);
  if (((double)after.tv_sec * 1000000 + (double)after.tv_usec) -
          ((double)before->tv_sec * 1000000 + (double)before->tv_usec) >
      timer) {
    *before = after;
    if (check_down_position(game->figure, game->field) == 0)
      game->figure.current_y++;
    else {
      figure_down(game, figures);
    }
    if (check_сreate_position(game->field) == 1) {
      *state = GAME_OVER;
    }
  }
  switch (key_now) {
    case Up:
      if (check_rotate_position(game->figure, game->field) == 0)
        game->figure = rotate_figure(game->figure);
      break;
    case Right:
      if (check_right_position(game->figure, game->field) == 0)
        game->figure.current_x += 2;
      break;
    case Left:
      if (check_left_position(game->figure, game->field) == 0)
        game->figure.current_x -= 2;
      break;
    case Down:
      if (check_down_position(game->figure, game->field) == 0)
        game->figure.current_y++;
      else
        figure_down(game, figures);
      if (check_сreate_position(game->field) == 1) {
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
}

/**
 * @brief Основной игровой процесс до нажатия 'q' или 'Q'
 *
 * @param figures Массив шаблонов фигур
 */
void game_process(const sketch_figure figures[]) {
  struct timeval before;
  gettimeofday(&before, NULL);

  state_of_game state = START;
  UserAction_t key_now = No_enter;

  bool quit_flag = true;

  // Играем пока не нажали на q или Q
  while (quit_flag) {
    FILE *high_score_file = NULL;
    GameInfo_t game = init_game(&high_score_file, figures);
    draw_main_field(game.field);
    state = START;
    // Играем пока не дошли до game_over
    while (game.game_over) {
      if (state == START || state == GAME || state == GAME_OVER) {
        key_now = userInput(getch(), true);
      }

      if (key_now == Exit) {
        delete_game(&game, high_score_file);
        quit_flag = false;
        break;
      }
      updateCurrentState(&state, key_now, &game, figures, &before,
                         high_score_file);

      draw_all(game, state, figures[game.number_next_figure],
               game.number_next_figure);
    }
  }
}

// Функция для отрисовки стакана-поля
void draw_main_field(char **field) {
  for (int i = 0; i < FIELD_H; i++) {
    for (int j = 0; j < FIELD_W; j++) {
      if (field[i][j] != 'X') {
        attron(COLOR_PAIR(12));  // Активация цвета
        mvaddch(OFFSET_Y + i, OFFSET_X + j * 2, ' ');
        mvaddch(OFFSET_Y + i, OFFSET_X + j * 2 + 1, ' ');
        attroff(COLOR_PAIR(12));  // Деактивация цвета
      }
      if (field[i][j] == 'X') {
        attron(COLOR_PAIR(13));  // Активация цвета
        mvaddch(OFFSET_Y + i, OFFSET_X + j * 2, '[');
        mvaddch(OFFSET_Y + i, OFFSET_X + j * 2 + 1, ']');
        attroff(COLOR_PAIR(13));  // Деактивация цвета
      }
    }
  }
}

// Функция для отрисовки фигуры на поле
void draw_figure(matrix_figure shape) {
  attron(COLOR_PAIR(shape.number_figure + 1));  // Активация цвета

  for (int i = 0; i < shape.rows; i++) {
    for (int j = 0; j < shape.columns; j++) {
      if (shape.work_figure[i][j] == 'X') {
        mvaddch(OFFSET_Y + shape.current_y + i,
                OFFSET_X + shape.current_x + j * 2, ' ');
        mvaddch(OFFSET_Y + shape.current_y + i,
                OFFSET_X + shape.current_x + j * 2 + 1, ' ');
      }
    }
  }
  attroff(COLOR_PAIR(shape.number_figure + 1));  // Деактивация цвета
}

// Отрисовка поля для вывода вида следующей фигуры
void draw_next_field(sketch_figure next_sketch, int number) {
  WINDOW *next_figure_win;
  int startx = 26;
  int starty = 12;
  int width = 12;
  int height = 6;

  next_figure_win = newwin(height, width, starty, startx);
  mvwprintw(next_figure_win, 0, 4, "NEXT");
  wbkgd(next_figure_win, COLOR_PAIR(8));

  wattron(next_figure_win, COLOR_PAIR(number + 1));  // Активация цвета

  int k = 0;
  for (int i = 0; i < next_sketch.height; i++) {
    for (int j = 0; j < next_sketch.width; j++) {
      if (next_sketch.figure[k] == 'X') {
        mvwaddch(next_figure_win, i + 3, j * 2 + 3, ' ');
        mvwaddch(next_figure_win, i + 3, j * 2 + 4, ' ');
      }
      k++;
    }
  }
  wattroff(next_figure_win, COLOR_PAIR(number + 1));  // Деактивация цвета
  wrefresh(next_figure_win);
}

/**
 * @brief Отрисовка поля для выводы текущего счета и рекорда
 *
 * @param high_score Текст сообщения в окне
 * @param width Ширина окна
 * @param height Высота окна
 *
 */
// Отрисовка поля для выводы текущего счета и рекорда
void draw_field_score(int high_score, int score) {
  WINDOW *score_win;
  int startx = 26;
  int starty = 2;
  int width = 12;
  int height = 9;

  score_win = newwin(height, width, starty, startx);
  mvwprintw(score_win, 0, 1, "HIGH SCORE");
  mvwprintw(score_win, 2, 4, "%d", high_score);
  mvwprintw(score_win, 4, 3, "SCORE");
  mvwprintw(score_win, 6, 4, "%d", score);
  wbkgd(score_win, COLOR_PAIR(8));
  wrefresh(score_win);
}

/**
 * @brief Отрисовка поля для вывода номера уровня
 *
 * @param level Значение уровня
 *
 */
void draw_field_level(int level) {
  WINDOW *level_win;
  int startx = 26;
  int starty = 19;
  int width = 12;
  int height = 3;

  level_win = newwin(height, width, starty, startx);
  box(level_win, 4, 4);
  // mvwprintw(level_win, 0, 3, "LEVEL");
  mvwprintw(level_win, 1, 2, "LEVEL %d", level);
  wbkgd(level_win, COLOR_PAIR(8));
  wrefresh(level_win);
}

/**
 * @brief Отрисовка сервисного поля (старт, пауза, геймовер)
 *
 * @param text Текст сообщения в окне
 * @param width Ширина окна
 * @param height Высота окна
 *
 */
void draw_service_field(char *text, int width, int height) {
  WINDOW *start_win;

  start_win = newwin(height, width, SERVICE_START_Y, SERVICE_START_X);
  box(start_win, 0, 0);

  mvwprintw(start_win, 1, 1, "%s", text);
  wbkgd(start_win, COLOR_PAIR(9));
  wrefresh(start_win);
}

/**
 * @brief трисовка игрового поля со всеми окнами
 *
 * @param game Описание игры в текущий момент
 * @param state Состояние игры
 * @param next_sketch Следующий эскиз фигуры
 * @param fnumber Номер фигуры
 *
 */
void draw_all(GameInfo_t game, state_of_game state, sketch_figure next_sketch,
              int number) {
  draw_next_field(next_sketch, number);
  draw_field_score(game.high_score, game.score);
  draw_field_level(game.level);
  draw_main_field(game.field);

  if (state != START) {
    draw_figure(game.figure);
  }
}

/**
 * @brief Функция для инициализации ncurses и используемых цветов
 */

void init_ncurses() {
  initscr();      // Инициализация ncurses
  start_color();  // Начало работы с цветами
  curs_set(0);    // Скрытие курсора
  noecho();  // Отключение отображения введенных символов
  keypad(stdscr, TRUE);  // Включение чтения специальных клавиш
  nodelay(stdscr, true);

  // Инициализация цвета
  // Цвета фигур
  init_pair(1, COLOR_GREEN, COLOR_GREEN);
  init_pair(2, COLOR_GREEN, COLOR_CYAN);
  init_pair(3, COLOR_GREEN, COLOR_BLUE);
  init_pair(4, COLOR_GREEN, COLOR_YELLOW);
  init_pair(5, COLOR_GREEN, COLOR_BLACK);
  init_pair(6, COLOR_GREEN, COLOR_MAGENTA);
  init_pair(7, COLOR_GREEN, COLOR_RED);

  // Цвета боковых полей
  init_pair(8, COLOR_BLUE, COLOR_WHITE);

  // Цвета сервисного поля
  init_pair(9, COLOR_WHITE, COLOR_BLUE);

  // Цвета игрового поля
  init_pair(12, COLOR_WHITE, COLOR_WHITE);
  init_pair(13, COLOR_WHITE, COLOR_BLUE);
}

/**
 * @brief Проверка
 *
 * @param state Состояние игры
 * @param key_now Нажатая клаивиша
 * @param game Описание игры в текущий момент
 * @param figures Массив шаблонов фигур
 * @param befor Старт времени для свободного падения фигуры
 * @param file Файл, в котором хранится рекордный результат
 *
 * @return Возвращается созданный объект игры
 */

/*

Perhaps you used a tool such as dmalloc or valgrind to check for memory leaks.
It will normally report a lot of memory still in use. That is normal.

The ncurses configure script has an option, --disable-leaks, which you can use
to continue the analysis. It tells ncurses to free memory if possible. However,
most of the in-use memory is “permanent” (normally not freed).

Any implementation of curses must not free the memory associated with a screen,
since (even after calling endwin()), it must be available for use in the next
call to refresh(). There are also chunks of memory held for performance reasons.
That makes it hard to analyze curses applications for memory leaks. To work
around this, build a debugging version of the ncurses library which frees those
chunks which it can, and provides the exit_curses() function to free the
remainder on exit. The ncurses utility and test programs use this feature, e.g.,
via the ExitProgram() macro.

*/

// gcc -Wall -Wextra -Werror -std=c11 tetris.c fsm_functions.c -lncurses