#include "stdafx.h"
#include <omp.h>
#include <stdlib.h>
#include "testPragma.h"
#include <math.h>

void emptyLongWork(int count) {
	printf_s("Start emptyLongWork: %d\tTread: %d\n", count, omp_get_thread_num());
	for (int i = 0; i < count; i++) {
		double a = count / sin(count);
	}
	printf_s("Finish emptyLongWork: %d\tTread: %d\n", count, omp_get_thread_num());
}


static float a[COUNT], b[COUNT], c[COUNT];

/**
Show the race for the data
use directives:
	parallel for
	parallel for reduction
	atomic
*/
void testSum(const double* a, int count) {
	printf_s("\n<-- Sum -->\n");
	double sum;
	double time0;

	sum = 0.;
	time0 = omp_get_wtime();
	for (int i = 0; i < count; i++) {
		sum += a[i];
	}
	printf_s("SUM serial time\t\t%.8g\tSum: %.8g\n", omp_get_wtime() - time0, sum);

	sum = 0.;
	time0 = omp_get_wtime();
	#pragma omp parallel for shared(sum)
		for (int i = 0; i < count; i++) {
			#pragma omp atomic
			sum += a[i];					// the race for the data
		}
	printf_s("SUM shared time\t\t%.8g\tSum: %.8g\n", omp_get_wtime() - time0, sum);

	sum = 0.;
	time0 = omp_get_wtime();
	#pragma omp parallel for reduction(+ : sum)
		for (int i = 0; i < count; i++) {
			sum += a[i];
		}
	printf_s("SUM reduction time\t%.8g\tSum: %.8g\n", omp_get_wtime() - time0, sum);
}

/**
Show how to use ordered
use directives :
	parallel for
	ordered
*/
void testOrdered1(int first, int last) {
	#pragma omp for schedule(static) ordered
	for (int i = first; i <= last; ++i) {
		// Do something here.
		if (i % 2) {
		#pragma omp ordered 
			printf_s("test1() iteration %d\n", i);
		}
	}
}

/**
Show how to use ordered
use directives :
	ordered
*/
void testOrdered2(int iter) {
#pragma omp ordered
	printf_s("test2() iteration %d\n", iter);
}

/**
Show how to use ordered
use directives :
	parallel
	for ordered
	ordered
*/
void testOrdered() {
	printf_s("\n<-- testOrdered -->\n");
	int i;
	#pragma omp parallel
	{
		testOrdered1(0, 8);
		#pragma omp for ordered
			for (i = 0; i < 5; i++)
				testOrdered2(i);
	}
}

/**
Show how to use critical sections
use directives :
	parallel
	critical(x)
	critical
*/
void testCritical(int count) {
	printf_s("\n<-- testCritical -->\n");
	int x = 0, y = 0;
	double time0;

	time0 = omp_get_wtime();
	#pragma omp parallel shared(x, y)
	{
		while (true) {
			#pragma omp critical(x)		// need use critical
			if (x < count) {			// because if statment is not atomic
				x += 1;					// only (x += 1) atomic operation
			}
			#pragma omp critical(y)	// need use critical
			if (y < count) {			// because if statment is not atomic
				y += 1;					// only (x += 1) atomic operation
			}
			if (x >= count && y >= count) {
				//printf_s("Thread: %d, X: %d, Y: %d\n", omp_get_thread_num(), x, y);
				break;
			}
		}
	}
	printf_s("Time with self critical %.16g\n", omp_get_wtime() - time0);

	x = 0; y = 0;
	time0 = omp_get_wtime();
	#pragma omp parallel shared(x, y)
	{
		while (true) {
			#pragma omp critical		// need use critical
			if (x < count) {			// because if statment is not atomic
				x += 1;					// only (x += 1) atomic operation
			}
			#pragma omp critical		// need use critical
			if (y < count) {			// because if statment is not atomic
				y += 1;					// only (x += 1) atomic operation
			}
			if (x >= count && y >= count) {
				//printf_s("Thread: %d, X: %d, Y: %d\n", omp_get_thread_num(), x, y);
				break;
			}
		}
	}
	printf_s("Time with shared critical %.16g\n", omp_get_wtime() - time0);

}

