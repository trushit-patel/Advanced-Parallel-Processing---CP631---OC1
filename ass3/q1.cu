// Question 1

#include <sys/time.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <unistd.h>

// Number of threads in one block (possible range is 32...1024):
#define blockSize 256

// Total number of threads (total number of elements to process in the kernel):
#define N 256 * 128

// Maximum value of distance
#define MAX_DIST 1.42

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define SQUARE(X) ((X) * (X))

// Input array (global host memory):
float X[N];
float Y[N];

__device__ float d_X[N];
__device__ float d_Y[N];
__device__ float d_min_k1;
__device__ float d_min_k2;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static inline __device__ float fatomicMin(float *addr, float value)
{
    float old = *addr, assumed;
    if (old <= value)
        return old;
    do
    {
        assumed = old;
        old = atomicCAS((unsigned int *)addr, __float_as_int(assumed), __float_as_int(value));
    } while (old != assumed);

    return old;
}

__global__ void init_kernel()
{
    d_min_k1 = MAX_DIST;
    d_min_k2 = MAX_DIST;

    return;
}

// one tread per particle
__global__ void OneThreadPerParticleKernel()
{

    __shared__ float shared_min[blockSize];

    int i = threadIdx.x + blockDim.x * blockIdx.x;
    float thread_min = MAX_DIST, next_dist = MAX_DIST;

    // calculate the distance related to i and find the min of current thread
    for (int j = i + 1; j < N; j++)
    {
        next_dist = sqrt(SQUARE(d_X[i] - d_X[j]) + SQUARE(d_Y[i] - d_Y[j]));
        thread_min = (next_dist < thread_min) ? next_dist : thread_min;
    }
    shared_min[threadIdx.x] = thread_min;

    // To make sure all threads finished calc
    __syncthreads();

    // find the min within the block
    int nTotalThreads = blockDim.x; // Total number of active threads;
    // only the first half of the threads will be active.
    while (nTotalThreads > 1)
    {
        int halfPoint = (nTotalThreads >> 1); // divide by two
        if (threadIdx.x < halfPoint)
        {
            int thread2 = threadIdx.x + halfPoint;
            shared_min[threadIdx.x] = MIN(shared_min[threadIdx.x], shared_min[thread2]); // Pairwise summation
        }
        __syncthreads();
        nTotalThreads = halfPoint; // Reducing the binary tree size by two
    }

    // find the min among blocks, i.e. global min
    if (threadIdx.x == 0)
    {
        fatomicMin(&d_min_k1, shared_min[0]);
    }

    return;
}

__global__ void OneThreadPerPairKernel()
{
    __shared__ float shared_min[blockSize];

    int k = threadIdx.x + blockDim.x * blockIdx.x;
   
    int row = k / (N - 1);
    int col = row + 1 + (k % (N - 1));

    int i = (col > N - 1) ? N - 1 - row : row;
    int j = (col > N - 1) ? col - row : col;

    // calculate the distance related to i and find the min of current thread
    shared_min[threadIdx.x] = sqrt(SQUARE(d_X[i] - d_X[j]) + SQUARE(d_Y[i] - d_Y[j]));

    // To make sure all threads finished calc
    __syncthreads();

    // find the min within the block
    int nTotalThreads = blockDim.x; // Total number of active threads;
    // only the first half of the threads will be active.
    while (nTotalThreads > 1)
    {
        int halfPoint = (nTotalThreads >> 1); // divide by two
        if (threadIdx.x < halfPoint)
        {
            int thread2 = threadIdx.x + halfPoint;
            shared_min[threadIdx.x] = MIN(shared_min[threadIdx.x], shared_min[thread2]); // Pairwise summation
        }
        __syncthreads();
        nTotalThreads = halfPoint; // Reducing the binary tree size by two
    }

    // find the min among blocks, i.e. global min
    if (threadIdx.x == 0)
    {
        fatomicMin(&d_min_k2, shared_min[0]);
    }

    return;
}

int main(int argc, char **argv)
{
    double min0;
    float next_dist, min_k1, min_k2;
    int numBlocks;

    // Initializing random number generator:
    srand(235829410);

    // Initializing the input array:
    for (int i = 0; i < N; i++)
    {
        X[i] = (float)rand() / (float)RAND_MAX;
        Y[i] = (float)rand() / (float)RAND_MAX;
    }

    //----------------------------------CPU-------------------------------------------
    // time_t begin_cpu, end_cpu;

    clock_t begin_cpu = clock();
    // time(&begin_cpu);

    min0 = MAX_DIST;
    next_dist = MAX_DIST;
    for (int i = 0; i < N; i++)
    {
        for (int j = i + 1; j < N; j++)
        {
            next_dist = sqrt(SQUARE(X[i] - X[j]) + SQUARE(Y[i] - Y[j]));
            min0 = (next_dist < min0) ? next_dist : min0;
        }
    }

    clock_t end_cpu = clock();
    // time(&end_cpu);

    double cpu_time = double(end_cpu - begin_cpu)/CLOCKS_PER_SEC;

    printf("------------------------- N  =  %d ------------------------- \n", N);

    printf("---------------------------------------\n");
    printf("Serial code on CPU\n");
    printf("CPU Time: %.7f ms \n", cpu_time * 1000);
    printf("CPU distance: %.7f \n", min0);
    printf("---------------------------------------\n");

    //------------------------------------INIT KERNELS-----------------------------------

    cudaMemcpyToSymbol(d_X, X, N * sizeof(float), 0, cudaMemcpyHostToDevice);
    cudaMemcpyToSymbol(d_Y, Y, N * sizeof(float), 0, cudaMemcpyHostToDevice);

    init_kernel<<<1, 1>>>();
    cudaDeviceSynchronize();

    //------------------------------------GPU 2-----------------------------------------
    cudaEvent_t start, stop;
    float kernel_timer;

    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start, 0);

    numBlocks = N / blockSize;
    OneThreadPerParticleKernel<<<numBlocks, blockSize>>>();

    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&kernel_timer, start, stop);

    cudaMemcpyFromSymbol(&min_k1, d_min_k1, sizeof(float), 0, cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();

    printf("--------------------------------------\n");
    printf("One thread per particle \n");
    printf("GPU kernel 1 took %.7f ms\n", kernel_timer);
    printf("GPU kernel 1 distance: %.7f \n", min_k1);
    printf("--------------------------------------\n");

    //------------------------------------GPU 2----------------------------------------
    cudaEvent_t start_2, stop_2;
    float kernel_timer_2;

    cudaEventCreate(&start_2);
    cudaEventCreate(&stop_2);
    cudaEventRecord(start_2, 0);

    numBlocks = N / blockSize / 2 * (N - 1);
    OneThreadPerPairKernel<<<numBlocks, blockSize>>>();

    cudaEventRecord(stop_2, 0);
    cudaEventSynchronize(stop_2);
    cudaEventElapsedTime(&kernel_timer_2, start_2, stop_2);

    cudaMemcpyFromSymbol(&min_k2, d_min_k2, sizeof(float), 0, cudaMemcpyDeviceToHost);
    cudaDeviceSynchronize();

    printf("----------------------------------------\n");
    printf("One thread per pair\n");
    printf("GPU kernel 2 took %.7f ms\n", kernel_timer_2);
    printf("GPU kernel 2 distance: %.7f \n", min_k2);
    printf("----------------------------------------\n");

    return 0;
}