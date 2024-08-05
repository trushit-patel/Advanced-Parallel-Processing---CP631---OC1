#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]) {

    MPI_Init(&argc, &argv);

    // Get the number of processes
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    //Get the rank of current process
    int my_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    //Calculate the new rank
    int new_rank = size - 1 - my_rank;

    // define new communicator
    MPI_Comm reversed_comm;
    MPI_Comm_split(MPI_COMM_WORLD, 0, new_rank, &reversed_comm);

    //add the process to it
    int reversed_rank;
    MPI_Comm_rank(reversed_comm, &reversed_rank);

    printf("Old rank: %d, New rank: %d\n", my_rank, reversed_rank);

    MPI_Finalize();
    return 0;
}



To achieve the above, I would perform the below steps -:

//Get the rank of the current process in the global communicator&nbsp;
my_rank = MPI_Comm_rank(MPI_COMM_WORLD)


2) //Calculate the new rank in the reversed communicator&nbsp;&nbsp;

new_rank = world_size - 1 - my_rank


3) //Create a new communicator based on the new ranks&nbsp;&nbsp;

reversed_comm = MPI_Comm_split(MPI_COMM_WORLD, 0, new_rank) 

4) //Get the rank in the new communicator&nbsp;&nbsp;

reversed_rank = MPI_Comm_rank(reversed_comm)