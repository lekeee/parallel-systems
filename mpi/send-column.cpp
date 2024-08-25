#include <mpi.h>
#include <iostream>
using namespace std;
//Napisati MPI program koji pronalazi proizvod matrice Amxn i vektora
//bn.Matrica A i vector b se inicijalizuju u procesu 0. Izracunavanje se
//obavlja tako sto se svakom procesu distribuira po kolona matrice A i po 1
//element vektora b.Za distribuciju kolona po procesima koristiti P - t - P
//operacije, za sve ostalo grupne operacije.Svi procesi ucestvuju u
//izracunavanju.Rezultat se prikazuje u procesu koji, nakon distribuiranja
//kolona matrice A, sadrži minimum svih elemenata matrice A.

#define m 3
#define n 3

struct {
	int value;
	int rank;
} inMin, outMin;

int main(int argc, char* argv[]) {

	int rank, p;
	int A[m][n], b[n];
	int columnA[m], elementB, x[m];
	int resColumn[m], result[m];

	MPI_Init(&argc, &argv);
	MPI_Status status;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	if (rank == 0) {
		for (int i = 0; i < m; i++) {
			b[i] = i;
			for (int j = 0; j < n; j++) {
				A[i][j] = i;
			}
		}

		for (int i = 0; i < m; i++)
			columnA[i] = A[i][0];

		for (int i = 1; i < p; i++) {
			for (int j = 0; j < m; j++)
				x[j] = A[j][i];
			MPI_Send(&x, m, MPI_INT, i, 0, MPI_COMM_WORLD);
		}
	}
	else {
		MPI_Recv(&columnA, m, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
	}

	MPI_Scatter(&b[0], 1, MPI_INT, &elementB, 1, MPI_INT, 0, MPI_COMM_WORLD);

	for (int i = 0; i < m; i++)
		resColumn[i] = columnA[i] * elementB;

	inMin.value = INT_MAX;
	for (int i = 0; i < m; i++)
		if(columnA[i] < inMin.value){
			inMin.value = columnA[i];
			outMin.rank = rank;
		}

	MPI_Reduce(&inMin, &outMin, 1, MPI_2INT, MPI_MINLOC, 0, MPI_COMM_WORLD);
	MPI_Bcast(&outMin, 1, MPI_2INT, 0, MPI_COMM_WORLD);

	MPI_Reduce(&resColumn, &result, m, MPI_INT, MPI_SUM, outMin.rank, MPI_COMM_WORLD);

	if (rank == outMin.rank) {
		for (int i = 0; i < m; i++) {
			cout << result[i] << " ";
		}
	}

	MPI_Finalize();
	return 0;
}