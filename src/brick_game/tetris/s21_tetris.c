// #include "s21_tetris.h"

#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

WINDOW *create_newwin(int height, int width, int start_y, int start_x);
void destroy_win(WINDOW *local_win);
void windows_manipulation(WINDOW *my_win, int height, int width, int* start_x, int* start_y);
WINDOW *create_figure(int height, int width, int start_y, int start_x);

int main()
{ WINDOW *my_win;
 int start_x, start_y, width, height;
 int ch;

 initscr(); 
 cbreak();   
 keypad(stdscr, TRUE);
    curs_set(0); // Если 0, прячет курсор
    noecho(); // Не показывает, что печатает пользователь

    //init_pair(1, COLOR_WHITE, COLOR_BLUE);
    

 height = 3;
 width = 10;
 start_y = (LINES - height) / 2; /* Calculating for a center placement */
 start_x = (COLS - width) / 2; /* of the window  */
 //printw("Press F1 to exit");
 refresh();
 create_newwin(30, 20, 0, 0);
//  my_win = create_newwin(height, width, start_y, start_x);
 int figure_x = 2, figure_y = 2;
 int square_x = 3, square_y = 3;
//  WINDOW *my_figure = create_figure(2, 10, 2, 2); // создали прямоугольник
//  windows_manipulation(my_figure, 2, 10, &figure_x, &figure_y);
 WINDOW *square = create_figure(3, 6, square_y, square_x); // создалии квадрат
 windows_manipulation(square, 3, 6, &square_x, &square_y);
//  windows_manipulation(my_win, height, width, &start_x, &start_y);

     
 endwin();   /* End curses mode    */
 return 0;
}

WINDOW *create_newwin(int height, int width, int start_y, int start_x) { 
   WINDOW *local_win;
      
   init_pair(1, COLOR_BLUE, COLOR_WHITE);
   local_win = newwin(height, width, start_y, start_x);
   box(local_win, 0 , 0);
   wbkgd(local_win, COLOR_PAIR(1));
   wrefresh(local_win);  /*Show that box*/

   return local_win;
}

void windows_manipulation(WINDOW *my_win, int height, int width, int* start_x, int* start_y) {
   int ch;
   while((ch = getch()) != 'q')
 { switch(ch)
  { case KEY_LEFT:
    destroy_win(my_win);
    my_win = create_newwin(height, width, *start_y, --*start_x);
    break;
   case KEY_RIGHT:
    destroy_win(my_win);
    my_win = create_newwin(height, width, *start_y, ++*start_x);
    break;
   case KEY_UP:
    destroy_win(my_win);
    my_win = create_newwin(height, width, --*start_y, *start_x);
    break;
   case KEY_DOWN:
    destroy_win(my_win);
    my_win = create_newwin(height, width, ++*start_y, *start_x);
    break; 
  }
 }   
}

WINDOW *create_figure(int height, int width, int start_y, int start_x) { 
   WINDOW *local_win;
      
   init_pair(1, COLOR_BLUE, COLOR_WHITE);
   local_win = newwin(height, width, start_y, start_x);
   box(local_win, 0 , 0);
   wbkgd(local_win, COLOR_PAIR(1));
   wrefresh(local_win);  /*Show that box*/

   return local_win;
}

void destroy_win(WINDOW *local_win) { 
   wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');

   wrefresh(local_win);
   delwin(local_win);
}

