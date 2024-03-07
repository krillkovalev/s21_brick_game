#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

// Функция для инициализации ncurses
void init_ncurses() {
  initscr();      // Инициализация ncurses
  start_color();  // Начало работы с цветами
  curs_set(0);    // Скрытие курсора
  noecho();  // Отключение отображения введенных символов
  keypad(stdscr, TRUE);  // Включение чтения специальных клавиш
}

// Функция для очистки окна
void clear_window() {
  clear();    // Очистка всего окна
  refresh();  // Обновление отображения
}

// Функция для отрисовки границы большого окна
void draw_border() {
  box(stdscr, 0, 0);  // Рисуем границу
  refresh();          // Обновление отображения
}

// Функция для отрисовки маленького окна зеленого цвета
void draw_small_window(int x, int y) {
  char shape[][4] = {
      {'X', 'X', ' ', ' '},
      {'X', 'X', 'X', 'X'},
      {' ', ' ', 'X', 'X'},
  };
  init_pair(1, COLOR_GREEN, COLOR_RED);  // Инициализация пары цветов
  attron(COLOR_PAIR(1));                 // Активация цвета
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 4; j++) {
      if (shape[i][j] == 'X') {
        mvaddch(y + i, x + j, ' ');  // Рисуем маленькое окно без символов
      }
    }
  }
}

// Функция для отрисовки маленького окна зеленого цвета
void draw_big_window(int x, int y) {
  init_pair(2, COLOR_GREEN, COLOR_WHITE);  // Инициализация пары цветов
  attron(COLOR_PAIR(2));                   // Активация цвета
  for (int i = 0; i < 20; i++) {
    for (int j = 0; j < 20; j++) {
      mvaddch(y + i, x + j, ' ');  // Рисуем маленькое окно без символов
    }
  }
  attroff(COLOR_PAIR(2));  // Деактивация цвета
}

int main() {
  init_ncurses();         // Инициализация ncurses
  double timer = 1000000;  // half second
  struct timeval before, after;
  gettimeofday(&before, NULL);
  nodelay(stdscr, TRUE);  // Разрешить getch не блокировать
  int x = 7, y = 1;       // Координаты маленького окна
  while (1) {
    draw_big_window(1, 1);
    draw_small_window(x, y);  // Отрисовка маленького окна
    int ch;
    if ((ch = getch()) != ERR) {
      switch (ch) {
        case KEY_UP:
          break;
        case KEY_DOWN:
          if (y < 18) y++;
          break;
        case KEY_LEFT:
          if (x > 1) x--;
          break;
        case KEY_RIGHT:
          if (x < 17) x++;
          break;
        case 'q':    // Выход по нажатию 'q'
          endwin();  // Завершение работы с ncurses
          exit(0);
      }
    }
    gettimeofday(&after, NULL);
    if (((double)after.tv_sec * 100000000 + (double)after.tv_usec) -
            ((double)before.tv_sec * 100000000 + (double)before.tv_usec) >
        timer) {  // time difference in microsec accuracy
      before = after;
      if (y < 18) y++;
    }
  }

  return 0;
}