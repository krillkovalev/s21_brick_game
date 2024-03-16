#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

#define OFFSET_X 4
#define OFFSET_Y 2

#define START_X 8
#define START_Y 0


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
    int number_figure;
} matrix_figure;


void draw_field(char (*field)[10]);
void draw_figure(matrix_figure shape);
void init_ncurses();
void clear_window();
void draw_border();
matrix_figure create_figure(int number);
char **create_matrix(int rows, int columns);
void free_matrix(char **matrix, int rows);
int check_down_position(matrix_figure shape, char (*field)[10]);
int check_left_position(matrix_figure shape, char (*field)[10]);
int check_right_position(matrix_figure shape, char (*field)[10]);
void part_of_ship(matrix_figure shape, char (*field)[10]);

int check_rotate_position(matrix_figure shape, char (*field)[10]);
int scoring(char (*field)[10]);
void remove_figure(matrix_figure *shape);
int get_random_shape(void);
matrix_figure rotate_figure(matrix_figure shape);
int check_full_row(char (*field)[10]);
void remove_shift_row(int row, char (*field)[10]);

void draw_next(int number);
void draw_field_score(int score);
void draw_field_level(int level);



int main() {
    // char field[20][10] = {0};

     char field[20][10] = {
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' ,' ', }, 
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' ,' ', },
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' ,' ', },
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' ,' ', },
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' ,' ', },
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' ,' ', },
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' ,' ', },
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' ,' ', },
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' ,' ', },
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' ,' ', },
        {' ', ' ', ' ', ' ', 'X', ' ', ' ', 'X', ' ' ,' ', },
        {' ', ' ', ' ', ' ', 'X', ' ', ' ', 'X', ' ' ,' ', },
        {' ', ' ', ' ', ' ', 'X', ' ', ' ', 'X', ' ' ,' ', },
        {' ', ' ', ' ', ' ', 'X', ' ', ' ', 'X', ' ' ,' ', },
        {' ', ' ', ' ', ' ', 'X', ' ', ' ', 'X', ' ' ,' ', },
        {' ', ' ', ' ', ' ', 'X', ' ', ' ', 'X', ' ' ,' ', },
        {'X', 'X', ' ', ' ', 'X', ' ', ' ', 'X', ' ' ,' ', },
        {'X', 'X', ' ', ' ', 'X', ' ', ' ', 'X', ' ' ,' ', },
        {'X', 'X', 'X', ' ', 'X', ' ', ' ', 'X', 'X' ,'X', },
        {'X', 'X', 'X', 'X', 'X', ' ', ' ', 'X', 'X' ,'X', },
    };

    init_ncurses(); // Инициализация ncurses
	double timer = 700000; //half second
	struct timeval before, after;
    gettimeofday(&before, NULL);
	nodelay(stdscr, TRUE); // Разрешить getch не блокировать
    //timeout(10000);
    // int x = 7, y = 1; // Стартовые координаты для фигуры

    //matrix_figure figure = create_figure(5);

    int random_number_of_figure = get_random_shape();
    matrix_figure figure = create_figure(random_number_of_figure);
    int level = check_full_row(field);
    while(1) {
        int check = 0;
        random_number_of_figure = get_random_shape();
        while (check == 0) {
            
            draw_field(field);
            draw_figure(figure); 
            draw_next(random_number_of_figure);
            int current_score = scoring(field);
            draw_field_score(current_score);
            draw_field_level(level);
            int ch;
            if ((ch = getch()) != ERR) {
                switch (ch) {
                    case KEY_UP:
                        if(check_rotate_position(figure, field) == 0) figure = rotate_figure(figure);
                        break;
                    case KEY_DOWN:
                        if (check_down_position(figure, field) == 0) figure.current_y++;
                        else {
                            part_of_ship(figure, field);
                            remove_figure(&figure);
                            check_full_row(field);
                            check++;
                            figure = create_figure(random_number_of_figure);
                        }
                        break;
                    case KEY_LEFT:
                        if (check_left_position(figure, field) == 0) figure.current_x -= 2;
                        break;
                    case KEY_RIGHT:
                        if (check_right_position(figure, field) == 0) figure.current_x += 2;
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
                else{
                    part_of_ship(figure, field);
                    remove_figure(&figure);
                    check_full_row(field);
                    check++;
                    figure = create_figure(random_number_of_figure);
                } 
            }

            
            }
    }

    return 0;
}


// Проверка на движение вниз
int check_down_position(matrix_figure shape, char (*field)[10]) {
    int answer_code = 0;
    if (shape.current_y == FIELD_H - shape.rows) {
        answer_code = 1;
    } else {
        for(int i = shape.rows - 1; i >= 0; i--){
            for(int j = shape.columns - 1; j >= 0; j--){
                if(shape.work_figure[i][j] == 'X') {
                    if(field[i + shape.current_y + 1][j + shape.current_x / 2] == 'X'){
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


// Проверка на движение влево
int check_left_position(matrix_figure shape, char (*field)[10]) {
    int answer_code = 0;
    if (shape.current_x == 0) {
        answer_code = 1;
    } else {
        for(int i = shape.rows - 1; i >= 0; i--){
            for(int j = shape.columns - 1; j >= 0; j--){
                if(shape.work_figure[i][j] == 'X') {
                    if(field[i + shape.current_y][j + shape.current_x / 2 - 1] == 'X'){
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


// Проверка на движение вправо
int check_right_position(matrix_figure shape, char (*field)[10]) {
    int answer_code = 0;
    if (shape.current_x == FIELD_W * 2 - shape.columns * 2) {
        answer_code = 1;
    } else {
        for(int i = shape.rows - 1; i >= 0; i--){
            for(int j = shape.columns - 1; j >= 0; j--){
                if(shape.work_figure[i][j] == 'X') {
                    if(field[i + shape.current_y][j + shape.current_x / 2 + 1] == 'X'){
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


// Проверка на возможность поворота фигуры
int check_rotate_position(matrix_figure shape, char (*field)[10])
{
    int answer_code = 0;

    for (int y = shape.current_y; y < shape.current_y + shape.rows; y++) {
        for(int x = shape.current_x; x < shape.current_x + shape.columns; x++){
            if(field[y][x] == 'X' || shape.current_x >= (FIELD_W * 2 - shape.columns * 2)) {
                answer_code = 1;
                break;
            }
        }
        if(answer_code) break;
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

    for (int i = 0; i < shape.rows; i++)
        {
            for (int j = 0; j < shape.columns; j++)
            {
                rotated_figure.work_figure[j][shape.rows - i - 1] = shape.work_figure[i][j];
            }
        }
        // Добавить удаление shape
    return  rotated_figure; // Возвращаем  повернутую фигуру
}


// Фигура становиться частью игрового поля
void part_of_ship(matrix_figure shape, char (*field)[10]) {
    for (int i = 0; i < shape.rows; i++) {
        for (int j = 0; j < shape.columns; j++) {
            if (shape.work_figure[i][j] == 'X'){
                field[i + shape.current_y][j + shape.current_x / 2] = 'X';
                shape.work_figure[i][j] = ' '; // потом переписать!!! надо удалять фигуру а не стирать!!!

            }
        }
    }
}



// Функция проверки на заполненность рядов, возвращает от 0 до 4(максимум заполненных рядов) - нужно для подсчета очков!
 int check_full_row(char (*field)[10]) {
    int full_row = 0;
    for (int i = FIELD_H - 1; i >= 0; i--) {
        int full_flag = 0;
        for (int j = 0; j < FIELD_W; j++) {
            if(field[i][j] == 'X') {
                full_flag++;
            }
        }
    if(full_flag == FIELD_W) {
        full_row++;
        remove_shift_row(i, field);
        }
    }
    return full_row;
}

int scoring(char (*field)[10]) {
    int score = 0;
    int full_row = check_full_row(field);
    switch (full_row) {
        case 1:
            score = 100;
            break;
        case 2:
            score = 300;
            break;
        case 3:
            score = 700;
            break;
        case 4:
            score = 1500;
            break;
        default:
            score = 0;    
    }
    return score;
}

// Удаление фигуры
void remove_figure(matrix_figure *shape) {
  memset(shape, 0, sizeof(*shape));
  if (shape->work_figure != NULL) {
    for (int i = 0; i < shape->rows; i++) {
      free(shape->work_figure[i]);
    }
    free(shape->work_figure); // Дописать обнуление всех полей
  }

}



// Удаляем заполненную строку
void remove_shift_row(int row, char (*field)[10]) {  // принимает номер которую удаляем и все что выше едет вниз
  for (int y = row; y > 0; y--) {            // y строка
    for (int x = 0; x < FIELD_W; x++) {  // x столбец
      field[y][x] = field[y - 1][x];
    }
  }
}




// Удаление матрицы фигуры
void free_matrix(char **matrix, int rows) {
  for (int i = 0; i < rows; i++) {
    free(matrix[i]); // Дописать обнуление всех полей
  }
  free(matrix);
}


// Получаем фигуру рандомно
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



// Создаем фигуру
matrix_figure create_figure(int number) {
    int rows = figures[number].height;
    int columns = figures[number].width;

    matrix_figure figure;
    figure.work_figure = create_matrix(rows, columns);
    figure.rows = rows;
    figure.columns = columns;
    figure.current_x = START_X; //OFFSET_X + 8; // Стартовая позиция по X
    figure.current_y = START_Y; //OFFSET_Y; // Стартовая позиция по Y
    figure.number_figure = number;

    int k = 0;
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < columns; j++) {
            figure.work_figure[i][j] = figures[number].figure[k];
            k++;
        }
    }

    return figure; // Возвращаем созданную фигуру
}


// Создаем матрицу фигуры 
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

// Функция для отрисовки фигуры на поле
void draw_figure(matrix_figure shape) {

    init_pair(1, COLOR_GREEN, COLOR_RED); // Инициализация пары цветов
    attron(COLOR_PAIR(1)); // Активация цвета

    for (int i = 0; i < shape.rows; i++) {
        for (int j = 0; j < shape.columns; j++) {
			if(shape.work_figure[i][j] == 'X') {
            	mvaddch(OFFSET_Y + shape.current_y + i, OFFSET_X + shape.current_x + j * 2, ' '); 
                mvaddch(OFFSET_Y + shape.current_y + i, OFFSET_X + shape.current_x + j * 2 + 1, ' '); 

			}
        }
    }
	
}

// Функция для отрисовки поля
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


// Отрисовка поля для выводы текущего счета и рекорда
void draw_field_score(int score) {
    init_pair(4, COLOR_BLUE, COLOR_WHITE); // Инициализация пары цветов
    WINDOW *score_win;
    int startx = 26; 
    int starty = 2;
    int width = 12;
    int height = 9;

    score_win = newwin(height, width, starty, startx);
    mvwprintw(score_win, 0, 1, "HIGH SCORE");
    mvwprintw(score_win, 4, 3, "SCORE: %d", score);
    wbkgd(score_win, COLOR_PAIR(4));
    wrefresh(score_win);
    
}

// Отрисовка поля для вывода вида следующей фигуры
void draw_next(int number) {
    init_pair(4, COLOR_BLUE, COLOR_WHITE); // Инициализация пары цветов
    WINDOW *next_figure_win;
    int startx = 26; 
    int starty = 12;
    int width = 12;
    int height = 6;

    next_figure_win = newwin(height, width, starty, startx);
    //box(next_figure_win, 0 , 0);
    mvwprintw(next_figure_win, 0, 4, "NEXT");
    wbkgd(next_figure_win, COLOR_PAIR(4));

    init_pair(10, COLOR_GREEN, COLOR_RED); // Инициализация пары цветов
    wattron(next_figure_win, COLOR_PAIR(10)); // Активация цвета

    int k = 0;
    for(int i = 0; i < figures[number].height; i++) {
        for(int j = 0; j < figures[number].width; j++) {

            if(figures[number].figure[k] == 'X') {
                
            	mvwaddch(next_figure_win, i + 3, j * 2 + 3, ' '); 
                mvwaddch(next_figure_win, i + 3, j * 2 + 4, ' '); 
                
			}
            k++;
        }
    }
    wattroff(next_figure_win, COLOR_PAIR(10)); // Деактивация цвета
    wrefresh(next_figure_win); 
}

// Отрисовка поля для вывода номера уровня
void draw_field_level(int level) {
    init_pair(4, COLOR_BLUE, COLOR_WHITE); // Инициализация пары цветов
    WINDOW *level_win;
    int startx = 26; 
    int starty = 19;
    int width = 12;
    int height = 3;

    level_win = newwin(height, width, starty, startx);
    box(level_win, 4 , 4);
    // mvwprintw(level_win, 0, 3, "LEVEL");
    mvwprintw(level_win, 1, 2, "LEVEL %d", level);
    wbkgd(level_win, COLOR_PAIR(4));
    wrefresh(level_win);
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

// gcc -Wall -Wextra -Werror -std=c11 start.c -lncurses

