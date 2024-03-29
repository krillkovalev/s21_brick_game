#include "fsm.h"

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// -----------------------------  ЛОГИКА!!  ---------------------------------
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

// Функция для инициализации ncurses
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

// Получаем номер фигуры
int get_random_shape(void) {
  time_t now;
  struct tm *work_time;

  now = time(0);
  if ((work_time = localtime(&now)) == NULL) {
    return 1;
  }
  return work_time->tm_sec % 7;
}

// Функция, которая должна быть !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
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

// Создаем матрицу фигуры или поля
char **create_matrix(int rows, int columns) {
  char **matrix = (char **)calloc(rows, sizeof(char *));
  if (matrix == NULL) {
    return NULL;
  }
  for (int i = 0; i < rows; i++) {
    matrix[i] = (char *)calloc(columns, sizeof(char));
    if (matrix[i] == NULL) {
      while (i-- > 0) free(matrix[i]);
      free(matrix);
      return NULL;
    }
  }
  return matrix;
}

// Создаем фигуру
matrix_figure create_figure(sketch_figure sketch, int number) {
  matrix_figure figure;
  figure.work_figure = create_matrix(sketch.height, sketch.width);
  figure.rows = sketch.height;
  figure.columns = sketch.width;
  figure.current_x = START_X;  // OFFSET_X + 8; // Стартовая позиция по X
  figure.current_y = START_Y;  // OFFSET_Y; // Стартовая позиция по Y
  figure.number_figure = number;

  int k = 0;
  for (int i = 0; i < sketch.height; i++) {
    for (int j = 0; j < sketch.width; j++) {
      figure.work_figure[i][j] = sketch.figure[k];
      k++;
    }
  }
  return figure;  // Возвращаем созданную фигуру
}

// Инициализируем игру
GameInfo_t init_game(FILE **file, const sketch_figure figures[]) {
  GameInfo_t new_game = {0};
  new_game.field = create_matrix(FIELD_H, FIELD_W);
  new_game.score = 0;

  *file = fopen("record.txt", "r+");  // Открываем файл для чтения и записи
  if (*file == NULL) {  // Если файл не существует, создаем его
    *file = fopen("record.txt", "w+");
    if (*file == NULL) {
      exit(EXIT_FAILURE);
    }
  }

  if (fscanf(*file, "%d", &new_game.high_score) !=
      1) {  // Считываем число из файла
    new_game.high_score = 0;
  }
  new_game.number_next_figure = get_random_shape();
  new_game.level = 0;
  new_game.speed = 600000;  // В версии start - это timer
  new_game.game_over = 1;  // В версии start не используется

  // Инициализируем фигуру
  new_game.figure = create_figure(figures[new_game.number_next_figure],
                                  new_game.number_next_figure);

  return new_game;
}

void delete_game(GameInfo_t *game, FILE *file) {
  free_matrix(game->field, FIELD_H);

  free_matrix(game->figure.work_figure, game->figure.rows);
  game->figure.rows = 0;
  game->figure.columns = 0;
  game->figure.current_x = 0;
  game->figure.current_y = 0;
  game->figure.number_figure = 0;

  rewind(file);  // Перемещаем указатель файла в начало
  fprintf(file, "%d", game->high_score);
  fclose(file);

  game->number_next_figure = 0;
  game->score = 0;
  game->high_score = 0;  // !!!!!!!!!!!!!
  game->level = 0;
  game->speed = 0;
  game->game_over = 0;
}

// void upgrade_game(GameInfo_t *game, FILE *file) {

//     memset(game->field, 0, sizeof(game->field));

//     free_matrix(game->figure.work_figure, game->figure.rows);
//     game->figure.rows = 0;
//     game->figure.columns = 0;
//     game->figure.current_x = 0;
//     game->figure.current_y = 0;
//     game->figure.number_figure = 0;

//     game->number_next_figure = 0;
//     game->high_score = game->score;
//     game->score = 0;
//     game->level = 0;
//     game->speed = 0;
//     game->game_over = 1;

// }

void free_matrix(char **matrix, int rows) {
  for (int i = 0; i < rows; i++) {
    free(matrix[i]);
  }
  free(matrix);
}

