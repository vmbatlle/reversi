#include "botones_antirebotes.h"
#include "timer0.h"
#include "button.h"

/* Estado actual de la máquina de estados */
static enum estado_rebotes estado;

/* trp y trd en ticks */
static const unsigned int trp = 20;
static const unsigned int trd = 20;

static unsigned int nextTime = 0;

void (*const tabla_estados [MAX_STATES]) (void) = {
	action_unpressed, action_wait_trp, action_pressed, action_wait_trd
};


enum pulsacion check_pulsacion(void) {
	if (estado >= 0 && estado < MAX_STATES) {
		return tabla_estados[estado]();
	} else {
		/* TODO: estado invalido. Tratamiento */
		return pulsacion_none;
	}
}

/* TODO: cabeceras */

enum pulsacion action_unpressed (void) {
	enum estado_button button = button_estado();
	if (button != button_none) {
		timer0_empezar();
		estado = wait_trp;
		return button == button_iz ? pulsacion_iz : pulsacion_dr;
	} else {
		return pulsacion_none;
	}
}

enum pulsacion action_wait_trp (void) {
	unsigned int time = timer0_leer();
	if (time >= trp) {
		nextTime = time + 20; /* TODO: Ahora son 20 ticks, deberian ser 20 ms */
		estado = pressed;
	}
	return pulsacion_none;
}

enum pulsacion action_pressed (void) {
	unsigned int time = timer0_leer();
	if (time >= nextTime) {
		if (button_estado() == button_none) {
			nextTime = time + trd;
		} else {
			nextTime = time + 20; /* TODO: Ahora son 20 ticks, deberian ser 20 ms */
		}
	}
	return pulsacion_none;
}

enum pulsacion action_wait_trd (void) {
	unsigned int time = timer0_leer();
	if (time >= nextTime) {
		button_empezar();
	}
	return pulsacion_none;
}


