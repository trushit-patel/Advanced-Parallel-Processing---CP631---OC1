#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <time.h>
#include <unistd.h>

#define SIZE 9
// #define puzzel_id 5

#define N 95

double start;

void print_grid(int grid[SIZE][SIZE])
{
    int row, col;
    for (row = 0; row < SIZE; row++)
    {
        for (col = 0; col < SIZE; col++)
        {
            printf("%d ", grid[row][col]);
        }
        printf("\n");
    }
}

int is_safe(int grid[SIZE][SIZE], int row, int col, int num)
{
    int startRow = row - row % 3;
    int startCol = col - col % 3;
    int i;

    for (i = 0; i < 9; i++)
    {
        // Check the row and column
        if (grid[row][i] == num || grid[i][col] == num)
        {
            return 0;
        }

        // Check the 3x3 subgrid
        int boxRow = startRow + i / 3;
        int boxCol = startCol + i % 3;
        if (grid[boxRow][boxCol] == num)
        {
            return 0;
        }
    }

    return 1;
}

int find_unassigned(int grid[SIZE][SIZE], int *row, int *col)
{
    for (*row = 0; *row < SIZE; (*row)++)
    {
        for (*col = 0; *col < SIZE; (*col)++)
        {
            if (grid[*row][*col] == 0)
            {
                return 1;
            }
        }
    }
    return 0;
}

int solve(int grid[SIZE][SIZE], int level)
{

    int row = 0;
    int col = 0;

    int num;
    if (!find_unassigned(grid, &row, &col))
        return 1;

    for (num = 1; num <= SIZE; num++)
    {
        if (is_safe(grid, row, col, num))
        {
            #pragma omp task default(none) firstprivate(grid, row, col, num, level) shared(start) final(level > 1)
            {
                int copy_grid[SIZE][SIZE];
                memcpy(copy_grid, grid, SIZE * SIZE * sizeof(int));
                copy_grid[row][col] = num;
                if (solve(copy_grid, level + 1))
                {
                    print_grid(copy_grid);
                    // exit(0);
                }
            }
        }
    }

    #pragma omp taskwait
    return 0;
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

int main(int argc, char **argv)
{
    int puzzles[N][SIZE][SIZE];
    int num_puzzles = 0;

    char filename[20];
    int i, j, k;

    // #pragma omp for schedule (dynamic)
    for (k = 0; k < N; k++) {

        snprintf(filename, sizeof(filename), "puzzles/p%d.txt", i);
        FILE *file = fopen(filename, "r");
        if (file) {
            fclose(file);
            read_puzzle_from_file(filename, puzzles[num_puzzles]);
            num_puzzles++;
        }
    }

    start = omp_get_wtime();

    #pragma omp for schedule (dynamic)
    for( k = 0 ; k < N ; k++){

        printf("Solving puzzle %d: \n", k);

        int curr_puzzle[SIZE][SIZE];
        memcpy(curr_puzzle, puzzles[k], SIZE * SIZE * sizeof(int));

        #pragma omp parallel default(none) shared(curr_puzzle) num_threads(atoi(argv[1]))
        #pragma omp single nowait
        {
            solve(curr_puzzle, 1);
        }
    }

    double end = omp_get_wtime();
    double time_spent = end - start;
    printf("\nTime Taken %f seconds\n\n", time_spent);

    return 0;
}