#include "led.h"

enum {
	PERIODO = 50 //Número de interrupciones del timer0 por segundo
};

void latido_inicializar(){
	leds_off();				// Apagar leds
}

void latido_loop(unsigned long int ahora){
	static unsigned long int anterior = 0;
	/* TODO: PERIODO/4 = 12,13,13,12 */
	if (anterior + PERIODO/4 <= ahora) {
		if (led1_status()){
			led1_off();
		} else {
			led1_on();
		}
		anterior = ahora;
	}
}
