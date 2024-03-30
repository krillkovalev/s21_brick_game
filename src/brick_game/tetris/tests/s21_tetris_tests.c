#include "s21_tetris_tests.h"

START_TEST(test_get_random_shape) {
    int result = get_random_shape();
    ck_assert_int_le(result, 6);
    ck_assert_int_ge(result, 0);
}
END_TEST

START_TEST(test_userInput_up) {
    ck_assert_int_eq(userInput(KEY_UP, true), Up);
}
END_TEST

START_TEST(test_userInput_down) {
  ck_assert_int_eq(userInput(KEY_DOWN, true), Down);
}
END_TEST

START_TEST(test_userInput_left) {
  ck_assert_int_eq(userInput(KEY_LEFT, true), Left);
}
END_TEST

START_TEST(test_userInput_right) {
  ck_assert_int_eq(userInput(KEY_RIGHT, true), Right);
}
END_TEST

START_TEST(test_userInput_space) {
  ck_assert_int_eq(userInput(' ', true), Pause);
}
END_TEST

START_TEST(test_userInput_start) {
  ck_assert_int_eq(userInput(10, true), Start);
}
END_TEST

START_TEST(test_userInput_start_1) {
  ck_assert_int_eq(userInput(13, true), Start);
}
END_TEST

START_TEST(test_userInput_exit) {
  ck_assert_int_eq(userInput('q', true), Exit);
}
END_TEST

START_TEST(test_userInput_exit_1) {
  ck_assert_int_eq(userInput('Q', true), Exit);
}
END_TEST

START_TEST(test_create_matrix_allocation) {
    int rows = 3;
    int columns = 4;
    char **matrix = create_matrix(rows, columns);

    ck_assert_ptr_nonnull(matrix);

    for (int i = 0; i < rows; i++) {
        ck_assert_ptr_nonnull(matrix[i]);
    }

    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
}
END_TEST

START_TEST (test_create_matrix_returns_null)
{
    char **matrix = create_matrix(INT_MAX, INT_MAX);
    ck_assert_ptr_null(matrix);
}
END_TEST

START_TEST(test_create_figure)
{
    sketch_figure test_sketch = {
        .figure = "XXXX",
        .height = 2,
        .width = 2
    };
    int test_number = 1;

    matrix_figure result = create_figure(test_sketch, test_number);

    ck_assert_int_eq(result.rows, 2);
    ck_assert_int_eq(result.columns, 2);
    ck_assert_int_eq(result.current_x, START_X);
    ck_assert_int_eq(result.current_y, START_Y);
    ck_assert_int_eq(result.number_figure, 1);
    ck_assert_int_eq(result.work_figure[0][0], 'X');
    ck_assert_int_eq(result.work_figure[0][1], 'X');
    ck_assert_int_eq(result.work_figure[1][0], 'X');
    ck_assert_int_eq(result.work_figure[1][1], 'X');

    free_matrix(result.work_figure, result.rows);
}
END_TEST

START_TEST(test_init_game) {
        const sketch_figure figures[7] = {
        {"0XXXX0", 3, 2}, // S-образная
        {"XX00XX", 3, 2}, // Z-образная
        {"0X0XXX", 3, 2}, // T-образная
        {"00XXXX", 3, 2}, // L-образная
        {"X00XXX", 3, 2}, // зеркальная L
        {"XXXX", 2, 2}, // квадрат
        {"XXXX", 4, 1}, // прямая
    }; 
    FILE *high_score_file = NULL;
    GameInfo_t game = init_game(&high_score_file, figures); 


    ck_assert_ptr_nonnull(game.field);
    ck_assert_int_eq(game.score, 0);
    ck_assert_int_eq(game.high_score, 0); 
    ck_assert_int_eq(game.number_next_figure, get_random_shape()); 
    ck_assert_int_eq(game.level, 0);
    ck_assert_int_eq(game.speed, START_SPEED);
    ck_assert_int_eq(game.game_over, 1);

}
END_TEST

START_TEST(test_delete_game) {
    const sketch_figure figures[7] = {
        {"0XXXX0", 3, 2}, // S-образная
        {"XX00XX", 3, 2}, // Z-образная
        {"0X0XXX", 3, 2}, // T-образная
        {"00XXXX", 3, 2}, // L-образная
        {"X00XXX", 3, 2}, // зеркальная L
        {"XXXX", 2, 2}, // квадрат
        {"XXXX", 4, 1}, // прямая
    }; 
    FILE *high_score_file = NULL;
    GameInfo_t game = init_game(&high_score_file, figures); 
    FILE *file = fopen("test_file.txt", "w");

    delete_game(&game, file);

    ck_assert_int_eq(game.figure.rows, 0);
    ck_assert_int_eq(game.figure.columns, 0);
    ck_assert_int_eq(game.figure.current_x, 0);
    ck_assert_int_eq(game.figure.current_y, 0);
    ck_assert_int_eq(game.figure.number_figure, 0);
    ck_assert_int_eq(game.number_next_figure, 0);
    ck_assert_int_eq(game.score, 0);
    ck_assert_int_eq(game.high_score, 0);
    ck_assert_int_eq(game.level, 0);
    ck_assert_int_eq(game.speed, 0);
    ck_assert_int_eq(game.game_over, 0);


    fclose(file);
}
END_TEST

