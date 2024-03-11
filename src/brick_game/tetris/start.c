
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

#define OFFSET_X 4
#define OFFSET_Y 2

#define FIELD_H 20
#define FIELD_W 10


typedef struct sketch_figure {
    const char *figure; 
    int width;
    int height;
} sketch_figure;


const sketch_figure figures[7] = {
    {"0XXXX0", 3, 2}, // S-образная
    {"XX00XX", 3, 2}, // Z-образная
    {"0X0XXX", 3, 2}, // T-образная
    {"00XXXX", 3, 2}, // L-образная
    {"X00XXX", 3, 2}, // зеркальная L
    {"XXXX", 2, 2}, // квадрат
    {"XXXX", 4, 1}, // прямая
};

typedef struct matrix_figure {
    char **work_figure;
    int rows;
    int columns;
    int current_x;
    int current_y;
} matrix_figure;


void draw_field(char (*field)[10]);
void draw_figure(matrix_figure shape);
void init_ncurses();
void clear_window();
void draw_border();
matrix_figure create_figure(int number);
char **create_matrix(int rows, int columns);
int get_random_shape(void);
int check_down_position(matrix_figure shape, char (*field)[10]);
int check_left_position(matrix_figure shape, char (*field)[10]);
int check_right_position(matrix_figure shape, char (*field)[10]);
void part_of_ship(matrix_figure shape, char (*field)[10]);
matrix_figure rotate_figure(matrix_figure shape);
void remove_figure(matrix_figure *shape);

// Создаем фигуру
matrix_figure create_figure(int number) {
    int rows = figures[number].height;
    int columns = figures[number].width;

    matrix_figure figure;
    figure.work_figure = create_matrix(rows, columns);
    figure.rows = rows;
    figure.columns = columns;
    figure.current_x = OFFSET_X + 8; // Стартовая позиция по X
    figure.current_y = OFFSET_Y; // Стартовая позиция по Y

    int k = 0;
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < columns; j++) {
            figure.work_figure[i][j] = figures[number].figure[k];
            k++;
        }
    }

    return figure; // Возвращаем созданную фигуру
}


char **create_matrix(int rows, int columns) {
  char **matrix = (char **)malloc(rows * sizeof(char *));
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

void remove_figure(matrix_figure *shape) {
  memset(shape, 0, sizeof(*shape));
  if (shape->work_figure != NULL) {
    for (int i = 0; i < shape->rows; i++) {
      free(shape->work_figure[i]);
    }
    free(shape->work_figure);
  }

}

// Функция для инициализации ncurses
void init_ncurses() {
    initscr(); // Инициализация ncurses
    start_color(); // Начало работы с цветами
    curs_set(0); // Скрытие курсора
    noecho(); // Отключение отображения введенных символов
    keypad(stdscr, TRUE); // Включение чтения специальных клавиш
}

// Функция для очистки окна
void clear_window() {
    clear(); // Очистка всего окна
    refresh(); // Обновление отображения
}

// Функция для отрисовки границы большого окна
void draw_border() {
    box(stdscr, 0, 0); // Рисуем границу
    refresh(); // Обновление отображения
}

// Функция для отрисовки маленького окна зеленого цвета
void draw_figure(matrix_figure shape) {

    init_pair(1, COLOR_GREEN, COLOR_RED); // Инициализация пары цветов
    attron(COLOR_PAIR(1)); // Активация цвета

    for (int i = 0; i < shape.rows; i++) {
        for (int j = 0; j < shape.columns; j++) {
			if(shape.work_figure[i][j] == 'X') {
            	mvaddch(shape.current_y + i, shape.current_x + j * 2, ' '); 
                mvaddch(shape.current_y + i, shape.current_x + j * 2 + 1, ' '); 
				//mvaddch(y + i, x + j + 1, 'X'); // Рисуем маленькое окно без символов
			}
        }
    }
	
}

// Функция для отрисовки маленького окна зеленого цвета
void draw_field(char (*field)[10]) {

    init_pair(2, COLOR_WHITE, COLOR_WHITE); // Инициализация пары цветов
    init_pair(3, COLOR_WHITE, COLOR_BLUE); // Инициализация пары цветов

    //attron(COLOR_PAIR(3)); // Активация цвета
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 10; j++) {
            if (field[i][j] != 'X') {
                attron(COLOR_PAIR(2)); // Активация цвета
                mvaddch(OFFSET_Y + i, OFFSET_X + j * 2, ' '); // Рисуем маленькое окно без символов
                mvaddch(OFFSET_Y + i, OFFSET_X + j * 2 + 1, ' ');
            }
            if (field[i][j] == 'X') {
                attron(COLOR_PAIR(3)); // Активация цвета
                mvaddch(OFFSET_Y + i, OFFSET_X + j * 2, '['); // Рисуем маленькое окно без символов
                mvaddch(OFFSET_Y + i, OFFSET_X + j * 2 + 1, ']');
            }
        }
    }
    attroff(COLOR_PAIR(2)); // Деактивация цвета
    attroff(COLOR_PAIR(3)); // Деактивация цвета
    // refresh(); // Обновление отображения
}

