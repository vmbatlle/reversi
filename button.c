/**
 * @file button.c
 * Funciones de control para los botones del s3c44b0x
 *
 * @author Victor M. Batlle <736478@unizar.es>, Diego Royo Meneses <740388@unizar.es>
 */
/*--- ficheros de cabecera ---*/
#include "button.h"
#include "options_environment.h"
#include "44blib.h"
#include "44b.h"
#include "8led.h"
#include "nested_interrupts.h"

#if defined(ENVIRONMENT_EMULATOR)
static volatile int emular_estado_button = 0;
#endif

/* Función de callback */
static void (*f_callback)(enum estado_button button);

/* declaración de función que es rutina de servicio de interrupción
 * https://gcc.gnu.org/onlinedocs/gcc/ARM-Function-Attributes.html */
void button_ISR(void) __attribute__((interrupt("IRQ")));

void button_ISR(void)
{

	/* Desactivar las interrupciones para la línea correspondiente a EINT4/5/6/7 */
	rINTMSK |= BIT_EINT4567; 		// Enmascarar interrupciones línea EINT4/5/6/7

	volatile int which_int = rEXTINTPND;

	/* La RSI debe poner a 0 el bit correspondiente de INTPND después de
	 * limpiar los bits correspondientes de EXTINTPND */
	rI_ISPC |= BIT_EINT4567;		// Poner a 0 el bit de INTPND
									// correspondiente a la línea EINT4/5/6/7

	/* Permitir interrupciones anidadas */
	//enableNestedInterrupts();

	/* Identificar qué boton se ha pulsado */
	if (which_int & 0x8) {
		f_callback(button_dr);
	} else if (which_int & 0x4) {
		f_callback(button_iz);
	} else {
		f_callback(button_none);
	}

	/* Desactivar las interrupciones anidadas */
	//disableNestedInterrupts();

	rEXTINTPND |= 0xF;				// Pone a 0 los bits de EXTINTPND escribiendo 1s en el propio registro
}

void button_iniciar(void)
{

	/* Desactivar las interrupciones para la línea correspondiente a EINT4/5/6/7 */
	rINTMSK |= BIT_EINT4567; 	    // Deshabilitar int. linea EINT4/5/6/7 en vector de máscaras
									// poniendo el bit correspondiente a 0.

	/* Configuracion del controlador de interrupciones. Estos registros están definidos en 44b.h */
	rEXTINTPND |= 0xF;				// Pone a 0 los bits de EXTINTPND escribiendo 1s en el propio registro
	rI_ISPC |= BIT_EINT4567;		// Pone a 0 el bit correspondiente a las EINT4/5/6/7
									// de INTPND escribiendo 1s en I_ISPC
	rINTMOD &= ~(BIT_EINT4567);		// Configura la línea correspondiente a EINT4/5/6/7
									// como de tipo IRQ poniendo el bit correspondiente a 0.
	/* NO se configura el vector de interrupciones. Ya se ha realizado en 44binit.asm. */
	// rINTCON &= 0x1; // Habilita int. vectorizadas (bit 2 = 0) y la linea IRQ (bit 1 = 0)

	/* Establece la rutina de servicio para Eint4567 */
	pISR_EINT4567 = (unsigned) button_ISR;

	/* Puerto G ya configurado en Port_Init() de 44blib.c, pero se vuelve a configurar */
	rPCONG |= 0xF000;			// Establece la funcion de los pines 6 y 7 como EINT6-7
	rPUPG &= ~(0xC0);			// Habilita el "pull up" de los puertos 6 y 7

	rEXTINT &= 0x00FFFFFF;		// Configura las lineas 6 y 7 de int. como de flanco de bajada
	rEXTINT |= 0x22000000;		// Los bits [30:28] corresponden a la línea 7 y
								// los bits [26:24] a la línea 6. Ambos conjuntos de bits
								// han de tomar el valor 01X, por lo que se pone a 0
								// el primer bit y se pone a 1 el segundo.

}

void button_empezar(void (*callback)(enum estado_button))
{
	f_callback = callback;

	/* Borrar los bits de INTPND y EXTINTPND por si había alguna petición
	 * pendiente mientras estas estaban enmascaradas */
	rEXTINTPND |= 0xF;				// Pone a 0 los bits de EXTINTPND escribiendo 1s en el propio registro
	rI_ISPC |= BIT_EINT4567;		// Poner a 0 el bit de INTPND
									// correspondiente a la línea EINT4/5/6/7
	/* Activar las interrupciones para la línea correspondiente a EINT4/5/6/7 */
	rINTMSK &= ~(BIT_EINT4567); 	// Habilitar interrupciones línea EINT4/5/6/7
}

enum estado_button button_estado(void)
{
#if defined(ENVIRONMENT_EMULATOR)
	/**
	 * emular_estado_button:
	 * 1 -> button_iz
	 * 2 -> button_dr
	 * otro valor -> button_none
	 */
	switch(emular_estado_button){
	case 1:
		return button_iz;
	case 2:
		return button_dr;
	default:
		return button_none;
	}
#else
	unsigned int which = ~(rPDATG) & 0xFF;
	if (which & 0x80) { // bit 7
		return button_dr;
	} else if (which & 0x40) { // bit 6
		return button_iz;
	} else {
		return button_none;
	}
#endif
}