/**
Show how to use critical sections
use directives :
	parallel
	critical(x)
	critical
*/
void testAtomic(int count, int x) {
	printf_s("\n<-- testAtomic -->\n");
	int start_x = x;
	int start_count = count;
	int c = 0;
	int flag = 1;
	#pragma omp parallel shared(x, flag) private(c)
	{
		{
			c = 0;
			while (c < 100000 && flag) {
											// need use critical
				if (x < count) {			// because if statment is not atomic
					#pragma omp atomic
					x += 1;					// only (x += 1) atomic operation
				} 
				if (x > count) {
					printf_s("Thread: %d, c: %d, X: %d\n", omp_get_thread_num(), c, x);
					flag = 0;
					break;
				} else {
					c++;
					x = start_x;
					count = start_count;
				}
			}
			printf_s("Thread: %d, c: %d, X: %d\n", omp_get_thread_num(), c, x);
		}
	}
}

/**
Show how to use critical sections
use directives :
	parallel
	sections
	critical
*/
void testNowait() {
	printf_s("\n<-- testNowait -->\n");
	#pragma omp parallel //num_threads(2) 
	{
		#pragma omp master 
		{
			printf_s("omp_get_num_threads: %d / omp_get_num_procs: %d\r\n",
				omp_get_num_threads(), omp_get_num_procs());
		}
		#pragma omp sections nowait
		{
			#pragma omp section
			{
				#pragma omp critical(nw) 
				{
					printf_s("Sectoins: 1\n");
					emptyLongWork(COUNT / 4);
				}
			}
		}

		#pragma omp sections
		{
			#pragma omp section
			{
				printf_s("Sectoins: 2\n");
				emptyLongWork(COUNT / 4);
			}

			#pragma omp section
			{
				printf_s("Sectoins: 2\n");
				emptyLongWork(COUNT / 2);
			}
		}
	}
}

void testParallelFor(int n) {
	printf_s("\n<-- testParallelFor -->\n");

	#pragma omp parallel 
	{
		int portion = n / omp_get_num_threads();
		int flag = 1;
		int end = 0;
		#pragma omp for private(end) schedule(static)
			for (int i = 0; i < n; i++) {
				if (flag) {
					end = i + portion;
					printf_s("End: %d\n", end);
					flag = 0;
				}
				for (int j = i; j < end; j++) {
					printf_s("T: %d:\ti: %d:\tj: %d\n", omp_get_thread_num(), i, j);
				}
			}
	}
}

void testParallelSections(int nestedLevel) {
	/* Nested level: 0
	Section1 thread: 0      Num_Threads: 4
	Section2 thread: 1      Num_Threads: 4
	Parallel thread: 1      Num_Threads: 4
	Parallel thread: 2      Num_Threads: 4
	Parallel thread: 3      Num_Threads: 4
	Parallel thread: 0      Num_Threads: 4
	Master thread: 0        Num_Threads: 4
	*/
	/* Nested level: 2
	Section1 thread: 0      Num_Threads: 4
	Section2 thread: 1      Num_Threads: 4
	Parallel thread: 1      Num_Threads: 4
	Parallel thread: 2      Num_Threads: 4
	Parallel thread: 3      Num_Threads: 4
	Parallel thread: 0      Num_Threads: 4
	Master thread: 0        Num_Threads: 4
	*/
	printf_s("\n<-- testParallelSections -->\n");

	omp_set_nested(nestedLevel);
	printf_s("Nested levels: %d\n", omp_get_nested());

	#pragma omp parallel 
	{
		{
			printf_s("Inside thread: %d\tNum_Threads: %d\n", omp_get_thread_num(), omp_get_num_threads());
			#pragma omp sections
			{
				#pragma omp section
				{
					printf_s("Section1 thread: %d\tNum_Threads: %d\n", omp_get_thread_num(), omp_get_num_threads());
					emptyLongWork(COUNT);
				}
				#pragma omp section
				{
					printf_s("Section2 thread: %d\tNum_Threads: %d\n", omp_get_thread_num(), omp_get_num_threads());
					emptyLongWork(COUNT);
				}
			}
		}
		printf_s("Parallel thread: %d\tNum_Threads: %d\n", omp_get_thread_num(), omp_get_num_threads());
		emptyLongWork(COUNT);
	
		#pragma omp master
			printf_s("Master thread: %d\tNum_Threads: %d\n", omp_get_thread_num(), omp_get_num_threads());

	}
}

