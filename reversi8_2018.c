/*
 * @file reversi8_2018.c
 * @author Victor M. Batlle <736478@unizar.es>, Diego Royo Meneses <740388@unizar.es>
 * @date 2018/10/21
 */

/* TODO */
#include "timer0.h"
#include "botones_antirebotes.h"

/* Descomente SÓLO UNA línea cada vez */
// TEST_BENCH_1: Juego normal con patron_volteo_test que compara los resultados de todas las implementaciones
// TEST_BENCH_2: Uso de test_bench: Casos conflictivos (tests 1-5) y medidas de tiempo (tests 6-9)
// TEST_BENCH_3: Juego automático de una partida completa con cada una de las implementaciones de patron_volteo
#define NORMAL_PLAY
//#define TEST_BENCH_1
//#define TEST_BENCH_2
//#define TEST_BENCH_3

/* Descomente SÓLO UNA línea cada vez */
//#define ENVIRONMENT_EMULATOR
#define ENVIRONMENT_IO

#if defined (TEST_BENCH_2)
//#include "test_bench.h"
#endif
#if defined (ENVIRONMENT_IO)
#include "timer2.h"
#endif

// Número de implementaciones diferentes de patron_volteo
// Son: patron_volteo, patron_volteo_arm_c, patron_volteo_arm_arm
//      patron_volteo_c_iter, patron_volteo_c_iter_inline, patron_volteo_arm_iter, patron_volteo_arm_iter_v2
enum {
	N=7
};

// Tamaño del tablero
enum { DIM=8 };

// Valores que puede devolver la función patron_volteo())
enum {
	NO_HAY_PATRON = 0,
  PATRON_ENCONTRADO = 1
};

// Estados de las casillas del tablero
enum {
CASILLA_VACIA = 0,
FICHA_BLANCA = 1,
FICHA_NEGRA = 2
};

// candidatas: indica las posiciones a explorar
// Se usa para no explorar todo el tablero innecesariamente
// Sus posibles valores son NO, SI, CASILLA_OCUPADA
const char  NO              = 0;
const char  SI              = 1;
const char  CASILLA_OCUPADA = 2;

/////////////////////////////////////////////////////////////////////////////
// TABLAS AUXILIARES
// declaramos las siguientes tablas como globales para que sean más fáciles visualizarlas en el simulador
// __attribute__ ((aligned (8))): specifies a minimum alignment for the variable or structure field, measured in bytes, in this case 8 bytes

static const char __attribute__ ((aligned (8))) tabla_valor[DIM][DIM] =
{
    {8,2,7,3,3,7,2,8},
    {2,1,4,4,4,4,1,2},
    {7,4,6,5,5,6,4,7},
    {3,4,5,0,0,5,4,3},
    {3,4,5,0,0,5,4,3},
    {7,4,6,5,5,6,4,7},
    {2,1,4,4,4,4,1,2},
    {8,2,7,3,3,7,2,8}
};


// Tabla de direcciones. Contiene los desplazamientos de las 8 direcciones posibles
const char vSF[DIM] = {-1,-1, 0, 1, 1, 1, 0,-1};
const char vSC[DIM] = { 0, 1, 1, 1, 0,-1,-1,-1};

//////////////////////////////////////////////////////////////////////////////////////
// Variables globales que no deberían serlo
// tablero, fila, columna y ready son varibles que se deberían definir como locales dentro de reversi8.
// Sin embargo, las hemos definido como globales para que sea más fácil visualizar el tablero y las variables en la memoria
//////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// Tablero sin inicializar
////////////////////////////////////////////////////////////////////
char __attribute__ ((aligned (8))) tablero[DIM][DIM] = {
	        {CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA},
	        {CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA},
	        {CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA},
	        {CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA},
	        {CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA},
	        {CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA},
	        {CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA},
	        {CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA,CASILLA_VACIA}
	    };

  ////////////////////////////////////////////////////////////////////
     // VARIABLES PARA INTERACCIONAR CON LA ENTRADA SALIDA
     // Pregunta: ¿hay que hacer algo con ellas para que esto funcione bien?
     // (por ejemplo añadir alguna palabra clave para garantizar que la sincronización a través de esa variable funcione)
  volatile unsigned char fila=0, columna=0, ready = 0;



