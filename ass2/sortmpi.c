#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 100000000

void send_exchange_size(double *arr, int p, int *send_counts, int *recv_counts);
void send_numbers(double *numbers, int *send_counts, int *recv_counts, double *new_local_arr, int size);
int compare(const void *a, const void *b);
void generate_random_numbers(double *arr, int count);
void print_array(double *arr, int count, int my_rank);

int main(int argc, char **argv)
{
    int my_rank, p;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    double start_time, end_time;
    if (my_rank == 0)
    {
        start_time = MPI_Wtime();
    }

    srand(time(NULL) + my_rank);

    double *local_arr = (double *)malloc(N * sizeof(double));
    generate_random_numbers(local_arr, N);
    
    print_array(local_arr, N, my_rank);
    printf("\n");
    
    qsort(local_arr, N, sizeof(double), compare);

    // Count how many elements each process will send to every other process
    int *send_counts = (int *)malloc(p * sizeof(int));
    int *recv_counts = (int *)malloc(p * sizeof(int));
    int i = 0;

    // Initialize send counts
    for (i = 0; i < p; i++)
    {
        send_counts[i] = 0;
        recv_counts[i] = 0;
    }

    send_exchange_size(local_arr, p, send_counts, recv_counts);

    // Determine total receive count
    int total_recv_count = 0;
    for (i = 0; i < p; i++)
    {
        total_recv_count += recv_counts[i];
    }

    double *new_local_arr = (double *)malloc(total_recv_count * sizeof(double));

    // Perform the actual data exchange
    send_numbers(local_arr, send_counts, recv_counts, new_local_arr, p);

    // Sort the array individually
    // After this all the array elements across all processes will be in sequential order
    qsort(new_local_arr, total_recv_count, sizeof(double), compare);

    printf("Sorted \n");
    print_array(new_local_arr, total_recv_count, my_rank);

    MPI_Barrier(MPI_COMM_WORLD);

    if (my_rank == 0)
    {
        end_time = MPI_Wtime();
        printf("\nExecution time: %f seconds\n", end_time - start_time);
    }

    free(local_arr);
    free(new_local_arr);
    free(send_counts);
    free(recv_counts);

    MPI_Finalize();
    return 0;
}

void send_exchange_size(double *arr, int p, int *send_counts, int *recv_counts)
{
    int i = 0;
    // Determine send counts based on value ranges
    for (i = 0; i < N; i++)
    {
        int to_process = (int)(arr[i] * p);
        if (to_process >= p)
            to_process = p - 1;
        send_counts[to_process]++;
    }

    // Exchange send counts to determine recv counts
    MPI_Alltoall(send_counts, 1, MPI_INT, recv_counts, 1, MPI_INT, MPI_COMM_WORLD);
}

void send_numbers(double *local_arr, int *send_counts, int *recv_counts, double *new_local_arr, int p)
{
    
    int *send_offset = (int *)malloc(p * sizeof(int));
    int *recv_offset = (int *)malloc(p * sizeof(int));

    // Calculate send and receive displacements
    send_offset[0] = recv_offset[0] = 0;

    int i = 0;
    for (i = 1; i < p; i++)
    {
        send_offset[i] = send_offset[i - 1] + send_counts[i - 1];
        recv_offset[i] = recv_offset[i - 1] + recv_counts[i - 1];
    }

    int total_recv_count = recv_offset[p - 1] + recv_counts[p - 1];

    MPI_Alltoallv(local_arr, send_counts, send_offset, MPI_DOUBLE, new_local_arr, recv_counts, recv_offset, MPI_DOUBLE, MPI_COMM_WORLD);

    free(send_offset);
    free(recv_offset);
}

// Conparator for qsort
int compare(const void *a, const void *b)
{
    if (*(double *)a < *(double *)b)
        return -1;
    if (*(double *)a > *(double *)b)
        return 1;
    return 0;
}

void generate_random_numbers(double *arr, int count)
{
    int i = 0;
    for (i = 0; i < count; i++)
    {
        arr[i] = (double)rand() / RAND_MAX;
    }
}

void print_array(double *arr, int count, int my_rank)
{
    printf("Process %d: ", my_rank);

    int i = 0;
    // Only 5 elements as N can be really big
    for (i = 0; i < (count > 5 ? 5 : count); i++)
    {
        printf("%f ", arr[i]);
    }

    if (count > 10)
    {
        printf("... ");
    }

    for (i = (count > 5 ? count - 5 : 0); i < count; i++)
    {
        if (i >= 5)
            printf("%f ", arr[i]);
    }

    printf("\n");
}