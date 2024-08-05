#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_ORDER 1000

int main(int argc, char *argv[]) {
    int my_rank, size, N, row, col;
    float matrix[MAX_ORDER][MAX_ORDER];
    float local_max[MAX_ORDER];
    float global_max[MAX_ORDER];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    N = 500;
    row = N;
    col = N / size;

    int i = 0, j = 0;
    for (i = 0; i < row; ++i) {
        for (j = 0; j < col; ++j) {
            matrix[i][j] = (float)rand() / RAND_MAX;
        }
    }

    for (i = 0; i < row; ++i) {
        local_max[i] = matrix[i][0];
        for (j = 1; j < col; ++j) {
            if (matrix[i][j] > local_max[i]) {
                local_max[i] = matrix[i][j];
            }
        }
    }

    MPI_Reduce(local_max, global_max, row, MPI_FLOAT, MPI_MAX, 0, MPI_COMM_WORLD);

    if (my_rank == 0) {
        printf("Maximum values along each row:\n");
        for (i = 0; i < row; ++i) {
            printf("%f\n", global_max[i]);
        }
    }

    MPI_Finalize();

    return 0;
}
