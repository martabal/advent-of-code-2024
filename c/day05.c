#include "helpers.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Rule {
  int key;
  int *values;
  int value_count;
};

struct PrintQueueChecker {
  struct Rule *rules;
  int rule_count;
  int **page_numbers;
  int *page_numbers_sizes;
  int page_numbers_count;
};

static struct Rule *find_rule(struct PrintQueueChecker *checker, int key) {
  for (int i = 0; i < checker->rule_count; i++) {
    if (checker->rules[i].key == key) {
      return &checker->rules[i];
    }
  }
  return NULL;
}

static bool find_value_in_array(const int *arr, int size, int start_idx,
                                int value) {
  for (int i = start_idx; i < size; i++) {
    if (arr[i] == value) {
      return true;
    }
  }
  return false;
}

static bool check_valid(const struct PrintQueueChecker *checker,
                        const int *order, int order_size) {
  for (int idx = 0; idx < order_size; idx++) {
    struct Rule *rule =
        find_rule((struct PrintQueueChecker *)checker, order[idx]);
    if (rule == NULL)
      continue;

    for (int j = 0; j < rule->value_count; j++) {
      if (find_value_in_array(order, order_size, idx, rule->values[j])) {
        return false;
      }
    }
  }
  return true;
}

static void parse_puzzle(struct PrintQueueChecker *checker,
                         const char *puzzle) {
  checker->rule_count = 0;
  checker->page_numbers_count = 0;

  checker->rules = (struct Rule *)malloc(sizeof(struct Rule) * 2000);
  for (int i = 0; i < 2000; i++) {
    checker->rules[i].values = (int *)malloc(sizeof(int) * 500);
  }

  checker->page_numbers = (int **)malloc(sizeof(int *) * 500);
  checker->page_numbers_sizes = (int *)malloc(sizeof(int) * 500);

  char line[2048];
  const char *puzzle_ptr = puzzle;
  int line_start = 0;
  int line_end = 0;

  while (puzzle_ptr[line_end] != '\0') {
    if (puzzle_ptr[line_end] == '\n' || puzzle_ptr[line_end] == '\0') {
      if (line_end - line_start == 0) {
        puzzle_ptr += line_end - line_start + 1;
        line_start = 0;
        line_end = 0;
        break;
      }

      strncpy(line, puzzle_ptr + line_start, line_end - line_start);
      line[line_end - line_start] = '\0';

      char *delimiter = strchr(line, '|');
      if (delimiter != NULL) {
        *delimiter = '\0';
        int left = atoi(line);
        int right = atoi(delimiter + 1);

        struct Rule *rule = find_rule(checker, right);
        if (rule == NULL) {
          rule = &checker->rules[checker->rule_count++];
          rule->key = right;
          rule->value_count = 0;
        }
        rule->values[rule->value_count++] = left;
      }

      puzzle_ptr += line_end - line_start + 1;
      line_start = 0;
      line_end = 0;
      continue;
    }
    line_end++;
  }

  while (*puzzle_ptr != '\0') {
    while (*puzzle_ptr == '\n')
      puzzle_ptr++;
    if (*puzzle_ptr == '\0')
      break;

    int *current_order = (int *)malloc(sizeof(int) * 100);
    int current_size = 0;

    while (*puzzle_ptr != '\n' && *puzzle_ptr != '\0') {
      char number_str[20] = {0};
      int i = 0;

      while (*puzzle_ptr != ',' && *puzzle_ptr != '\n' && *puzzle_ptr != '\0') {
        number_str[i++] = *puzzle_ptr++;
      }

      current_order[current_size++] = atoi(number_str);

      if (*puzzle_ptr == ',')
        puzzle_ptr++;
    }

    checker->page_numbers[checker->page_numbers_count] = current_order;
    checker->page_numbers_sizes[checker->page_numbers_count] = current_size;
    checker->page_numbers_count++;

    if (*puzzle_ptr == '\n')
      puzzle_ptr++;
  }
}

static int check_page_order(const struct PrintQueueChecker *checker) {
  int sum = 0;
  for (int i = 0; i < checker->page_numbers_count; i++) {
    if (check_valid(checker, checker->page_numbers[i],
                    checker->page_numbers_sizes[i])) {
      sum += checker->page_numbers[i][checker->page_numbers_sizes[i] / 2];
    }
  }
  return sum;
}

static void clone_array(const int *source, int *dest, int size) {
  memcpy(dest, source, size * sizeof(int));
}

static int check_and_sort_page_order(const struct PrintQueueChecker *checker) {
  int sum = 0;
  int *order_clone = (int *)malloc(sizeof(int) * 100);

  for (int i = 0; i < checker->page_numbers_count; i++) {
    const int *order = checker->page_numbers[i];
    int order_size = checker->page_numbers_sizes[i];

    if (!check_valid(checker, order, order_size)) {
      clone_array(order, order_clone, order_size);

      while (!check_valid(checker, order_clone, order_size)) {
        bool found_rule_break = false;
        int old_idx = 0;
        int new_idx = 0;
        int replace_val = 0;

        for (int idx = 0; idx < order_size && !found_rule_break; idx++) {
          struct Rule *rule =
              find_rule((struct PrintQueueChecker *)checker, order_clone[idx]);
          if (rule == NULL)
            continue;

          for (int check_idx = order_size - 1; check_idx >= idx; check_idx--) {
            for (int rule_idx = 0; rule_idx < rule->value_count; rule_idx++) {
              if (order_clone[check_idx] == rule->values[rule_idx]) {
                new_idx = check_idx;
                found_rule_break = true;
                old_idx = idx;
                replace_val = order_clone[idx];
                break;
              }
            }
            if (found_rule_break)
              break;
          }
        }

        if (found_rule_break) {
          for (int j = old_idx; j < order_size - 1; j++) {
            order_clone[j] = order_clone[j + 1];
          }
          for (int j = order_size - 1; j > new_idx; j--) {
            order_clone[j] = order_clone[j - 1];
          }
          order_clone[new_idx] = replace_val;
        }
      }
      sum += order_clone[order_size / 2];
    }
  }

  free(order_clone);
  return sum;
}

static void free_print_queue_checker(struct PrintQueueChecker *checker) {
  for (int i = 0; i < checker->rule_count; i++) {
    free(checker->rules[i].values);
  }
  free(checker->rules);
  for (int i = 0; i < checker->page_numbers_count; i++) {
    free(checker->page_numbers[i]);
  }
  free(checker->page_numbers);
  free(checker->page_numbers_sizes);
}

int main() {
  char *message = read_file("crates/day05/input.txt");
  struct PrintQueueChecker checker = {0};

  parse_puzzle(&checker, message);

  // --- Part One ---
  printf("Part One solution: %d\n", check_page_order(&checker));

  // --- Part Two ---
  printf("Part Two solution: %d\n", check_and_sort_page_order(&checker));

  free_print_queue_checker(&checker);
  free(message);
  return 0;
}