START_TEST(test_check_left_position)
{
    char **test_field = (char **)malloc(3 * sizeof(char *));
    for (int i = 0; i < 3; i++) {
        test_field[i] = (char *)malloc(3 * sizeof(char));
        memset(test_field[i], '.', 3);
    }

    test_field[1][0] = 'X';

    matrix_figure test_shape = {
        .rows = 1,
        .columns = 1,
        .current_x = 1,
        .current_y = 1
    };
    test_shape.work_figure = (char **)malloc(test_shape.rows * sizeof(char *));
    test_shape.work_figure[0] = (char *)malloc(test_shape.columns * sizeof(char));
    test_shape.work_figure[0][0] = 'X';

    int result = check_left_position(test_shape, test_field);

    ck_assert_int_eq(result, 0); 

    for (int i = 0; i < 3; i++) {
        free(test_field[i]);
    }
    free(test_field);
    free(test_shape.work_figure[0]);
    free(test_shape.work_figure);
}
END_TEST

START_TEST(test_check_right_position)
{
    char **test_field = (char **)malloc(3 * sizeof(char *));
    for (int i = 0; i < 3; i++) {
        test_field[i] = (char *)malloc(3 * sizeof(char));
        memset(test_field[i], '.', 3);
    }

    test_field[1][2] = 'X'; 

    matrix_figure test_shape = {
        .rows = 1,
        .columns = 1,
        .current_x = 1,
        .current_y = 1
    };
    test_shape.work_figure = (char **)malloc(test_shape.rows * sizeof(char *));
    test_shape.work_figure[0] = (char *)malloc(test_shape.columns * sizeof(char));
    test_shape.work_figure[0][0] = 'X';

    int result = check_right_position(test_shape, test_field);

    ck_assert_int_eq(result, 0); 

    for (int i = 0; i < 3; i++) {
        free(test_field[i]);
    }
    free(test_field);
    free(test_shape.work_figure[0]);
    free(test_shape.work_figure);
}
END_TEST

START_TEST(test_check_down_position)
{
    char **test_field = (char **)malloc(3 * sizeof(char *));
    for (int i = 0; i < 3; i++) {
        test_field[i] = (char *)malloc(3 * sizeof(char));
        memset(test_field[i], '.', 3);
    }

    test_field[2][1] = 'X'; // Устанавливаем препятствие под текущей позицией фигуры

    matrix_figure test_shape = {
        .rows = 1,
        .columns = 1,
        .current_x = 1,
        .current_y = 1
    };
    test_shape.work_figure = (char **)malloc(test_shape.rows * sizeof(char *));
    test_shape.work_figure[0] = (char *)malloc(test_shape.columns * sizeof(char));
    test_shape.work_figure[0][0] = 'X';

    int result = check_down_position(test_shape, test_field);

    ck_assert_int_eq(result, 0); 

    for (int i = 0; i < 3; i++) {
        free(test_field[i]);
    }
    free(test_field);
    free(test_shape.work_figure[0]);
    free(test_shape.work_figure);
}
END_TEST

START_TEST(test_part_of_ship)
{
    char **test_field = (char **)malloc(5 * sizeof(char *));
    for (int i = 0; i < 5; i++) {
        test_field[i] = (char *)malloc(5 * sizeof(char));
        memset(test_field[i], ' ', 5);
    }

    matrix_figure test_shape = {
        .rows = 2,
        .columns = 2,
        .current_x = 1,
        .current_y = 1
    };
    test_shape.work_figure = (char **)malloc(test_shape.rows * sizeof(char *));
    test_shape.work_figure[0] = (char *)malloc(test_shape.columns * sizeof(char));
    test_shape.work_figure[1] = (char *)malloc(test_shape.columns * sizeof(char));
    test_shape.work_figure[0][0] = 'X';
    test_shape.work_figure[0][1] = 'X';
    test_shape.work_figure[1][0] = 'X';
    test_shape.work_figure[1][1] = 'X';

    part_of_ship(test_shape, test_field);

    ck_assert_int_eq(test_field[1][2], ' '); 
    ck_assert_int_eq(test_field[2][2], ' ');
    ck_assert_int_eq(test_field[1][3], ' ');
    ck_assert_int_eq(test_field[2][3], ' ');

    for (int i = 0; i < 5; i++) {
        free(test_field[i]);
    }
    free(test_field);
    free(test_shape.work_figure[0]);
    free(test_shape.work_figure[1]);
    free(test_shape.work_figure);
}
END_TEST

