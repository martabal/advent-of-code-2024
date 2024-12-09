#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *read_file(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    fprintf(stderr, "Unable to open file: %s\n", filename);
    exit(1);
  }

  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  rewind(file);

  char *buffer = (char *)malloc(file_size + 1);
  if (buffer == NULL) {
    fclose(file);
    fprintf(stderr, "Memory allocation failed\n");
    exit(1);
  }

  size_t read_size = fread(buffer, 1, file_size, file);
  buffer[read_size] = '\0';

  fclose(file);
  return buffer;
}