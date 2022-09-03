#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "asserts.h"
// Necessary due to static functions in state.c
#include "state.c"

char* COLOR_GREEN = "";
char* COLOR_RESET = "";

bool clear_unit_test_files() {
  int err_in = remove("unit-test-in.snk");
  int err_out = remove("unit-test-out.snk");
  int err_ref = remove("unit-test-ref.snk");
  return err_in == 0 && err_out == 0 && err_ref == 0;
}

bool test_create_default_state() {
  game_state_t* state = create_default_state();

  if (state == NULL) {
    printf("%s\n", "create_default_state is not implemented, skipping...");
    return false;
  }

  if (!assert_equals_unsigned_int("board width", DEFAULT_BOARD_WIDTH, 20)) {
    return false;
  }

  if (!assert_equals_unsigned_int("board height", DEFAULT_BOARD_HEIGHT, state->num_rows)) {
    return false;
  }

  for (unsigned int x = 0; x < DEFAULT_BOARD_WIDTH; x++) {
    for (unsigned int y = 0; y < DEFAULT_BOARD_HEIGHT; y++) {
      if (x == 0 || y == 0 || x == DEFAULT_BOARD_WIDTH - 1 || y == DEFAULT_BOARD_HEIGHT - 1) {
        if (!assert_map_equals(state, x, y, '#')) {
          return false;
        }
      } else if (x == 9 && y == 2) {
        if (!assert_map_equals(state, x, y, '*')) {
          return false;
        }
      } else if (x == 2 && y == 2) {
        if (!assert_map_equals(state, x, y, 'd')) {
          return false;
        }
      } else if (x == 3 && y == 2) {
        if (!assert_map_equals(state, x, y, '>')) {
          return false;
        }
      } else if (x == 4 && y == 2) {
        if (!assert_map_equals(state, x, y, 'D')) {
          return false;
        }
      } else {
        if (!assert_map_equals(state, x, y, ' ')) {
          return false;
        }
      }
    }
  }

  if (!assert_equals_unsigned_int("number of snakes", 1, state->num_snakes)) {
    return false;
  }

  if (!assert_equals_unsigned_int("x-coordinate of snake tail", 2, state->snakes->tail_x)) {
    return false;
  }
  if (!assert_equals_unsigned_int("y-coordinate of snake tail", 2, state->snakes->tail_y)) {
    return false;
  }
  if (!assert_equals_unsigned_int("x-coordinate of snake head", 4, state->snakes->head_x)) {
    return false;
  }
  if (!assert_equals_unsigned_int("y-coordinate of snake head", 2, state->snakes->head_y)) {
    return false;
  }
  if (!assert_true("snake is alive", state->snakes->live)) {
    return false;
  }

  return true;
}


bool test_free_state() {
  game_state_t* state = create_default_state();
  free_state(state);

  printf("%s\n", "Make sure that no Valgrind errors are printed!");

  return true;
}


bool test_print_board() {
  clear_unit_test_files();

  char* expected =
    "####################\n"
    "#                  #\n"
    "# d>D    *         #\n"
    "#                  #\n"
    "#                  #\n"
    "#                  #\n"
    "#                  #\n"
    "#                  #\n"
    "#                  #\n"
    "#                  #\n"
    "#                  #\n"
    "#                  #\n"
    "#                  #\n"
    "#                  #\n"
    "#                  #\n"
    "#                  #\n"
    "#                  #\n"
    "####################\n";

  game_state_t* state = create_default_state();
  save_board(state, "unit-test-out.snk");

  // We hardcoded a filesize here since we know the default board is 378 characters.
  // You shouldn't hardcode sizes in your own implementation.
  char actual[378 + 1];

  FILE* f = fopen("unit-test-out.snk", "r");
  fread(actual, 378, 1, f);
  fclose(f);
  actual[378] = '\0';

  // check that file ends in a newline
  if (actual[strlen(actual) - 1] != '\n') {
    printf("%s\n", "Assertion error: Make sure your printed board ends in a newline.");
    return false;
  }

  if (strcmp(expected, actual) != 0) {
    printf("%s\n", "Your printed board doesn't match the expected output. See unit-test-out.snk for what you printed.");
    return false;
  }

  return true;
}

