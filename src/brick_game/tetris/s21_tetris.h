#ifndef S21_TETRIS
#define S21_TETRIS

#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <stdbool.h>

typedef enum {
    Start = 0,
    Pause,
    Terminate,
    Left,
    Right,
    Up,
    Down,
    Action
} UserAction_t;

typedef struct {
    int **field;
    int **next;
    int score;
    int high_score;
    int level;
    int speed;
    int pause;
} GameInfo_t;

typedef struct {
    int weight;
    int height;
} field_settings;


void userInput(UserAction_t action, bool hold);
GameInfo_t updateCurrentState();

#endif