/* patron_volteo_implementation: define qué implementación se usar para jugar */
static int (*patron_volteo_implementation) (char tablero[][8], int *longitud, char f, char c, char SF, char SC, char color);
extern int patron_volteo_arm_c(char tablero[][8], int *longitud,char f, char c, char SF, char SC, char color);
extern int patron_volteo_arm_arm(char tablero[][8], int *longitud,char f, char c, char SF, char SC, char color);
extern int patron_volteo_arm_iter(char tablero[][8], int *longitud,char f, char c, char SF, char SC, char color);
extern int patron_volteo_arm_iter_v2(char tablero[][8], int *longitud,char f, char c, char SF, char SC, char color);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 0 indica CASILLA_VACIA, 1 indica FICHA_BLANCA y 2 indica FICHA_NEGRA
// pone el tablero a cero y luego coloca las fichas centrales.
void init_table(char tablero[][DIM], char candidatas[][DIM])
{
    int i, j;

    for (i = 0; i < DIM; i++)
    {
        for (j = 0; j < DIM; j++)
            tablero[i][j] = CASILLA_VACIA;
    }
#if 0
    for (i = 3; i < 5; ++i) {
	for(j = 3; j < 5; ++j) {
	    tablero[i][j] = i == j ? FICHA_BLANCA : FICHA_NEGRA;
	}
    }

    for (i = 2; i < 6; ++i) {
	for (j = 2; j < 6; ++j) {
	    if((i>=3) && (i < 5) && (j>=3) && (j<5)) {
		candidatas[i][j] = CASILLA_OCUPADA;
	    } else {
		candidatas[i][j] = SI; //CASILLA_LIBRE;
	    }
	}
    }
#endif
    // arriba hay versión alternativa
    tablero[3][3] = FICHA_BLANCA;
    tablero[4][4] = FICHA_BLANCA;
    tablero[3][4] = FICHA_NEGRA;
    tablero[4][3] = FICHA_NEGRA;

    candidatas[3][3] = CASILLA_OCUPADA;
    candidatas[4][4] = CASILLA_OCUPADA;
    candidatas[3][4] = CASILLA_OCUPADA;
    candidatas[4][3] = CASILLA_OCUPADA;

    // casillas a explorar:
    candidatas[2][2] = SI;
    candidatas[2][3] = SI;
    candidatas[2][4] = SI;
    candidatas[2][5] = SI;
    candidatas[3][2] = SI;
    candidatas[3][5] = SI;
    candidatas[4][2] = SI;
    candidatas[4][5] = SI;
    candidatas[5][2] = SI;
    candidatas[5][3] = SI;
    candidatas[5][4] = SI;
    candidatas[5][5] = SI;
}

////////////////////////////////////////////////////////////////////////////////
// Espera a que ready valga 1.
// CUIDADO: si el compilador coloca esta variable en un registro, no funcionará.
// Hay que definirla como "volatile" para forzar a que antes de cada uso la cargue de memoria

void esperar_mov(volatile unsigned char *ready)
{

    while (*ready == 0) {};  // bucle de espera de respuestas hasta que el se modifique el valor de ready (hay que hacerlo manualmente)

    *ready = 0;  //una vez que pasemos el bucle volvemos a fijar ready a 0;

}

////////////////////////////////////////////////////////////////////////////////
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// IMPORTANTE: AL SUSTITUIR FICHA_VALIDA() Y PATRON_VOLTEO()
// POR RUTINAS EN ENSAMBLADOR HAY QUE RESPETAR LA MODULARIDAD.
// DEBEN SEGUIR SIENDO LLAMADAS A FUNCIONES Y DEBEN CUMPLIR CON EL ATPCS
// (VER TRANSPARENCIAS Y MATERIAL DE PRACTICAS):
//  - DEBEN PASAR LOS PARAMETROS POR LOS REGISTROS CORRESPONDIENTES
//  - GUARDAR EN PILA SOLO LOS REGISTROS QUE TOCAN
//  - CREAR UN MARCO DE PILA TAL Y COMO MUESTRAN LAS TRANSPARENCIAS
//    DE LA ASIGNATURA (CON EL PC, FP, LR,....)
//  - EN EL CASO DE LAS VARIABLES LOCALES, SOLO HAY QUE APILARLAS
//    SI NO SE PUEDEN COLOCAR EN UN REGISTRO.
//    SI SE COLOCAN EN UN REGISTRO NO HACE FALTA
//    NI GUARDARLAS EN PILA NI RESERVAR UN ESPACIO EN LA PILA PARA ELLAS
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
////////////////////////////////////////////////////////////////////////////////
// Devuelve el contenido de la posición indicadas por la fila y columna actual.
// Además informa si la posición es válida y contiene alguna ficha.
// Esto lo hace por referencia (en *posicion_valida)
// Si devuelve un 0 no es válida o está vacia.
char ficha_valida(char tablero[][DIM], unsigned char f, unsigned char c, int *posicion_valida)
{
    char ficha;

    // ficha = tablero[f][c];
    // no puede accederse a tablero[f][c]
    // ya que algún índice puede ser negativo

    if ((f < DIM) && (f >= 0) && (c < DIM) && (c >= 0) && (tablero[f][c] != CASILLA_VACIA))
    {
        *posicion_valida = 1;
        ficha = tablero[f][c];
    }
    else
    {
        *posicion_valida = 0;
        ficha = CASILLA_VACIA;
    }
    return ficha;
}

