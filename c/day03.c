#include "helpers.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  const char *iterator;
  const char *end;
} ProgramParser;

static inline char peek(const ProgramParser *parser) {
  return (parser->iterator < parser->end) ? *parser->iterator : '\0';
}

static inline void next(ProgramParser *parser) {
  if (parser->iterator < parser->end)
    parser->iterator++;
}

static bool parse_mul(ProgramParser *parser) {
  if (parser->end - parser->iterator < 3)
    return false;
  if (strncmp(parser->iterator, "mul", 3) == 0) {
    parser->iterator += 3;
    return true;
  }
  return false;
}

static int parse_number(ProgramParser *parser) {
  int value = 0, count = 0;

  while (isdigit(peek(parser)) && count < 3) {
    value = value * 10 + (peek(parser) - '0');
    next(parser);
    count++;
  }

  return count > 0 ? value : -1;
}

static int parse(ProgramParser *parser) {
  int result = 0;

  while (parser->iterator < parser->end) {
    if (parse_mul(parser) && peek(parser) == '(') {
      next(parser);
      int a = parse_number(parser);

      if (a != -1 && peek(parser) == ',') {
        next(parser);
        int b = parse_number(parser);

        if (b != -1 && peek(parser) == ')') {
          next(parser);
          result += a * b;
        }
      }
    } else {
      next(parser);
    }
  }

  return result;
}

typedef struct {
  size_t *data;
  size_t size;
  size_t capacity;
} SizeVector;

static SizeVector create_size_vector(size_t initial_capacity) {
  SizeVector vec = {.data = malloc(initial_capacity * sizeof(size_t)),
                    .size = 0,
                    .capacity = initial_capacity};
  return vec;
}

static void push_back(SizeVector *vec, size_t value) {
  if (vec->size == vec->capacity) {
    vec->capacity = (vec->capacity * 3) / 2;
    vec->data = realloc(vec->data, vec->capacity * sizeof(size_t));
  }
  vec->data[vec->size++] = value;
}

static SizeVector find_all_indexes(const char *haystack, const char *needle) {
  SizeVector indexes = create_size_vector(16);
  const size_t needle_len = strlen(needle);
  const char *pos = haystack;

  while ((pos = strstr(pos, needle)) != NULL) {
    push_back(&indexes, pos - haystack);
    pos += needle_len;
  }

  return indexes;
}

int closest_greater_than(size_t x, const SizeVector *values) {
  int closest = -1;
  for (size_t i = 0; i < values->size; i++) {
    if (values->data[i] > x && (closest == -1 || values->data[i] < closest)) {

      closest = values->data[i];
    }
  }
  return closest;
}

static int count_mul(const char *message) {
  ProgramParser parser = {.iterator = message,
                          .end = message + strlen(message)};
  return parse(&parser);
}

static int parse_with_rules(const char *message) {
  const char *do_substring = "do()";
  const char *dont_substring = "don't()";

  SizeVector do_positions = find_all_indexes(message, do_substring);
  SizeVector dont_positions = find_all_indexes(message, dont_substring);

  if (do_positions.size == 0 && dont_positions.size == 0) {
    int result = count_mul(message);
    free(do_positions.data);
    free(dont_positions.data);
    return result;
  }

  size_t len = do_positions.data[0] + strlen(do_substring);
  char *first_substring = strndup(message, len);
  int count = count_mul(first_substring);
  free(first_substring);

  int next_dont = -1;

  for (size_t i = 0; i < do_positions.size; i++) {
    size_t pos = do_positions.data[i];
    if (next_dont == -1 || pos > next_dont) {
      int next_dont_opt = closest_greater_than(pos, &dont_positions);
      if (next_dont_opt != -1) {
        size_t substring_len = next_dont_opt - pos;
        char *substring = strndup(message + pos, substring_len);
        count += count_mul(substring);
        free(substring);
        next_dont = next_dont_opt;
      }
    }
  }

  free(do_positions.data);
  free(dont_positions.data);
  return count;
}

int main() {
  char *message = read_file("crates/day03/input.txt");

  printf("Part One solution: sum is %d\n", count_mul(message));
  printf("Part Two solution: sum is %d\n", parse_with_rules(message));

  free(message);
  return 0;
}