bool test_next_square_board_1() {
  clear_unit_test_files();

  /*
  Board 1 (default):
  ####################
  #                  #
  # d>D    *         #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  ####################
  */

  // set up board
  game_state_t* actual_state = create_default_state();
  save_board(actual_state, "unit-test-out.snk");

  // the next square for the snake should be ' '
  if (!assert_equals_char("next_square on board 1", ' ', next_square(actual_state, 0))) {
    return false;
  }

  // check that board has not changed
  game_state_t* expected_state = create_default_state();
  if (!assert_state_equals(expected_state, actual_state)) {
    printf("%s\n", "Error: next_square should not modify board");
    return false;
  }
  return true;
}


bool test_next_square_board_2() {
  clear_unit_test_files();

  /*
  Board 2:
  ####################
  #                  #
  # d>D*   *         #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  ####################
  */

  game_state_t* state = create_default_state();

  // set up board
  set_board_at(state, 5, 2, '*');
  save_board(state, "unit-test-out.snk");

  // the next square for the snake should be '*'
  return assert_equals_char("next_square on board 2", '*', next_square(state, 0));
}


bool test_next_square_board_3() {
  clear_unit_test_files();

  /*
  Board 3:
  ####################
  #                  #
  # d>Dx   *         #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  ####################
  */

  game_state_t* state = create_default_state();

  // set up board
  set_board_at(state, 5, 2, 'x');
  save_board(state, "unit-test-out.snk");

  // the next square for the snake should be 'x'
  return assert_equals_char("next_square on board 3", 'x', next_square(state, 0));
}

bool test_next_square_board_4() {
  clear_unit_test_files();

  /*
  Board 4:
  ####################
  #   #              #
  # d>W    *         #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  ####################
  */

  game_state_t* state = create_default_state();

  // set up board
  set_board_at(state, 4, 2, 'W');
  set_board_at(state, 4, 1, '#');
  state->snakes->head_x = 4;
  state->snakes->head_y = 2;
  save_board(state, "unit-test-out.snk");

  // the next square for the snake should be '#'
  return assert_equals_char("next_square on board 4", '#', next_square(state, 0));
}

bool test_next_square_board_5() {
  clear_unit_test_files();

  /*
  Board 5:
  ####################
  #                  #
  # d>v    *         #
  #   v              #
  #   S              #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  ####################
  */

  game_state_t* state = create_default_state();

  // set up board
  set_board_at(state, 4, 2, 'v');
  set_board_at(state, 4, 3, 'v');
  set_board_at(state, 4, 4, 'S');
  state->snakes->head_x = 4;
  state->snakes->head_y = 4;
  save_board(state, "unit-test-out.snk");

  // the next square for the snake should be ' '
  return assert_equals_char("next_square on board 5", ' ', next_square(state, 0));
}

bool test_next_square_board_6() {
  clear_unit_test_files();

  /*
  Board 6:
  ####################
  #                  #
  # d>v    *         #
  #   v              #
  #   S              #
  #   #              #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  ####################
  */

  game_state_t* state = create_default_state();

  // set up board
  set_board_at(state, 4, 2, 'v');
  set_board_at(state, 4, 3, 'v');
  set_board_at(state, 4, 4, 'S');
  set_board_at(state, 4, 5, '#');
  state->snakes->head_x = 4;
  state->snakes->head_y = 4;
  save_board(state, "unit-test-out.snk");

  // the next square for the snake should be '#'
  return assert_equals_char("next_square on board 6", '#', next_square(state, 0));
}

bool test_next_square() {
  if (!test_next_square_board_1()) {
    printf("%s\n", "test_next_square_board_1 failed. Check unit-test-out.snk for a diagram of the board.");
    return false;
  }

  if (!test_next_square_board_2()) {
    printf("%s\n", "test_next_square_board_2 failed. Check unit-test-out.snk for a diagram of the board.");
    return false;
  }

  if (!test_next_square_board_3()) {
    printf("%s\n", "test_next_square_board_3 failed. Check unit-test-out.snk for a diagram of the board.");
    return false;
  }

  if (!test_next_square_board_4()) {
    printf("%s\n", "test_next_square_board_4 failed. Check unit-test-out.snk for a diagram of the board.");
    return false;
  }

  if (!test_next_square_board_5()) {
    printf("%s\n", "test_next_square_board_5 failed. Check unit-test-out.snk for a diagram of the board.");
    return false;
  }

  if (!test_next_square_board_6()) {
    printf("%s\n", "test_next_square_board_6 failed. Check unit-test-out.snk for a diagram of the board.");
    return false;
  }

  return true;
}

