#include "state.h"
#include "snake_utils.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Helper function definitions */
static void set_board_at(game_state_t *state, unsigned int x, unsigned int y,
                         char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_x(unsigned int cur_x, char c);
static unsigned int get_next_y(unsigned int cur_y, char c);
static void find_head(game_state_t *state, unsigned int snum);
static char next_square(game_state_t *state, unsigned int snum);
static void update_tail(game_state_t *state, unsigned int snum);
static void update_head(game_state_t *state, unsigned int snum);

/* Task 1 */
game_state_t *create_default_state() {
  game_state_t *ans = (game_state_t *)malloc(sizeof(game_state_t));

  ans->num_rows = 18;
  ans->num_snakes = 1;
  ans->board = (char **)calloc(ans->num_rows, sizeof(char *));

  ans->board[0] = strdup("####################");
  ans->board[1] = strdup("#                  #");
  ans->board[2] = strdup("# d>D    *         #");
  for (int i = 3; i <= 16; ++i)
    ans->board[i] = strdup("#                  #");
  ans->board[17] = strdup("####################");

  ans->snakes = (snake_t *)malloc(sizeof(snake_t));
  ans->snakes[0].live = true;
  ans->snakes[0].head_x = 4;
  ans->snakes[0].head_y = 2;
  ans->snakes[0].tail_x = 2;
  ans->snakes[0].tail_y = 2;

  return ans;
}

/* Task 2 */
void free_state(game_state_t *state) {
  for (unsigned i = 0; i < state->num_rows; ++i) {
    free(state->board[i]);
  }
  free(state->board);
  free(state->snakes);
  free(state);
  return;
}

/* Task 3 */
void print_board(game_state_t *state, FILE *fp) {
  for (unsigned i = 0; i < state->num_rows; ++i) {
    fprintf(fp, state->board[i]);
    fprintf(fp, "\n");
  }
  return;
}

/*
  Saves the current state into filename. Does not modify the state object.
  (already implemented for you).
*/
void save_board(game_state_t *state, char *filename) {
  FILE *f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_state_t *state, unsigned int x, unsigned int y) {
  return state->board[y][x];
}

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_state_t *state, unsigned int x, unsigned int y,
                         char ch) {
  state->board[y][x] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  return c == 'w' || c == 'a' || c == 's' || c == 'd';
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  return c == 'W' || c == 'A' || c == 'S' || c == 'D' || c == 'x';
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<>vWASDx"
*/
static bool is_snake(char c) {
  return is_tail(c) || is_head(c) || c == 'v' || c == '>' || c == '<' ||
         c == '^';
}

/*
  Converts a character in the snake's body ("^<>v")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
  switch (c) {
  case '^':
    return 'w';
  case '>':
    return 'd';
  case '<':
    return 'a';
  case 'v':
    return 's';
  }
  __builtin_unreachable();
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<>v").
*/
static char head_to_body(char c) {
  switch (c) {
  case 'W':
    return '^';
  case 'A':
    return '<';
  case 'S':
    return 'v';
  case 'D':
    return '>';
  }
  __builtin_unreachable();
}

/*
  Returns cur_x + 1 if c is '>' or 'd' or 'D'.
  Returns cur_x - 1 if c is '<' or 'a' or 'A'.
  Returns cur_x otherwise.
*/
static unsigned int get_next_x(unsigned int cur_x, char c) {
  if (c == '>' || c == 'd' || c == 'D')
    return cur_x + 1;
  if (c == '<' || c == 'a' || c == 'A')
    return cur_x - 1;
  return cur_x;
}

/*
  Returns cur_y + 1 if c is '^' or 'w' or 'W'.
  Returns cur_y - 1 if c is 'v' or 's' or 'S'.
  Returns cur_y otherwise.
*/
static unsigned int get_next_y(unsigned int cur_y, char c) {
  if (c == '^' || c == 'w' || c == 'W')
    return cur_y - 1;
  if (c == 'v' || c == 's' || c == 'S')
    return cur_y + 1;
  return cur_y;
}

/*
  Task 4.2

  Helper function for update_state. Return the character in the cell the snake
  is moving into.

  This function should not modify anything.
*/
static char next_square(game_state_t *state, unsigned int snum) {
  char now_char = get_board_at(state, state->snakes[snum].head_x,
                               state->snakes[snum].head_y);
  unsigned next_x = get_next_x(state->snakes[snum].head_x, now_char);
  unsigned next_y = get_next_y(state->snakes[snum].head_y, now_char);
  return get_board_at(state, next_x, next_y);
}

/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the x and y coordinates of the head

  Note that this function ignores food, walls, and snake bodies when moving the
  head.
*/
static void update_head(game_state_t *state, unsigned int snum) {
  unsigned now_x = state->snakes[snum].head_x;
  unsigned now_y = state->snakes[snum].head_y;

  char now_char = get_board_at(state, state->snakes[snum].head_x,
                               state->snakes[snum].head_y);
  unsigned next_x = get_next_x(state->snakes[snum].head_x, now_char);
  unsigned next_y = get_next_y(state->snakes[snum].head_y, now_char);
  set_board_at(state, next_x, next_y, now_char);
  set_board_at(state, now_x, now_y, head_to_body(now_char));

  state->snakes[snum].head_x = next_x;
  state->snakes[snum].head_y = next_y;
}

/*
  Task 4.4

  Helper function for update_state. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^v<>) into a tail character (wasd)

  ...in the snake struct: update the x and y coordinates of the tail
*/
static void update_tail(game_state_t *state, unsigned int snum) {
  unsigned now_x = state->snakes[snum].tail_x;
  unsigned now_y = state->snakes[snum].tail_y;

  char now_char = get_board_at(state, state->snakes[snum].tail_x,
                               state->snakes[snum].tail_y);
  unsigned next_x = get_next_x(state->snakes[snum].tail_x, now_char);
  unsigned next_y = get_next_y(state->snakes[snum].tail_y, now_char);

  set_board_at(state, next_x, next_y,
               body_to_tail(get_board_at(state, next_x, next_y)));
  set_board_at(state, now_x, now_y, ' ');
  state->snakes[snum].tail_x = next_x;
  state->snakes[snum].tail_y = next_y;
}

/* Task 4.5 */
void update_state(game_state_t *state, int (*add_food)(game_state_t *state)) {
  for (unsigned snum = 0; snum < state->num_snakes; ++snum) {
    unsigned now_x = state->snakes[snum].head_x;
    unsigned now_y = state->snakes[snum].head_y;
    char now_char = get_board_at(state, state->snakes[snum].head_x,
                                 state->snakes[snum].head_y);
    unsigned next_x = get_next_x(state->snakes[snum].head_x, now_char);
    unsigned next_y = get_next_y(state->snakes[snum].head_y, now_char);
    char next_char = get_board_at(state, next_x, next_y);
    if (next_char == '#' || is_snake(next_char)) {
      state->snakes[snum].live = false;
      set_board_at(state, now_x, now_y, 'x');
    } else if (next_char == '*') {
      update_head(state, snum);
      add_food(state);
    } else {
      update_head(state, snum);
      update_tail(state, snum);
    }
  }
}

/* Task 5 */
game_state_t *load_board(char *filename) {
  game_state_t *ans = (game_state_t *)malloc(sizeof(game_state_t));
  FILE *fp = fopen(filename, "r");
  if (fp == NULL)
    return NULL;
  unsigned row = 0, max_len = 0, now_len = 0;

  while (!feof(fp)) {
    int ch = fgetc(fp);
    if (ch == '\n') {
      ++row;
      max_len = (now_len > max_len ? now_len : max_len);
      now_len = 0;
    } else
      ++now_len;
  }
  rewind(fp);
  ans->num_rows = row;
  ans->board = (char **)calloc(ans->num_rows, sizeof(char *));
  char *buf = (char *)calloc(1, (max_len + 1));
  unsigned pos = 0, row_pos = 0;
  while (!feof(fp)) {
    int ch = fgetc(fp);
    if (ch != '\n')
      buf[pos++] = (char)ch;
    else {
      ans->board[row_pos] = (char *)calloc(1, pos + 1);
      strcpy(ans->board[row_pos++], buf);
      memset(buf, 0, max_len + 1);
      pos = 0;
    }
  }
  return ans;
}

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail coordinates filled in,
  trace through the board to find the head coordinates, and
  fill in the head coordinates in the struct.
*/
static void find_head(game_state_t *state, unsigned int snum) {
  unsigned now_x = state->snakes[snum].tail_x;
  unsigned now_y = state->snakes[snum].tail_y;
  char now_char = get_board_at(state, state->snakes[snum].tail_x,
                               state->snakes[snum].tail_y);

  while (!is_head(now_char)) {
    now_x = get_next_x(now_x, now_char);
    now_y = get_next_y(now_y, now_char);
    now_char = get_board_at(state, now_x, now_y);
  }

  state->snakes[snum].head_x = now_x;
  state->snakes[snum].head_y = now_y;
}

/* Task 6.2 */
game_state_t *initialize_snakes(game_state_t *state) {
  state->num_snakes = 0;
  for (unsigned i = 0; i < state->num_rows; ++i) {
    for (unsigned j = 0; state->board[i][j]; ++j) {
      if (is_tail(state->board[i][j])) {
        ++state->num_snakes;
      }
    }
  }
  state->snakes = (snake_t *)calloc(state->num_snakes, sizeof(snake_t));

  unsigned snum = 0;
  for (unsigned i = 0; i < state->num_rows; ++i) {
    for (unsigned j = 0; state->board[i][j]; ++j) {
      if (is_tail(state->board[i][j])) {
        state->snakes[snum].tail_x = j;
        state->snakes[snum].tail_y = i;
        state->snakes[snum].live = true;
        find_head(state, snum);
        ++snum;
      }
    }
  }
  return state;
}
