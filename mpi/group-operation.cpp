#include <mpi.h>
#include <iostream>
using namespace std;
//Svaki od N procesa sadrži 30 realnih brojeva.Napisati MPI
//program koji nalazi maksimalnu vrednost na svakoj od 30 lokacija, kao
//i identifikator procesa koji sadrži tu vrednost.

	struct {
		double value;
		int rank;
	}in[30], out[30];

int main(int argc, char* argv[]) {


	int rank, size, value;

	MPI_Status status;
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	for (int i = 0; i < 30; i++) {
		in[i].value = double(rank + i);
		in[i].rank = rank;
	}

	MPI_Reduce(&in, &out, 30, MPI_DOUBLE_INT, MPI_MAXLOC, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		for (int i = 0; i < 30; i++) {
			cout << "Out vrednost:" << out[i].value << "rank" << out[i].rank << endl;
		}
	}

	MPI_Finalize();
	return 0;
}