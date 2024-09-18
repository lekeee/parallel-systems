%%writefile hello.cu

#include <iostream>
#include <cuda_runtime.h>

#define n 3
#define PI 3.14159

struct Kruznica {
    int xi;
    int yi;
    int r;
};

__global__ void findMax(Kruznica *d_array, int *d_maxIdx) {
    int tid = threadIdx.x + blockIdx.x * blockDim.x;
    __shared__ float sharedAreas[128]; 
    __shared__ int sharedIdx[128];    

    if (tid < n) {
        sharedAreas[threadIdx.x] = PI * d_array[tid].r * d_array[tid].r;
        sharedIdx[threadIdx.x] = tid;
    } else {
        sharedAreas[threadIdx.x] = 0;
        sharedIdx[threadIdx.x] = -1;
    }

    __syncthreads();

    for (int stride = blockDim.x / 2; stride > 0; stride /= 2) {
        if (threadIdx.x < stride && sharedAreas[threadIdx.x] < sharedAreas[threadIdx.x + stride]) {
            sharedAreas[threadIdx.x] = sharedAreas[threadIdx.x + stride];
            sharedIdx[threadIdx.x] = sharedIdx[threadIdx.x + stride];
        }
        __syncthreads();
    }

    if (threadIdx.x == 0) {
        atomicMax(d_maxIdx, sharedIdx[0]);
    }
}

int main() {

    Kruznica array[n] = {
        {1, 2, 3},
        {4, 5, 4},
        {10, 1, 1}
    };

    Kruznica *d_array;
    int *d_maxIdx;
    int maxIdx = 0;

    size_t arrayBytes = n * sizeof(Kruznica);

    cudaMalloc(&d_array, arrayBytes);
    cudaMalloc(&d_maxIdx, sizeof(int));


    cudaMemcpy(d_array, array, arrayBytes, cudaMemcpyHostToDevice);
    cudaMemcpy(d_maxIdx, &maxIdx, sizeof(int), cudaMemcpyHostToDevice);

    int threadNum = 128;
    int blockNum = (n + threadNum - 1) / threadNum;

    findMax<<<blockNum, threadNum>>>(d_array, d_maxIdx);

    cudaMemcpy(&maxIdx, d_maxIdx, sizeof(int), cudaMemcpyDeviceToHost);

    std::cout << "Kruznica sa najvecom povrsinom: \n";
    std::cout << "Centar: (" << array[maxIdx].xi << ", " << array[maxIdx].yi << ")\n";
    std::cout << "Poluprecnik: " << array[maxIdx].r << "\n";
    std::cout << "Povrsina: " << PI * array[maxIdx].r * array[maxIdx].r << "\n";

    cudaFree(d_array);
    cudaFree(d_maxIdx);

    return 0;
}
