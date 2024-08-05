#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int send_data = rank; // Each process sends its rank
    int recv_data;

    int next = (rank + 1) % size; // Next process in the ring
    int prev = (rank - 1 + size) % size; // Previous process in the ring

    printf("Process %d sending data %d to next process %d\n", rank, send_data, next);
    MPI_Send(&send_data, 1, MPI_INT, next, 0, MPI_COMM_WORLD);
    
    printf("Process %d receiving data %d from prev process %d\n", rank, recv_data, prev);
    MPI_Recv(&recv_data, 1, MPI_INT, prev, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    
    
    
    
    printf("Process %d receiving data %d from next process %d\n", rank, recv_data, next);
    MPI_Recv(&recv_data, 1, MPI_INT, next, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    
    printf("Process %d sending data %d to prev process %d\n", rank, send_data, prev);
    MPI_Send(&send_data, 1, MPI_INT, prev, 0, MPI_COMM_WORLD);


    MPI_Finalize();
    return 0;
}
