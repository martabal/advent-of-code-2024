#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OBJECT '#'
#define MAX_GRID_SIZE 1000
#define MAX_SET_SIZE 10000

typedef enum { UP, DOWN, LEFT, RIGHT } Direction;

typedef struct {
  size_t x;
  size_t y;
} Position;

typedef struct {
  Position pos1;
  Position pos2;
} PositionPair;

typedef struct {
  Position pos_init;
  char grid[MAX_GRID_SIZE][MAX_GRID_SIZE];
  size_t rows;
  size_t cols;
} Lab;

typedef struct {
  bool is_out;
  bool has_pos;
  bool has_block;
  Position pos;
  Position block;
} MoveResult;

typedef struct {
  Position positions[MAX_SET_SIZE];
  size_t size;
} PositionSet;

typedef struct {
  PositionPair pairs[MAX_SET_SIZE];
  size_t size;
} PositionPairSet;

void init_position_set(PositionSet *set) { set->size = 0; }

void init_position_pair_set(PositionPairSet *set) { set->size = 0; }

bool position_in_set(PositionSet *set, Position pos) {
  for (size_t i = 0; i < set->size; i++) {
    if (set->positions[i].x == pos.x && set->positions[i].y == pos.y) {
      return true;
    }
  }
  return false;
}

bool position_pair_in_set(PositionPairSet *set, PositionPair pair) {
  for (size_t i = 0; i < set->size; i++) {
    if (set->pairs[i].pos1.x == pair.pos1.x &&
        set->pairs[i].pos1.y == pair.pos1.y &&
        set->pairs[i].pos2.x == pair.pos2.x &&
        set->pairs[i].pos2.y == pair.pos2.y) {
      return true;
    }
  }
  return false;
}

void add_position_to_set(PositionSet *set, Position pos) {
  if (!position_in_set(set, pos)) {
    set->positions[set->size++] = pos;
  }
}

void add_position_pair_to_set(PositionPairSet *set, PositionPair pair) {
  if (!position_pair_in_set(set, pair)) {
    set->pairs[set->size++] = pair;
  }
}

Direction change_direction(Direction direction) {
  switch (direction) {
  case UP:
    return RIGHT;
  case RIGHT:
    return DOWN;
  case DOWN:
    return LEFT;
  case LEFT:
    return UP;
  default:
    return UP;
  }
}

MoveResult change_position(Lab *lab, Direction direction, Position position) {
  MoveResult result = {false, false, false, {0, 0}, {0, 0}};

  switch (direction) {
  case UP:
    if (position.y == 0) {
      result.is_out = true;
      return result;
    }
    if (lab->grid[position.y - 1][position.x] != OBJECT) {
      result.has_pos = true;
      result.pos = (Position){position.x, position.y - 1};
    } else {
      result.has_block = true;
      result.block = (Position){position.x, position.y - 1};
    }
    break;

  case LEFT:
    if (position.x == 0) {
      result.is_out = true;
      return result;
    }
    if (lab->grid[position.y][position.x - 1] != OBJECT) {
      result.has_pos = true;
      result.pos = (Position){position.x - 1, position.y};
    } else {
      result.has_block = true;
      result.block = (Position){position.x - 1, position.y};
    }
    break;

  case DOWN:
    if (position.y + 1 >= lab->rows) {
      result.is_out = true;
      return result;
    }
    if (lab->grid[position.y + 1][position.x] != OBJECT) {
      result.has_pos = true;
      result.pos = (Position){position.x, position.y + 1};
    } else {
      result.has_block = true;
      result.block = (Position){position.x, position.y + 1};
    }
    break;

  case RIGHT:
    if (position.x + 1 >= lab->cols) {
      result.is_out = true;
      return result;
    }
    if (lab->grid[position.y][position.x + 1] != OBJECT) {
      result.has_pos = true;
      result.pos = (Position){position.x + 1, position.y};
    } else {
      result.has_block = true;
      result.block = (Position){position.x + 1, position.y};
    }
    break;
  }
  return result;
}

Lab *parse_file(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    perror("Failed to open file");
    exit(EXIT_FAILURE);
  }

  Lab *lab = malloc(sizeof(Lab));
  char line[MAX_GRID_SIZE];
  size_t row = 0;
  size_t col_max = 0;

  while (fgets(line, sizeof(line), file)) {
    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == '\n')
      line[len--] = '\0';

    if (len > col_max)
      col_max = len;

    for (size_t i = 0; i < len; i++) {
      if (line[i] == '^') {
        lab->pos_init = (Position){i, row};
        lab->grid[row][i] = '.';
      } else {
        lab->grid[row][i] = line[i];
      }
    }
    row++;
  }

  lab->rows = row;
  lab->cols = col_max;
  fclose(file);
  return lab;
}

size_t find_path(Lab *lab) {
  Direction direction = UP;
  Position current_position = lab->pos_init;
  PositionSet explored;
  init_position_set(&explored);
  add_position_to_set(&explored, current_position);

  if (current_position.y == 0 ||
      (current_position.y > 0 &&
       lab->grid[current_position.y][current_position.x] == OBJECT)) {
    direction = change_direction(direction);
  }

  bool second_try = false;

  while (true) {
    MoveResult result = change_position(lab, direction, current_position);
    if (result.is_out)
      break;

    if (result.has_pos) {
      current_position = result.pos;
      add_position_to_set(&explored, current_position);
      second_try = false;
    } else {
      direction = change_direction(direction);
      if (second_try)
        break;
      second_try = true;
    }
  }

  return explored.size;
}

size_t find_if_stuck(Lab *lab) {
  char original_grid[MAX_GRID_SIZE][MAX_GRID_SIZE];
  memcpy(original_grid, lab->grid, sizeof(original_grid));
  size_t count = 0;

  for (size_t row = 0; row < lab->rows; row++) {
    for (size_t col = 0; col < lab->cols; col++) {
      if (OBJECT != lab->grid[row][col]) {
        lab->grid[row][col] = OBJECT;
        Direction direction = UP;
        Position current_position = lab->pos_init;
        PositionPairSet detect_loop;
        init_position_pair_set(&detect_loop);

        if (current_position.y == 0 ||
            (current_position.y > 0 &&
             lab->grid[current_position.y][current_position.x] == OBJECT)) {
          direction = change_direction(direction);
        }

        bool second_try = false;

        while (true) {
          MoveResult result = change_position(lab, direction, current_position);
          if (result.is_out)
            break;

          if (result.has_pos) {
            current_position = result.pos;
            second_try = false;
          } else if (result.has_block) {
            PositionPair blocking = {current_position, result.block};

            if (position_pair_in_set(&detect_loop, blocking)) {
              count++;
              break;
            }
            direction = change_direction(direction);
            add_position_pair_to_set(&detect_loop, blocking);
            second_try = true;
          }
        }

        lab->grid[row][col] = original_grid[row][col];
      }
    }
  }

  return count;
}

int main() {
  Lab *lab = parse_file("crates/day06/input.txt");

  // --- Part One ---
  printf("Part One solution: %zu\n", find_path(lab));
  // --- Part Two ---
  printf("Part Two solution: %zu\n", find_if_stuck(lab));

  free(lab);
  return 0;
}