////////////////////////////////////////////////////////////////////////////////
// La función patrón volteo es una función recursiva que busca el patrón de volteo
// (n fichas del rival seguidas de una ficha del jugador actual) en una dirección determinada
// SF y SC son las cantidades a sumar para movernos en la dirección que toque
// color indica el color de la pieza que se acaba de colocar
// la función devuelve PATRON_ENCONTRADO (1) si encuentra patrón y NO_HAY_PATRON (0) en caso contrario
// FA y CA son la fila y columna a analizar
// longitud es un parámetro por referencia. Sirve para saber la longitud del patrón que se está analizando. Se usa para saber cuantas fichas habría que voltear
int patron_volteo(char tablero[][DIM], int *longitud, char FA, char CA, char SF, char SC, char color)
{
    int posicion_valida; // indica si la posición es valida y contiene una ficha de algún jugador
    int patron; //indica si se ha encontrado un patrón o no
    char casilla;   // casilla es la casilla que se lee del tablero
    FA = FA + SF;
    CA = CA + SC;
    casilla = ficha_valida(tablero, FA, CA, &posicion_valida);
    // mientras la casilla está en el tablero, no está vacía,
    // y es del color rival seguimos buscando el patron de volteo
    if ((posicion_valida == 1) && (casilla != color))
    {
        *longitud = *longitud + 1;
        patron = patron_volteo(tablero, longitud, FA, CA, SF, SC, color);
        //printf("longitud: %d \n", *longitud);
        //printf("fila: %d; columna: %d \n", FA, CA);
        return patron;
    }
    // si la ultima posición era válida y la ficha es del jugador actual,
    // entonces hemos encontrado el patrón
    else if ((posicion_valida == 1) && (casilla == color))
    {
        if (*longitud > 0) // longitud indica cuantas fichas hay que voltear
            {
            return PATRON_ENCONTRADO; // si hay que voltear una ficha o más hemos encontrado el patrón
            //printf("PATRON_ENCONTRADO \n");
            }
        else {
            return NO_HAY_PATRON; // si no hay que voltear no hay patrón
            //printf("NO_HAY_PATRON \n");
            }
    }
    // en caso contrario es que no hay patrón
    else
    {
        return NO_HAY_PATRON;
        //printf("NO_HAY_PATRON \n");
    }
}
////////////////////////////////////////////////////////////////////////////////
// Implementación de patrón_volteo iterativa haciendo una llamada a ficha_valida
int patron_volteo_c_iter(char tablero[][DIM], int *longitud, char FA, char CA, char SF, char SC, char color){
	int posicion_valida; // indica si la posición es valida y contiene una ficha de algún jugador
	char casilla;   // casilla es la casilla que se lee del tablero

	int fin = 0;
	while (fin == 0) {
		FA = FA + SF;
		CA = CA + SC;
		casilla = ficha_valida(tablero, FA, CA, &posicion_valida);
		if ((posicion_valida == 1) && (casilla != color))
		{
			*longitud = *longitud + 1;
		}
		else if ((posicion_valida == 1) && (casilla == color)) {
			fin = 1;
		} else {
			*longitud = 0;
			fin = 1;
		}
	}
	if (*longitud > 0) {
		return PATRON_ENCONTRADO;
	} else {
		return NO_HAY_PATRON;
	}
}
////////////////////////////////////////////////////////////////////////////////
// Implementación de patrón_volteo iterativa con inlining de la función ficha_valida
int patron_volteo_c_iter_inline(char tablero[][DIM], int *longitud, char FA, char CA, char SF, char SC, char color){
	int fin = 0;
	while (fin == 0) {
		FA = FA + SF;
		CA = CA + SC;
		if ((FA < DIM) && (FA >= 0) && (CA < DIM) && (CA >= 0) && (tablero[(int)FA][(int)CA] != CASILLA_VACIA))
		{
			if (tablero[(int)FA][(int)CA] != color)
			{
				*longitud = *longitud + 1;
			}
			else {
				fin = 1;
			}
		}
		else
		{
			*longitud = 0;
			fin = 1;
		}
	}
	if (*longitud > 0) {
		return PATRON_ENCONTRADO;
	} else {
		return NO_HAY_PATRON;
	}
}
////////////////////////////////////////////////////////////////////////////////
// voltea n fichas en la dirección que toque
// SF y SC son las cantidades a sumar para movernos en la dirección que toque
// color indica el color de la pieza que se acaba de colocar
// FA y CA son la fila y columna a analizar
void voltear(char tablero[][DIM], unsigned char FA, unsigned char CA, unsigned char SF, unsigned char SC, int n, char color)
{
    int i;

    for (i = 0; i < n; i++)
    {
        FA = FA + SF;
        CA = CA + SC;
        tablero[FA][CA] = color;
    }
}
////////////////////////////////////////////////////////////////////////////////
// comprueba si hay que actualizar alguna ficha
// no comprueba que el movimiento realizado sea válido
// f y c son la fila y columna a analizar
// char vSF[DIM] = {-1,-1, 0, 1, 1, 1, 0,-1};
// char vSC[DIM] = { 0, 1, 1, 1, 0,-1,-1,-1};
int actualizar_tablero(char tablero[][DIM], unsigned char f, unsigned char c, char color)
{
    char SF, SC; // cantidades a sumar para movernos en la dirección que toque
    int i, flip, patron;

    for (i = 0; i < DIM; i++) // 0 es Norte, 1 NE, 2 E ...
    {
        SF = vSF[i];
        SC = vSC[i];
        // flip: numero de fichas a voltear
        flip = 0;
        patron = patron_volteo_implementation(tablero, &flip, f, c, SF, SC, color);
        //printf("Flip: %d \n", flip);
        if (patron == PATRON_ENCONTRADO )
        {
            voltear(tablero, f, c, SF, SC, flip, color);
        }
    }
    return 0;
}

