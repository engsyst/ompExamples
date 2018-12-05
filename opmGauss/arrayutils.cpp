#include "stdafx.h"
#include <omp.h>
#include <stdlib.h>
#include "arrayutils.h"

double** new_array(int n, int m) {
	double **d = new double*[n];
	for (int i = 0; i < n; i++) {
		d[i] = new double[m];
	}
	return d;
}

/**
if **d == null will create new array
*/
double* randomInit(double *d, int length, int range_min = -50, int range_max = -50) {
	if (length <= 0)
		return NULL;
	if (d == NULL)
		d = new double[length];
	int i = 0;
	#pragma omp parallel for
		for (i = 0; i < length; i++) {
			d[i] = (double) rand() / (RAND_MAX + 1) * (range_max - range_min)
				+ range_min;
		}
	return d;
}

/**
if **d == null will create new array 
*/
double** randomInit(double **d, int rows, int cols, int range_min = -50, int range_max = 50) {
	if (rows <= 0 || cols < 0)
		return NULL;
	if (d == NULL)
		d = new_array(rows, cols);
	int i = 0;
	#pragma omp parallel for
	for (i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			d[i][j] = (double) rand() / (RAND_MAX + 1) * (range_max - range_min)
				+ range_min;
		}
	}
	return d;
}

int scale(double *d, int n, double scale) {
	if (n <= 0)
		return 0;
	int i = 0;
	#pragma omp for private(i)
		for (i = 0; i < n; i++) {
			d[i] *= scale;
		}
	return i;
}

void print(double *d, int n) {
#pragma omp master
{
	if (n <= 0)
		return;

	printf_s("[");
	for (int i = 0; i < n; i++) {
		printf_s("%7.2f", d[i]);
		//if ((i + 1) % 4 == 0)
		//	printf_s("\n");
	}
	printf_s("]\n");
}
return;
}

void print(double **d, int rows, int cols) {
	#pragma omp master
	{
		if (rows <= 0 || cols <= 0)
			return;

		for (int i = 0; i < rows; i++) {
			printf_s("[");
			for (int j = 0; j < cols; j++) {
				printf_s("%7.2f", d[i][j]);
			}
			printf_s("]\n");
		}
	}
	return;
}

