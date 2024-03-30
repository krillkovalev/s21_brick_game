#include "fsm.h"

/**
 * @brief Преобразуем пользовательский ввод в структуру
 *
 * @param file Файл, в котором хранится рекордный результат
 * @param figures Массив шаблонов фигур
 * 
 * @return Возвращается созданный объект игры
 */
// Инициализируем игру
GameInfo_t init_game(FILE **file, const sketch_figure figures[]) {
  GameInfo_t new_game;
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
  new_game.speed = START_SPEED;  // В версии start - это timer
  new_game.game_over = 1;  // В версии start не используется

  // Инициализируем фигуру
  new_game.figure = create_figure(figures[new_game.number_next_figure],
                                  new_game.number_next_figure);

  return new_game;
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


/**
 * @brief Проверка на движение влево
 *
 * @param shape Текущая фигура
 * @param field Матрица поля
 * 
 * @return 0 - если движение возможно, 1 - если движение не возможно
 */
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

/**
 * @brief Проверка на движение вправо
 *
 * @param shape Текущая фигура
 * @param field Матрица поля
 * 
 * @return 0 - если движение возможно, 1 - если движение не возможно
 */
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

/**
 * @brief Проверка на движение вниз
 *
 * @param shape Текущая фигура
 * @param field Матрица поля
 * 
 * @return 0 - если движение возможно, 1 - если движение не возможно
 */
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

/**
 * @brief Проверка на возможность поворота фигуры
 *
 * @param shape Текущая фигура
 * @param field Матрица поля
 * 
 * @return 0 - если поворот, 1 - если поворот не возможно
 */
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

/**
 * @brief Проверка на заполненность рядов
 *
 * @param field Матрица поля
 * 
 * @return Возвращает от 0 до 4-х
 */
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

/**
 * @brief Проверка на game_over
 *
 * @param field Матрица поля
 * 
 * @return Возвращает от 0 до 4-х
 */
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


/**
 * @brief Проверка на game_over
 * 
 * @return Возвращает номер фигуры от 0 до 6
 */
int get_random_shape() {
  time_t now;
  struct tm *work_time;

  now = time(0);
  if ((work_time = localtime(&now)) == NULL) {
    return 1;
  }
  return work_time->tm_sec % 7;
}

/**
 * @brief Создаем матрицуБ которая нужна для фигуры или для поля
 *
 * @param rows Количество рядов
 * @param columns Количество столбцов
 * 
 * @return Возвращаем указатель на выделенную память для матрицы
 */
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

/**
 * @brief Создание фигуры
 *
 * @param sketch Массив шаблонов фигур
 * @param number Номер создаваемой фигуры
 * 
 * @return Возвращается созданная фигура
 */
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

/**
 * @brief Удаление текущей игры и очистка памяти
 *
 * @param game Состояние игры
 * @param file Файл с рекордным счетом
 */
void delete_game(GameInfo_t *game, FILE *file) {
  if (game != NULL) {
    // free_matrix(game->field, FIELD_H);

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
}


/**
 * @brief Очистка памяти для матрицы
 *
 * @param matrix Матрица для очистки памяти
 * @param rows Количество рядов
 */
void free_matrix(char **matrix, int rows) {
  for (int i = 0; i < rows; i++) {
    free(matrix[i]);
  }
  free(matrix);
}

/**
 * @brief Функция для перевода фигуры в часть игрового поля
 *
 * @param shape Фигура
 * @param field Игровое поле
 */
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

/**
 * @brief Удаление фигуры и очистка памяти
 *
 * @param shape Фигура
 */
void remove_figure(matrix_figure *shape) {
  memset(shape, 0, sizeof(*shape));
  free_matrix(shape->work_figure, shape->rows);

  shape->rows = 0;
  shape->columns = 0;
  shape->current_x = 0;
  shape->current_y = 0;
  shape->number_figure = 0;
}

/**
 * @brief Вращение фигуры
 *
 * @param shape Текущая фигура
 * 
 * @return Возвращается новая, повернутая фигура
 */
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
  // Возвращаем  повернутую фигуру
  return rotated_figure;
}

/**
 * @brief Удаление заполненных рядов
 *
 * @param row Номер, удаляемого ряда
 * @param field Игровое поле
 */
void remove_shift_row(int row, char **field) {  
  for (int y = row; y > 0; y--) {        
    for (int x = 0; x < FIELD_W; x++) {  
      field[y][x] = field[y - 1][x];
    }
  }
}

/**
 * @brief Функция подсчета очков
 *
 * @param rows Количество удаляемых рядов
 * 
 * @return Возвращается количество набранных очков
 */
int count_score(int rows) {
  int answer = pow(2, rows) * 100 - 100;
  return answer;
}

/**
 * @brief Состояние после падения фигуры
 *
 * @param game Текущее состояние игры
 * @param figures Массив эскизов фигур
 */
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

