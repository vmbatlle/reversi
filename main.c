/*********************************************************************************************
* Fichero:	main.c
* Autor:    Diego Royo Meneses <740388@unizar.es>, Victor M. Batlle <736478@unizar.es>
* Descrip:	punto de entrada de C
* Version:  <P4-ARM.timer-leds>
*********************************************************************************************/

#include "timer0.h"
#include "timer2.h"
#include "exceptions.h"
#include "button.h"
#include "botones_antirebotes.h"
#include "push_debug.h"
#include "8led.h"
#include "44blib.h"
#include "44b.h"
#include "reversi_main.h"

//#define TEST_BENCH_TIMER0
//#define TEST_BENCH_REBOTES

void insertar_pulsacion(enum estado_button button) {
	push_debug((uint8_t) evento_button, (uint32_t)button);
	button_empezar(insertar_pulsacion);
}

/*--- codigo de funciones ---*/
void Main(void)
{
	/* Inicializa controladores */
	sys_init();         // Inicializacion de la placa, interrupciones y puertos
	excepciones_inicializar(); // Inicialización del capturador de excepciones
	timer0_inicializar();	// Inicialización del timer 0
	timer2_inicializar();	// Inicialización del timer 2
	D8led_inicializar();
	push_iniciar();
	button_iniciar();
	antirebotes_iniciar();

	reversi_main();

#if defined(TEST_BENCH_TIMER0)
	unsigned long int anterior = 0;
	timer0_empezar();
	while(1){
		int ahora = timer0_leer();
		if (anterior + 25 <= ahora) {
			if (led1_status()){
				led1_off();
			} else {
				led1_on();
			}
			anterior = ahora;
		}
	}
#elif defined(TEST_BENCH_REBOTES)
	button_empezar(insertar_pulsacion);
	while(1);
#endif
}
