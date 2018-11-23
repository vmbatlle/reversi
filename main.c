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
#include "botones_antirrebotes.h"
#include "push_debug.h"
#include "8led.h"
#include "latido.h"
#include "44blib.h"
#include "44b.h"
#include "reversi_main.h"

/*--- function declare ---*/
void Main(void);

/*--- extern function ---*/
extern void Lcd_Test();

//#define TEST_BENCH_TIMER0
//#define TEST_BENCH_REBOTES

void insertar_pulsacion(enum estado_button button) {
	push_debug((uint8_t) evento_button, (uint32_t)button);
	button_empezar(insertar_pulsacion);
}

/*--- codigo de funciones ---*/
void Main(void)
{

    sys_init();        /* Initial 44B0X's Interrupt,Port and UART */
    _Link();           /* Print Misc info */

	/******************/
	/* user interface */
	/******************/
	Uart_Printf("\n\rEmbest 44B0X Evaluation Board(S3CEV40)");
	Uart_Printf("\n\rLCD display Test Example(please look at LCD screen)\n");

	Lcd_Test();
	while(1);


	/* Inicializa controladores */
	sys_init();         // Inicializacion de la placa, interrupciones y puertos
	excepciones_inicializar(); // Inicialización del capturador de excepciones

	//asm volatile (".word 0xe7f000f0\n");

	timer0_inicializar();	// Inicialización del timer 0
	timer2_inicializar();	// Inicialización del timer 2
	latido_inicializar();
	D8led_inicializar();
	push_iniciar();
	button_iniciar();
	antirrebotes_iniciar();

#if defined(TEST_BENCH_TIMER0)
	timer0_empezar();
	while(1){
		latido_gestionar(timer0_leer());
	}
#elif defined(TEST_BENCH_REBOTES)
	button_empezar(insertar_pulsacion);
	while(1);
#else
	// Ejecución normal
	reversi_main();
#endif
}