/////////////////////////////////////////////////////////////////////////////////
// Recorre todo el tablero comprobando en cada posición si se puede mover
// En caso afirmativo, consulta la puntuación de la posición y si es la mejor
// que se ha encontrado la guarda
// Al acabar escribe el movimiento seleccionado en f y c

// Candidatas
// NO    0
// SI    1
// CASILLA_OCUPADA 2
int elegir_mov(char candidatas[][DIM], char tablero[][DIM], unsigned char *f, unsigned char *c)
{
    int i, j, k, found;
    int mf = -1; // almacena la fila del mejor movimiento encontrado
    int mc;      // almacena la columna del mejor movimiento encontrado
    char mejor = 0; // almacena el mejor valor encontrado
    int patron, longitud;
    char SF, SC; // cantidades a sumar para movernos en la dirección que toque

    // Recorremos todo el tablero comprobando dónde podemos mover
    // Comparamos la puntuación de los movimientos encontrados y nos quedamos con el mejor
    for (i=0; i<DIM; i++)
    {
        for (j=0; j<DIM; j++)
        {   // indica en qué casillas quizá se pueda mover
            if (candidatas[i][j] == SI)
            {
                if (tablero[i][j] == CASILLA_VACIA)
                {
                    found = 0;
                    k = 0;

                    // en este bucle comprobamos si es un movimiento válido
                    // (es decir si implica voltear en alguna dirección)
                    while ((found == 0) && (k < DIM))
                    {
                        SF = vSF[k];    // k representa la dirección que miramos
                        SC = vSC[k];    // 1 es norte, 2 NE, 3 E ...

                        // nos dice qué hay que voltear en cada dirección
                        longitud = 0;
                        patron = patron_volteo_implementation(tablero, &longitud, i, j, SF, SC, FICHA_BLANCA);
                        //  //printf("%d ", patron);
                        if (patron == PATRON_ENCONTRADO)
                        {
                            found = 1;
                            if (tabla_valor[i][j] > mejor)
                            {
                                mf = i;
                                mc = j;
                                mejor = tabla_valor[i][j];
                            }
                        }
                        k++;
                        // si no hemos encontrado nada probamos con la siguiente dirección
                    }
                }
            }
        }
    }
    *f = (char) mf;
    *c = (char) mc;
    // si no se ha encontrado una posición válida devuelve -1
    return mf;
}
////////////////////////////////////////////////////////////////////////////////
// Cuenta el número de fichas de cada color.
// Los guarda en la dirección b (blancas) y n (negras)
void contar(char tablero[][DIM], int *b, int *n)
{
    int i,j;

    *b = 0;
    *n = 0;

    // recorremos todo el tablero contando las fichas de cada color
    for (i=0; i<DIM; i++)
    {
        for (j=0; j<DIM; j++)
        {
            if (tablero[i][j] == FICHA_BLANCA)
            {
                (*b)++;
            }
            else if (tablero[i][j] == FICHA_NEGRA)
            {
                (*n)++;
            }
        }
    }
}

void actualizar_candidatas(char candidatas[][DIM], unsigned char f, unsigned char c)
{
    // donde ya se ha colocado no se puede volver a colocar
    // En las posiciones alrededor sí
    candidatas[f][c] = CASILLA_OCUPADA;
    if (f > 0)
    {
        if (candidatas[f-1][c] != CASILLA_OCUPADA)
            candidatas[f-1][c] = SI;

        if ((c > 0) && (candidatas[f-1][c-1] != CASILLA_OCUPADA))
            candidatas[f-1][c-1] = SI;

        if ((c < 7) && (candidatas[f-1][c+1] != CASILLA_OCUPADA))
            candidatas[f-1][c+1] = SI;
    }
    if (f < 7)
    {
        if (candidatas[f+1][c] != CASILLA_OCUPADA)
            candidatas[f+1][c] = SI;

        if ((c > 0) && (candidatas[f+1][c-1] != CASILLA_OCUPADA))
            candidatas[f+1][c-1] = SI;

        if ((c < 7) && (candidatas[f+1][c+1] != CASILLA_OCUPADA))
            candidatas[f+1][c+1] = SI;
    }
    if ((c > 0) && (candidatas[f][c-1] != CASILLA_OCUPADA))
        candidatas[f][c-1] = SI;

    if ((c < 7) && (candidatas[f][c+1] != CASILLA_OCUPADA))
        candidatas[f][c+1] = SI;
}


#if defined (TEST_BENCH_1) || defined (TEST_BENCH_3)
static unsigned int rand_seed;
////////////////////////////////////////////////////////////////////////////////
// Genera una semilla aleatoria dada una cadena de caracteres, en especial
// el macro __TIME__ ("hh:mm:ss")
void srand(char time[8]){
	rand_seed = (((time[7] - '0') * 10 + (time[6] - '0')) * 60 +
			((time[4] - '0') * 10 + (time[3] - '0'))) * 60 +
			(time[1] - '0') * 10 + (time[0] - '0');
}

