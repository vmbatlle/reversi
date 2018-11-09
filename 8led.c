/*********************************************************************************************
* Fichero:	8led.c
* Autor:
* Descrip:	Funciones de control del display 8-segmentos
* Version:
*********************************************************************************************/

/*--- ficheros de cabecera ---*/
#include "8led.h"
#include "44b.h"
#include "44blib.h"

#define ENVIRONMENT_EMULATOR

volatile int emular_8led = 0;

#if !defined(ENVIRONMENT_EMULATOR)
/*--- variables globales del módulo ---*/
/* tabla de segmentos */
static int Symbol[size_8led] = { cero, uno, dos, tres, cuatro, cinco, seis, siete, ocho, nueve, A, B, C, D, E, F, blank};
#endif

/*--- código de las funciones ---*/
void D8led_inicializar(void)
{
	/* Estado inicial del display con todos los segmentos iluminados
	   (buscar en los ficheros de cabecera la direccion implicada) */
	LED8ADDR = (unsigned char) cero;
}

void D8led_gestionar(int value)
{
#if defined(ENVIRONMENT_EMULATOR)
	emular_8led = value;
	asm("" : [none] "=r" (emular_8led) : ); // TODO
#else
	/* muestra el Symbol[value] en el display (analogo al caso anterior) */
	if ((value >= 0) && (value < size_8led)) {
		LED8ADDR = (unsigned char) Symbol[value];
	}
#endif
}
