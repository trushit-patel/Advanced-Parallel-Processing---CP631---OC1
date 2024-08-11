#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define SIZE 9
#define MAX_PUZZLES 95

int valid(int puzzle[][SIZE], int row, int column, int guess);
int solve(int puzzle[][SIZE], int depth);
int find_empty_cell(int puzzle[][SIZE], int *row, int *column);
void read_puzzle_from_file(const char *filename, int puzzle[][SIZE]);
void print_puzzle(int puzzle[][SIZE]);

int main(int argc, char **argv) {
    int puzzles[MAX_PUZZLES][SIZE][SIZE];
    int num_puzzles = 0;

    // Read puzzles from files p1.txt to p95.txt
    char filename[20];
    int i;

    for (i = 1; i <= MAX_PUZZLES; i++) {
        snprintf(filename, sizeof(filename), "puzzles/p%d.txt", i);
        FILE *file = fopen(filename, "r");
        if (file) {
            fclose(file);
            read_puzzle_from_file(filename, puzzles[num_puzzles]);
            num_puzzles++;
        }
    }

    double begin_cpu = omp_get_wtime();

    #pragma omp parallel for schedule(dynamic)
    for (i = 0; i < num_puzzles; i++) {
        int puzzle_copy[SIZE][SIZE];
        
        // Copy puzzle to avoid data races
        int row;
        int col;

        #pragma omp parallel for collapse(2)
        for (row = 0; row < SIZE; row++) {
            for (col = 0; col < SIZE; col++) {
                puzzle_copy[row][col] = puzzles[i][row][col];
            }
        }
        
        printf("Solving puzzle %d by thread %d:\n", i + 1, omp_get_thread_num());

        if (solve(puzzle_copy, 0)) {
            print_puzzle(puzzle_copy);
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

int solve(int puzzle[][SIZE], int depth) {
    int row, column;

    if (!find_empty_cell(puzzle, &row, &column)) return 1;

    int solved = 0;
    int guess;
    for (guess = 1; guess <= SIZE; guess++) {
        if (valid(puzzle, row, column, guess)) {
            puzzle[row][column] = guess;

            if (depth < 1) {  // Depth limit for task creation
                #pragma omp task firstprivate(puzzle, row, column, depth) shared(solved)
                {
                    if (solve(puzzle, depth + 1)) {
                        #pragma omp atomic write
                        solved = 1;
                    }
                }
            } else {
                if (solve(puzzle, depth + 1)) {
                    solved = 1;
                }
            }

            #pragma omp taskwait
            if (solved) break;
            puzzle[row][column] = 0;
        }
    }

    return solved;
}

void read_puzzle_from_file(const char *filename, int puzzle[][SIZE]) {
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
