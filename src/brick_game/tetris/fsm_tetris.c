#include "fsm.h"

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
  double timer = game->speed;
  struct timeval after;

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
          break;
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
          else {
            figure_down(game, figures);
          }
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
 * @brief Преобразуем пользовательский ввод в структуру
 *
 * @param user_input Пользовательский ввод
 * @param hold Зажатие клавиши (для будующих проектов)
 * 
 * @return Преобразованный ввод
 */
UserAction_t userInput(int user_input, bool hold) {
  UserAction_t action = No_enter;

  if (hold) {
    if (user_input == KEY_UP)
      action = Up;
    else if (user_input == KEY_DOWN)
      action = Down;
    else if (user_input == KEY_LEFT)
      action = Left;
    else if (user_input == KEY_RIGHT)
      action = Right;
    else if (user_input == ' ')
      action = Pause;
    else if (user_input == 10 || user_input == 13)
      action = Start;
    else if (user_input == 'q' || user_input == 'Q')
      action = Exit;
  }

  return action;
}