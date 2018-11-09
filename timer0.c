/**
 * @file    timer0.c
 * @author  Victor M. Batlle <736478@unizar.es>, Diego Royo Meneses <740388@unizar.es>
 * @brief   Funciones de control del timer0 del s3c44b0x
 * @version 1.0.0
 */

/*--- ficheros de cabecera ---*/
#include "timer0.h"
#include "44b.h"
#include "44blib.h"

//#define ENVIRONMENT_EMULATOR

enum {
	MIN_COUNT = 0, //0x0000
	MAX_COUNT = 41237 // (MCLK/DIVISOR)/FRECUENCIA_DESEADA = (66MHz/32)/50Hz
};

/*--- variables globales ---*/
/* static -> duración igual a la duración del programa, ámbito restringido al fichero.
 * */
static volatile unsigned long int timer0_num_int = 0; // Número de ciclos completos

/* declaración de función que es rutina de servicio de interrupción
 * https://gcc.gnu.org/onlinedocs/gcc/ARM-Function-Attributes.html */
void timer0_ISR(void) __attribute__((interrupt("IRQ")));

/*--- codigo de las funciones ---*/
void timer0_ISR(void)
{
	timer0_num_int++;

	/* borrar bit en I_ISPC para desactivar la solicitud de interrupción*/
	rI_ISPC |= BIT_TIMER0; // BIT_TIMER0 está definido en 44b.h y pone un uno en el bit 11 que correponde al Timer0
}

void timer0_inicializar(void)
{
	/* Configuraion controlador de interrupciones */
	rINTMOD &= ~(BIT_TIMER0); // Configura la linea INT_TIMER0 como de tipo IRQ (bit 11 = 0)

	/* NO se configura el vector de interrupciones. Ya se ha realizado en 44binit.asm. */
	rINTMSK &= ~(BIT_TIMER0); // habilitamos en vector de mascaras de interrupcion el Timer0 (bits 26 y 11, BIT_GLOBAL y BIT_TIMER0 están definidos en 44b.h)

	/* Establece la rutina de servicio para TIMER0 */
	pISR_TIMER0 = (unsigned) timer0_ISR;

	/* Configura el Timer0
	 * Timer_input = MCLK / (preesclado + 1) / divisor */
	rTCFG0 &= 0xFFFFFF00; // ajusta el preescalado para Timer0 y Timer1 al mínimo (bits 7:0)
	rTCFG1 = (rTCFG1 & 0xFFFFFFF0) | 0x00000004; // selecciona la entrada del MUX0 (bits 3:0) que proporciona el reloj. Estableciendo el divisor a 1/32
							// 0000 -> 1/2, 0001 -> 1/4, 0010 -> 1/8, 0011 -> 1/16, 01xx -> 1/32.
	rTCNTB0 = MAX_COUNT;// valor inicial de cuenta (la cuenta es descendente)
	rTCMPB0 = MIN_COUNT;// valor de comparación
}

void timer0_empezar(void) {
	timer0_num_int = 0;
	/* establecer update=manual (bit 1 = 1) */
	rTCON |= 0x1<<1;
	/* iniciar timer (bit 0 = 1) con auto-reload (bit 3 = 1) y desactivar manual update (bit 1 = 0) */
	rTCON &= ~(0x1<<1);
	rTCON |= 0x1<<0 | 0x1<<3;
}

unsigned long int timer0_leer(void) {
#if defined(ENVIRONMENT_EMULATOR)
	static volatile unsigned long int emular_timer0 = 0;
	emular_timer0++;
	return emular_timer0;
#else
	return timer0_num_int;
#endif
}

unsigned long int timer0_parar(void) {
	/* detener timer (bit 0 = 0) */
	rTCON &= ~(0x1<<0);
	return timer0_leer();
}

