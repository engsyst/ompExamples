// ompGauss.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <omp.h>
#include <stdlib.h>
#include "arrayutils.h"

const int N = 10;
const int M = 10;

int main()
{
	double **matr = randomInit(NULL, N, M, -100, 100);
	double *vals = randomInit(NULL, N, -100, 100);
	print(matr, N, M);
	print(vals, N);

    return 0;
}