bool test_update_head_board_1() {
  clear_unit_test_files();

  /*
  Board 1 (default):
  ####################            ####################
  #                  #            #                  #
  # d>D    *         #            # d>>D   *         #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  # ---------> #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  ####################            ####################
  */

  // set up expected board
  game_state_t* expected = create_default_state();
  set_board_at(expected, 4, 2, '>');
  set_board_at(expected, 5, 2, 'D');
  expected->snakes->head_x = 5;
  expected->snakes->head_y = 2;
  save_board(expected, "unit-test-ref.snk");

  // set up actual board
  game_state_t* actual = create_default_state();
  save_board(actual, "unit-test-in.snk");

  update_head(actual, 0);
  save_board(actual, "unit-test-out.snk");

  // check that actual board matches expected board
  return assert_state_equals(expected, actual);
}

bool test_update_head_board_2() {
  clear_unit_test_files();

  /*
  Board 2:
  ####################            ####################
  #                  #            #   W              #
  # d>W    *         #            # d>^    *         #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  # ---------> #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  ####################            ####################
  */

  // set up expected board
  game_state_t* expected = create_default_state();
  set_board_at(expected, 4, 2, '^');
  set_board_at(expected, 4, 1, 'W');
  expected->snakes->head_x = 4;
  expected->snakes->head_y = 1;
  save_board(expected, "unit-test-ref.snk");

  // set up actual board
  game_state_t* actual = create_default_state();
  set_board_at(actual, 4, 2, 'W');
  actual->snakes->head_x = 4;
  actual->snakes->head_y = 2;
  save_board(actual, "unit-test-in.snk");

  update_head(actual, 0);
  save_board(actual, "unit-test-out.snk");

  // check that actual board matches expected board
  return assert_state_equals(expected, actual);
}

bool test_update_head_board_3() {
  clear_unit_test_files();

  /*
  Board 3:
  ####################            ####################
  #   A              #            #  A<              #
  # d>^    *         #            # d>^    *         #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  # ---------> #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  ####################            ####################
  */

  // set up expected board
  game_state_t* expected = create_default_state();
  set_board_at(expected, 4, 2, '^');
  set_board_at(expected, 4, 1, '<');
  set_board_at(expected, 3, 1, 'A');
  expected->snakes->head_x = 3;
  expected->snakes->head_y = 1;
  save_board(expected, "unit-test-ref.snk");

  // set up actual board
  game_state_t* actual = create_default_state();
  set_board_at(actual, 4, 2, '^');
  set_board_at(actual, 4, 1, 'A');
  actual->snakes->head_x = 4;
  actual->snakes->head_y = 1;
  save_board(actual, "unit-test-in.snk");

  update_head(actual, 0);
  save_board(actual, "unit-test-out.snk");

  // check that actual board matches expected board
  return assert_state_equals(expected, actual);
}


bool test_update_head() {
  if (!test_update_head_board_1()) {
    printf("%s\n", "test_update_head_board_1 failed. Check unit-test-in.snk, unit-test-out.snk, and unit-test-ref.snk.");
    return false;
  }

  if (!test_update_head_board_2()) {
    printf("%s\n", "test_update_head_board_2 failed. Check unit-test-in.snk, unit-test-out.snk, and unit-test-ref.snk.");
    return false;
  }

  if (!test_update_head_board_3()) {
    printf("%s\n", "test_update_head_board_3 failed. Check unit-test-in.snk, unit-test-out.snk, and unit-test-ref.snk.");
    return false;
  }

  return true;
}