START_TEST(test_rotate_figure)
{
    matrix_figure test_shape = {
        .rows = 2,
        .columns = 3,
        .current_x = 1,
        .current_y = 1
    };
    test_shape.work_figure = (char **)malloc(test_shape.rows * sizeof(char *));
    test_shape.work_figure[0] = (char *)malloc(test_shape.columns * sizeof(char));
    test_shape.work_figure[1] = (char *)malloc(test_shape.columns * sizeof(char));
    test_shape.work_figure[0][0] = 'X';
    test_shape.work_figure[0][1] = ' ';
    test_shape.work_figure[0][2] = 'X';
    test_shape.work_figure[1][0] = ' ';
    test_shape.work_figure[1][1] = 'X';
    test_shape.work_figure[1][2] = ' ';

    matrix_figure rotated_shape = rotate_figure(test_shape);

    ck_assert_int_eq(rotated_shape.rows, 3); // Проверяем, что количество строк и столбцов поменялись местами
    ck_assert_int_eq(rotated_shape.columns, 2);
    
    ck_assert_int_eq(rotated_shape.work_figure[0][0], ' '); // Проверяем, что фигура была правильно повернута
    ck_assert_int_eq(rotated_shape.work_figure[0][1], 'X');
    ck_assert_int_eq(rotated_shape.work_figure[1][0], 'X');
    ck_assert_int_eq(rotated_shape.work_figure[1][1], ' ');
    ck_assert_int_eq(rotated_shape.work_figure[2][0], ' ');
    ck_assert_int_eq(rotated_shape.work_figure[2][1], 'X');

    free(test_shape.work_figure[0]);
    free(test_shape.work_figure[1]);
    free(test_shape.work_figure);
    free(rotated_shape.work_figure[0]);
    free(rotated_shape.work_figure[1]);
    free(rotated_shape.work_figure);
}
END_TEST

START_TEST(test_check_full_row)
{
    char **test_field = (char **)malloc(FIELD_H * sizeof(char *));
    for (int i = 0; i < FIELD_H; i++) {
        test_field[i] = (char *)malloc(FIELD_W * sizeof(char));
        for (int j = 0; j < FIELD_W; j++) {
            test_field[i][j] = '.';
        }
    }

    // Создаем полную строку
    for (int j = 0; j < FIELD_W; j++) {
        test_field[1][j] = 'X';
    }

    int removed_rows = check_full_row(test_field);

    ck_assert_int_eq(removed_rows, 1); // Проверяем, что одна полная строка была удалена

    // Проверяем, что строка была удалена
    for (int j = 0; j < FIELD_W; j++) {
        ck_assert_int_eq(test_field[1][j], '.');
    }

    for (int i = 0; i < FIELD_H; i++) {
        free(test_field[i]);
    }
    free(test_field);
}
END_TEST

START_TEST(test_remove_shift_row)
{
    char **test_field = (char **)malloc(FIELD_H * sizeof(char *));
    for (int i = 0; i < FIELD_H; i++) {
        test_field[i] = (char *)malloc(FIELD_W * sizeof(char));
        for (int j = 0; j < FIELD_W; j++) {
            test_field[i][j] = '.';
        }
    }
    test_field[0][0] = 'X';
    test_field[1][0] = 'X';
    test_field[2][0] = 'X';

    remove_shift_row(1, test_field);

    ck_assert_int_eq(test_field[0][0], 'X');
    ck_assert_int_eq(test_field[1][0], 'X');
    ck_assert_int_eq(test_field[2][0], 'X');

    for (int i = 0; i < FIELD_H; i++) {
        free(test_field[i]);
    }
    free(test_field);
}
END_TEST

START_TEST(test_count_score)
{
    int rows = 3;
    int expected_score = pow(2, rows) * 100 - 100;

    int result = count_score(rows);

    ck_assert_int_eq(result, expected_score);
}
END_TEST