////////////////////////////////////////////////////////////////////////////////
// Genera un número aleatorio
int rand_r (unsigned int *seed)
{
  unsigned int next = *seed;
  int result;

  next *= 1103515245;
  next += 12345;
  result = (unsigned int) (next / 65536) % 2048;

  next *= 1103515245;
  next += 12345;
  result <<= 10;
  result ^= (unsigned int) (next / 65536) % 1024;

  next *= 1103515245;
  next += 12345;
  result <<= 10;
  result ^= (unsigned int) (next / 65536) % 1024;

  *seed = next;

  return result;
}
////////////////////////////////////////////////////////////////////////////////
// Genera un número aleatorio
int rand(){
	return rand_r(&rand_seed);
}
////////////////////////////////////////////////////////////////////////////////
// Genera un número aleatorio dentro de un intervalo [min, max]
int rand_interval(int min, int max) {
	return (rand() % (max - min + 1)) + min;
}

#if defined (TEST_BENCH_3)
static unsigned char mov_auto_fila[DIM*DIM/2];
static unsigned char mov_auto_columna[DIM*DIM/2];
static int mov_auto_cursor = 0;
static int mov_auto_num = 0;
#endif // defined (TEST_BENCH_3)

////////////////////////////////////////////////////////////////////////////////
// Genera movimientos válidos de menera aleatoria para jugar partidas contra
// la IA programada para las fichas blancas. Con proposito de automatizar las
// pruebas de los TEST_BENCH_1 y TEST_BENCH 3.
int elegir_mov_auto(char candidatas[][DIM], char tablero[][DIM], volatile unsigned char *f, volatile unsigned char *c){
	int i, j, k, found, exist, fin;
	int patron, longitud;
	char SF, SC; // cantidades a sumar para movernos en la dirección que toque

	// Recorremos todo el tablero comprobando dónde podemos mover
	// Comparamos la puntuación de los movimientos encontrados y nos quedamos con el mejor

	fin = 0;
	do {
		exist = 0; // Si existe patrón en alguna casilla candidata

		i=0;
		while((i<DIM) && (fin == 0))
		{
			j=0;
			while((j<DIM) && (fin == 0))
			{   // indica en qué casillas quizá se pueda mover
				if (candidatas[i][j] == SI)
				{
					if (tablero[i][j] == CASILLA_VACIA)
					{
						found = 0;
						k = 0;

						// en este bucle comprobamos si es un movimiento válido
						// (es decir si implica voltear en alguna dirección)
						while ((found == 0) && (k < DIM))
						{
							SF = vSF[k];    // k representa la dirección que miramos
							SC = vSC[k];    // 1 es norte, 2 NE, 3 E ...

							// nos dice qué hay que voltear en cada dirección
							longitud = 0;
							patron = patron_volteo_implementation(tablero, &longitud, i, j, SF, SC, FICHA_NEGRA);

							if (patron == PATRON_ENCONTRADO)
							{
								found = 1;
								exist = 1;
							}
							k++;
							// si no hemos encontrado nada probamos con la siguiente dirección
						}

						if (found==1) {
							// seleccionar ese movimiento o no de manera aleatoria
							if (rand_interval(0,100) < 33 ) { // 33% probabilidad de ser aceptada
								// Aceptar movimiento
								*f = (char) i;
								*c = (char) j;

#if defined (TEST_BENCH_3)
								// Almacenar movimiento en vector
								mov_auto_fila[mov_auto_cursor] = *f;
								mov_auto_columna[mov_auto_cursor] = *c;
								mov_auto_cursor++;
								mov_auto_num++;
#endif // defined (TEST_BENCH_3)

								fin = 1;
							} else {
								// Continuar explorando movimientos
								found = 0;
							}
						}
					}
				}
				j++;
			}
			i++;
		}
	} while ((exist == 1) && (fin == 0));

	if (exist == 1) {
		return 0; // Se puede colocar ficha
	} else {
		return -1; // No se puede colocar
	}
}

