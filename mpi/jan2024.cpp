#include <mpi.h>
#include <iostream>
#include <cmath>
using namespace std;

#define k 3
#define n 3

void printMatrix(int mat[k][n])
{
    for (int i = 0; i < k; i++)
    {
        for (int j = 0; j < n; j++)
        {
            cout << mat[i][j] << " ";
        }
        cout << endl;
    }
}

int main(int argc, char* argv[])
{

    int rank, p;
    int A[k][n], b[n], d[k];
    int* local_d;

    local_d = (int*)malloc(n * sizeof(int));

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Status status;

    int numOfRows = (int)pow(2, rank);
    // array of n elements with value 0
    int* res = (int*)calloc(numOfRows, sizeof(int));

    if (rank == 0)
    {
        for (int i = 0; i < k; i++)
        {
            for (int j = 0; j < n; j++)
            {
                A[i][j] = i + 1;
            }
        }

        for (int j = 0; j < n; j++)
        {
            b[j] = 1;
            d[j] = 0;
        }

        printMatrix(A);
    }

    MPI_Bcast(b, n, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        for (int i = 1; i < p; i++)
        {
            MPI_Send(&A[i][0], pow(2, i) * n, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    }

    int* rowsA;
    rowsA = (int*)calloc(numOfRows * n, sizeof(int));

    // samo primam
    if (rank == 0)
    {
        memcpy(rowsA, A, n * sizeof(int));
    }
    else
    {
        MPI_Recv(&rowsA[0], numOfRows * n, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    }

    int* localRes = (int*)malloc(k * sizeof(int));
    for (int i = 0; i < k; i++)
    {
        localRes[i] = 0;
    }

    for (int i = 0; i < numOfRows; i++)
    {
        for (int j = 0; j < n; j++)
        {
            localRes[i + (numOfRows - 1)] += rowsA[i * n + j] * b[j];
        }
    }

    MPI_Reduce(localRes, d, k, MPI_INT, MPI_SUM, p - 1, MPI_COMM_WORLD);

    if (rank == p - 1)
    {
        for (int i = 0; i < k; i++)
        {
            cout << d[i] << " ";
        }
    }

    MPI_Finalize();
    return 0;
}