bool test_update_tail_board_1() {
  clear_unit_test_files();

  /*
  Board 1:
  ####################            ####################
  #                  #            #                  #
  # d>>D   *         #            #  d>D   *         #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  # ---------> #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  ####################            ####################
  */

  // set up expected board
  game_state_t* expected = create_default_state();
  set_board_at(expected, 2, 2, ' ');
  set_board_at(expected, 3, 2, 'd');
  set_board_at(expected, 4, 2, '>');
  set_board_at(expected, 5, 2, 'D');
  expected->snakes->head_x = 5;
  expected->snakes->head_y = 2;
  expected->snakes->tail_x = 3;
  expected->snakes->head_y = 2;
  save_board(expected, "unit-test-ref.snk");

  // set up actual board
  game_state_t* actual = create_default_state();
  set_board_at(actual, 4, 2, '>');
  set_board_at(actual, 5, 2, 'D');
  actual->snakes->head_x = 5;
  actual->snakes->head_y = 2;
  save_board(actual, "unit-test-in.snk");

  update_tail(actual, 0);
  save_board(actual, "unit-test-out.snk");

  // check that actual board matches expected board
  return assert_state_equals(expected, actual);
}

bool test_update_tail_board_2() {
  clear_unit_test_files();

  /*
  Board 2:
  ####################            ####################
  #                  #            #                  #
  # dv     *         #            #   s    *         #
  #  v               #            #   v              #
  #  S               #            #   S              #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  # ---------> #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  ####################            ####################
  */

  // set up expected board
  game_state_t* expected = create_default_state();
  set_board_at(expected, 2, 2, ' ');
  set_board_at(expected, 3, 2, 's');
  set_board_at(expected, 4, 2, ' ');
  set_board_at(expected, 3, 3, 'v');
  set_board_at(expected, 3, 4, 'S');
  expected->snakes->head_x = 3;
  expected->snakes->head_y = 4;
  expected->snakes->tail_x = 3;
  expected->snakes->tail_y = 2;
  save_board(expected, "unit-test-ref.snk");

  // set up actual board
  game_state_t* actual = create_default_state();
  set_board_at(actual, 3, 2, 'v');
  set_board_at(actual, 4, 2, ' ');
  set_board_at(actual, 3, 3, 'v');
  set_board_at(actual, 3, 4, 'S');
  actual->snakes->head_x = 3;
  actual->snakes->head_y = 4;
  save_board(actual, "unit-test-in.snk");

  update_tail(actual, 0);
  save_board(actual, "unit-test-out.snk");

  // check that actual board matches expected board
  return assert_state_equals(expected, actual);
}

bool test_update_tail_board_3() {
  clear_unit_test_files();

  /*
  Board 3:
  ####################            ####################
  #                  #            #                  #
  # S<     *         #            # Sa     *         #
  #  w               #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  # ---------> #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  ####################            ####################
  */

  // set up expected board
  game_state_t* expected = create_default_state();

  save_board(expected, "unit-test-ref.snk");
  set_board_at(expected, 2, 2, 'S');
  set_board_at(expected, 3, 2, 'a');
  set_board_at(expected, 4, 2, ' ');
  expected->snakes->head_x = 2;
  expected->snakes->head_y = 2;
  expected->snakes->tail_x = 3;
  expected->snakes->tail_y = 2;

  // set up actual board
  game_state_t* actual = create_default_state();
  set_board_at(actual, 2, 2, 'S');
  set_board_at(actual, 3, 2, '<');
  set_board_at(actual, 4, 2, ' ');
  set_board_at(actual, 3, 3, 'w');
  actual->snakes->head_x = 2;
  actual->snakes->head_y = 2;
  actual->snakes->tail_x = 3;
  actual->snakes->tail_y = 3;
  save_board(actual, "unit-test-in.snk");

  update_tail(actual, 0);
  save_board(actual, "unit-test-out.snk");

  // check that actual board matches expected board
  return assert_state_equals(expected, actual);
}


bool test_update_tail() {
  if (!test_update_tail_board_1()) {
    printf("%s\n", "test_update_tail_board_1 failed. Check unit-test-in.snk, unit-test-out.snk, and unit-test-ref.snk.");
    return false;
  }

  if (!test_update_tail_board_2()) {
    printf("%s\n", "test_update_tail_board_2 failed. Check unit-test-in.snk, unit-test-out.snk, and unit-test-ref.snk.");
    return false;
  }

  if (!test_update_tail_board_3()) {
    printf("%s\n", "test_update_tail_board_3 failed. Check unit-test-in.snk, unit-test-out.snk, and unit-test-ref.snk.");
    return false;
  }

  return true;
}