#if defined (TEST_BENCH_1)
////////////////////////////////////////////////////////////////////////////////
// Ejecuta todas las implementaciones de patron_volteo y compara sus resultados,
// permite diferenciar qué función genera resultados diferentes mediante el uso
// de breakpoints. Si todo funciona bien, devuelve el resultado del patron_volteo original
int patron_volteo_test(char tablero[][DIM], int *longitud, char FA, char CA, char SF, char SC, char color)
{
	int longitud_c_c = 0, return_c_c = 0;
	return_c_c = patron_volteo(tablero, &longitud_c_c, FA, CA, SF, SC, color);
	int longitud_arm_c = 0, return_arm_c = 0;
	return_arm_c = patron_volteo_arm_c(tablero, &longitud_arm_c, FA, CA, SF, SC, color);
	int longitud_arm_arm = 0, return_arm_arm = 0;
	return_arm_arm = patron_volteo_arm_arm(tablero, &longitud_arm_arm, FA, CA, SF, SC, color);
	int longitud_c_iter = 0, return_c_iter = 0;
	return_c_iter = patron_volteo_c_iter(tablero, &longitud_c_iter, FA, CA, SF, SC, color);
	int longitud_c_iter_inline = 0, return_c_iter_inline = 0;
	return_c_iter_inline = patron_volteo_c_iter_inline(tablero, &longitud_c_iter_inline, FA, CA, SF, SC, color);
	int longitud_arm_iter = 0, return_arm_iter = 0;
	return_arm_iter = patron_volteo_arm_iter(tablero, &longitud_arm_iter, FA, CA, SF, SC, color);
	int longitud_arm_iter_v2 = 0, return_arm_iter_v2 = 0;
	return_arm_iter_v2 = patron_volteo_arm_iter_v2(tablero, &longitud_arm_iter_v2, FA, CA, SF, SC, color);
	
	if ((return_c_c != return_arm_c) || ((return_c_c != NO_HAY_PATRON) && (longitud_c_c != longitud_arm_c))) {
		// Fallo de función arm_c
		/* [BREAKPOINT] */
		longitud_arm_c = 0;
		return_arm_c = patron_volteo_arm_c(tablero, &longitud_arm_c, FA, CA, SF, SC, color);
		while(1);
		return NO_HAY_PATRON;
	} else if ((return_c_c != return_arm_arm) || ((return_c_c != NO_HAY_PATRON) && (longitud_c_c != longitud_arm_arm))) {
		// Fallo de función arm_arm
		/* [BREAKPOINT] */
		longitud_arm_arm = 0;
		return_arm_arm = patron_volteo_arm_arm(tablero, &longitud_arm_arm, FA, CA, SF, SC, color);
		while(1);
		return NO_HAY_PATRON;
	} else if ((return_c_c != return_c_iter) || ((return_c_c != NO_HAY_PATRON) && (longitud_c_c != longitud_c_iter))) {
		// Fallo de función c_iter
		/* [BREAKPOINT] */
		longitud_c_iter = 0;
		return_c_iter = patron_volteo_c_iter(tablero, &longitud_c_iter, FA, CA, SF, SC, color);
		while(1);
		return NO_HAY_PATRON;
	} else if ((return_c_c != return_c_iter_inline) || ((return_c_c != NO_HAY_PATRON) && (longitud_c_c != longitud_c_iter_inline))) {
		// Fallo de función c_iter_inline
		/* [BREAKPOINT] */
		longitud_c_iter_inline = 0;
		return_c_iter_inline = patron_volteo_c_iter_inline(tablero, &longitud_c_iter_inline, FA, CA, SF, SC, color);
		while(1);
		return NO_HAY_PATRON;
	} else if ((return_c_c != return_arm_iter) || ((return_c_c != NO_HAY_PATRON) && (longitud_c_c != longitud_arm_iter))) {
		// Fallo de función arm_iter
		/* [BREAKPOINT] */
		longitud_arm_iter = 0;
		return_arm_iter = patron_volteo_arm_iter(tablero, &longitud_arm_iter, FA, CA, SF, SC, color);
		while(1);
		return NO_HAY_PATRON;
	} else if ((return_c_c != return_arm_iter_v2) || ((return_c_c != NO_HAY_PATRON) && (longitud_c_c != longitud_arm_iter_v2))) {
		// Fallo de función arm_iter_v2
		/* [BREAKPOINT] */
		longitud_arm_iter_v2 = 0;
		return_arm_iter_v2 = patron_volteo_arm_iter_v2(tablero, &longitud_arm_iter_v2, FA, CA, SF, SC, color);
		while(1);
		return NO_HAY_PATRON;
	} else {
		/* Todo ha ido bien. Devolver valores de c_c */
		*longitud = longitud_c_c;
		return return_c_c;
	}
}
// defined (TEST_BENCH_1)
#elif defined (TEST_BENCH_3)
////////////////////////////////////////////////////////////////////////////////
// Establece al comienzo (índice 0) el iterador que recorre el vector de
// movimientos usado para jugar varias partidas iguales en el TEST_BENCH_3
// de manera que sean aleatorias pero todas iguales.
void mov_auto_iterator_begin(){
	mov_auto_cursor = 0;
}
////////////////////////////////////////////////////////////////////////////////
// Avanza el iterador que recorre el vector de movimientos explicado en
// el comentario de la función @ref mov_auto_cursor
int mov_auto_iterator_next(volatile unsigned char *f, volatile unsigned char *c){
	// Almacenar movimiento en vector
	if (mov_auto_cursor < mov_auto_num) {
		*f = mov_auto_fila[mov_auto_cursor];
		*c = mov_auto_columna[mov_auto_cursor];
		mov_auto_cursor++;
		return 0; // Se puede colocar ficha
	} else {
		return -1; // No se puede colocar
	}
}

