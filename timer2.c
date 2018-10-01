/**
 * @file    timer2.c
 * @author  Victor M. Batlle <736478@unizar.es>
 * @brief   Funciones de control del timer2 del s3c44b0x
 * @version 1.0.0
 */

/*--- ficheros de cabecera ---*/
#include "timer2.h"
#include "44b.h"
#include "44blib.h"

volatile int switch2_leds = 0;

/*--- variables globales ---*/
/* static -> duración igual a la duración del programa, ámbito restringido al fichero.
 * */
static volatile unsigned long int timer2_num_int = 0; // Número de ciclos completos

/* declaración de función que es rutina de servicio de interrupción
 * https://gcc.gnu.org/onlinedocs/gcc/ARM-Function-Attributes.html */
void timer2_ISR(void) __attribute__((interrupt("IRQ")));

/*--- codigo de las funciones ---*/
void timer2_ISR(void)
{
	timer2_num_int++;
	switch2_leds = 1;

	/* borrar bit en I_ISPC para desactivar la solicitud de interrupción*/
	rI_ISPC |= BIT_TIMER2; // BIT_TIMER2 está definido en 44b.h y pone un uno en el bit 11 que correponde al Timer2
}

void timer2_inicializar(void)
{
	/* Configuraion controlador de interrupciones */
	rINTMOD &= ~(BIT_TIMER2); // Configura la linea INT_TIMER2 como de tipo IRQ (bit 11 = 0)
	/* TODO: Comprobar si es necesario desactivar explicitamente la linea FIQ */
	rINTCON &= 0b001; // Habilita int. vectorizadas (bit 2 = 0) y la linea IRQ (bit 1 = 0)
	rINTMSK &= ~(BIT_TIMER2); // habilitamos en vector de mascaras de interrupcion el Timer2 (bits 26 y 11, BIT_GLOBAL y BIT_TIMER2 están definidos en 44b.h)

	/* Establece la rutina de servicio para TIMER2 */
	pISR_TIMER2 = (unsigned) timer2_ISR;

	/* Configura el Timer2 
	 * Timer_input = MCLK / (preesclado + 1) / divisor */
	/* TODO: ajustar precisión del temporizador */
	//rTCFG0 |= 255<<8;
	rTCFG0 &= ~(255<<8); // ajusta el preescalado para Timer2 y Timer3 al máximo (bits 15:8)
	rTCFG1 &= ~(~(0b0000)<<8); // selecciona la entrada del MUX2 (bits 11:8) que proporciona el reloj. Estableciendo el divisor a 1/2
							// 0000 -> 1/2, 0001 -> 1/4, 0010 -> 1/8, 0011 -> 1/16, 01xx -> 1/32.
	rTCNTB2 = 65535;// valor inicial de cuenta (la cuenta es descendente)
	rTCMPB2 = 0;// valor de comparación
//	rTCNTB2 = 0xFFFFFFFF;// valor inicial de cuenta (la cuenta es descendente) al máximo
//	rTCMPB2 = 0;// valor de comparación
}

void timer2_empezar(void) {
	timer2_num_int = 0;
	/* establecer update=manual (bit 13 = 1) */
	/* TODO: Comprobar si se necesita activar el inverter a la salida de TOUT2 */
	rTCON |= 0x1<<13;
	/* iniciar timer (bit 12 = 1) con auto-reload (bit 15 = 1) y desactivar manual update (bit 13 = 0) */
	rTCON &= ~(0x1<<13);
	rTCON |= 0x1<<12 | 0x1<<15;
}

unsigned int timer2_leer(void) {
	/* TODO: Implementar calculo del tiempo transcurrido */
	return (198500 * timer2_num_int + 3 * (rTCNTB2 - rTCNTO2)) / 100;

}

unsigned int timer2_parar(void) {
	/* detener timer (bit 12 = 0) */
	unsigned int aux = timer2_leer();
	rTCON &= ~(0x1<<12);
	/* TODO: Comprobar que se puede llamar a timer2_leer() tras haber detenido el timer2 */
	return aux;
}

