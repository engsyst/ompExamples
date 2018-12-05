#pragma once
double* randomInit(double* d, int cols, int rmin, int rmax);
double** randomInit(double** d, int rows, int cols, int rmin, int rmax);
void print(double *d, int n);
void print(double**, int, int);
int scale(double*, int, double);
