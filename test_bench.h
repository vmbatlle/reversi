/*
 * @file test_bench.h
 * @author Victor M. Batlle <736478@unizar.es>
 * @date 2018/10/04
 */

#ifndef TEST_BENCH_H_
#define TEST_BENCH_H_

#define DIM 8

int test_version(int (*f[])(char[][DIM], int*, char, char, char, char, char),
		int dim, char tablero[][DIM] , char state[][DIM],
		char FA, char CA, char SF, char SC, char color);

int test_version1(int (*f[])(char[][DIM], int*, char, char, char, char, char),
		int dim, char tablero[][DIM]);
int test_version2(int (*f[])(char[][DIM], int*, char, char, char, char, char),
		int dim, char tablero[][DIM]);
int test_version3(int (*f[])(char[][DIM], int*, char, char, char, char, char),
		int dim, char tablero[][DIM]);
int test_version4(int (*f[])(char[][DIM], int*, char, char, char, char, char),
		int dim, char tablero[][DIM]);
int test_version5(int (*f[])(char[][DIM], int*, char, char, char, char, char),
		int dim, char tablero[][DIM]);
int test_version6(int (*f)(char[][DIM], int*, char, char, char, char, char));
int test_version7(int (*f)(char[][DIM], int*, char, char, char, char, char));

#undef DIM

#endif /* TEST_BENCH_H_ */