static char __attribute__ ((aligned (8))) tablero_salvado[DIM][DIM];
////////////////////////////////////////////////////////////////////////////////
// Guarda el contenido de tablero en tablero_salvado
void salvar_tablero(char tablero[][DIM]) {
	int i,j;
	for (i=0; i<DIM; i++)
	{
		for (j=0; j<DIM; j++)
		{
			tablero_salvado[i][j] = tablero[i][j];
		}
	}
}
////////////////////////////////////////////////////////////////////////////////
// Compara tablero con tablero_salvado, devuelve 1 si son iguales, 0 en caso contrario
int comparar_tablero(char tablero[][DIM]) {
	int i,j;
	for (i=0; i<DIM; i++)
	{
		for (j=0; j<DIM; j++)
		{
			if (tablero_salvado[i][j] != tablero[i][j]){
				return 0;
			}
		}
	}
	return 1;
}

#if defined (ENVIRONMENT_IO)
static volatile unsigned int time_patron_volteo = 0;
////////////////////////////////////////////////////////////////////////////////
// Mide el tiempo de ejecución de la implementación correspondiente de patron_volteo
// y lo suma a time_patron_volteo
int patron_volteo_time(char tablero[][DIM], int *longitud, char FA, char CA, char SF, char SC, char color){
	unsigned int t0 = timer2_leer();
	int ret = patron_volteo_implementation(tablero,longitud,FA,CA,SF,SC,color);
	unsigned int tf = timer2_leer();
	time_patron_volteo += tf-t0;
	return ret;
}
#endif // defined (ENVIRONMENT_IO)
#endif // defined (TEST_BENCH_3)
#endif // defined (TEST_BENCH_1) || defined (TEST_BENCH_3)

void reversi8_inicializar() {

}

