#include "s21_tetris.h"
#include "objects.h"

#ifndef FSM_H
#define FSM_H

typedef enum
{
    START = 0,
    SPAWN,
    MOVING,
    SHIFTING,
    ATTACHING,
    GAMEOVER,
    EXIT_STATE
} game_states;

typedef enum
{
    MOVE_UP = 0,
    MOVE_DOWN,
    MOVE_RIGHT,
    MOVE_LEFT,
    FIG_FLIP,
    NOSIG
} signals;

#endif