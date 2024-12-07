#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024
#define MAX_EQUATIONS 1024
#define MAX_VALUES_PER_EQUATION 20
#define MAX_OPERATORS 3

typedef struct {
  uint64_t result;
  uint64_t values[MAX_VALUES_PER_EQUATION];
  size_t values_count;
} Equation;

typedef struct {
  Equation equations[MAX_EQUATIONS];
  size_t count;
} Calibration;

uint64_t add(uint64_t a, uint64_t b) { return a + b; }

uint64_t multiply(uint64_t a, uint64_t b) { return a * b; }

uint64_t concatenate(uint64_t a, uint64_t b) {
  char buffer[32];
  snprintf(buffer, sizeof(buffer), "%lu%lu", a, b);
  return strtoull(buffer, NULL, 10);
}

typedef uint64_t (*OperatorFunc)(uint64_t, uint64_t);

void parse_line(char *line, Equation *equation) {
  char *colon = strchr(line, ':');
  if (!colon)
    return;

  *colon = '\0';
  equation->result = strtoull(line, NULL, 10);

  char *token = strtok(colon + 1, " \n");
  equation->values_count = 0;

  while (token && equation->values_count < MAX_VALUES_PER_EQUATION) {
    equation->values[equation->values_count++] = strtoull(token, NULL, 10);
    token = strtok(NULL, " \n");
  }
}

Calibration *read_file(const char *file_path) {
  FILE *file = fopen(file_path, "r");
  if (!file) {
    perror("Failed to open file");
    exit(EXIT_FAILURE);
  }

  Calibration *cal = malloc(sizeof(Calibration));
  cal->count = 0;

  char line[MAX_LINE_LENGTH];
  while (fgets(line, sizeof(line), file) && cal->count < MAX_EQUATIONS) {
    parse_line(line, &cal->equations[cal->count]);
    cal->count++;
  }

  fclose(file);
  return cal;
}

int compute_combinations(const Equation *eq, OperatorFunc *operators,
                         size_t operators_count) {
  size_t num_operators = eq->values_count - 1;
  uint64_t total_combinations = pow(operators_count, num_operators);

  for (uint64_t combination = 0; combination < total_combinations;
       combination++) {
    uint64_t result = eq->values[0];
    uint64_t current_combination = combination;

    for (size_t i = 0; i < num_operators; i++) {
      size_t operator_index = current_combination % operators_count;
      current_combination /= operators_count;
      result = operators[operator_index](result, eq->values[i + 1]);
    }

    if (result == eq->result) {
      return 1;
    }
  }

  return 0;
}

uint64_t compute_calibration(const Calibration *cal, OperatorFunc *operators,
                             size_t operators_count) {
  uint64_t count = 0;

  for (size_t i = 0; i < cal->count; i++) {
    if (compute_combinations(&cal->equations[i], operators, operators_count)) {
      count += cal->equations[i].result;
    }
  }

  return count;
}

int main() {
  Calibration *checker = read_file("crates/day07/input.txt");

  OperatorFunc operators[MAX_OPERATORS] = {add, multiply};

  // --- Part One ---
  uint64_t count = compute_calibration(checker, operators, 2);
  printf("Part One solution: %lu\n", count);

  // --- Part Two ---
  operators[2] = concatenate;
  count = compute_calibration(checker, operators, 3);
  printf("Part Two solution: %lu\n", count);

  free(checker);
  return 0;
}