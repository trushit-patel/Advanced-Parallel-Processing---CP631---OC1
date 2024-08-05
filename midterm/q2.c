#include <stdio.h>
#include <string.h>
#include "mpi.h"
#include <stdlib.h>

#define NUMPROCS 4

int main(int argc, char* argv[])
{
    int rank, my_rank, p;
    int processdata[NUMPROCS], mydata[NUMPROCS];
    unsigned int randseed;
    MPI_Request send_requests[NUMPROCS];
    MPI_Request recv_requests[NUMPROCS];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    if (p != NUMPROCS) return 0;

    randseed = 10 * my_rank;
    srand(randseed);

    for (rank = 0; rank < p; rank++) mydata[rank] = rand();

    for (rank = 0; rank < p; rank++) {
        MPI_Isend(&mydata[rank], 1, MPI_INT, rank, 10, MPI_COMM_WORLD, &send_requests[rank]);
        MPI_Irecv(&processdata[rank], 1, MPI_INT, rank, 10, MPI_COMM_WORLD, &recv_requests[rank]);
    }

    MPI_Waitall(p, send_requests, MPI_STATUSES_IGNORE);
    MPI_Waitall(p, recv_requests, MPI_STATUSES_IGNORE);

    MPI_Finalize();
    return 0;
}