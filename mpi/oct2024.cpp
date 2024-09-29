#include <iostream>
#include <mpi.h>

using namespace std;

#define n 3
#define r 3
#define k 6

int main(int argc, char* argv[]) {

	int rank, p;
	int A[n][r], B[r][k];
	int* C = (int*)calloc(n * k, sizeof(int));

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	int* colsB = (int*)calloc(r * k / p, sizeof(int));
	int* lC = (int*)calloc(n * k, sizeof(int));

	MPI_Datatype vector, resized;
	int stride = p;
	if (k == p) stride = k;
	MPI_Type_vector(k / p * r, 1, stride, MPI_INT, &vector);
	MPI_Type_commit(&vector);
	MPI_Type_create_resized(vector, 0, 1 * sizeof(int), &resized);
	MPI_Type_commit(&resized);


	if (rank == 0) {
		for (int i = 0; i < n; i++)
			for (int j = 0; j < r; j++)
				A[i][j] = 1;
		for (int i = 0; i < r; i++)
			for (int j = 0; j < k; j++)
				B[i][j] = 1;
	}

	MPI_Bcast(A, n * r, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatter(B, 1, resized, colsB, k / p * r, MPI_INT, 0, MPI_COMM_WORLD);

	for (int i = 0; i < n; i++)
		for (int j = 0; j < k / p; j++) {
			lC[i * k + j * p + rank] = 0;
			for (int q = 0; q < r; q++)
				lC[i * k + j * p + rank] += A[i][q] * colsB[j + q * k / p];

		}

	MPI_Reduce(lC, C, n * k, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		cout << "[A]" << endl;
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < r; j++)
				cout << A[i][j] << " ";
			cout << endl;
		}

		cout << "[B]" << endl;
		for (int i = 0; i < r; i++) {
			for (int j = 0; j < k / p; j++)
				cout << colsB[i * k / p + j] << " ";
			cout << endl;
		}

		cout << "[C]" << endl;
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < k; j++)
				cout << C[i * k + j] << " ";
			cout << endl;
		}
	}

	MPI_Finalize();
	return 0;
}
