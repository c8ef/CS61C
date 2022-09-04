#include "snake_utils.h"
#include "state.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
  char *in_filename = NULL;
  char *out_filename = NULL;
  game_state_t *state = NULL;

  // Parse arguments
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-i") == 0 && i < argc - 1) {
      in_filename = argv[i + 1];
      i++;
      continue;
    }
    if (strcmp(argv[i], "-o") == 0 && i < argc - 1) {
      out_filename = argv[i + 1];
      i++;
      continue;
    }
    fprintf(stderr, "Usage: %s [-i filename] [-o filename]\n", argv[0]);
    return 1;
  }

  // Do not modify anything above this line.

  /* Task 7 */

  // Read board from file, or create default board
  if (in_filename != NULL) {
    state = load_board(in_filename);
    if (!state)
      return -1;
    initialize_snakes(state);
  } else {
    state = create_default_state();
  }

  // (already implemented in state_utils.h) to add food.
  update_state(state, deterministic_food);
  // Write updated board to file or stdout
  if (out_filename != NULL) {
    FILE *fp = fopen(out_filename, "w");
    print_board(state, fp);
    fclose(fp);
  } else {
    print_board(state, stdout);
  }

  free_state(state);
  return 0;
}
