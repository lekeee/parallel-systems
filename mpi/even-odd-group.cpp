#include <mpi.h>
#include <iostream>
using namespace std;
//Napisati MPI program kojim se vrši podela procesa članova
//komunikatora MPI_COMM_WORLD u dve grupe : grupu procesa sa neparnim
//identifikatorima i grupu procesa sa parnim identifikatorima




#define n 2
#define m 3
#define k 4

int main(int argc, char* argv[]) {

	int rank, p;
	int members[8], evenRank, oddRank;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	MPI_Group worldGroup, oddGroup, evenGroup;
	MPI_Comm_group(MPI_COMM_WORLD, &worldGroup);

	int numberEven = (p + 1) / 2, numberOdd = p - numberEven;

	for (int i = 0; i < numberEven; i++)
		members[i] = 2 * i;

	MPI_Group_incl(worldGroup, numberEven, members, &evenGroup);
	MPI_Group_rank(evenGroup, &evenRank);

	MPI_Group_excl(worldGroup, numberEven, members, &oddGroup);
	MPI_Group_rank(oddGroup, &oddRank);

	cout << "world rank " << rank;
	cout << " even rank " << evenRank;
	cout << " odd rank " << oddRank << endl;

	MPI_Finalize();
	return 0;
}