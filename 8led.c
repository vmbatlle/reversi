/*********************************************************************************************
* Fichero:	8led.c
* Autor:
* Descrip:	Funciones de control del display 8-segmentos
* Version:
*********************************************************************************************/

/*--- ficheros de cabecera ---*/
#include "options_environment.h"
#include "8led.h"
#include "44b.h"
#include "44blib.h"

/*--- variables globales del módulo ---*/
#if defined(ENVIRONMENT_EMULATOR)
static int emular_8led = 0;
#else
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
	// Evitar la optimización de la variable por parte del compilador
	asm("" : [none] "=r" (emular_8led) : );
#else
	/* muestra el Symbol[value] en el display (analogo al caso anterior) */
	if ((value >= 0) && (value < size_8led)) {
		LED8ADDR = (unsigned char) Symbol[value];
	}
#endif
}
