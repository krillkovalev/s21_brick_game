#ifndef S21_BRICK_GAME_H
#define S21_BRICK_GAME_H

#include <stdio.h>
#include <check.h>
#include "../fsm.h"
#include <limits.h>

Suite *tetris_suite(void);
void run_testcase(Suite *testcase);

#endif

