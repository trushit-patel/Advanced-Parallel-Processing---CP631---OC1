#include "mpi.h"
#include "stdio.h"
#include "limits.h"
#include <string.h>

char* concat(const char *s1, const char *s2);

int main(int argc, char *argv[])
{
    int my_rank;       /* rank of process      */
    int p;             /* number of processes  */
    int data_size = 1;
    char *str = (char*)malloc(2); // Initial buffer for "t"
    strcpy(str, "t");
    
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    MPI_Comm_size(MPI_COMM_WORLD, &p);
    
    if(p > 2) MPI_Finalize();
    
    while(1){
        printf("Process %d: Sending message '%s' of size %d\n", my_rank, str, data_size);
        MPI_Send(str, data_size, MPI_INT, p - my_rank - 1, 0 , MPI_COMM_WORLD);
        
        printf("Process %d: Receiving message '%s' of size %d\n", my_rank, str, data_size);
        MPI_Recv(str, data_size, MPI_INT, p - my_rank - 1, 0 , MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        data_size *= 2;
        char *newstr = concat(str, str);
        free(str);
        str = newstr;
    }
    
    MPI_Finalize();

    return 0;
}

char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}
