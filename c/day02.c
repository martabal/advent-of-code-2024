#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 256
#define MAX_VALUES 100

bool has_small_difference(int *vec, size_t size) {
  for (size_t i = 0; i < size - 1; i++) {
    int diff = abs(vec[i + 1] - vec[i]);
    if (diff < 1 || diff > 3) {
      return false;
    }
  }
  return true;
}

bool is_safe(int *e, size_t size) {
  bool non_increasing = true;
  bool non_decreasing = true;

  for (size_t i = 0; i < size - 1; i++) {
    if (e[i] < e[i + 1]) {
      non_increasing = false;
    }
    if (e[i] > e[i + 1]) {
      non_decreasing = false;
    }
  }

  return (non_increasing || non_decreasing) && has_small_difference(e, size);
}

int **read_file(const char *input, size_t *row_count) {
  FILE *file = fopen(input, "r");
  if (!file) {
    perror("Failed to open file");
    return NULL;
  }

  int **report = malloc(MAX_VALUES * sizeof(int *));
  *row_count = 0;

  char line[MAX_LINE_LENGTH];
  while (fgets(line, sizeof(line), file) && *row_count < MAX_VALUES) {
    int *row = malloc(MAX_VALUES * sizeof(int));
    char *token = strtok(line, " ");
    size_t col_count = 0;

    while (token != NULL) {
      row[col_count++] = atoi(token);
      token = strtok(NULL, " ");
    }
    report[*row_count] = realloc(row, col_count * sizeof(int));
    (*row_count)++;
  }

  fclose(file);
  return report;
}

int main() {
  size_t row_count;
  int **report = read_file("crates/day02/input.txt", &row_count);
  if (!report) {
    return EXIT_FAILURE;
  }

  // --- Part One ---
  unsigned int safe_count = 0;
  for (size_t i = 0; i < row_count; i++) {
    if (is_safe(report[i], MAX_VALUES)) {
      safe_count++;
    }
  }
  printf("Part One solution: sum is %u\n", safe_count);

  // --- Part Two ---
  safe_count = 0;
  for (size_t i = 0; i < row_count; i++) {
    if (is_safe(report[i], MAX_VALUES)) {
      safe_count++;
    } else {
      for (size_t pos = 0; pos < MAX_VALUES; pos++) {
        if (report[i][pos] == 0)
          continue;
        int *new = malloc((MAX_VALUES - 1) * sizeof(int));
        size_t new_count = 0;

        for (size_t j = 0; j < MAX_VALUES; j++) {
          if (j != pos) {
            new[new_count++] = report[i][j];
          }
        }

        if (is_safe(new, new_count)) {
          safe_count++;
          free(new);
          break;
        }

        free(new);
      }
    }
  }
  printf("Part Two solution: sum is %u\n", safe_count);

  for (size_t i = 0; i < row_count; i++) {
    free(report[i]);
  }
  free(report);

  return 0;
}
