/*********************************************************************************************
* Fichero:	main.c
* Autor:
* Descrip:	punto de entrada de C
* Version:  <P4-ARM.timer-leds>
*********************************************************************************************/

/*--- ficheros de cabecera ---*/
#include "8led.h"
#include "button.h"
#include "led.h"
//#include "timer0.h"
#include "timer2.h"
#include "44blib.h"
#include "44b.h"
#include "reversi8_2018.h"
#include "test_bench.h"

/*--- variables globales ---*/


/*--- codigo de funciones ---*/
void Main(void)
{
	/* Inicializa controladores */
	sys_init();         // Inicializacion de la placa, interrupciones y puertos
//	timer_init();	    // Inicializacion del temporizador
	timer2_inicializar();	// Inicializaci�n del timer 2
//	Eint4567_init();	// inicializamos los pulsadores. Cada vez que se pulse se ver� reflejado en el 8led
//	D8Led_init();       // inicializamos el 8led


	/* Valor inicial de los leds */
	leds_off();
	led1_on();

#if 1

	volatile unsigned int tiempo1ms,tiempo10ms,tiempo1s,tiempo10s;
	while (1)
	{

		timer2_empezar();
		Delay(10);
		tiempo1ms = timer2_parar();
		tiempo1ms++;tiempo1ms--;

		timer2_empezar();
		Delay(100);
		tiempo10ms = timer2_parar();
		tiempo10ms++;tiempo10ms--;

		timer2_empezar();
		Delay(10000);
		tiempo1s = timer2_parar();
		tiempo1s++;tiempo1s--;

		timer2_empezar();
		Delay(100000);
		tiempo10s = timer2_parar();
		tiempo10s++;tiempo10s--;
	}
#else
	reversi8();
#endif
}