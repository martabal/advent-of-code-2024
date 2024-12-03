#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char *program;
} Problem;

typedef struct {
  const char *iterator;
  const char *end;
} ProgramParser;

Problem create_problem(const char *input) {
  Problem p;
  p.program = strdup(input);
  return p;
}

void free_problem(Problem *p) { free(p->program); }

char peek(ProgramParser *parser) {
  return parser->iterator != parser->end ? *parser->iterator : '\0';
}

void next(ProgramParser *parser) {
  if (parser->iterator != parser->end)
    parser->iterator++;
}

bool parse_mul(ProgramParser *parser) {
  if (peek(parser) == 'm') {
    next(parser);
    if (peek(parser) == 'u') {
      next(parser);
      if (peek(parser) == 'l') {
        next(parser);
        return true;
      }
    }
  }
  return false;
}

typedef struct {
  bool has_value;
  int value;
} OptionalInt;

OptionalInt parse_number(ProgramParser *parser) {
  char digits[4] = {0};
  int digit_count = 0;

  for (int i = 0; i < 3; ++i) {
    if (isdigit(peek(parser))) {
      digits[digit_count++] = peek(parser);
      next(parser);
    } else {
      break;
    }
  }

  OptionalInt result = {false, 0};
  if (digit_count > 0) {
    result.has_value = true;
    result.value = atoi(digits);
  }
  return result;
}

int parse(ProgramParser *parser) {
  int result = 0;

  while (parser->iterator != parser->end) {
    if (parse_mul(parser)) {
      if (peek(parser) == '(') {
        next(parser);

        OptionalInt a = parse_number(parser);
        if (a.has_value && peek(parser) == ',') {
          next(parser);

          OptionalInt b = parse_number(parser);
          if (b.has_value && peek(parser) == ')') {
            next(parser);
            result += a.value * b.value;
          }
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

SizeVector create_size_vector() {
  const size_t initial_capacity = 16;
  SizeVector vec = {.data = malloc(initial_capacity * sizeof(size_t)),
                    .size = 0,
                    .capacity = initial_capacity};
  return vec;
}

void free_size_vector(SizeVector *vec) {
  free(vec->data);
  vec->data = NULL;
  vec->size = vec->capacity = 0;
}

void push_back(SizeVector *vec, size_t value) {
  if (vec->size == vec->capacity) {
    vec->capacity *= 2;
    vec->data = realloc(vec->data, vec->capacity * sizeof(size_t));
  }
  vec->data[vec->size++] = value;
}

SizeVector find_all_indexes(const char *haystack, const char *needle) {
  SizeVector indexes = create_size_vector();
  const char *pos = haystack;

  while ((pos = strstr(pos, needle)) != NULL) {
    push_back(&indexes, pos - haystack);
    pos += strlen(needle);
  }

  return indexes;
}

typedef struct {
  bool has_value;
  size_t value;
} OptionalSize;

OptionalSize closest_greater_than(size_t x, const SizeVector *values) {
  OptionalSize closest = {false, 0};

  for (size_t i = 0; i < values->size; i++) {
    size_t value = values->data[i];
    if (value > x && (!closest.has_value || value < closest.value)) {
      closest.has_value = true;
      closest.value = value;
    }
  }

  return closest;
}

int count_mul(const char *message) {
  Problem problem = create_problem(message);
  ProgramParser parser = {problem.program,
                          problem.program + strlen(problem.program)};
  int result = parse(&parser);
  free_problem(&problem);
  return result;
}

int parse_with_rules(const char *message) {
  const char *do_substring = "do()";
  const char *dont_substring = "don't()";

  SizeVector do_positions = find_all_indexes(message, do_substring);
  SizeVector dont_positions = find_all_indexes(message, dont_substring);

  if (do_positions.size == 0 && dont_positions.size == 0) {
    int result = count_mul(message);
    free_size_vector(&do_positions);
    free_size_vector(&dont_positions);
    return result;
  }

  char *first_substring =
      malloc(do_positions.data[0] + strlen(do_substring) + 1);
  strncpy(first_substring, message,
          do_positions.data[0] + strlen(do_substring));
  first_substring[do_positions.data[0] + strlen(do_substring)] = '\0';

  int count = count_mul(first_substring);
  free(first_substring);

  OptionalSize next_dont = {false, 0};

  for (size_t i = 0; i < do_positions.size; i++) {
    size_t pos = do_positions.data[i];
    if (!next_dont.has_value || pos > next_dont.value) {
      OptionalSize next_dont_opt = closest_greater_than(pos, &dont_positions);

      if (next_dont_opt.has_value) {
        size_t substring_len = next_dont_opt.value - pos;
        char *substring = malloc(substring_len + 1);
        strncpy(substring, message + pos, substring_len);
        substring[substring_len] = '\0';

        count += count_mul(substring);
        free(substring);

        next_dont = next_dont_opt;
      }
    }
  }

  free_size_vector(&do_positions);
  free_size_vector(&dont_positions);

  return count;
}

char *read_file(const char *file_path) {
  FILE *file = fopen(file_path, "r");
  if (!file) {
    fprintf(stderr, "Unable to open file: %s\n", file_path);
    exit(1);
  }

  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  rewind(file);

  char *content = malloc(file_size + 1);
  size_t read_size = fread(content, 1, file_size, file);
  content[read_size] = '\0';

  fclose(file);
  return content;
}

int main() {
  char *message = read_file("crates/day03/input.txt");

  // --- Part One ---
  int count = count_mul(message);
  printf("Part One solution: sum is %d\n", count);

  // --- Part Two ---
  count = parse_with_rules(message);
  printf("Part Two solution: sum is %d\n", count);

  free(message);
  return 0;
}