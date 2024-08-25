#include <mpi.h>
#include <iostream>
using namespace std;
//Napisati MPI program koji pronalazi proizvod matrice Anxn i vektora
//bn.Matrica A i vector b se inicijalizuju u procesu 0. Izracunavanje se
//obavlja tako što se svakom procesu distribuira po vrsta matrice A i ceo
//vektor b.Svi procesi ucestvuju u izracunavanju.Rezultat se prokazuje u
//procesu 0.

#define n 3

int main(int argc, char* argv[]) {


	int rank, size;
	int A[n][n], b[n];
	int rowA[n], resultRow, result[n];

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (rank == 0) {
		for (int i = 0; i < n; i++) {
			b[i] = i;
			for (int j = 0; j < n; j++) {
				A[i][j] = i;
			}
		}
	}

	MPI_Scatter(&A[0][0], n, MPI_INT, &rowA, n, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(b, n, MPI_INT, 0, MPI_COMM_WORLD);

	resultRow = 0;
	for (int i = 0; i < n; i++)
		resultRow += rowA[i] * b[i];

	MPI_Gather(&resultRow, 1, MPI_INT, &result[0], 1, MPI_INT, 0, MPI_COMM_WORLD);

	if (rank == 0)
		for (int i = 0; i < n; i++)
			cout << result[i] << " ";

	MPI_Finalize();
	return 0;
}