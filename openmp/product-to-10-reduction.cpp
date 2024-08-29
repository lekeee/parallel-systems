#include <omp.h>
#include <iostream>
#include <cstdio>

using namespace std;

#define n 10

int main() {
    int  A[n];
#pragma omp parallel for 
	for (int i = 0; i < n; i++)
	{
		A[i] = i + 1;
	}

	int res = 1;
#pragma omp parallel for reduction(*:res)
	for (int i = 0; i < n; i++)
		res *= A[i];

	printf("%d", res);

}
