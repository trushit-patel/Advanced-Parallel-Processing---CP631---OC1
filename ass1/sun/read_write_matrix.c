#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

void read_matrix(char *filename, int n, int p, int *local_block, int my_rank);
void write_matrix(char *filename, int n, int p, int *local_block, int my_rank);
void print_buffer(int *row_buffer, int size);

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    double start_time = MPI_Wtime();

    //get the number of processes as argument
    int n = atoi(argv[1]); // Matrix size
    int p, my_rank;

    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    //I am chekcing the divisibility of matrix size by processes
    if (n % p != 0)
    {
        printf("Matrix size should be divisible by number of processes");
        return 0;
    }

    // store incoming blocks in row-major order buffer
    int *local_block = (int *)malloc((n * n / p) * sizeof(int));

    // read and distribute matrix
    read_matrix("input_matrix_1000.txt", n, p, local_block, my_rank);

    // collect and write matrix
    write_matrix("output_matrix_1000.txt", n, p, local_block, my_rank);

    //deallocating the space of buffer as we no longer need it 
    free(local_block);

    double end_time = MPI_Wtime();

    if (my_rank == 0)
    {
        printf("Total execution time: %f seconds\n", end_time - start_time);
    }

    MPI_Finalize();
    return 0;
}

void read_matrix(char *filename, int n, int p, int *local_block, int my_rank)
{
    int i = 0, j = 0, k = 0;
    int buffer_counter_1 = 0;
    int step_size = n / p, current_step = 0;
    int block_size = step_size * step_size;

    if (my_rank == 0)
    {
        //Opening the file in read mode
        FILE *file = fopen(filename, "r");
        
        if (file == NULL)
        {
            fprintf(stderr, "Error opening file: %s\n", filename);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        // read the row block p times
        while (current_step < p)
        {
            //row block to be divided between processes
            int *row_buffer = (int *)malloc(n * step_size * sizeof(int));

            // number of lines to read
            for (i = 0; i < step_size; i++)
            {
                // number of words/number to read per line
                for (j = 0; j < n; j++)
                {
                    if (fscanf(file, "%d", &row_buffer[i * n + j]) != 1)
                    {
                        fprintf(stderr, "Error reading file: %s\n", filename);
                        fclose(file);
                        MPI_Abort(MPI_COMM_WORLD, 1);
                    }
                }
            }

            //loop for every processes to send
            for (k = 0; k < p; k++)
            {
                //if its process zero save it in local buffer rather than sending it
                //one more thing I am not combining both logic in the inner most loop 
                //to avoid memmory allocation of send_buffer for process 0 as it would be a waste of space
                if (k == 0)
                {
                    // add the send buffer part to local buffer for process 0
                    for (i = 0; i < step_size; i++)
                    {
                        for (j = 0; j < step_size; j++)
                        {
                            local_block[buffer_counter_1++] = row_buffer[i * n + j];
                        }
                    }
                }
                else
                {

                    int *send_buffer = (int *)malloc(block_size * sizeof(int));
                    for (i = 0; i < step_size; i++)
                    {
                        for (j = 0; j < step_size; j++)
                        {
                            send_buffer[i * step_size + j] = row_buffer[(i * n + j) + k * step_size];// the index displacement would be equal to step_size everytime
                        }
                    }

                    //send it to all processes
                    MPI_Send(send_buffer, block_size, MPI_INT, k, k, MPI_COMM_WORLD);
                    
                    //deallocate
                    free(send_buffer);
                }
            }

            current_step++;
        }
        fclose(file);
    }
    else
    {
        // Receive the block for this process
        for (k = 0; k < p; k++)
        {
            int *recv_block = (int *)malloc(block_size * sizeof(int));
            MPI_Recv(recv_block, block_size, MPI_INT, 0, my_rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            for (i = 0; i < block_size; i++)
            {
                local_block[buffer_counter_1++] = recv_block[i];
            }

            free(recv_block);
        }
    }
}

//it is worth to note that the logic of write is polar oposite to that of read
void write_matrix(char *filename, int n, int p, int *local_block, int my_rank)
{
    int i = 0, j = 0, k = 0;
    int buffer_counter_1 = 0;
    int step_size = n / p, current_step = 0;
    int block_size = step_size * step_size;

    if (my_rank == 0)
    {
        // Open the file using standard I/O functions
        FILE *file = fopen(filename, "w");

        if (file == NULL)
        {
            fprintf(stderr, "Error opening file: %s\n", filename);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        while (current_step < p)
        {
            int *row_buffer = (int *)malloc(n * step_size * sizeof(int));

            for (k = 0; k < p; k++)
            {
                if (k == 0)
                {
                    for (i = 0; i < step_size; i++)
                    {
                        for (j = 0; j < step_size; j++)
                        {
                            row_buffer[i * n + j] = local_block[buffer_counter_1++];
                        }
                    }
                }
                else
                {
                    int *recv_buffer = (int *)malloc(block_size * sizeof(int));
                    MPI_Recv(recv_buffer, block_size, MPI_INT, k, k, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    for (i = 0; i < step_size; i++)
                    {
                        for (j = 0; j < step_size; j++)
                        {
                            row_buffer[(i * n + j) + k * step_size] = recv_buffer[i * step_size + j];
                        }
                    }
                    free(recv_buffer);
                }
            }

            for (j = 0; j < n * step_size;)
            {
                fprintf(file, "%d ", row_buffer[j]);
                j++;
                if (j % n == 0)
                    fprintf(file, "\n");
            }

            current_step++;
        }
        fclose(file);
    }
    else
    {
        // Receive the block for this process
        for (k = 0; k < p; k++)
        {
            int *send_block = (int *)malloc(block_size * sizeof(int));

            for (i = 0; i < block_size; i++)
            {
                send_block[i] = local_block[buffer_counter_1++];
            }

            MPI_Send(send_block, block_size, MPI_INT, 0, my_rank, MPI_COMM_WORLD);

            free(send_block);
        }
    }
}

//utill for debugging
void print_buffer(int *row_buffer, int size)
{
    int i = 0;
    for (i = 0; i < size; i++)
    {
        printf("row_buffer[%d] == %d \n", i, row_buffer[i]);
    }
}