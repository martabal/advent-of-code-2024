#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char **data;
  size_t rows;
  size_t cols;
} Grid;

Grid read_file(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    perror("Failed to open the file");
    exit(EXIT_FAILURE);
  }

  Grid grid = {NULL, 0, 0};
  char *line = NULL;
  size_t len = 0;

  while (getline(&line, &len, file) != -1) {
    grid.data = realloc(grid.data, (grid.rows + 1) * sizeof(char *));
    grid.data[grid.rows] = strdup(line);
    grid.rows++;

    if (grid.rows == 1) {
      grid.cols = strlen(line) - 1;
    }
  }

  free(line);
  fclose(file);

  return grid;
}

size_t count_word(const Grid *grid, const char *word) {
  size_t count = 0;
  int word_len = strlen(word);

  int directions[][2] = {{0, 1}, {0, -1}, {1, 0},  {-1, 0},
                         {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

  for (size_t row = 0; row < grid->rows; ++row) {
    for (size_t col = 0; col < grid->cols; ++col) {
      for (size_t d = 0; d < sizeof(directions) / sizeof(directions[0]); ++d) {
        int dx = directions[d][0];
        int dy = directions[d][1];
        int found = 1;

        for (int i = 0; i < word_len; ++i) {
          int x = row + i * dx;
          int y = col + i * dy;

          if (x < 0 || y < 0 || x >= grid->rows || y >= grid->cols ||
              grid->data[x][y] != word[i]) {
            found = 0;
            break;
          }
        }

        if (found) {
          count++;
        }
      }
    }
  }

  return count;
}

size_t count_x_pattern(const Grid *grid, const char *word) {
  if (grid->rows == 0) {
    fprintf(stderr, "Grid must be a valid grid\n");
    exit(EXIT_FAILURE);
  }

  size_t count = 0;
  int word_len = strlen(word);
  int bounded = word_len / 2;
  char middle_char = word[bounded];

  char *reverse_word = strdup(word);
  for (int i = 0; i < word_len / 2; i++) {
    char temp = reverse_word[i];
    reverse_word[i] = reverse_word[word_len - 1 - i];
    reverse_word[word_len - 1 - i] = temp;
  }

  for (int r = bounded; r < grid->rows - bounded; ++r) {
    for (int c = bounded; c < grid->cols - bounded; ++c) {
      if (grid->data[r][c] == middle_char) {
        int first_diagonal = 1;
        int second_diagonal = 1;

        // Check forward diagonal '\'
        for (int i = 0; i < word_len; ++i) {
          if (grid->data[r - bounded + i][c - bounded + i] != word[i]) {
            first_diagonal = 0;
            break;
          }
        }

        // Check backward diagonal '\'
        if (!first_diagonal) {
          first_diagonal = 1;
          for (int i = 0; i < word_len; ++i) {
            if (grid->data[r - bounded + i][c - bounded + i] !=
                reverse_word[i]) {
              first_diagonal = 0;
              break;
            }
          }
        }

        if (first_diagonal) {
          // Check forward diagonal '/'
          for (int i = 0; i < word_len; ++i) {
            if (grid->data[r + bounded - i][c - bounded + i] != word[i]) {
              second_diagonal = 0;
              break;
            }
          }

          // Check backward diagonal '/'
          if (!second_diagonal) {
            second_diagonal = 1;
            for (int i = 0; i < word_len; ++i) {
              if (grid->data[r + bounded - i][c - bounded + i] !=
                  reverse_word[i]) {
                second_diagonal = 0;
                break;
              }
            }
          }

          if (second_diagonal) {
            count++;
          }
        }
      }
    }
  }

  free(reverse_word);
  return count;
}

void free_grid(Grid *grid) {
  for (size_t i = 0; i < grid->rows; i++) {
    free(grid->data[i]);
  }
  free(grid->data);
}

int main() {
  Grid grid = read_file("crates/day04/input.txt");

  // --- Part One ---
  size_t count = count_word(&grid, "XMAS");
  printf("%zu\n", count);

  // --- Part Two ---
  count = count_x_pattern(&grid, "MAS");
  printf("%zu\n", count);

  free_grid(&grid);
  return 0;
}
