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

	timer2_empezar();

	int tiempo = 0;
	int i = 0;
	while (1)
	{

		Delay(1);

		tiempo = timer2_parar();
		i++;
		timer2_empezar();

		/* Cambia los leds con cada interrupcion del temporizador */
		/*if (switch2_leds == 1)
		{
			leds_switch();
			switch2_leds = 0;
		}*/
		leds_switch();
	}
}
