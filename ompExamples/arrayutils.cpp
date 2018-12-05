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
if *d == null will create new array 
*/
double* randomInit(double *d, const int length, const int start, const int end) {
	if (length <= 0)
		return NULL;
	if (d == NULL)
		d = new double[length];
	#pragma omp parallel for shared(d)
		for (int i = 0; i < length; i++) {
			double x = (double) rand() / (RAND_MAX + 1) * (end - start)
				+ start;
			d[i] = x;
		}
	return d;
}

/**
if *d == null will create new array
*/
double* sequentalInit(double* d, const int length, const int start = 0) {
	if (length <= 0)
		return NULL;
	if (d == NULL) {
		d = new double[length];
	}

	int c = start;
	#pragma omp parallel for shared(d)
		for (int i = 0; i < length; i++) {
			d[i] = c++;
		}
	return d;
}

/**
if **d == null will create new array 
*/
double** randomInit(double **d, int rows, int cols, int start = -50, int end = 50) {
	if (rows <= 0 || cols < 0)
		return NULL;
	if (d == NULL)
		d = new_array(rows, cols);
	#pragma omp parallel for
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			d[i][j] = (double) rand() / (RAND_MAX + 1) * (end - start)
				+ start;
		}
	}
	return d;
}

void print(double *d, int n) {
	if (n <= 0)
		return;

	printf_s("[");
	for (int i = 0; i < n; i++) {
		printf_s("%7.4f, ", d[i]);
		if ((i + 1) % 4 == 0)
			printf_s("\n");
	}
	printf_s("]\n");
}

void print(double **d, int rows, int cols) {
		if (rows <= 0 || cols <= 0)
			return;

		for (int i = 0; i < rows; i++) {
			printf_s("[");
			for (int j = 0; j < cols; j++) {
				printf_s("%7.4f, ", d[i][j]);
			}
			printf_s("]\n");
		}
}