bool test_update_state_board_1() {
  clear_unit_test_files();

  /*
  Board 1 (default):
  ####################            ####################
  #                  #            #                  #
  # d>D    *         #            #  d>D   *         #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  # ---------> #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  ####################            ####################
  */

  // set up expected board
  game_state_t* expected = create_default_state();
  set_board_at(expected, 2, 2, ' ');
  set_board_at(expected, 3, 2, 'd');
  set_board_at(expected, 4, 2, '>');
  set_board_at(expected, 5, 2, 'D');
  expected->snakes->head_x = 5;
  expected->snakes->head_y = 2;
  expected->snakes->tail_x = 3;
  expected->snakes->tail_y = 2;
  save_board(expected, "unit-test-ref.snk");

  // set up actual board
  game_state_t* actual = create_default_state();
  save_board(actual, "unit-test-in.snk");

  update_state(actual, corner_food);
  save_board(actual, "unit-test-out.snk");

  // check that actual board matches expected board
  return assert_state_equals(expected, actual);
}

bool test_update_state_board_2() {
  clear_unit_test_files();

  /*
  Board 2:
  ####################            ####################
  #                  #            #*                 #
  # d>D*   *         #            # d>>D   *         #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  # ---------> #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  ####################            ####################
  */

  // set up expected board
  game_state_t* expected = create_default_state();
  set_board_at(expected, 4, 2, '>');
  set_board_at(expected, 5, 2, 'D');
  set_board_at(expected, 1, 1, '*');
  expected->snakes->head_x = 5;
  expected->snakes->head_y = 2;
  save_board(expected, "unit-test-ref.snk");

  // set up actual board
  game_state_t* actual = create_default_state();
  set_board_at(actual, 5, 2, '*');
  save_board(actual, "unit-test-in.snk");

  update_state(actual, corner_food);
  save_board(actual, "unit-test-out.snk");

  // check that actual board matches expected board
  return assert_state_equals(expected, actual);
}

bool test_update_state_board_3() {
  clear_unit_test_files();

  /*
  Board 3:
  ####################            ####################
  # >W               #            # >x               #
  # w      *         #            # w      *         #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  # ---------> #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  ####################            ####################
  */

  // set up expected board
  game_state_t* expected = create_default_state();
  set_board_at(expected, 2, 2, 'w');
  set_board_at(expected, 3, 2, ' ');
  set_board_at(expected, 4, 2, ' ');
  set_board_at(expected, 2, 1, '>');
  set_board_at(expected, 3, 1, 'x');
  expected->snakes->head_x = 3;
  expected->snakes->head_y = 1;
  expected->snakes->live = false;
  save_board(expected, "unit-test-ref.snk");

  // set up actual board
  game_state_t* actual = create_default_state();
  set_board_at(actual, 2, 2, 'w');
  set_board_at(actual, 3, 2, ' ');
  set_board_at(actual, 4, 2, ' ');
  set_board_at(actual, 2, 1, '>');
  set_board_at(actual, 3, 1, 'W');
  actual->snakes->head_x = 3;
  actual->snakes->head_y = 1;
  actual->snakes->tail_x = 2;
  actual->snakes->tail_y = 2;
  save_board(actual, "unit-test-in.snk");

  update_state(actual, 0);
  save_board(actual, "unit-test-out.snk");

  // check that actual board matches expected board
  return assert_state_equals(expected, actual);
}

bool test_update_state_board_4() {
  clear_unit_test_files();

  /*
  Board 4:
  ####################            ####################
  # >v               #            # >v               #
  # wA     *         #            # wx     *         #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  # ---------> #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  #                  #            #                  #
  ####################            ####################
  */

  // set up expected board
  game_state_t* expected = create_default_state();
  set_board_at(expected, 2, 2, 'w');
  set_board_at(expected, 3, 2, 'x');
  set_board_at(expected, 4, 2, ' ');
  set_board_at(expected, 2, 1, '>');
  set_board_at(expected, 3, 1, 'v');
  expected->snakes->head_x = 3;
  expected->snakes->head_y = 2;
  expected->snakes->live = false;
  save_board(expected, "unit-test-ref.snk");

  // set up actual board
  game_state_t* actual = create_default_state();
  set_board_at(actual, 2, 2, 'w');
  set_board_at(actual, 2, 1, '>');
  set_board_at(actual, 3, 1, 'v');
  set_board_at(actual, 3, 2, 'A');
  set_board_at(actual, 4, 2, ' ');
  actual->snakes->head_x = 3;
  actual->snakes->head_y = 2;
  actual->snakes->tail_x = 2;
  actual->snakes->tail_y = 2;
  save_board(actual, "unit-test-in.snk");

  update_state(actual, corner_food);
  save_board(actual, "unit-test-out.snk");

  // check that actual board matches expected board
  return assert_state_equals(expected, actual);
}

