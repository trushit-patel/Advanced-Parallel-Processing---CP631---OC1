#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <time.h>
#include <unistd.h>

#define SIZE 9
#define puzzel_id 5

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
                    double end = omp_get_wtime();
                    double time_spent = end - start;
                    printf("\nTime Taken %f seconds\n\n", time_spent);
                    exit(0);
                }

                grid[row][col] = 0;
            }
        }
    }

    #pragma omp taskwait
    return 0;
}

int main(int argc, char **argv)
{
    int puzzle[SIZE][SIZE];
    char filename[] = "puzzles/p5.txt";

    snprintf(filename, sizeof(filename), "puzzles/p%d.txt", puzzel_id);
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        printf("Error opening file: %s\n", filename);
        exit(1);
    }

    int i, j;
    for (i = 0; i < SIZE; i++)
    {
        for (j = 0; j < SIZE; j++)
        {
            fscanf(file, "%d", &puzzle[i][j]);
        }
    }

    fclose(file);

    // printf("Size: %d", SIZE);
    // printf("\n");

    start = omp_get_wtime();
    printf("Solving puzzle %d: \n", puzzel_id);

    #pragma omp parallel default(none) shared(puzzle) num_threads(atoi(argv[1]))
    #pragma omp single nowait
    {
        solve(puzzle, 1);
    }

    return 0;
}