#ifndef C7_TETRIS_H
#define C7_TETRIS_H

#include <curses.h>
#include <math.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

// Эскизы фигур
typedef struct {
  const char *figure;
  int width;
  int height;
} sketch_figure;

// Состояния, они же КА
typedef enum {
  START,
  PAUSE,
  GAME,
  GAME_OVER,
} state_of_game;

// Вся информация о фигуре
typedef struct {
  char **work_figure;
  int rows;
  int columns;
  int current_x;
  int current_y;
  int number_figure;
} matrix_figure;

// Структура для отрисовки игрового поля
typedef struct {
  char **field;  // главное поле
  matrix_figure figure;  // матрица фигуры переименовал из int **next;
  int number_next_figure;
  int score;       // текущий счет
  int high_score;  // рекорд
  int level;       // текущий уровень
  int speed;  // он же таймер из которого и считаем скорость
  int game_over;  // 0 или 1
  int exit_game;  // выход из игры
} GameInfo_t;

// Клавиши
typedef enum {
  Start,   // 0, enter
  Pause,   // 1, keyspace
  Exit,    // 2 q заменил с Terminate
  Left,    // 3 key_left
  Right,   // 4 key_right
  Up,      // 5 key_up - rotate
  Down,    // 6 key_down
  Action,  // 7 key_up - rotate клавишу не зарезервировал
  No_enter  // Нет ввода
} UserAction_t;

// Шаблоны фигур
#define FIGURES_INIT                                   \
  {                                                    \
    {"0XXXX0", 3, 2},     /* S-образная */     \
        {"XX00XX", 3, 2}, /* Z-образная */     \
        {"0X0XXX", 3, 2}, /* T-образная */     \
        {"00XXXX", 3, 2}, /* L-образная */     \
        {"X00XXX", 3, 2}, /* зеркальная L */ \
        {"XXXX", 2, 2},   /* квадрат */         \
    {                                                  \
      "XXXX", 4, 1                                     \
    } /* прямая */                               \
  }

// Отступы от окна терминала
#define OFFSET_X 4
#define OFFSET_Y 2

// Стартовая позиция фигуры при создании
#define START_X 8
#define START_Y 0

// Стартовая позиция сервисного окна
#define SERVICE_START_X 5
#define SERVICE_START_Y 9

// Размеры игрового поля/стакана
#define FIELD_H 20
#define FIELD_W 10

// Начальная скорость
#define START_SPEED 600000

// ------------------------------------------------------------------------------
// ----------------------------------- Логика
// ------------------------------------------------------------------------------

void init_ncurses();
void game_process(const sketch_figure figures[]);
int get_random_shape();
UserAction_t userInput(int user_input, bool hold);
GameInfo_t init_game(FILE **file, const sketch_figure figures[]);
char **create_matrix(int rows, int columns);
matrix_figure create_figure(sketch_figure sketch, int number);
void delete_game(GameInfo_t *game, FILE *file);
void free_matrix(char **matrix, int rows);
void remove_figure(matrix_figure *shape);

void updateCurrentState(state_of_game *state, UserAction_t key_now,
                        GameInfo_t *game, const sketch_figure figures[],
                        struct timeval *before, FILE *file);

int check_left_position(matrix_figure shape, char **field);
int check_right_position(matrix_figure shape, char **field);
int check_down_position(matrix_figure shape, char **field);
int check_rotate_position(matrix_figure shape, char **field);
int check_сreate_position(char **field);
void moving(state_of_game *state, UserAction_t key_now, GameInfo_t *game,
            const sketch_figure figures[], struct timeval *before);

void figure_down(GameInfo_t *game, const sketch_figure figures[]);

matrix_figure rotate_figure(matrix_figure shape);

void part_of_ship(matrix_figure shape, char **field);
void remove_shift_row(int row, char **field);
int check_full_row(char **field);
int count_score(int rows);

// ------------------------------------------------------------------------------
// ----------------------------------- Отрисовка
// ------------------------------------------------------------------------------

void draw_all(GameInfo_t game, state_of_game state, sketch_figure next_sketch,
              int number);
void draw_main_field(char **field);
void draw_figure(matrix_figure shape);
void draw_next_field(sketch_figure next_sketch, int number);
void draw_field_score(int high_score, int score);
void draw_field_level(int level);
void draw_service_field(char *text, int width, int height);

#endif

// https://rocketchat-student.21-school.ru/channel/content_help?msg=Y3j9AWpfxQGXHg8Sj
// - вопрос о неточностях