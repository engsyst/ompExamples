#pragma once
double* randomInit(double *d, const int length, const int start = -50, const int end = -50);
double** randomInit(double ** d, int rows, int cols, int start, int end);
void print(double* d, int n);
void print(double** d, int rows, int cols);