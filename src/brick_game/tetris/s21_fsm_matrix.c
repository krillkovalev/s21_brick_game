#include "fsm.h"

typedef struct game_params {
  GameInfo_t *stats;
  game_states *state;
  board_t *map;
  figure_pos *frog_pos;
  bool *break_flag;
} params_t;

// void userInput(UserAction_t action, bool hold) {
//     if (action == START)
// }

typedef void (*action)(params_t *prms);
