#include <mpi.h>
#include <iostream>
using namespace std;
//Napisati MPI program koji realizuje množenje matrice Amxn i matrice
//Bnxk, čime se dobija i prikazuje rezultujuća matrica Cmxk.Izračunavanje se
//obavlja tako što master proces šalje svakom procesu po jednu kolonu
//matrice A i po jednu vrstu matrice B.Svi elementi kolone matrice A se
//šalju odjednom.Svi procesi učestvuju u izračunavanjima potrebnim za
//generisanje rezultata programa.Zadatak rešiti isključivo
//primenom grupnih operacija



#define n 2
#define m 3
#define k 4

int main(int argc, char* argv[]) {

	int rank, p;
	int A[m][n], B[n][k], C[m][k];
	int columnA[m], rowB[k];
	int local_c[m][k];

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	MPI_Datatype vector, column;

	MPI_Type_vector(m, 1, n, MPI_INT, &vector);
	MPI_Type_commit(&vector);
	MPI_Type_create_resized(vector, 0, 1 * sizeof(int), &column);
	MPI_Type_commit(&column);

	if (rank == 0) {
		for (int i = 0; i < m; i++)
			for (int j = 0; j < n; j++)
				A[i][j] = i + j;
		for (int i = 0; i < n; i++)
			for (int j = 0; j < k; j++)
				B[i][j] = 1 + j - i;
	}

	MPI_Scatter(A, 1, column, columnA, m, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatter(B, k, MPI_INT, &rowB, k, MPI_INT, 0, MPI_COMM_WORLD);

	for (int i = 0; i < m; i++) {
		for (int j = 0; j < k; j++) {
			local_c[i][j] = columnA[i] * rowB[j];
		}
	}

	for (int i = 0; i < m; i++)
		for (int j = 0; j < k; j++)
			C[i][j] = 0;

	MPI_Reduce(&local_c, &C, m * k, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		for (int i = 0; i < m; i++)
		{
			for (int j = 0; j < k; j++)
				cout << C[i][j] << " ";
			cout << endl;
		}
	}

	MPI_Finalize();
	return 0;
}