#include <omp.h>
#include <iostream>
#include <cstdio>

using namespace std;

#define n 100

int main() {
    int  A[n];
#pragma omp parallel for 
	for (int i = 0; i < n; i++)
	{
		A[i] = i + 1;
	}


	int res = 1;
#pragma omp parallel
{
	int  threadRes = 1;
	#pragma omp for
	for (int i = 0; i < n; i++)
		threadRes *= A[i];

	#pragma omp critical
	{
		res *= threadRes;
	}
}
	printf("%d", res);

}
