/*
 * @file test_bench.c
 * @author Victor M. Batlle <736478@unizar.es>
 * @date 2018/10/04
 */

#include "test_bench.h"
#if defined (ENVIRONMENT_IO)
#include "timer2.h"
#endif

/* Constantes del programa */
#define DIM 8
enum { MAX=10, FALSE=0, TRUE=1 };
enum {
B = 1,
N = 2
};

/*
 * Comprueba que todas las funciones en f[] devuelvan el mismo
 * resultado para un caso de prueba dado.
 */
int test_version(int (*f[])(char[][DIM], int*, char, char, char, char, char),
		int dim, char tablero[][DIM] , char state[][DIM],
		char FA, char CA, char SF, char SC, char color) {
	if (dim > 1) {
		int i, j, k;
		int return_value[MAX], longitud[MAX];
		for (i = 0; i < MAX; i++){
			return_value[i] = -1;
			longitud[i] = 0;
		}
		if (dim > MAX) dim = MAX;
		for (i = 0; i < dim; i++){
			/* Preparar tablero en estado inicial */
			for (j = 0; j < DIM; j++) {
				for (k = 0; k < DIM; k++) {
					tablero[j][k] = state[j][k];
				}
			}
			return_value[i] = f[i](tablero, &longitud[i], FA, CA, SF, SC, color);
		}
		for (i = 1; i < dim; i++){
			if (return_value[i] != return_value[0] ||
					(return_value[i] == 1 && (longitud[i] != longitud[0])) ) return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

/*
 * Prueba 1: COLOCADA + VACÍA
 *   Acción: Colocar en F3:C5 y explorar hacia el ESTE.
 *   Resultado: NO_HAY_PATRON (casilla_vacia)
 */

int test_version1(int (*f[])(char[][DIM], int*, char, char, char, char, char), int dim, char tablero[][DIM]) {
	char state[DIM][DIM] = {
			{0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0},
			{0,0,0,B,N,0,0,0},
			{0,0,0,N,B,0,0,0},
			{0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0}
	};
	char FA = 3,
		 CA = 5,
		 SF = 0,
		 SC = 1,
		 color = B;
	return test_version(f, dim, tablero, state, FA, CA, SF, SC, color);
}

/*
 * Prueba 2: COLOCADA + MISMO COLOR
 *   Acción: Colocar en F4:C5 y explorar hacia el OESTE.
 *   Resultado: NO_HAY_PATRON (longitud = 0)
 */

int test_version2(int (*f[])(char[][DIM], int*, char, char, char, char, char), int dim, char tablero[][DIM]) {
	char state[DIM][DIM] = {
			{0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0},
			{0,0,0,B,N,0,0,0},
			{0,0,0,N,B,0,0,0},
			{0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0}
	};
	char FA = 4,
		 CA = 5,
		 SF = 0,
		 SC = -1,
		 color = B;
	return test_version(f, dim, tablero, state, FA, CA, SF, SC, color);
}

/*
 * Prueba 3: COLOCADA + DISTINTA + MISMO COLOR
 *   Acción: Colocar en F1:C3 y explorar hacia el SUR.
 *   Resultado: PATRON_ENCONTRADO  (longitud = 2)
 */

int test_version3(int (*f[])(char[][DIM], int*, char, char, char, char, char), int dim, char tablero[][DIM]) {
	char state[DIM][DIM] = {
			{0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0},
			{0,0,0,B,0,0,0,0},
			{0,0,0,B,N,0,0,0},
			{0,0,0,N,B,0,0,0},
			{0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0}
	};
	char FA = 1,
		 CA = 3,
		 SF = 1,
		 SC = 0,
		 color = N;
	return test_version(f, dim, tablero, state, FA, CA, SF, SC, color);
}

/*
 * Prueba 4: COLOCADA + DISTINTA + VACÍA
 *   Acción: Colocar en F2:C3 y explorar hacia el SUROESTE.
 *   Resultado: NO_HAY_PATRON
 */

int test_version4(int (*f[])(char[][DIM], int*, char, char, char, char, char), int dim, char tablero[][DIM]) {
	char state[DIM][DIM] = {
			{0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0},
			{0,0,N,N,N,0,0,0},
			{0,N,N,N,B,0,0,0},
			{0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0}
	};
	char FA = 2,
		 CA = 3,
		 SF = 1,
		 SC = -1,
		 color = B;
	return test_version(f, dim, tablero, state, FA, CA, SF, SC, color);
}

/*
 * Prueba 5: COLOCADA + DISTINTA + FUERA DE TABLERO
 *   Acción: Colocar en F2:C2, F2:C5, F5:C2; F5:C5 y explorar hacia fin de tablero.
 *   Resultado: NO_HAY_PATRON
 */

int test_version5(int (*f[])(char[][DIM], int*, char, char, char, char, char), int dim, char tablero[][DIM]) {
	char state[DIM][DIM] = {
			{B,B,B,0,0,B,B,B},
			{B,B,B,0,0,B,B,B},
			{B,B,0,0,0,0,B,B},
			{0,0,0,B,N,0,0,0},
			{0,0,0,N,B,0,0,0},
			{B,B,0,0,0,0,B,B},
			{B,B,B,0,0,B,B,B},
			{B,B,B,0,0,B,B,B}
	};
	char color = N;
	return test_version(f, dim, tablero, state, 2, 2, 0, -1, color) &&
		   test_version(f, dim, tablero, state, 2, 2, -1, -1, color) &&
		   test_version(f, dim, tablero, state, 2, 2,0 -1, 0, color) &&
		   //test_version(f, dim, tablero, state, 2, 5, -1, 0, color) &&
		   test_version(f, dim, tablero, state, 2, 5, -1, 1, color) &&
		   test_version(f, dim, tablero, state, 2, 5, 0, 1, color) &&
		   //test_version(f, dim, tablero, state, 5, 5, 0, 1, color) &&
		   test_version(f, dim, tablero, state, 5, 5, 1, 1, color) &&
		   test_version(f, dim, tablero, state, 5, 5, 1, 0, color) &&
		   //test_version(f, dim, tablero, state, 5, 2, 1, 0, color) &&
		   test_version(f, dim, tablero, state, 5, 2, 1, -1, color);
}

#if defined (ENVIRONMENT_IO)
int test_version6(int (*f)(char[][DIM], int*, char, char, char, char, char)) {
	char state[DIM][DIM] = {
			{0,0,0,0,0,0,0,0},
			{0,B,0,0,0,0,0,0},
			{0,0,B,0,0,0,0,0},
			{0,0,0,B,N,0,0,0},
			{0,0,0,N,B,0,0,0},
			{0,0,0,0,0,B,0,0},
			{0,0,0,0,0,0,B,0},
			{0,0,0,0,0,0,0,B}
	};
	char FA = 0,
		 CA = 0,
		 SF = 1,
		 SC = 1,
		 color = N;
	int longitud = 0;
	timer2_empezar();
	f(state, &longitud, FA, CA, SF, SC, color);
	return timer2_parar();
}

int test_version7(int (*f)(char[][DIM], int*, char, char, char, char, char)) {
	char state[DIM][DIM] = {
			{0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0},
			{0,0,0,B,N,0,0,0},
			{0,0,0,N,B,0,0,0},
			{0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0}
	};
	char FA = 2,
		 CA = 5,
		 SF = 1,
		 SC = -1,
		 color = N;
	int longitud = 0;
	timer2_empezar();
	f(state, &longitud, FA, CA, SF, SC, color);
	return timer2_parar();
}

int test_version8(int (*f)(char[][DIM], int*, char, char, char, char, char)) {
	char state[DIM][DIM] = {
			{0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0},
			{0,0,0,B,N,0,0,0},
			{0,0,0,N,B,0,0,0},
			{0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0}
	};
	char FA = 2,
		 CA = 3,
		 SF = 1,
		 SC = 0,
		 color = N;
	int longitud = 0;
	timer2_empezar();
	f(state, &longitud, FA, CA, SF, SC, color);
	return timer2_parar();
}

int test_version9(int (*f)(char[][DIM], int*, char, char, char, char, char)) {
	char state[DIM][DIM] = {
			{0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0},
			{0,0,0,B,N,0,0,0},
			{0,0,0,N,B,0,0,0},
			{0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0}
	};
	char FA = 2,
		 CA = 3,
		 SF = -1,
		 SC = +1,
		 color = N;
	int longitud = 0;
	timer2_empezar();
	f(state, &longitud, FA, CA, SF, SC, color);
	return timer2_parar();
}
#endif

#undef DIM
