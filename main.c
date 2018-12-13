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
#include "reversi_gui.h"
#include "calibracion_tp.h"
#include "tp.h"

/* Tests posibles, solamente descomentar uno.
 * Todos comentados = ejecución normal del juego de reversi */
//#define TEST_BENCH_TIMER0
//#define TEST_BENCH_REBOTES
//#define TEST_BENCH_LCD_TS

/*--- function declare ---*/
void Main(void);


// Registrar una pulsación de botón insertándola en la pila de debug
// Útil para ver los rebotes producidos por el botón (TEST_BENCH_REBOTES)
void insertar_pulsacion(enum estado_button button) {
	push_debug((uint8_t) evento_button, (uint32_t)button);
	button_empezar(insertar_pulsacion);
}


void Main(void)
{
	/* Inicializa controladores */
	sys_init();         // Inicializacion de la placa, interrupciones y puertos
	excepciones_inicializar(); // Inicialización del capturador de excepciones

	timer0_inicializar();	// Inicialización del timer 0
	timer2_inicializar();	// Inicialización del timer 2
	latido_inicializar();	// Latido LED
	D8led_inicializar();	// 8led
	push_iniciar();			// Pila de debug
	button_iniciar();		// Controlador de botones
	antirrebotes_iniciar();	// Controlador de antirrebotes
	gui_inicializar();      // Inicializar GUI

#if defined(TEST_BENCH_TIMER0)
	// Latido LED a 2hz
	timer0_empezar();
	while(1){
		latido_gestionar(timer0_leer());
	}
#elif defined(TEST_BENCH_REBOTES)
	button_empezar(insertar_pulsacion);
	while(1);
#elif defined(TEST_BENCH_LCD_TS)
	gui_empezar();
	calibracion_empezar();
	while(1) {
		gui_touch_screen_test();
	}
	gui_parar();
#else
	// Ejecución normal
	unsigned int cpsr;
	asm("MRS r0, CPSR" : [cpsr] "=r" (cpsr));
	asm("BIC %[cpsr_out], %[cpsr_in], #0x1f" : [cpsr_out] "=r" (cpsr) : [cpsr_in] "r" (cpsr));
	asm("ORR %[cpsr_out], %[cpsr_in], #0x10" : [cpsr_out] "=r" (cpsr) : [cpsr_in] "r" (cpsr));
	asm("MSR CPSR_c, %[cpsr]" : : [cpsr] "r" (cpsr));
	reversi_main();
#endif
}