////////////////////////////////////////////////////////////////////////////////
// Proceso principal del juego
// Utiliza el tablero,
// y las direcciones en las que indica el jugador la fila y la columna
// y la señal de ready que indica que se han actualizado fila y columna
// tablero, fila, columna y ready son variables globales aunque deberían ser locales de reversi8,
// la razón es que al meterlas en la pila no las pone juntas, y así jugar es más complicado.
// en esta versión el humano lleva negras y la máquina blancas
// no se comprueba que el humano mueva correctamente.
// Sólo que la máquina realice un movimiento correcto.
void reversi8()
{

#if defined (NORMAL_PLAY) || defined (TEST_BENCH_1) || defined (TEST_BENCH_3)
	 ////////////////////////////////////////////////////////////////////
	 // Tablero candidatas: se usa para no explorar todas las posiciones del tablero
	 // sólo se exploran las que están alrededor de las fichas colocadas
	 ////////////////////////////////////////////////////////////////////
	char __attribute__ ((aligned (8))) candidatas[DIM][DIM] =
    {
        {NO,NO,NO,NO,NO,NO,NO,NO},
        {NO,NO,NO,NO,NO,NO,NO,NO},
        {NO,NO,NO,NO,NO,NO,NO,NO},
        {NO,NO,NO,NO,NO,NO,NO,NO},
        {NO,NO,NO,NO,NO,NO,NO,NO},
        {NO,NO,NO,NO,NO,NO,NO,NO},
        {NO,NO,NO,NO,NO,NO,NO,NO},
        {NO,NO,NO,NO,NO,NO,NO,NO}
    };


    int done;     // la máquina ha conseguido mover o no
    int move = 0; // el humano ha conseguido mover o no
    int blancas, negras; // número de fichas de cada color
    int fin = 0;  // fin vale 1 si el humano no ha podido mover
                  // (ha introducido un valor de movimiento con algún 8)
                  // y luego la máquina tampoco puede
    unsigned char f, c;    // fila y columna elegidas por la máquina para su movimiento
	init_table(tablero, candidatas);
#if defined (NORMAL_PLAY)
	patron_volteo_implementation = &patron_volteo_arm_iter_v2; /* Mejor versión */
#elif defined (TEST_BENCH_1)
	patron_volteo_implementation = &patron_volteo_test; /* Prueba todas las implementaciones */
#elif defined (TEST_BENCH_3)
	mov_auto_iterator_begin();
	static int num_invocacion = 1; // Veces que ha sido invocada
	switch (num_invocacion){
	
	/* Primera invocación: generar movimientos aleatorios */
	case 1:
		srand(__TIME__);
		patron_volteo_implementation = &patron_volteo;
		break;
	
	/* Resto de iteraciones: utilizar movimientos generados */
	case 2:
		patron_volteo_implementation = &patron_volteo;
		break;
	case 3:
		patron_volteo_implementation = &patron_volteo_arm_c;
		break;
	case 4:
		patron_volteo_implementation = &patron_volteo_arm_arm;
		break;
	case 5:
		patron_volteo_implementation = &patron_volteo_c_iter;
		break;
	case 6:
		patron_volteo_implementation = &patron_volteo_arm_iter;
		break;
	case 7:
		patron_volteo_implementation = &patron_volteo_arm_iter_v2;
		break;
	case 8:
		patron_volteo_implementation = &patron_volteo_c_iter_inline;
		break;

	default:
		contar(tablero, &blancas, &negras);
		/* [BREAKPOINT] */
		while(1) { /* FIN */ }
		break;
	}
#if defined (ENVIRONMENT_IO)
    time_patron_volteo = 0;
    timer2_empezar(); /* Comenzar a medir tiempo total de partida */
#endif // defined (ENVIRONMENT_IO)
#endif // defined (TEST_VERSION_3)

    while (fin == 0)
    {
        move = 0;

#if defined (NORMAL_PLAY)
        esperar_mov(&ready); /* [BREAKPOINT] */
#elif defined (TEST_BENCH_1)
		/* Genera movimientos aleatorios */
       	done = elegir_mov_auto(candidatas, tablero, &fila, &columna);
#elif defined (TEST_BENCH_3)
        if (num_invocacion == 1) {
			/* Primera iteración: generar movimientos aleatorios */
        	done = elegir_mov_auto(candidatas, tablero, &fila, &columna);
        } else {
			/* Resto de iteraciones: utilizar movimientos generados */
        	done = mov_auto_iterator_next(&fila, &columna);
        }
#endif
#if defined (TEST_BENCH_1) || defined (TEST_BENCH_3)
        /* Detección de NO_HAY_PATRÓN */
        if (done == -1) {
			fila = DIM;
			columna = DIM;
		}		
#endif // defined (TEST_BENCH_3)

        // si la fila o columna son 8 asumimos que el jugador no puede mover
        if (((fila) != DIM) && ((columna) != DIM))
        {
            tablero[fila][columna] = FICHA_NEGRA;
            actualizar_tablero(tablero, fila, columna, FICHA_NEGRA);
            actualizar_candidatas(candidatas, fila, columna);
            move = 1;
        }

        // escribe el movimiento en las variables globales fila columna
        done = elegir_mov(candidatas, tablero, &f, &c);
        if (done == -1)
        {
            if (move == 0)
                fin = 1;
        }
        else
        {
            tablero[f][c] = FICHA_BLANCA;
            actualizar_tablero(tablero, f, c, FICHA_BLANCA);
            actualizar_candidatas(candidatas, f, c);
        }
	}
#if defined (TEST_BENCH_3)
#if defined (ENVIRONMENT_IO)
	/* Terminar de medir tiempo de partida */
	volatile unsigned int time = timer2_parar(); /* Tiempo total de partida */
    time++;time--; /* [BREAKPOINT] */
    time_patron_volteo = 0; /* Tiempo de usado por la implementación de patrón volteo */
#endif // defined (ENVIRONMENT_IO)

	switch (num_invocacion){
	case 1:
		/* Primera iteración: salvar tablero */
		salvar_tablero(tablero);
		break;
	default:
		/* Resto de iteraciones: comparar tablero */
		if (comparar_tablero(tablero) == 0) {
			volatile int kk = 1;
			kk++; /* [BREAKPOINT] */
			while(1) { /* ERROR */ }
		}
		break;
	}
	num_invocacion++; // Nueva invocación
	reversi8();
// defined (TEST_BENCH_3)
#else
	 contar(tablero, &blancas, &negras);
#endif // defined (ENVIRONMENT_IO)
// defined (NORMAL_PLAY) || defined (TEST_BENCH_1) || defined (TEST_BENCH_3)
#elif defined (TEST_BENCH_2)
    int (*func[N])(char[][DIM], int*, char, char, char, char, char) = {
    		patron_volteo,
    		patron_volteo_arm_c,
    		patron_volteo_arm_arm,
    		patron_volteo_c_iter,
    		patron_volteo_c_iter_inline,
    		patron_volteo_arm_iter,
    		patron_volteo_arm_iter_v2};
    volatile int result1 = test_version1(func, N, tablero);
    volatile int result2 = test_version2(func, N, tablero);
    volatile int result3 = test_version3(func, N, tablero);
    volatile int result4 = test_version4(func, N, tablero);
    volatile int result5 = test_version5(func, N, tablero);
	
#if defined (ENVIRONMENT_IO)
	int i;
    for (i = 0; i < N; i++) {
		volatile int result6 = test_version6(func[i]);
		volatile int result7 = test_version7(func[i]);
		volatile int result8 = test_version8(func[i]);
		volatile int result9 = test_version9(func[i]);
	    result6++;result6--; /* [BREAKPOINT] */
	    result7++;result7--;
	    result8++;result8--;
	    result9++;result9--;
    }
#endif // defined (ENVIRONMENT_IO)
#endif // defined (TEST_BENCH_2)

    while(1){/* FIN */}; /* [BREAKPOINT] */
}

void reversi_inicializar() {
	/* TODO juntar con la otra funcion, esto es de prueba */
}

void reversi_main() {
	/* TODO juntar con la otra funcion, esto es de prueba */
	reversi_inicializar();
	volatile int kk;
	while(1) {
		unsigned int ahora = timer0_leer();
		enum pulsacion_button buttonPulsado = antirebotes_gestionar(ahora);
		switch (buttonPulsado) {
		case pulsacion_iz:
			kk++;
			break;
		case pulsacion_dr:
			kk++;
			break;
		default:
			kk++;
		}
	}
}
