#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define SIZE 9
#define MAX_PUZZLES 100

int valid(int puzzle[][SIZE], int row, int column, int guess);
int solve(int puzzle[][SIZE]);
int find_empty_cell(int puzzle[][SIZE], int *row, int *column);
void read_puzzle_from_file(const char *filename, int puzzle[][SIZE]);
void print_puzzle(int puzzle[][SIZE]);

int main() {
    int puzzles[MAX_PUZZLES][SIZE][SIZE];
    int num_puzzles = 0;

    // Read puzzles from files p1.txt to p95.txt
    char filename[20];
    int i;

    for (i = 1; i <= 95; i++) {
        snprintf(filename, sizeof(filename), "puzzles/p%d.txt", i);
        FILE *file = fopen(filename, "r");
        if (file) {
            fclose(file);
            read_puzzle_from_file(filename, puzzles[num_puzzles]);
            num_puzzles++;
        }
    }

    double begin_cpu = omp_get_wtime();

    #pragma omp parallel for
    for (i = 0; i < num_puzzles; i++) {
        printf("Solving puzzle %d by thread %d:\n", i + 1, omp_get_thread_num());
        if (solve(puzzles[i])) {
            print_puzzle(puzzles[i]);
        } else {
            printf("NO SOLUTION FOUND\n");
        }
        printf("\n");
    }

    double end_cpu = omp_get_wtime();
    double cpu_time = end_cpu - begin_cpu;

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

int find_empty_cell(int puzzle[][SIZE], int *row, int *column) {
    int x;
    int y;
    for (x = 0; x < SIZE; x++) {
        for (y = 0; y < SIZE; y++) {
            if (!puzzle[x][y]) {
                *row = x;
                *column = y;
                return 1;
            }
        }
    }
    return 0;
}

int solve(int puzzle[][SIZE]) {
    int row, column;
    int guess;

    if (!find_empty_cell(puzzle, &row, &column)) return 1;

    for (guess = 1; guess <= SIZE; guess++) {
        if (valid(puzzle, row, column, guess)) {
            puzzle[row][column] = guess;

            if (solve(puzzle)) return 1;
            puzzle[row][column] = 0;
        }
    }
    return 0;
}

void read_puzzle_from_file(const char *filename, int puzzle[][SIZE]) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file: %s\n", filename);
        exit(1);
    }

    int i;
    int j;
    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            fscanf(file, "%d", &puzzle[i][j]);
        }
    }

    fclose(file);
}

void print_puzzle(int puzzle[][SIZE]) {
    int x;
    int y;

    for (x = 0; x < SIZE; ++x) {
        for (y = 0; y < SIZE; ++y) {
            printf("%d ", puzzle[x][y]);
        }
        printf("\n");
    }
}
