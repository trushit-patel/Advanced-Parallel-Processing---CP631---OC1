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

    MPI_Request send_req_next, send_req_prev, recv_req_next, recv_req_prev;
    MPI_Status status;

    // Use MPI_Isend and MPI_Irecv for non-blocking communication
    MPI_Irecv(&recv_data, 1, MPI_INT, prev, 0, MPI_COMM_WORLD, &recv_req_prev);
    printf("Process %d received data %d from prev process %d\n", rank, recv_data, prev);
    
    MPI_Isend(&send_data, 1, MPI_INT, next, 0, MPI_COMM_WORLD, &send_req_next);
    printf("Process %d sent data %d to next process %d\n", rank, send_data, next);

    
    MPI_Irecv(&recv_data, 1, MPI_INT, next, 0, MPI_COMM_WORLD, &recv_req_next);
    printf("Process %d received data %d from next process %d\n", rank, recv_data, next);
    
    MPI_Isend(&send_data, 1, MPI_INT, prev, 0, MPI_COMM_WORLD, &send_req_prev);
    printf("Process %d sent data %d to prev process %d\n", rank, send_data, prev);
    
    
    // Wait for the communication to complete
 
    MPI_Wait(&recv_req_next, &status);
    MPI_Wait(&send_req_next, &status);
    MPI_Wait(&recv_req_prev, &status);
    MPI_Wait(&send_req_prev, &status);
    
    printf("Process %d received data %d from process %d\n", rank, recv_data, prev);

    MPI_Finalize();
    return 0;
}
