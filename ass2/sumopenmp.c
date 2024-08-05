#include "stdio.h"
#include "omp.h"

#define MAT_DIM 1000 // Matrix dimension, change as needed
int N = MAT_DIM;
int A[MAT_DIM][MAT_DIM];
int Asum[MAT_DIM][MAT_DIM]; // Compute serially
int Asumtask[MAT_DIM][MAT_DIM]; // Compute with OpenMP tasks
int Asumtaskblock[MAT_DIM][MAT_DIM]; // Compute with blocks and OpenMP tasks

// Serial computation of Asum
int compute_serial() {
    int i, j;

    // Compute corner
    Asum[0][0] = A[0][0];

    // Compute top row
    i = 0;
    for (j = 1; j < N; j++) {
        Asum[0][j] = A[0][j] + Asum[0][j - 1];
    }

    // Compute left column
    j = 0;
    for (i = 1; i < N; i++) {
        Asum[i][j] = A[i][j] + Asum[i - 1][j];
    }

    // Compute interior
    for (i = 1; i < N; i++) {
        for (j = 1; j < N; j++) {
            Asum[i][j] = A[i][j] + Asum[i - 1][j] + Asum[i][j - 1] - Asum[i - 1][j - 1];
        }
    }

    return 0;
}

//write this function, use OpenMP tasks
void compute_tasks() {
    #pragma omp parallel
    {
        #pragma omp single
        {
            // Compute corner
            #pragma omp task depend(out: Asumtask[0][0])
            Asumtask[0][0] = A[0][0];

            // Compute top row
            int j = 0, i = 0;
            for (j = 1; j < N; j++) {
                #pragma omp task depend(in: Asumtask[0][j-1]) depend(out: Asumtask[0][j])
                Asumtask[0][j] = A[0][j] + Asumtask[0][j - 1];
            }

            // Compute left column
            for (i = 1; i < N; i++) {
                #pragma omp task depend(in: Asumtask[i-1][0]) depend(out: Asumtask[i][0])
                Asumtask[i][0] = A[i][0] + Asumtask[i - 1][0];
            }

            // Compute interior
            for (i = 1; i < N; i++) {
                for (j = 1; j < N; j++) {
                    #pragma omp task depend(in: Asumtask[i-1][j], Asumtask[i][j-1]) depend(out: Asumtask[i][j])
                    Asumtask[i][j] = A[i][j] + Asumtask[i - 1][j] + Asumtask[i][j - 1] - Asumtask[i - 1][j - 1];
                }
            }
        }
    }
}