bool test_update_state() {
  if (!test_update_state_board_1()) {
    printf("%s\n", "test_update_state_board_1 failed. Check unit-test-in.snk, unit-test-out.snk, and unit-test-ref.snk.");
    return false;
  }

  if (!test_update_state_board_2()) {
    printf("%s\n", "test_update_state_board_2 failed. Check unit-test-in.snk, unit-test-out.snk, and unit-test-ref.snk.");
    return false;
  }

  if (!test_update_state_board_3()) {
    printf("%s\n", "test_update_state_board_3 failed. Check unit-test-in.snk, unit-test-out.snk, and unit-test-ref.snk.");
    return false;
  }

  if (!test_update_state_board_4()) {
    printf("%s\n", "test_update_state_board_4 failed. Check unit-test-in.snk, unit-test-out.snk, and unit-test-ref.snk.");
    return false;
  }

  return true;
}

bool test_load_board_1() {
  clear_unit_test_files();

  /*
  Board 01-simple (default):
  ####################
  #                  #
  # d>D    *         #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  ####################
  */
  char* expected =
    "####################\n"
    "#                  #\n"
    "# d>D    *         #\n"
    "#                  #\n"
    "#                  #\n"
    "#                  #\n"
    "#                  #\n"
    "#                  #\n"
    "#                  #\n"
    "#                  #\n"
    "#                  #\n"
    "#                  #\n"
    "#                  #\n"
    "#                  #\n"
    "#                  #\n"
    "#                  #\n"
    "#                  #\n"
    "####################\n";
  return assert_load_equals("tests/01-simple-in.snk", expected);
}

bool test_load_board_2() {
  clear_unit_test_files();

  /*
  Board 06-small (small):
  #####
  #   #
  # W #
  # ^ #
  # w #
  #####
  */
  char* expected =
    "#####\n"
    "#   #\n"
    "# W #\n"
    "# ^ #\n"
    "# w #\n"
    "#####\n";
  return assert_load_equals("tests/06-small-in.snk", expected);
}

bool test_load_board_3() {
  clear_unit_test_files();

  /*
  Board 13-sus (non square):
  ##############
  #            #
  #  ########  #
  #  #      #  #####
  #  ########      #
  #                #
  #      #         #
  #                #
  #   ######   #####
  #   #    #   #
  #   #    #   #
  #####    #####

  */
  char* expected =
    "##############\n"
    "#            #\n"
    "#  ########  #\n"
    "#  #      #  #####\n"
    "#  ########      #\n"
    "#                #\n"
    "#      #         #\n"
    "#                #\n"
    "#   ######   #####\n"
    "#   #    #   #\n"
    "#   #    #   #\n"
    "#####    #####\n";
  return assert_load_equals("tests/13-sus-in.snk", expected);
}

bool test_load_board() {
  if (!test_load_board_1()) {
    printf("%s\n", "test_load_board_1 failed. Check tests/01-simple-in.snk for a diagram of the board.");
    return false;
  }

  if (!test_load_board_2()) {
    printf("%s\n", "test_load_board_2 failed. Check tests/06-small-in.snk for a diagram of the board.");
    return false;
  }
  if (!test_load_board_3()) {
    printf("%s\n", "test_load_board_3 failed. Check tests/13-sus-in.snk for a diagram of the board.");
    return false;
  }

  return true;
}


