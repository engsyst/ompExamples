#include "stdafx.h"
#include <omp.h>
#include <stdio.h>
#include "testShedule.h"

void testStatic(int n, double *costs, double factor, int chunk) {
#pragma omp parallel
	{
		int id = omp_get_thread_num();
	#pragma omp parallel for schedule(static)
		for (int i = 0; i < n; i++)
		{
			costs[i] *= factor;
			//printf("Thread %d, i %d\n", id, i);
		}

	}
}

void testDynamic(int n, double *costs, double factor, int chunk) {
#pragma omp parallel 
	{
		int id = omp_get_thread_num();
	#pragma omp for schedule(dynamic)
		for (int i = 0; i < n; i++)
		{
			costs[i] *= factor;
			//printf("Thread %d, i %d\n", id, i);
		}

	}
}

void testGuided(int n, double *costs, double factor, int chunk) {
#pragma omp parallel 
	{
		int id = omp_get_thread_num();
#pragma omp for schedule(guided)
		for (int i = 0; i < n; i++)
		{
			costs[i] *= factor;
			//printf("Thread %d, i %d\n", id, i);
		}

	}
}