%% writefile hello.cu

#include <iostream>
#include <cuda_runtime.h>

using namespace std;

__global__ void findShortestSides(int* d_A, int* d_B, int n) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < n) {

        int a = d_A[3 * idx];
        int b = d_A[3 * idx + 1];
        int c = d_A[3 * idx + 2];


        d_B[idx] = min(a, min(b, c));
    }
}


__global__ void findMaxSide(int* d_B, int* d_maxSide, int n) {
    __shared__ int shared_max[256];
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int threadId = threadIdx.x;


    shared_max[threadId] = (idx < n) ? d_B[idx] : INT_MIN;
    __syncthreads();

    for (int stride = blockDim.x / 2; stride > 0; stride >>= 1) {
        if (threadId < stride && idx + stride < n) {
            shared_max[threadId] = max(shared_max[threadId], shared_max[threadId + stride]);
        }
        __syncthreads();
    }

    if (threadId == 0) {
        atomicMax(d_maxSide, shared_max[0]);
    }
}

int main() {
    int n;
    cout << "Unesi broj trouglova: ";
    cin >> n;

    int* A = new int[3 * n]; 
    int* B = new int[n];     


    cout << "Unesi stranice trouglova: ";
    for (int i = 0; i < 3 * n; i++) {
        cin >> A[i];
    }


    int* d_A, * d_B, * d_maxSide;
    cudaMalloc(&d_A, 3 * n * sizeof(int));
    cudaMalloc(&d_B, n * sizeof(int));
    cudaMalloc(&d_maxSide, sizeof(int));

    cudaMemcpy(d_A, A, 3 * n * sizeof(int), cudaMemcpyHostToDevice);


    int maxSide = 0;
    cudaMemcpy(d_maxSide, &maxSide, sizeof(int), cudaMemcpyHostToDevice);

    int threadsPerBlock = 256;
    int blocksPerGrid = (n + threadsPerBlock - 1) / threadsPerBlock;

  
    findShortestSides << <blocksPerGrid, threadsPerBlock >> > (d_A, d_B, n);

    findMaxSide << <blocksPerGrid, threadsPerBlock >> > (d_B, d_maxSide, n);

    cudaMemcpy(&maxSide, d_maxSide, sizeof(int), cudaMemcpyDeviceToHost);


    cout << "Najveca od najkracih stranica je: " << maxSide << endl;


    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_maxSide);
    delete[] A;
    delete[] B;

    return 0;
}