bool test_find_head_board_1() {
  clear_unit_test_files();

  /*
  Board 1:
  ####################
  #                  #
  # d>v    *         #
  #   v  W           #
  #   v  ^           #
  #   >>>^           #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  ####################
  */

  // set up expected board
  game_state_t* expected = create_default_state();
  set_board_at(expected, 4, 2, 'v');
  set_board_at(expected, 4, 3, 'v');
  set_board_at(expected, 4, 4, 'v');
  set_board_at(expected, 4, 5, '>');
  set_board_at(expected, 5, 5, '>');
  set_board_at(expected, 6, 5, '>');
  set_board_at(expected, 7, 5, '^');
  set_board_at(expected, 7, 4, '^');
  set_board_at(expected, 7, 3, 'W');
  expected->snakes->head_x = 7;
  expected->snakes->head_y = 3;
  save_board(expected, "unit-test-out.snk");

  // set up actual board
  game_state_t* actual = create_default_state();
  set_board_at(actual, 4, 2, 'v');
  set_board_at(actual, 4, 3, 'v');
  set_board_at(actual, 4, 4, 'v');
  set_board_at(actual, 4, 5, '>');
  set_board_at(actual, 5, 5, '>');
  set_board_at(actual, 6, 5, '>');
  set_board_at(actual, 7, 5, '^');
  set_board_at(actual, 7, 4, '^');
  set_board_at(actual, 7, 3, 'W');

  find_head(actual, 0);

  // check that actual board matches expected board
  return assert_state_equals(expected, actual);
}


bool test_find_head_board_2() {
  clear_unit_test_files();

  /*
  Board 2:
  ####################
  #                  #
  #    s   *         #
  # d>Dv             #
  #    S             #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  ####################
  */

  // set up expected board
  game_state_t* expected = create_default_state();
  set_board_at(expected, 2, 3, 'd');
  set_board_at(expected, 3, 3, '>');
  set_board_at(expected, 4, 3, 'D');
  set_board_at(expected, 5, 2, 's');
  set_board_at(expected, 5, 3, 'v');
  set_board_at(expected, 5, 4, 'S');
  expected->snakes = realloc(expected->snakes, sizeof(snake_t) * 2);
  snake_t* tmp = expected->snakes;
  tmp->tail_x = 5;
  tmp->tail_y = 2;
  tmp->head_x = 5;
  tmp->head_y = 4;
  tmp++;
  tmp->tail_x = 2;
  tmp->tail_y = 3;
  tmp->head_x = 4;
  tmp->head_y = 3;
  tmp->live = true;
  expected->num_snakes = 2;
  save_board(expected, "unit-test-out.snk");

  // set up actual board
  game_state_t* actual = create_default_state();
  set_board_at(actual, 2, 3, 'd');
  set_board_at(actual, 3, 3, '>');
  set_board_at(actual, 4, 3, 'D');
  set_board_at(actual, 5, 2, 's');
  set_board_at(actual, 5, 3, 'v');
  set_board_at(actual, 5, 4, 'S');
  actual->snakes = realloc(actual->snakes, sizeof(snake_t) * 2);
  tmp = actual->snakes;
  tmp->tail_x = 5;
  tmp->tail_y = 2;
  tmp->head_x = 5;
  tmp->head_y = 4;
  tmp++;
  tmp->tail_x = 2;
  tmp->tail_y = 3;
  tmp->live = true;
  actual->num_snakes = 2;

  find_head(actual, 1);

  // check that actual board matches expected board
  return assert_state_equals(expected, actual);
}


bool test_find_head() {
  if (!test_find_head_board_1()) {
    printf("%s\n", "test_find_head_board_1 failed. Check unit-test-out.snk for a diagram of the board.");
    return false;
  }

  if (!test_find_head_board_2()) {
    printf("%s\n", "test_find_head_board_2 failed. Check unit-test-out.snk for a diagram of the board.");
    return false;
  }

  return true;
}

bool test_initialize_snakes_board_1() {
  clear_unit_test_files();

  /*
  Board 1 (default):
  ####################
  #                  #
  # d>D    *         #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  ####################
  */

  // set up expected board
  game_state_t* expected = create_default_state();
  save_board(expected, "unit-test-out.snk");

  // set up actual board
  game_state_t* actual = create_default_state();
  free(actual->snakes);
  actual->num_snakes = 0;

  actual = initialize_snakes(actual);

  if (actual == NULL) {
    printf("%s\n", "initialize_snakes is not implemented, skipping...");
    return false;
  }

  // check that actual board matches expected board
  return assert_state_equals(expected, actual);
}

