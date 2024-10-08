#include <mpi.h>
#include <iostream>
using namespace std;
//Napisati program koji uzima podatke od nultog procesa i šalje ih svim
//drugim procesima tako što proces i treba da primi podatke i pošalje ih procesu
//i + 1, sve dok se ne stigne do poslednjeg procesa.Unos podataka se završava
//nakon što se prenese negativna vrednost podatka.

int main(int argc, char* argv[]) {

	int rank, size, value;

	MPI_Status status;
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	do {
		if (rank == 0) {
			cin >> value;
			MPI_Send(&value, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
		}
		else {
			MPI_Recv(&value, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
			if (rank < size - 1)
				MPI_Send(&value, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
			cout << "Process: " << rank << " je dobio vrednost: " << value << endl;
		}
	} while (value > 0);
	MPI_Finalize();
	return 0;
}