void testParallelNestedSections(int nestedLevel) {
	/* Nested level: 0
	Section1 thread: 0      Num_Threads: 1
	Section2 thread: 0      Num_Threads: 1
	Section1 thread: 0      Num_Threads: 1
	Section2 thread: 0      Num_Threads: 1
	Section1 thread: 0      Num_Threads: 1
	Section2 thread: 0      Num_Threads: 1
	Section1 thread: 0      Num_Threads: 1
	Section2 thread: 0      Num_Threads: 1
	Parallel thread: 2      Num_Threads: 4
	Parallel thread: 3      Num_Threads: 4
	Master thread: 0        Num_Threads: 4
	Parallel thread: 1      Num_Threads: 4
	Parallel thread: 0      Num_Threads: 4
	*/
	/* Nested level: 2
	Section1 thread: 0      Num_Threads: 4
	Section1 thread: 0      Num_Threads: 4
	Section2 thread: 1      Num_Threads: 4
	Section1 thread: 0      Num_Threads: 4
	Section2 thread: 0      Num_Threads: 4
	Section1 thread: 0      Num_Threads: 4
	Section2 thread: 0      Num_Threads: 4
	Section2 thread: 0      Num_Threads: 4
	Parallel thread: 2      Num_Threads: 4
	Parallel thread: 0      Num_Threads: 4
	Parallel thread: 3      Num_Threads: 4
	Master thread: 0        Num_Threads: 4
	Parallel thread: 1      Num_Threads: 4
	*/

	printf_s("\n<-- testParallelNestedSections -->\n");
	printf_s("Nested levels: %d\n", nestedLevel);

	omp_set_nested(nestedLevel);
	#pragma omp parallel /*num_threads(nestedLevel)*/
	{
		{
			printf_s("Inside thread: %d\tNum_Threads: %d\n", omp_get_thread_num(), omp_get_num_threads());
			#pragma omp parallel sections
			{
				#pragma omp section
				{
					printf_s("Section 1, Level 1 thread: %d\tNum_Threads: %d\n", omp_get_thread_num(), omp_get_num_threads());
					emptyLongWork(COUNT);
					
					#pragma omp parallel sections
					{
						#pragma omp section
						{
							printf_s("Section 1, Level 2 thread: %d\tNum_Threads: %d\n", omp_get_thread_num(), omp_get_num_threads());
							emptyLongWork(COUNT);
						}
						#pragma omp section
						{
							printf_s("Section 2, Level 2 thread: %d\tNum_Threads: %d\n", omp_get_thread_num(), omp_get_num_threads());
							emptyLongWork(COUNT);
						}

					}
				}
				#pragma omp section
				{
					printf_s("Section 2, Level 1 thread: %d\tNum_Threads: %d\n", omp_get_thread_num(), omp_get_num_threads());
					emptyLongWork(COUNT);
				}
			}
		}

		printf_s("Parallel thread: %d\tNum_Threads: %d\n", omp_get_thread_num(), omp_get_num_threads());
		emptyLongWork(COUNT);

		#pragma omp master
			printf_s("Master thread: %d\tNum_Threads: %d\n", omp_get_thread_num(), omp_get_num_threads());

	}
	omp_set_nested(0);
}

void testParralelStructureBlock() {
	printf_s("\n<-- testParralelStructureBlock -->\n");

	#pragma omp parallel
	{
		{
			printf_s("Structured block 1 - Thread: %d of %d\n", 
				omp_get_thread_num(), omp_get_num_threads());
		}
		{
			printf_s("Structured block 2 - Thread: %d of %d\n",
				omp_get_thread_num(), omp_get_num_threads());
		}
		{
			printf_s("Structured block 3 - Thread: %d of %d\n",
				omp_get_thread_num(), omp_get_num_threads());
		}
		printf_s("Unsructured block - Thread: %d of %d\n",
			omp_get_thread_num(), omp_get_num_threads());
	}
}