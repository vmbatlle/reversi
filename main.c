/*********************************************************************************************
* Fichero:	main.c
* Autor:    Diego Royo Meneses <740388@unizar.es>, Victor M. Batlle <736478@unizar.es>
* Descrip:	punto de entrada de C
* Version:  <P4-ARM.timer-leds>
*********************************************************************************************/

#include "timer2.h"
#include "button.h"
#include "44blib.h"
#include "44b.h"
#include "reversi8_2018.h"

//#define TEST_BENCH_TIMER2
#define TEST_BENCH_BUTTON

void button_tratar(enum estado_button button)
{
	static int int_count = 0;
	// TODO: quitar int_count y el 8led, estan solo por tests
	if (button == button_iz) {
		int_count--;
	} else if (button == button_dr) {
		int_count++;
	}
	//D8Led_symbol(int_count & 0x000f); // sacamos el valor por pantalla (módulo 16)
}

/*--- codigo de funciones ---*/
void Main(void)
{
	/* Inicializa controladores */
	sys_init();         // Inicializacion de la placa, interrupciones y puertos
	//timer2_inicializar();	// Inicialización del timer 2

#if defined (TEST_BENCH_TIMER2)
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
		tiempo10s++;tiempo10s--; /* [BREAKPOINT] */
	}
#elif defined(TEST_BENCH_BUTTON)
	button_iniciar();
	button_empezar(&button_tratar);
	int kk = 1;
	while(1){
		kk = kk + 1;
		button_empezar(&button_tratar);
	}
#else
	reversi8();
#endif
}
