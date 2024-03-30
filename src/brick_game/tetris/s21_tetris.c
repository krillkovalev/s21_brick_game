#include "fsm.h"

/**
 * @brief Запускаем игру ТЕТРИС в консоли
 */

int main() {
  // Массив эскизов фигур - 7 шт.
  const sketch_figure figures[7] = {
      {"0XXXX0", 3, 2},  // S-образная
      {"XX00XX", 3, 2},  // Z-образная
      {"0X0XXX", 3, 2},  // T-образная
      {"00XXXX", 3, 2},  // L-образная
      {"X00XXX", 3, 2},  // зеркальная L
      {"XXXX", 2, 2},    // квадрат
      {"XXXX", 4, 1},    // прямая
  };

  init_ncurses();  // Инициализация ncurses и цвета
  game_process(figures);
  endwin();  // Завершение работы с ncurses

  return 0;
}

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

