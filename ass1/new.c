#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

void read_matrix(char * filename, int n, int p, int * local_block, int my_rank);
void write_matrix(char * filename, int n, int p, int * local_block, int my_rank);
void print_buffer(int * row_buffer, int size);

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int n = 16; // Matrix size
    int p, my_rank;

    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if(n % p != 0) {
        printf("Matrix size should be divisible by number of processes");
        return 0;
    }

    int *local_block = (int *)malloc((n * n / p) * sizeof(int));
    
    read_matrix("input_matrix.txt", n, p, local_block, my_rank);
    //write_matrix("output_matrix.txt", n, p, local_block, my_rank);

    free(local_block);

    MPI_Finalize();
    return 0;
}

void read_matrix(char * filename, int n, int p, int * local_block, int my_rank){

    int i = 0, j = 0, k = 0;
    int buffer_counter_1 = 0;
    int step_size = n / p, current_step = 0;
    int block_size = step_size * step_size;

    MPI_Status status;
    MPI_Datatype my_col;
    MPI_Type_vector(block_size, step_size, n, MPI_INT, &my_col);
    MPI_Type_commit(&my_col);

    if(my_rank == 0){

        // Open the file using standard I/O functions
        
        FILE *file = fopen(filename, "r");
        // FILE *file = fopen(filename, "rb");
        if (file == NULL)
        {
            fprintf(stderr, "Error opening file: %s\n", filename);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        long int file_pos = 0;

        while(current_step < p){
            int *row_buffer = (int *)malloc(n * step_size * sizeof(int));
            
            fseek(file, file_pos, SEEK_SET);
            for(i = 0; i < step_size; i++){
                for(j = 0; j < n; j++){
                    if (fscanf(file, "%d", &row_buffer[i * n + j]) != 1){
                        fprintf(stderr, "Error reading file: %s\n", filename);
                        fclose(file);
                        MPI_Abort(MPI_COMM_WORLD, 1);
                    }
                    //printf("row_buffer[%d] == %d \n",i * n + j , row_buffer[i * n + j]);
                }
            }

            // fread(row_buffer, sizeof(int), step_size*n, file);

            for(k = 0 ; k < step_size ; k++){
                if(k == 0){
                    for(i = 0 ; i < step_size ; i++){
                        for(j = 0 ; j < step_size ; j++){
                            local_block[buffer_counter_1++] = row_buffer[i * n + j];
                        }
                    }
                }
                else{
                    
                    int *send_buffer = (int *)malloc(block_size * sizeof(int));
                    for(i = 0 ; i < step_size ; i++){
                        for(j = 0 ; j < step_size ; j++){
                            send_buffer[i * step_size + j] = row_buffer[(i * n + j) + k * step_size];
                            // if(k == 1) printf("buffer [%d] == %d \n",(i * n + j) + k * step_size , row_buffer[(i * n + j) + k * step_size]);
                        }
                    }

                    // if(k == 1) print_buffer(send_buffer, block_size);
                    MPI_Send(send_buffer, block_size, MPI_INT, k, k, MPI_COMM_WORLD);
                    free(send_buffer);

                    // for(j=1;j<p;j++){
                    //     MPI_Send(&(row_buffer[j*step_size]), 1, my_col, j, current_step, MPI_COMM_WORLD);          
                    // }
                }
                
            }

            // print_buffer(row_buffer, step_size * n);
            printf("-----------\n");
            free(row_buffer);
            file_pos = ftell(file);
            current_step++;
        }
    }
    else{
        //write the recieve logic
        
        // Receive the block for this process
        for (int k = 0; k < step_size ; k++)
        {
            int *recv_block = (int *)malloc(block_size * sizeof(int));
            MPI_Recv(recv_block, block_size, MPI_INT, 0, my_rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for (int i = 0; i < block_size; i++){
                local_block[buffer_counter_1++] = recv_block[i];
            }

            if(k == 1) print_buffer(recv_block, block_size);
            free(recv_block);
        }
        
        // print_buffer(local_block, step_size * n);
        free(local_block);
    }
}

void write_matrix(char * filename, int n, int p, int * local_block, int my_rank){

    
}

void print_buffer(int * row_buffer, int size){
    int i = 0;
    for(i = 0; i < size; i++){
        printf("row_buffer[%d] == %d \n",i, row_buffer[i]);
    }
}