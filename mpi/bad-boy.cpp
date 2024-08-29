#include <mpi.h>
#include <iostream>
#include <cmath>

using namespace std;

//Napisati MPI program koji realizuje množenje matrice Anxn i vektora bn,
//?ime se dobija rezultuju?i vektor cn.Matrica A i vektor b se inicijalizuju u
//master procesu.Broj procesa je p i ure?eni su kao matrica qxq(q2 = p).
//Matrica A je podeljena u podmatrice dimenzika kxk(k = n / q) i master
//proces distribuira odgovaraju?e blokove matrica A po procesima kao što je
//prikazano na Slici 1. za n = 8 i p = 16. Vektor b se distribuira u delovima od
//po n / q elemenata, tako da nakon slanja procesi u prvoj koloni matrice
//procesa sadrže prvih n / q elemenata, u 2. koloni matrice procesa slede?ih
//n / q elemenata itd.osnovu primljenih podataka svaki proces obavlja
//odgovaraju?a izra?unavanja i u?estvuje u generisanju rezultata koji se
//prikazuje u master procesu.Predvideti da se slanje podmatrica matrice A
//svakom procesu obavlja sa po jednom naredbom MPI_Send kojom se šalje
//samo 1 izvedeni tip podatka.Slanje blokova vektora b i generisanje
//rezultata implementirati koriš?enjem grupnih operacija i funkcija za
//kreiranje novih komunikatora.


#define n 8

int main(int argc, char* argv[]) {
    int irow, jcol, p, i, j, k, q, l, y = 0;
    MPI_Comm row_comm, col_comm, com;
    int rank, row_id, col_id;
    static int a[n][n], b[n], c[n];
    MPI_Datatype vrblok;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    q = (int)sqrt((double)p);
    k = n / q;

    int* local_a = (int*)calloc(k * k, sizeof(int));
    int* local_b = (int*)calloc(k, sizeof(int));

    MPI_Type_vector(k, k, n, MPI_INT, &vrblok);
    MPI_Type_commit(&vrblok);

    if (rank == 0) {
        for (i = 0; i < n; i++) {
            for (j = 0; j < n; j++) {
                a[i][j] = i + j;
            }
        }
        for (i = 0; i < n; i++) {
            b[i] = 1;
        }

        // Send blocks of the matrix A to other processes
        for (i = 0; i < k; i++) {
            for (j = 0; j < k; j++) {
                local_a[y++] = a[i][j];
            }
        }

        l = 1;
        for (j = 0; j < q; j++) {
            for (i = 0; i < q; i++) {
                if ((i + j) != 0) {
                    MPI_Send(&a[j * k][i * k], 1, vrblok, l, 2, MPI_COMM_WORLD);
                    l++;
                }
            }
        }
    }
    else {
        MPI_Recv(local_a, k * k, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);
    }

    irow = rank / q;
    jcol = rank % q;
    com = MPI_COMM_WORLD;

    MPI_Comm_split(com, irow, jcol, &row_comm);
    MPI_Comm_split(com, jcol, irow, &col_comm);

    MPI_Comm_rank(row_comm, &row_id);
    MPI_Comm_rank(col_comm, &col_id);

    if (col_id == 0) {
        MPI_Scatter(b, k, MPI_INT, local_b, k, MPI_INT, 0, row_comm);
    }

    MPI_Bcast(local_b, k, MPI_INT, 0, col_comm);

    int* MyResult = (int*)malloc(k * sizeof(int));
    int* Result = (int*)malloc(n * sizeof(int));
    int index = 0;

    // Perform the local matrix-vector multiplication
    for (i = 0; i < k; i++) {
        MyResult[i] = 0;
        for (j = 0; j < k; j++) {
            MyResult[i] += local_a[index++] * local_b[j];
        }
    }

    // Gather the results from all processes
    MPI_Gather(MyResult, k, MPI_INT, Result, k, MPI_INT, 0, col_comm);

    if (col_id == 0) {
        MPI_Reduce(Result, c, n, MPI_INT, MPI_SUM, 0, row_comm);
    }

    if (rank == 0) {
        for (i = 0; i < n; i++) {
            printf("c[%d]=%d ", i, c[i]);
        }
        printf("\n");
    }

    MPI_Finalize();
    return 0;
}
