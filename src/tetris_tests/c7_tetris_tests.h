#ifndef C7_TETRIS_TESTS_H
#define C7_TETRIS_TESTS_H

#include <check.h>
#include <limits.h>
#include <stdio.h>

#include "../brick_game/tetris/c7_tetris.h"

Suite *tetris_suite(void);
void run_testcase(Suite *testcase);

#endif
