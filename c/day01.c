#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 1000

void read_file(const char *filename, int **left_column, int **right_column, size_t *len) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    *len = 0;
    size_t capacity = INITIAL_CAPACITY;
    *left_column = malloc(capacity * sizeof(int));
    *right_column = malloc(capacity * sizeof(int));
    
    if (!*left_column || !*right_column) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    char line[40];
    while (fgets(line, sizeof(line), file)) {
        if (*len >= capacity) {
            capacity *= 2;
            if (!*left_column || !*right_column) {
                perror("Failed to reallocate memory");
                exit(EXIT_FAILURE);
            }
        }

        int left_num, right_num;
        if (sscanf(line, "%d %d", &left_num, &right_num) == 2) {
            (*left_column)[*len] = left_num;
            (*right_column)[*len] = right_num;
            (*len)++;
        }
    }

    fclose(file);
}

int compare_ints(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

int main() {
    int *left_column, *right_column;
    size_t len;
    read_file("crates/day01/input.txt", &left_column, &right_column, &len);

    // --- Part One ---
    qsort(left_column, len, sizeof(int), compare_ints);
    qsort(right_column, len, sizeof(int), compare_ints);

    unsigned int *distances = malloc(len * sizeof(unsigned int));
    unsigned int sum = 0;

    for (size_t i = 0; i < len; i++) {
        distances[i] = abs(left_column[i] - right_column[i]);
        sum += distances[i];
    }
    printf("Part One solution: sum is %u\n", sum);
    free(distances);

    // --- Part Two ---
    unsigned int *appear = malloc(len * sizeof(unsigned int));
    sum = 0;

    for (size_t i = 0; i < len; i++) {
        int count = 0;
        for (size_t j = 0; j < len; j++) {
            if (right_column[j] == left_column[i]) {
                count++;
            }
        }
        appear[i] = left_column[i] * count;
        sum += appear[i];
    }

    printf("Part Two solution: sum is %u\n", sum);
    free(appear);
    free(left_column);
    free(right_column);

    return 0;
}