bool test_initialize_snakes_board_2() {
  clear_unit_test_files();

  /*
  Board 2:
  ####################
  #                  #
  # d>v    *         #
  #   v  W           #
  #   v  ^           #
  #   >>>^           #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  #                  #
  ####################
  */

  // set up expected board
  game_state_t* expected = create_default_state();
  set_board_at(expected, 4, 2, 'v');
  set_board_at(expected, 4, 3, 'v');
  set_board_at(expected, 4, 4, 'v');
  set_board_at(expected, 4, 5, '>');
  set_board_at(expected, 5, 5, '>');
  set_board_at(expected, 6, 5, '>');
  set_board_at(expected, 7, 5, '^');
  set_board_at(expected, 7, 4, '^');
  set_board_at(expected, 7, 3, 'W');
  expected->snakes->head_x = 7;
  expected->snakes->head_y = 3;
  save_board(expected, "unit-test-out.snk");

  // set up actual board
  game_state_t* actual = create_default_state();
  set_board_at(actual, 4, 2, 'v');
  set_board_at(actual, 4, 3, 'v');
  set_board_at(actual, 4, 4, 'v');
  set_board_at(actual, 4, 5, '>');
  set_board_at(actual, 5, 5, '>');
  set_board_at(actual, 6, 5, '>');
  set_board_at(actual, 7, 5, '^');
  set_board_at(actual, 7, 4, '^');
  set_board_at(actual, 7, 3, 'W');
  free(actual->snakes);
  actual->num_snakes = 0;

  actual = initialize_snakes(actual);

  if (actual == NULL) {
    printf("%s\n", "initialize_snakes is not implemented, skipping...");
    return false;
  }

  // check that actual board matches expected board
  return assert_state_equals(expected, actual);
}

bool test_initialize_snakes() {
  if (!test_initialize_snakes_board_1()) {
    printf("%s\n", "test_initialize_snakes_board_1 failed. Check unit-test-out.snk for a diagram of the board.");
    return false;
  }

  if (!test_initialize_snakes_board_2()) {
    printf("%s\n", "test_initialize_snakes_board_2 failed. Check unit-test-out.snk for a diagram of the board.");
    return false;
  }

  return true;
}

void init_colors() {
  if (!isatty(STDOUT_FILENO)) {
    return;
  }

  if (getenv("NO_COLOR") != NULL) {
    return;
  }

  char* term = getenv("TERM");
  if (term == NULL || strstr(term, "xterm") == NULL) {
    return;
  }

  COLOR_GREEN = "\033[0;32m";
  COLOR_RESET = "\033[0m";
}

bool test_and_print(char* label, bool (*run_test)()) {
  printf("\nTesting %s...\n", label);
  bool result = run_test();
  if (result) {
    printf("%sAll %s tests passed!%s\n", COLOR_GREEN, label, COLOR_RESET);
  } else {
    printf("Not all %s tests passed.\n", label);
  }
  return result;
}

/*
  Feel free to comment out tests.
*/
int main(int argc, char* argv[]) {
  bool MEMCHECK_MODE = false;

  // Parse arguments
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-m") == 0) {
      MEMCHECK_MODE = true;
      continue;
    }
    fprintf(stderr, "Usage: %s [-m]\n", argv[0]);
    return 1;
  }

  init_colors();

  printf("%s\n", "Reminder: These tests are not comprehensive, and passing them does not guarantee that your implementation is working.");

  if (MEMCHECK_MODE) {
    printf("\nTesting free_state...\n");
    if (!test_free_state()) {
      return 0;
    }
  } else {
    if (!test_and_print("create_default_state", test_create_default_state)) {
      return 0;
    }
    if (!test_and_print("print_board", test_print_board)) {
      return 0;
    }
    if (!test_and_print("next_square", test_next_square)) {
      return 0;
    }
    if (!test_and_print("update_head", test_update_head)) {
      return 0;
    }
    if (!test_and_print("update_tail", test_update_tail)) {
      return 0;
    }
    if (!test_and_print("update_state", test_update_state)) {
      return 0;
    }
    if (!test_and_print("load_board", test_load_board)) {
      return 0;
    }
    if (!test_and_print("find_head", test_find_head)) {
      return 0;
    }
    if (!test_and_print("initialize_snakes", test_initialize_snakes)) {
      return 0;
    }
  }
}
