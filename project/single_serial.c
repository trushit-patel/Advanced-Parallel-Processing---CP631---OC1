#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 9
#define puzzel_id 5

int valid(int puzzle[][SIZE], int row, int column, int guess);
int solve(int puzzle[][SIZE], int row, int column);
void read_puzzle_from_file(const char *filename, int puzzle[][SIZE]);
void print_puzzle(int puzzle[][SIZE]);

int main() {
    int puzzle[SIZE][SIZE];
    // int num_puzzles = 0;

    // Read puzzles from files p1.txt to p95.txt
    char filename[] = "puzzles/p5.txt";
    // int i;

    snprintf(filename, sizeof(filename), "puzzles/p%d.txt", puzzel_id);
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file: %s\n", filename);
        exit(1);
    }

    int i, j;
    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            fscanf(file, "%d", &puzzle[i][j]);
        }
    }

    fclose(file);

    clock_t begin_cpu = clock();

    printf("Solving puzzle %d:\n", puzzel_id);
    if (solve(puzzle, 0, 0)) {
        print_puzzle(puzzle);
    } else {
        printf("NO SOLUTION FOUND\n");
    }
    printf("\n");

    clock_t end_cpu = clock();
    double cpu_time = (double)(end_cpu - begin_cpu)/CLOCKS_PER_SEC;

    printf("Time taken: %f seconds \n", cpu_time);

    return 0;
}

int valid(int puzzle[][SIZE], int row, int column, int guess) {
    int corner_x = row / 3 * 3;
    int corner_y = column / 3 * 3;
    int x;

    for (x = 0; x < SIZE; ++x) {
        if (puzzle[row][x] == guess) return 0;
        if (puzzle[x][column] == guess) return 0;
        if (puzzle[corner_x + (x % 3)][corner_y + (x / 3)] == guess) return 0;
    }
    return 1;
}

int solve(int puzzle[SIZE][SIZE], int row, int column) {
    if (row == SIZE) {
        return 1;
    }

    if (puzzle[row][column] != 0) {
        if (column == SIZE - 1) {
            if (solve(puzzle, row + 1, 0)) return 1;
        } else {
            if (solve(puzzle, row, column + 1)) return 1;
        }
    } else {
        int guess;
        for (guess = 1; guess <= SIZE; guess++) {
            if (valid(puzzle, row, column, guess)) {
                puzzle[row][column] = guess;

                int next_row = (column == SIZE - 1) ? row + 1 : row;
                int next_col = (column == SIZE - 1) ? 0 : column + 1;

                if (solve(puzzle, next_row, next_col)) {
                    return 1;
                }

                puzzle[row][column] = 0;
            }
        }
    }

    return 0;
}

void print_puzzle(int puzzle[][SIZE]) {
    int x, y;

    for (x = 0; x < SIZE; ++x) {
        for (y = 0; y < SIZE; ++y) {
            printf("%d ", puzzle[x][y]);
        }
        printf("\n");
    }
}
