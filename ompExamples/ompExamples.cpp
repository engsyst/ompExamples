// ompExamples.cpp: определяет точку входа для консольного приложения.

#include "stdafx.h"
#include <omp.h>
#include <stdlib.h>
#include "testPragma.h"
#include "arrayutils.h"
#include <algorithm>
#include <vector>

void printOmpTime(char const*, double, double);
void merge(double *d, int left, int right, int n);
void merge_parallel(double* d, int size);
int bubbleSort(double *d, int size, int(*comparator)(double o1, double o2));

int SIZE = 100000;

int dobleComparator(double o1, double o2) {
	if (o1 > o2)
		return 1;
	else if (o1 < o2)
		return -1;
	else
		return 0;
}

int compare_as_double(double o1, double o2) {
	return o1 < o2;
}

void report_num_threads() {
	#pragma omp master
	{
		printf("Threads %d: \r\n", omp_get_num_threads());
	}
}

void scale(double *d, int n, double scale) {
	printf_s("\n<-- Scale -->\n");
	if (n <= 0)
		return;
#pragma omp parallel for
	for (int i = 0; i < n; i++) {
		d[i] *= scale;
	}
}

int main() {
	double* costs = NULL; // = new double[SIZE];

	double time0;
	double time1;

	printf_s("omp_get_num_threads: %d / omp_get_num_procs: %d\r\n",
		omp_get_num_threads(), omp_get_num_procs());

	testParallelFor(12);
	testParralelStructureBlock();
	testParallelSections(0);
	testParallelSections(2);
	testParallelNestedSections(0);
	testParallelNestedSections(1);
	testParallelNestedSections(2);
	testNowait();
	testOrdered();
	testAtomic(10, 5);
	//testCritical(SIZE * 100);

	costs = randomInit(NULL, SIZE, -100, 100);
	testSum(costs, SIZE);
	time0 = omp_get_wtime();
	scale(costs, SIZE, 0.3);
	time1 = omp_get_wtime();
	printOmpTime("Time of scale", time0, time1);
	delete[] costs;

	printf_s("\n<-- Sort -->\n");
	if (SIZE > 10000) {
		SIZE = 10000;
	}
	costs = randomInit(NULL, SIZE, -100, 100);
	time0 = omp_get_wtime();
	bubbleSort(costs, SIZE, dobleComparator);
	//time1 = omp_get_wtime();
	//printOmpTime("Time of sort", time0, time1);
	//print(costs, SIZE);

	printf_s("\n<-- Merge -->\n");
	//time0 = omp_get_wtime();
	merge_parallel(costs, SIZE);
	time1 = omp_get_wtime();
	printOmpTime("Time of sort", time0, time1);
	//println(costs, SIZE);
	delete[] costs;

	printf_s("\n<-- Serial bubbleSort -->\n");
	costs = randomInit(NULL, SIZE, -100, 100);
	omp_set_num_threads(1);
	time0 = omp_get_wtime();
	bubbleSort(costs, SIZE, dobleComparator);
	time1 = omp_get_wtime();
	printOmpTime("Time of serial bubbleSort", time0, time1);

	printf_s("\n<-- Serial standart C++ sort -->\n");
	costs = randomInit(NULL, SIZE, -100, 100);
	std::vector<double> costsvector;
	costsvector.assign(costs, costs + SIZE);
	omp_set_num_threads(1);
	time0 = omp_get_wtime();
	std::sort(costsvector.begin(), costsvector.end(), compare_as_double);
	time1 = omp_get_wtime();
	printOmpTime("Time of serial standart C++ sort", time0, time1);
	omp_set_num_threads(omp_get_num_procs());

	delete[]costs;
	//getchar();
	return 0;
}

void merge(double *d, int left, int right, int n) {
	//report_num_threads();
	printf_s("merge Thread: %d - l: %d, r: %d, n: %d \n", omp_get_thread_num(), left, right, n);

	int i = left;
	int j = right;
	while (i < right && j < n) {
		if (d[i] > d[j] && i < j) {
			double t = d[i];
			d[i] = d[j];
			d[j] = t;
			i++;
			int k = j;
			while (k < n - 1 && d[k] > d[k + 1]) {
				double t = d[k];
				d[k] = d[k + 1];
				d[k + 1] = t;
				k++;
			}
		} else {
			//j++;
			i++;
		}
	}
}

void merge_parallel(double* costs, int size) {
	int num_threads = 0;

	// omp_get_num_threads() must be called from parallel region
	// in the serial region it will return 1
	#pragma omp parallel shared(costs) 
	num_threads = omp_get_num_threads();

	while (num_threads > 1) {
		num_threads = num_threads / 2;
		omp_set_num_threads(num_threads);
		#pragma omp parallel shared(costs) 
		{
			#pragma omp master
			printf_s("Merge threads: %d\n", num_threads);

			int portion = SIZE / num_threads;
			#pragma omp for
			for (int i = 0; i < num_threads; i++) {
				merge(costs, i * portion, i * portion + portion / 2, i * portion + portion);
			}
		}
	}
}

int bubbleSort(double *d, int size, int(*comparator)(double o1, double o2)) {
	if (size <= 0)
		return 0;
	int num_threads = 0;
	#pragma omp parallel
	num_threads = omp_get_num_threads();
	int portion = size / num_threads;
	int flag = 1;
	int end = 0;
	const int shedulePortion = 1000;
	#pragma omp parallel for schedule(static) \
				shared(portion) private(end) firstprivate(flag)
	for (int i = 0; i < size - 1; i++) {
		if (flag) {
			end = i + portion;
			printf_s("Start: %d\tEnd: %d -- thread %d\n", i, end, omp_get_thread_num());
			flag = 0;
		}
		for (int j = i + 1; j < end; j++) {
			if (comparator(d[j], d[i])) {
				double t = d[j];
				d[j] = d[i];
				d[i] = t;
			}
		}
		//if (i % 500 == 0)
		//	printf_s("Sort Thread: %d\ti: %d\n", omp_get_thread_num(), i);
	}
	return 0;
}

void printOmpTime(char const *c, double t0, double t1) {
	double delta = t1 - t0;
	//printf_s(c);
	printf_s("%s: %.16g\n", c, delta);
}