//write this function, use OpenMP tasks and blocks
void compute_tasks_blocks(int block_size) {
    #pragma omp parallel
    {
        #pragma omp single
        {
            // Compute corner block
            #pragma omp task depend(out: Asumtaskblock[0:block_size][0:block_size])
            {
                int i = 0, j = 0;
                for (i = 0; i < block_size && i < N; i++) {
                    for (j = 0; j < block_size && j < N; j++) {
                        if (i == 0 && j == 0) {
                            Asumtaskblock[i][j] = A[i][j];
                        } else if (i == 0) {
                            Asumtaskblock[i][j] = A[i][j] + Asumtaskblock[i][j-1];
                        } else if (j == 0) {
                            Asumtaskblock[i][j] = A[i][j] + Asumtaskblock[i-1][j];
                        } else {
                            Asumtaskblock[i][j] = A[i][j] + Asumtaskblock[i-1][j] + Asumtaskblock[i][j-1] - Asumtaskblock[i-1][j-1];
                        }
                    }
                }
            }

            // Compute top row of blocks
            int bj = 0, i = 0, j = 0;
            for (int bj = block_size; bj < N; bj += block_size) {
                #pragma omp task depend(in: Asumtaskblock[0:block_size][bj-block_size:block_size]) \
                              depend(out: Asumtaskblock[0:block_size][bj:block_size])
                {
                    int j_end = (bj + block_size < N) ? (bj + block_size) : N;
                    for (i = 0; i < block_size && i < N; i++) {
                        for (j = bj; j < j_end; j++) {
                            if (i == 0) {
                                Asumtaskblock[i][j] = A[i][j] + Asumtaskblock[i][j-1];
                            } else {
                                Asumtaskblock[i][j] = A[i][j] + Asumtaskblock[i-1][j] + Asumtaskblock[i][j-1] - Asumtaskblock[i-1][j-1];
                            }
                        }
                    }
                }
            }

            // Compute left column of blocks
            for (int bi = block_size; bi < N; bi += block_size) {
                #pragma omp task depend(in: Asumtaskblock[bi-block_size:block_size][0:block_size]) \
                              depend(out: Asumtaskblock[bi:block_size][0:block_size])
                {
                    int i_end = (bi + block_size < N) ? (bi + block_size) : N;
                    for (i = bi; i < i_end; i++) {
                        for (j = 0; j < block_size && j < N; j++) {
                            if (j == 0) {
                                Asumtaskblock[i][j] = A[i][j] + Asumtaskblock[i-1][j];
                            } else {
                                Asumtaskblock[i][j] = A[i][j] + Asumtaskblock[i-1][j] + Asumtaskblock[i][j-1] - Asumtaskblock[i-1][j-1];
                            }
                        }
                    }
                }
            }

            // Compute interior blocks
            for (int bi = block_size; bi < N; bi += block_size) {
                for (int bj = block_size; bj < N; bj += block_size) {
                    #pragma omp task depend(in: Asumtaskblock[bi-block_size:block_size][bj:block_size], \
                                            Asumtaskblock[bi:block_size][bj-block_size:block_size]) \
                                  depend(out: Asumtaskblock[bi:block_size][bj:block_size])
                    {
                        int i_end = (bi + block_size < N) ? (bi + block_size) : N;
                        int j_end = (bj + block_size < N) ? (bj + block_size) : N;
                        for (i = bi; i < i_end; i++) {
                            for (j = bj; j < j_end; j++) {
                                Asumtaskblock[i][j] = A[i][j] + Asumtaskblock[i-1][j] + Asumtaskblock[i][j-1] - Asumtaskblock[i-1][j-1];
                            }
                        }
                    }
                }
            }
        }
    }
}

int main() {
    int i, j;
    int thread_counts[] = {4, 8, 16, 32, 64};
    int block_size = 64;

    for (int t = 0; t < sizeof(thread_counts) / sizeof(int); t++) {
        
        int num_threads = thread_counts[t];
        omp_set_num_threads(num_threads);
        printf("Block size: %d, Threads: %d, N: %d\n", block_size, num_threads, N);
        double serial_time = 0, tasks_time = 0, tasks_blocks_time = 0;

        // Initialize matrix
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                A[i][j] = i * N + j;
                Asum[i][j] = -99999;
                Asumtask[i][j] = -99999;
                Asumtaskblock[i][j] = -99999;
            }
        }

        // Compute serially and measure time
        double start_time = omp_get_wtime();
        compute_serial();
        double end_time = omp_get_wtime();
        serial_time = end_time - start_time;
        printf("Serial computation time: %f seconds\n", serial_time);

        // Compute with OpenMP tasks and measure time
        start_time = omp_get_wtime();
        compute_tasks();
        end_time = omp_get_wtime();
        tasks_time = end_time - start_time;
        printf("OpenMP tasks computation time: %f seconds\n", tasks_time);

        // Compute with OpenMP tasks and blocks and measure time
        start_time = omp_get_wtime();
        compute_tasks_blocks(block_size);
        end_time = omp_get_wtime();
        tasks_blocks_time = end_time - start_time;
        printf("OpenMP tasks with blocks computation time: %f seconds\n", tasks_blocks_time);

        // Compare matrices obtained with the three functions
        int mismatch_count = 0;
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                if (Asum[i][j] != Asumtask[i][j] || Asum[i][j] != Asumtaskblock[i][j]) {
                    if (mismatch_count < 5) {  // Print only the first 5 mismatches
                        printf("Mismatch at i=%d, j=%d: Asum=%d, Asumtask=%d, Asumtaskblock=%d\n", 
                            i, j, Asum[i][j], Asumtask[i][j], Asumtaskblock[i][j]);
                    }
                    mismatch_count++;
                }
            }
        }
        if (mismatch_count == 0) {
            printf("All matrices match!\n");
        } else {
            printf("Matrices do not match. Total mismatches: %d\n", mismatch_count);
        }

        printf("\n");
    }

    return 0;
}