START_TEST(test_figure_down)
{ 
    const sketch_figure figures[7] = {
        {"0XXXX0", 3, 2}, // S-образная
        {"XX00XX", 3, 2}, // Z-образная
        {"0X0XXX", 3, 2}, // T-образная
        {"00XXXX", 3, 2}, // L-образная
        {"X00XXX", 3, 2}, // зеркальная L
        {"XXXX", 2, 2}, // квадрат
        {"XXXX", 4, 1}, // прямая
    };
    FILE *high_score_file = NULL;
    GameInfo_t game = init_game(&high_score_file, figures); 

    int initial_score = game.score;
    int initial_high_score = game.high_score;

    figure_down(&game, figures);

    ck_assert_int_eq(game.score, initial_score + count_score(check_full_row(game.field)));

    if (game.score >= initial_high_score) {
        ck_assert_int_eq(game.high_score, game.score);
    } else {
        ck_assert_int_eq(game.high_score, initial_high_score);
    }

    if (game.level < 10) {
        ck_assert_int_eq(game.level, game.score / 600);
        ck_assert_int_eq(game.speed, 550000 - 50000 * game.level);
    }
}
END_TEST

START_TEST(test_check_rotate_position)
{
    matrix_figure shape;
    char **field;

    shape.rows = 2;
    shape.columns = 3;
    shape.current_x = 0;
    shape.current_y = 0;
    shape.number_figure = 1;

    field = malloc(20 * sizeof(char *));
    for (int i = 0; i < 20; i++) {
        field[i] = malloc(10 * sizeof(char));
        for (int j = 0; j < 10; j++) {
            field[i][j] = ' ';
        }
    }

    shape.work_figure = malloc(shape.rows * sizeof(char *));
    for (int i = 0; i < shape.rows; i++) {
        shape.work_figure[i] = malloc(shape.columns * sizeof(char));
        for (int j = 0; j < shape.columns; j++) {
            shape.work_figure[i][j] = 'X';
        }
    }

    int initial_answer_code = 0;

    int result = check_rotate_position(shape, field);

    ck_assert_int_eq(result, initial_answer_code);

    // Очистка памяти
    for (int i = 0; i < shape.rows; i++) {
        free(shape.work_figure[i]);
    }
    free(shape.work_figure);

    for (int i = 0; i < 20; i++) {
        free(field[i]);
    }
    free(field);
}
END_TEST

START_TEST(test_check_create_position)
{
    char **field;

    field = malloc(20 * sizeof(char *));
    for (int i = 0; i < 20; i++) {
        field[i] = malloc(10 * sizeof(char));
        for (int j = 0; j < 10; j++) {
            field[i][j] = '.';
        }
    }

    for (int i = 0; i < 10; i++) {
        field[0][i] = 'X';
    }

    int initial_answer_code = 1;

    int result = check_сreate_position(field);

    ck_assert_int_eq(result, initial_answer_code);

    for (int i = 0; i < 20; i++) {
        free(field[i]);
    }
    free(field);
}
END_TEST


Suite *tetris_suite(void) {
    Suite *suite;
    suite = suite_create("Functions checking");
    TCase *tc = tcase_create("s21_tetris_tc");

    tcase_add_test(tc, test_get_random_shape);
    tcase_add_test(tc, test_userInput_up);
    tcase_add_test(tc, test_userInput_down);
    tcase_add_test(tc, test_userInput_left);
    tcase_add_test(tc, test_userInput_right);
    tcase_add_test(tc, test_userInput_space);
    tcase_add_test(tc, test_userInput_start);
    tcase_add_test(tc, test_userInput_start_1);
    tcase_add_test(tc, test_userInput_exit);
    tcase_add_test(tc, test_userInput_exit_1);
    tcase_add_test(tc, test_create_matrix_allocation);
    tcase_add_test(tc, test_create_matrix_returns_null);
    tcase_add_test(tc, test_create_figure);
    tcase_add_test(tc, test_init_game);
    tcase_add_test(tc, test_delete_game);
    tcase_add_test(tc, test_check_left_position);
    tcase_add_test(tc, test_check_right_position);
    tcase_add_test(tc, test_check_down_position);
    tcase_add_test(tc, test_check_rotate_position);
    tcase_add_test(tc, test_part_of_ship);
    tcase_add_test(tc, test_rotate_figure);
    tcase_add_test(tc, test_check_full_row);
    tcase_add_test(tc, test_remove_shift_row);
    tcase_add_test(tc, test_count_score);
    tcase_add_test(tc, test_figure_down);
    tcase_add_test(tc, test_check_create_position);


    suite_add_tcase(suite, tc);

    return suite;

}

void run_testcase(Suite *testcase) {
  static int counter_testcase = 1;

  if (counter_testcase > 0) putchar('\n');
  counter_testcase++;
  SRunner *sr = srunner_create(testcase);

  srunner_set_fork_status(sr, CK_NOFORK);
  srunner_run_all(sr, CK_NORMAL);

  srunner_free(sr);
}

int main(void) {
  run_testcase(tetris_suite());
  return 0;
}

