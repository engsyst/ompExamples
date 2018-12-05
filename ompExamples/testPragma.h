#pragma once
void emptyLongWork(int count);
void testSum(const double * a, int count);
void testOrdered();
void testCritical(int count);
void testAtomic(int count, int x);
void testNowait();
void testParallelFor(int n);
void testParallelSections(int nestedLevel);
void testParallelNestedSections(int nestedLevel);
void testParralelStructureBlock();
const int COUNT = 10000;