// Проверка на движение влево
int check_left_position(matrix_figure shape, char **field) {
  int answer_code = 0;
  if (shape.current_x == 0) {
    answer_code = 1;
  } else {
    for (int i = shape.rows - 1; i >= 0; i--) {
      for (int j = shape.columns - 1; j >= 0; j--) {
        if (shape.work_figure[i][j] == 'X') {
          if (field[i + shape.current_y][j + shape.current_x / 2 - 1] == 'X') {
            answer_code = 1;
            break;
          }
        }
      }
      if (answer_code == 1) break;
    }
  }

  return answer_code;
}

// Проверка на движение вправо
int check_right_position(matrix_figure shape, char **field) {
  int answer_code = 0;
  if (shape.current_x == FIELD_W * 2 - shape.columns * 2) {
    answer_code = 1;
  } else {
    for (int i = shape.rows - 1; i >= 0; i--) {
      for (int j = shape.columns - 1; j >= 0; j--) {
        if (shape.work_figure[i][j] == 'X') {
          if (field[i + shape.current_y][j + shape.current_x / 2 + 1] == 'X') {
            answer_code = 1;
            break;
          }
        }
      }
      if (answer_code == 1) break;
    }
  }

  return answer_code;
}

// Проверка на движение вниз
int check_down_position(matrix_figure shape, char **field) {
  int answer_code = 0;
  if (shape.current_y == FIELD_H - shape.rows) {
    answer_code = 1;
  } else {
    for (int i = shape.rows - 1; i >= 0; i--) {
      for (int j = shape.columns - 1; j >= 0; j--) {
        if (shape.work_figure[i][j] == 'X') {
          if (field[i + shape.current_y + 1][j + shape.current_x / 2] == 'X') {
            answer_code = 1;
            break;
          }
        }
      }
      if (answer_code == 1) break;
    }
  }

  return answer_code;
}

// Фигура становиться частью игрового поля
void part_of_ship(matrix_figure shape, char **field) {
  for (int i = 0; i < shape.rows; i++) {
    for (int j = 0; j < shape.columns; j++) {
      if (shape.work_figure[i][j] == 'X') {
        field[i + shape.current_y][j + shape.current_x / 2] = 'X';
        shape.work_figure[i][j] =
            ' ';  // потом переписать!!! надо удалять фигуру а не стирать!!!
      }
    }
  }
}

// Удаление фигуры
void remove_figure(matrix_figure *shape) {
  memset(shape, 0, sizeof(*shape));
  free_matrix(shape->work_figure, shape->rows);
  //   if (shape->work_figure != NULL) {
  //     for (int i = 0; i < shape->rows; i++) {
  //       free(shape->work_figure[i]);
  //     }
  //     free(shape->work_figure);
  //   }
  shape->rows = 0;
  shape->columns = 0;
  shape->current_x = 0;
  shape->current_y = 0;
  shape->number_figure = 0;
}

int check_rotate_position(matrix_figure shape, char **field) {
  int answer_code = 0;

  char temp_rotate[shape.columns][shape.rows];

  for (int i = 0; i < shape.rows; i++) {
    for (int j = 0; j < shape.columns; j++) {
      temp_rotate[j][shape.rows - i - 1] = shape.work_figure[i][j];
    }
  }

  int new_width = shape.rows;
  int new_height = shape.columns;

  for (int i = shape.current_y; i < shape.current_y + new_height; i++) {
    for (int j = shape.current_x / 2; j < shape.current_x / 2 + new_width;
         j++) {
      if ((field[i][j] == 'X' &&
           temp_rotate[i - shape.current_y][j - shape.current_x / 2] == 'X') ||
          j >= FIELD_W || i >= FIELD_H) {
        answer_code = 1;
        break;
      }
    }
    if (answer_code == 1) break;
  }
  return answer_code;
}

