/*********************************************************************************************
* Fichero:		timer.c
* Autor:		Victor M. Batlle <736478@unizar.es>
* Descrip:		funciones de control del timer2 del s3c44b0x
* Version:		1.0.0
*********************************************************************************************/

/*--- ficheros de cabecera ---*/
#include "timer.h"
#include "44b.h"
#include "44blib.h"

/*--- variables globales ---*/
/* static -> duraci�n igual a la duraci�n del programa, �mbito restringido al fichero.
 * */
static volatile uint32_t ticks = 0; // N�mero de ciclos completos

/* declaraci�n de funci�n que es rutina de servicio de interrupci�n
 * https://gcc.gnu.org/onlinedocs/gcc/ARM-Function-Attributes.html */
void timer_ISR(void) __attribute__((interrupt("IRQ")));

/*--- codigo de las funciones ---*/
void timer_ISR(void)
{
	ticks++;

	/* borrar bit en I_ISPC para desactivar la solicitud de interrupci�n*/
	rI_ISPC |= BIT_TIMER2; // BIT_TIMER2 est� definido en 44b.h y pone un uno en el bit 11 que correponde al Timer2
}

void timer_init(void)
{
	/* Configuraion controlador de interrupciones */
	rINTMOD &= ~(BIT_TIMER2); // Configura la linea INT_TIMER2 como de tipo IRQ (bit 11 = 0)
	/* TODO: Comprobar si es necesario desactivar explicitamente la linea FIQ */
	rINTCON &= 0b001; // Habilita int. vectorizadas (bit 3 = 0) y la linea IRQ (bit 2 = 0)
	rINTMSK &= ~(BIT_TIMER2); // habilitamos en vector de mascaras de interrupcion el Timer2 (bits 26 y 11, BIT_GLOBAL y BIT_TIMER2 est�n definidos en 44b.h)

	/* Establece la rutina de servicio para TIMER2 */
	pISR_TIMER2 = (unsigned) timer_ISR;

	/* Configura el Timer2 
	 * Timer_input = MCLK / (preesclado + 1) / divisor */
	/* TODO: ajustar precisi�n del temporizador */
	rTCFG0 |= 255<<8; // ajusta el preescalado para Timer2 y Timer3 al m�ximo (bits 15:8)
	rTCFG1 &= ~(0b1111<<8); // selecciona la entrada del MUX2 (bits 11:8) que proporciona el reloj. Estableciendo el divisor a 1/2
							// 0000 -> 1/2, 0001 -> 1/4, 0010 -> 1/8, 0011 -> 1/16, 01xx -> 1/32.
	rTCNTB2 = 0xFFFFFFFF;// valor inicial de cuenta (la cuenta es descendente) al m�ximo
	rTCMPB2 = 0;// valor de comparaci�n
	/* establecer update=manual (bit 13 = 1) */
	/* TODO: Comprobar si se necesita activar el inverter a la salida de TOUT2 */
	rTCON |= 0x1<<13;
	/* iniciar timer (bit 12 = 1) con auto-reload (bit 15 = 1) y desactivar manual update (bit 13 = 0) */
	rTCON |= 0x1<<12 | 0x1<<15 & ~(0x1<<13);
}

