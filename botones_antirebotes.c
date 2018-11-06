#include "botones_antirebotes.h"
#include "button.h"
#include "timer0.h"

/* Estado actual de la máquina de estados */
static enum antirebotes_estado estadoActual;

/* trp y trd en ticks (espera para los rebotes de entrada / salida) */
static const unsigned int trp = 20;
static const unsigned int trd = 20;

/* Acciones para cada estado del autómata */
enum pulsacion_button action_unpressed (unsigned int);
enum pulsacion_button action_wait_trp (unsigned int);
enum pulsacion_button action_pressed (unsigned int);
enum pulsacion_button action_wait_release (unsigned int);
enum pulsacion_button action_wait_trd (unsigned int);
/* Tabla que relaciona los estados con sus acciones */
enum pulsacion_button (*const tabla_estados [MAX_STATES]) (unsigned int) = {
	action_unpressed, action_wait_trp, action_pressed, action_wait_release, action_wait_trd
};


/* Almacena el último tiempo leído en un evento importante, para compararlo con el actual */
static unsigned int timeAntes = 0;

/* Indica qué botón se ha pulsado en la última ISR de los botones */
static enum pulsacion_button pulsacionRealizada;

/* Almacena qué boton se ha pulsado (button), callback desde la ISR de los botones */
void antirebotes_callback(enum estado_button button) {
	if (estadoActual == unpressed) {
		estadoActual = wait_trp;
		pulsacionRealizada = button == button_iz ? pulsacion_iz : pulsacion_dr;
		timeAntes = timer0_leer();
	}
}

void antirebotes_iniciar(void) {
	estadoActual = unpressed;
	button_empezar(antirebotes_callback);
}

enum pulsacion_button antirebotes_gestionar(unsigned int timeAhora) {
	if (estadoActual >= 0 && estadoActual < MAX_STATES) {
		return tabla_estados[estadoActual](timeAhora);
	} else {
		/* TODO: estado invalido. Tratamiento */
		return pulsacion_none;
	}
}

/** Acciones del autómata **/

/* unpressed: Ningún botón se ha presionado */
enum pulsacion_button action_unpressed (unsigned int timeAhora) {
	return pulsacion_none;
}

/* wait_trp: Se ha presionado un botón, esperar trp ticks (rebotes entrada) */
enum pulsacion_button action_wait_trp (unsigned int timeAhora) {
	if (timeAhora - timeAntes >= trp) {
		timeAntes = timeAhora; /* TODO: Ahora son 20 ticks, deberian ser 20 ms */
		estadoActual = pressed;
	}
	return pulsacion_none;
}

/* pressed: Se ha presionado un botón, devolver el valor correspondiente de pulsacion_button */
enum pulsacion_button action_pressed (unsigned int timeAhora) {
	estadoActual = wait_release;
	return pulsacionRealizada;
}

/* wait_release: Se ha presinado un botón y se ha devuelto el valor, esperar a que se levante ese botón */
enum pulsacion_button action_wait_release (unsigned int timeAhora) {
	if (timeAhora - timeAntes >= 20) { /* TODO ticks */
		if (button_estado() == button_none) {
			estadoActual = wait_trd;
		}
		timeAntes = timeAhora;
	}
	return pulsacion_none;
}

/* wait_trd: Se ha levantado el botón, esperar trd ticks para volver a activarlo (rebotes salida) */
enum pulsacion_button action_wait_trd (unsigned int timeAhora) {
	if (timeAhora - timeAntes >= trd) {
		button_empezar(antirebotes_callback);
		estadoActual = unpressed;
	}
	return pulsacion_none;
}


