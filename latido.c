/**
 * @file    latido.c
 * @author  Victor M. Batlle <736478@unizar.es>, Diego Royo Meneses <740388@unizar.es>
 * @brief   Funciones de control del latido
 * @version 1.0.0
 */
#include "latido.h"
#include "led.h"
#include "push_debug.h"

enum {
	TIMER0_FRECUENCIA = 50 //Número de interrupciones del timer0 por segundo
};

/* Frecuencia de parpadeo: 2Hz */
/* FRECUENCIA APAGADO ó ENCENDIDO: 4Hz */

void latido_inicializar(){
	leds_off();				// Apagar leds
}

/* Ajusta la frecuencia cuando el escalado no es entero */
unsigned long int latido_ajustar(){
	static short int Q = 0; /* Número de periodo: entero en [0,15] */
	Q = (Q + 1) % 16;
	if ((TIMER0_FRECUENCIA % 4) == 1) {
		if (Q == 0 || Q == 5 || Q == 10 || Q == 15) {
			return TIMER0_FRECUENCIA/4 + 1;
		} else {
			return TIMER0_FRECUENCIA/4;
		}
	} else if ((TIMER0_FRECUENCIA % 4) == 2) {
		if ((Q == 0 || Q == 1) || (Q == 5 || Q == 6) || (Q == 10 || Q == 11) || (Q == 15 || Q == 12)) {
			return TIMER0_FRECUENCIA/4 + 1;
		} else {
			return TIMER0_FRECUENCIA/4;
		}
	} else if ((TIMER0_FRECUENCIA % 4) == 3) {
		if (Q == 0 || Q == 5 || Q == 10 || Q == 15) {
			return TIMER0_FRECUENCIA/4;
		} else {
			return TIMER0_FRECUENCIA/4 + 1;
		}
	} else {
		return TIMER0_FRECUENCIA/4;
	}
}

void latido_gestionar(unsigned long int ahora){
	static unsigned long int anterior = 0;
	static unsigned long int periodo = TIMER0_FRECUENCIA/4;

	if ((ahora - anterior) >= periodo) {
		if (led1_status()){
			led1_off();
		} else {
			led1_on();
		}
		anterior = ahora;
		periodo = latido_ajustar();
	}
}