//  функция вращения фигуры на поле
matrix_figure rotate_figure(matrix_figure shape) {
  matrix_figure rotated_figure;
  rotated_figure.work_figure = create_matrix(shape.columns, shape.rows);
  rotated_figure.rows = shape.columns;
  rotated_figure.columns = shape.rows;
  rotated_figure.current_x = shape.current_x;
  rotated_figure.current_y = shape.current_y;
  rotated_figure.number_figure = shape.number_figure;

  for (int i = 0; i < shape.rows; i++) {
    for (int j = 0; j < shape.columns; j++) {
      rotated_figure.work_figure[j][shape.rows - i - 1] =
          shape.work_figure[i][j];
    }
  }
  // Удаление shape
  remove_figure(&shape);
  // free_matrix(shape.work_figure, shape.rows);

  return rotated_figure;  // Возвращаем  повернутую фигуру
}

// Функция проверки на заполненность рядов, возвращает от 0 до 4(максимум
// заполненных рядов) - нужно для подсчета очков!
int check_full_row(char **field) {
  int full_row = 0;
  int full_flag = 0;
  for (int i = FIELD_H - 1; i >= 0; i--) {
    full_flag = 0;
    for (int j = 0; j < FIELD_W; j++) {
      if (field[i][j] == 'X') {
        full_flag++;
      }
    }
    if (full_flag == FIELD_W) {
      remove_shift_row(i, field);
      i++;
      full_row++;
    }
  }
  return full_row;
}

void remove_shift_row(
    int row,
    char **field) {  // принимает номер которую удаляем и все что выше едет вниз
  for (int y = row; y > 0; y--) {        // y строка
    for (int x = 0; x < FIELD_W; x++) {  // x столбец
      field[y][x] = field[y - 1][x];
    }
  }
}

int count_score(int rows) {
  int answer = pow(2, rows) * 100 - 100;
  return answer;
}

// Фигура упала
void figure_down(GameInfo_t *game, const sketch_figure figures[]) {
  part_of_ship(game->figure, game->field);
  remove_figure(&game->figure);
  game->score += count_score(check_full_row(game->field));

  if (game->score >= game->high_score) game->high_score = game->score;
  if (game->level < 10) {
    game->level = game->score / 600;
    game->speed = 550000 - 50000 * game->level;
  }

  game->figure = create_figure(figures[game->number_next_figure],
                               game->number_next_figure);
  game->number_next_figure = get_random_shape();
}

// Проверка на возможность появление фигуры
int check_сreate_position(char **field) {
  int answer_code = 0;
  for (int i = 0; i < FIELD_W; i++) {
    if (field[0][i] == 'X') {
      answer_code = 1;
      break;
    }
  }
  return answer_code;
}

// // --------------------------------------------------------------------------
// // --------------------------------------------------------------------------
// // -----------------------------  ОТРИСОВКА!!  ------------------------------
// // --------------------------------------------------------------------------
// // --------------------------------------------------------------------------

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
  // attroff(COLOR_PAIR(12)); // Деактивация цвета
  // attroff(COLOR_PAIR(13)); // Деактивация цвета
  // refresh();
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

// Отрисовка поля для вывода номера уровня
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

// Отрисовка сервисного поля (старт, пауза, геймовер)
void draw_service_field(char *text, int width, int height) {
  WINDOW *start_win;

  start_win = newwin(height, width, SERVICE_START_Y, SERVICE_START_X);
  box(start_win, 0, 0);

  mvwprintw(start_win, 1, 1, text);
  wbkgd(start_win, COLOR_PAIR(9));
  wrefresh(start_win);
}

// Отрисовка игрового поля со всеми окнами
void draw_all(GameInfo_t game, state_of_game state, sketch_figure next_sketch,
              int number) {
  draw_next_field(next_sketch, number);
  draw_field_score(game.high_score, game.score);
  draw_field_level(game.level);
  draw_main_field(game.field);

  if (state != START) {
    draw_figure(game.figure);
  }

  // if (state == START) {
  //     draw_service_field("   Press ENTER\n     for start", 18, 4);
  // } else if (state == PAUSE) {
  //     draw_service_field("     PAUSE", 18, 3);
  // } else if (state == GAME_OVER) {
  //     draw_service_field("   GAME OVER!", 18, 3);
  // }
}
