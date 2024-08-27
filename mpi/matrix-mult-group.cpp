#include <mpi.h>
#include <iostream>
using namespace std;
//Napisati MPI program koji realizuje množenje matrica A i B reda
//n, čime se dobija rezultujuća matrica C = A * B.Množenje se obavlja
//tako što master proces(sa identifikatorom 0) šalje svakom
//procesu radniku jednu kolonu matrice A i jednu vrstu matrice B.
//Master proces ne učestvuje u izračunavanju.Štampati dobijenu
//matricu.

#define n 3

int main(int argc, char* argv[]) {

	int rank, p, mat_rank;
	int A[n][n], B[n][n], C[n][n];
	int columnA[n], rowB[n], localC[n][n];

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	MPI_Group mat, world;
	MPI_Comm com; // communicator excl process 0
	MPI_Datatype column;
	MPI_Status status;

	int nomembers[1] = { 0 };


	MPI_Comm_group(MPI_COMM_WORLD, &world);
	MPI_Group_excl(world, 1, nomembers, &mat);
	MPI_Group_rank(mat, &mat_rank);
	MPI_Comm_create(MPI_COMM_WORLD, mat, &com);

	MPI_Type_vector(n, 1, n, MPI_INT, &column);
	MPI_Type_commit(&column);

	if (rank == 0) {
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				A[i][j] = 3 * i + j + 1;
				B[i][j] = i + 1;
			}
		}

		for (int i = 0; i < n; i++) {
			MPI_Send(&A[0][i], 1, column, i + 1, 1, MPI_COMM_WORLD);
			MPI_Send(&B[i][0], n, MPI_INT, i + 1, 2, MPI_COMM_WORLD);
		}

	}
	else {
		MPI_Recv(&columnA, n, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
		MPI_Recv(&rowB, n, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);

		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
				localC[i][j] = columnA[i] * rowB[j];
		}

		MPI_Reduce(&localC, &C, n * n, MPI_INT, MPI_SUM, 0, com);
	}

	if (mat_rank == 0) {
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
				cout << C[i][j] << " ";
			cout << endl;
		}
	}

	MPI_Finalize();
	return 0;
}