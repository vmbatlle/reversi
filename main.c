/*********************************************************************************************
* Fichero:	main.c
* Autor:    Diego Royo Meneses <740388@unizar.es>, Victor M. Batlle <736478@unizar.es>
* Descrip:	punto de entrada de C
* Version:  <P4-ARM.timer-leds>
*********************************************************************************************/

#include "timer0.h"
#include "timer2.h"
#include "button.h"
#include "led.h"
#include "44blib.h"
#include "44b.h"
#include "reversi8_2018.h"

//#define TEST_BENCH_TIMER2
#define TEST_BENCH_TIMER0
//#define TEST_BENCH_BUTTON

/*--- codigo de funciones ---*/
void Main(void)
{
	/* Inicializa controladores */
	sys_init();         // Inicializacion de la placa, interrupciones y puertos
	timer0_inicializar();	// Inicialización del timer 0
	timer2_inicializar();	// Inicialización del timer 2
	leds_off();				// Apagar leds

#if defined (TEST_BENCH_TIMER2)
	volatile unsigned int tiempo1ms0,tiempo10ms0,tiempo1s0,tiempo10s0,
						  tiempo1ms2,tiempo10ms2,tiempo1s2,tiempo10s2;
	while (1)
	{
		timer2_empezar();
		timer0_empezar();
		Delay(10);
		tiempo1ms0 = timer0_parar();
		tiempo1ms2 = timer2_parar();
		tiempo1ms0++;tiempo1ms0--;
		tiempo1ms2++;tiempo1ms2--;

		timer2_empezar();
		timer0_empezar();
		Delay(100);
		tiempo10ms0 = timer0_parar();
		tiempo10ms2 = timer2_parar();
		tiempo10ms2++;tiempo10ms2--;
		tiempo10ms0++;tiempo10ms0--;

		timer2_empezar();
		timer0_empezar();
		Delay(10000);
		tiempo1s0 = timer0_parar();
		tiempo1s2 = timer2_parar();
		tiempo1s2++;tiempo1s2--;
		tiempo1s0++;tiempo1s0--;

		timer2_empezar();
		timer0_empezar();
		Delay(100000);
		tiempo10s0 = timer0_parar();
		tiempo10s2 = timer2_parar();
		tiempo10s2++;tiempo10s2--;
		tiempo10s0++;tiempo10s0--; /* [BREAKPOINT] */
	}
#elif defined(TEST_BENCH_TIMER0)
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
#elif defined(TEST_BENCH_BUTTON)
	button_iniciar();
	button_empezar(&button_tratar);
	double kk = 1.0;
	while(1){
		kk = kk * 372.47;
	}
#else
	reversi8();
#endif
}
