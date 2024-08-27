#include <mpi.h>
#include <iostream>
using namespace std;
//Napisati program kojim se elementi gornje trougaone matrice Anxn
//procesa 0, šalju u gornju trougaonu matricu Tnxn
//procesa 1.

#define n 4

int main(int argc, char* argv[]) {

	int rank, p;
	int A[n][n], T[n][n];

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	MPI_Status status;

	int arr_lengths[n], strides[n];

	for (int i = 0; i < n; i++) {
		arr_lengths[i] = n - i;
		strides[i] = (n + 1) * i;
	}

	MPI_Datatype triangle;
	MPI_Type_indexed(n, arr_lengths, strides, MPI_INT, &triangle);
	MPI_Type_commit(&triangle);

	if (rank == 0) {
		for (int i = 0; i < n; i++)
			for (int j = 0; j < n; j++)
				A[i][j] = i + j;
		
		MPI_Send(&(A[0][0]), 1, triangle, 1, 0, MPI_COMM_WORLD);
	}
	if (rank == 1) {
		for (int i = 0; i < n; i++)
			for (int j = 0; j < n; j++)
				T[i][j] = 0;

		MPI_Recv(T, 1, triangle, 0, 0, MPI_COMM_WORLD, &status);

		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++)
				cout << T[i][j] << " ";
			cout << endl;
		}
	}

	MPI_Finalize();
	return 0;
}