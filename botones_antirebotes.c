#include "botones_antirebotes.h"
#include "timer0.h"
#include "button.h"

/* Estado actual de la máquina de estados */
static enum estado_rebotes estado;

/* trp y trd en ticks */
static const unsigned int trp = 20;
static const unsigned int trd = 20;

static unsigned int timeAntes = 0;
static enum pulsacion pulsado_return;

enum pulsacion action_unpressed (void);
enum pulsacion action_wait_trp (void);
enum pulsacion action_pressed (void);
enum pulsacion action_wait_release (void);
enum pulsacion action_wait_trd (void);
enum pulsacion (*const tabla_estados [MAX_STATES]) (void) = {
	action_unpressed, action_wait_trp, action_pressed, action_wait_release, action_wait_trd
};


void antirebotes_callback(enum estado_button button) {
	if (estado == unpressed) {
		estado = wait_trp;
		pulsado_return = button == button_iz ? pulsacion_iz : pulsacion_dr;
		timeAntes = timer0_leer();
	}
}

void antirebotes_iniciar(void) {
	estado = unpressed;
	button_empezar(antirebotes_callback);
}

enum pulsacion antirebotes_gestionar(void) {
	if (estado >= 0 && estado < MAX_STATES) {
		return tabla_estados[estado]();
	} else {
		/* TODO: estado invalido. Tratamiento */
		return pulsacion_none;
	}
}

/* TODO: cabeceras */

enum pulsacion action_unpressed (void) {
	return pulsacion_none;
}

enum pulsacion action_wait_trp (void) {
	unsigned int timeAhora = timer0_leer();
	if (timeAhora - timeAntes >= trp) {
		timeAntes = timeAhora; /* TODO: Ahora son 20 ticks, deberian ser 20 ms */
		estado = pressed;
	}
	return pulsacion_none;
}

enum pulsacion action_pressed (void) {
	estado = wait_release;
	return pulsado_return;
}

enum pulsacion action_wait_release (void) {
	unsigned int timeAhora = timer0_leer();
	if (timeAhora - timeAntes >= 20) { /* TODO ticks */
		if (button_estado() == button_none) {
			estado = wait_trd;
		}
		timeAntes = timeAhora;
	}
	return pulsacion_none;
}

enum pulsacion action_wait_trd (void) {
	unsigned int timeAhora = timer0_leer();
	if (timeAhora - timeAntes >= trd) {
		button_empezar(antirebotes_callback);
		estado = unpressed;
	}
	return pulsacion_none;
}


