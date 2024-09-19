#include <iostream>
#include <mpi.h>
#include <cmath> // za sqrt
using namespace std;

#define n 20
#define m 2

int main(int argc, char* argv[]) {

	int numbers[n];
	int p, rank;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	int dim = sqrt(p);
	int br = 0;


	int** pids = new int* [dim];
	for (int i = 0; i < dim; i++) {
		pids[i] = new int[dim];
		for (int j = 0; j < dim; j++)
			pids[i][j] = br++;
	}

	MPI_Group zGroup, worldGroup;
	MPI_Comm_group(MPI_COMM_WORLD, &worldGroup);

	int numofmembers = 3 * sqrt(p) - 2;
	int* members = new int[numofmembers];

	br = 0;
	for (int i = 0; i < dim; i++) {
		for (int j = 0; j < dim; j++) {
			if (i == 0 || i == dim - 1 || i + j == dim - 1)
				members[br++] = pids[i][j];
		}
	}


	if (rank == 0) {
		for (int i = 0; i < n; i++)
			numbers[i] = i;

		for (int i = 0; i < numofmembers; i++)
			cout << members[i] << ' ';
		cout << endl;
	}


	int zRank;
	MPI_Group_incl(worldGroup, numofmembers, members, &zGroup);
	MPI_Group_rank(zGroup, &zRank);
	MPI_Comm zComm;
	MPI_Comm_create(MPI_COMM_WORLD, zGroup, &zComm);

	int localNumbers[m];


	if (zComm != MPI_COMM_NULL) {

		MPI_Scatter(numbers, m, MPI_INT, localNumbers, m, MPI_INT, 0, zComm);


		//cout << "Process" << zRank << ": ";
		int middle = 0;
		for (int i = 0; i < m; i++) {
			middle += localNumbers[i];
		}


		int finalMiddle = 0;
		MPI_Reduce(&middle, &finalMiddle, 1, MPI_INT, MPI_SUM, 0, zComm);

		if (zRank == 0) {
			cout << "Final middle: " << (float)finalMiddle / n << endl;
		}
	}


	MPI_Finalize();
	return 0;
}