int main() {
    // char field[20][20] = {0};

     char field[20][10] = {
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' ,' ', }, 
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' ,' ', },
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' ,' ', },
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' ,' ', },
        {' ', 'X', ' ', ' ', ' ', ' ', ' ', ' ', ' ' ,' ', },
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' ,' ', },
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' ,' ', },
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' ,' ', },
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' ,' ', },
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' ,' ', },
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' ,' ', },
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' ,' ', },
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' ,' ', },
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' ,' ', },
        {'X', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' ,' ', },
        {'X', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' ,' ', },
        {'X', 'X', ' ', ' ', ' ', ' ', ' ', ' ', ' ' ,' ', },
        {'X', 'X', ' ', ' ', ' ', ' ', ' ', ' ', ' ' ,' ', },
        {'X', 'X', 'X', ' ', 'X', ' ', ' ', ' ', 'X' ,'X', },
        {'X', 'X', 'X', 'X', 'X', ' ', ' ', ' ', 'X' ,'X', },
    };

    init_ncurses(); // Инициализация ncurses
	double timer = 700000; //half second
	struct timeval before, after;
    gettimeofday(&before, NULL);
	nodelay(stdscr, TRUE); // Разрешить getch не блокировать
    //timeout(10000);
    // int x = 7, y = 1; // Стартовые координаты для фигуры

    int random_number_of_figure = get_random_shape();
    matrix_figure figure = create_figure(random_number_of_figure);

    while (1) {
        //clear_window(); // Очистка окна
        //draw_border(); // Отрисовка границы
		draw_field(field);
        draw_figure(figure); // Отрисовка фигуры
		int ch;
		if ((ch = getch()) != ERR) {
			switch (ch) {
				case KEY_UP: // Это будет поворот фигуры
                    // matrix_figure rotate_figure(matrix_figure shape);
					if (figure.current_y > OFFSET_Y) figure.current_y--;
					break;
				case KEY_DOWN:
					if (check_down_position(figure, field) == 0) figure.current_y++;
                    else part_of_ship(figure, field);
					break;
				case KEY_LEFT:
                    if (check_left_position(figure, field) == 0) figure.current_x -= 2;
					// if (figure.current_x > OFFSET_X) figure.current_x -= 2;
					break;
				case KEY_RIGHT:
                    if (check_right_position(figure, field) == 0) figure.current_x += 2;
					// if (figure.current_x < FIELD_W + OFFSET_X + figure.columns) figure.current_x += 2;
					break;
				case 'q': // Выход по нажатию 'q'
					endwin(); // Завершение работы с ncurses
					exit(0);
			}
		}
        gettimeofday(&after, NULL);
        if (((double)after.tv_sec*100000000 + (double)after.tv_usec)-((double)before.tv_sec*100000000 + (double)before.tv_usec) > timer){ //time difference in microsec accuracy
            before = after;
            if (check_down_position(figure, field) == 0) figure.current_y++;
            else {
                part_of_ship(figure, field);
                remove_figure(&figure);
                random_number_of_figure = get_random_shape();
                figure = create_figure(random_number_of_figure);
            }
        }

        
        }

    return 0;
}



int check_down_position(matrix_figure shape, char (*field)[10]) {
    int answer_code = 0;
    if (shape.current_y == FIELD_H + OFFSET_Y - shape.rows) {
        answer_code = 1;
    } else {
        for(int i = shape.rows - 1; i >= 0; i--){
            for(int j = shape.columns - 1; j >= 0; j--){
                if(shape.work_figure[i][j] == 'X') {
                    if(field[i + shape.current_y - OFFSET_Y + 1][j + shape.current_x / 2 - OFFSET_X / 2] == 'X'){
                        answer_code = 1;
                        break;
                    }
                }
            }
            if(answer_code == 1) break;
        }
    }
    
    return answer_code;
}


int check_left_position(matrix_figure shape, char (*field)[10]) {
    int answer_code = 0;
    if (shape.current_x == OFFSET_X) {
        answer_code = 1;
    } else {
        for(int i = shape.rows - 1; i >= 0; i--){
            for(int j = shape.columns - 1; j >= 0; j--){
                if(shape.work_figure[i][j] == 'X') {
                    if(field[i + shape.current_y - OFFSET_Y][j + shape.current_x / 2 - OFFSET_X / 2 - 1] == 'X'){
                        // printw("                        Field = %d %d  Current = %d %d   I = %d J = %d\n", i + shape.current_x + OFFSET_X, j + shape.current_y + OFFSET_Y + 1, shape.current_x, shape.current_y, i, j);
                        answer_code = 1;
                        break;
                    }
                }
            }
            if(answer_code == 1) break;
        }
    }
    
    return answer_code;
}


int check_right_position(matrix_figure shape, char (*field)[10]) {
    int answer_code = 0;
    if (shape.current_x == (FIELD_W * 2 + OFFSET_X - shape.columns * 2)) {
        answer_code = 1;
    } else {
        for(int i = shape.rows - 1; i >= 0; i--){
            for(int j = shape.columns - 1; j >= 0; j--){
                if(shape.work_figure[i][j] == 'X') {
                    if(field[i + shape.current_y - OFFSET_Y][j + shape.current_x / 2 - OFFSET_X / 2 + 1] == 'X'){
                        answer_code = 1;
                        break;
                    }
                }
            }
            if(answer_code == 1) break;
        }
    }
    
    return answer_code;
}


matrix_figure rotate_figure(matrix_figure shape) {
  char **new_figure = create_matrix(shape.columns, shape.rows);
  if (new_figure != NULL) {
    for (int i = 0; i < shape.rows; i++) {
      for (int j = 0; j < shape.columns; j++) {
        new_figure[j][shape.rows - i - 1] = shape.work_figure[i][j];
      }
    }
    remove_figure(&shape);
    shape.work_figure = new_figure;
    int temp = shape.rows;
    shape.rows = shape.columns;
    shape.columns = temp;
  }
  return shape;
}


void part_of_ship(matrix_figure shape, char (*field)[10]) {
    for (int i = 0; i < shape.rows; i++) {
        for (int j = 0; j < shape.columns; j++) {
            if (shape.work_figure[i][j] == 'X') {
                field[i + shape.current_y - OFFSET_Y][j + shape.current_x - OFFSET_X] = 'X';
                shape.work_figure[i][j] = ' ';
            }
        }
    }
}

int get_random_shape(void) {
    time_t now;
    struct tm *tm;

    now = time(0);
    if ((tm = localtime(&now)) == NULL) {
        printf("Ошибка при извлечении времени\n");
        return 1;
    }

    return tm->tm_sec % 7;
}





/*
                Подсказки по библиотеке ncurses

initscr(); // Инициализация ncurses
start_color(); // Инициализация цветовой палитры
curs_set(0); // Если 0, прячет курсор
noecho(); // Не показывает, что печатает пользователь
keypad(stdscr, TRUE); // Включение чтения специальных клавиш
init_pair(1, COLOR_WHITE, COLOR_BLACK); // Инициализация цветовой пары


*/

// gcc -Wall -Wextra -Werror start.c -lncurses

