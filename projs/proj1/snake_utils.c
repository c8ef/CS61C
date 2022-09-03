#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "snake_utils.h"
#include "state.h"

unsigned int det_rand(unsigned int* state) {
  if (*state == 0) {
    *state = 1;
  }
  if (*state & 1) {
    *state = *state >> 1 ^ 0x80000057;
  } else {
    *state = *state >> 1;
  }
  return* state;
}

unsigned int seed = 1;

unsigned int get_num_cols(game_state_t* state, unsigned int row) {
  unsigned int num_cols = (unsigned int) strlen(state->board[row]);
  while (num_cols > 0 && state->board[row][num_cols - 1] == '\n') {
    num_cols--;
  }
  return num_cols;
}

int deterministic_food(game_state_t* state) {
  unsigned int y = det_rand(&seed) % state->num_rows;
  unsigned int x = det_rand(&seed) % get_num_cols(state, y);

  while (state->board[y][x] != ' ') {
    y = det_rand(&seed) % state->num_rows;
    x = det_rand(&seed) % get_num_cols(state, y);
  }
  state->board[y][x] = '*';

  return 1;
}

int corner_food(game_state_t* state) {
  state->board[1][1] = '*';
  return 1;
}

void redirect_snake(game_state_t* state, char input_direction) {
  unsigned int x;
  unsigned int y;
  x = state->snakes->head_x;
  y = state->snakes->head_y;

  if (!state->snakes->live) {
    return;
  }

  if (input_direction == 'w') {
    state->board[y][x] = 'W';
  } else if (input_direction == 'a') {
    state->board[y][x] = 'A';
  } else if (input_direction == 's') {
    state->board[y][x] = 'S';
  } else if (input_direction == 'd') {
    state->board[y][x] = 'D';
  }
}

unsigned int snake_seed = 1;

void random_turn(game_state_t* state, int snum) {
  snake_t* snake = &(state->snakes[snum]);
  char cur_head = state->board[snake->head_y][snake->head_x];
  char* heads = "<v>^";
  int i;
  for (i = 0; i < 4; ++i) {
    if (heads[i] == cur_head) break;
  }
  if (det_rand(&snake_seed) % 2 == 0) {
    i += 1;
  } else {
    i -= 1;
  }
  i = i % 4;

  state->board[snake->head_y][snake->head_x] = heads[i];
}
