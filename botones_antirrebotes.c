#include "botones_antirrebotes.h"
#include "button.h"
#include "timer0.h"

/* Estado actual de la máquina de estados */
static enum antirrebotes_estado estadoActual;

/* trp y trd en ticks (espera para los rebotes de entrada / salida) */
static const unsigned int trp = 7; // 134ms
static const unsigned int trd = 1; // 6ms
static const unsigned int check_state = 2; // 20ms, se realiza varias veces
static int numComprobaciones = 0; // veces que se TODO

/* Acciones para cada estado del autómata */
enum pulsacion_button action_unpressed (unsigned int);
enum pulsacion_button action_wait_trp (unsigned int);
enum pulsacion_button action_pressed (unsigned int);
enum pulsacion_button action_short_hold (unsigned int);
enum pulsacion_button action_long_hold (unsigned int);
enum pulsacion_button action_wait_trd (unsigned int);
/* Tabla que relaciona los estados con sus acciones */
enum pulsacion_button (*const tabla_estados [MAX_STATES]) (unsigned int) = {
	action_unpressed, action_wait_trp, action_pressed, action_short_hold, action_long_hold, action_wait_trd
};


/* Almacena el último tiempo leído en un evento importante, para compararlo con el actual */
static unsigned int timeAntes = 0;

/* Indica qué botón se ha pulsado en la última ISR de los botones */
static enum pulsacion_button pulsacionRealizada;

/* Almacena qué boton se ha pulsado (button), callback desde la ISR de los botones */
void antirrebotes_callback(enum estado_button button) {
	if (estadoActual == unpressed) {
		estadoActual = wait_trp;
		pulsacionRealizada = button == button_iz ? pulsacion_iz : pulsacion_dr;
		timeAntes = timer0_leer();
	}
}

void antirrebotes_iniciar(void) {
	estadoActual = unpressed;
	button_empezar(antirrebotes_callback);
}

enum pulsacion_button antirrebotes_gestionar(unsigned int timeAhora) {
	if (estadoActual >= 0 && estadoActual < MAX_STATES) {
		return tabla_estados[estadoActual](timeAhora);
	} else {
		/* Estado inválido */
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
		timeAntes = timeAhora;
		estadoActual = pressed;
	}
	return pulsacion_none;
}

/* pressed: Se ha presionado un botón, devolver el valor correspondiente de pulsacion_button */
enum pulsacion_button action_pressed (unsigned int timeAhora) {
	estadoActual = short_hold;
	numComprobaciones = 0;
	return pulsacionRealizada;
}

/* short_hold: Se ha presinado un botón y se ha devuelto el valor, esperar a que se levante ese botón TODO */
enum pulsacion_button action_short_hold (unsigned int timeAhora) {
	if (timeAhora - timeAntes >= check_state) {
		if (button_estado() == button_none) {
			estadoActual = wait_trd;
		} else {
			numComprobaciones++;
			if (numComprobaciones >= 25) { // TODO
				numComprobaciones = 0;
				estadoActual = long_hold;
			}
		}
		timeAntes = timeAhora;
	}
	return pulsacion_none;
}

/* long_hold: Se ha presinado un botón y se ha devuelto el valor, esperar a que se levante ese botón TODO */
enum pulsacion_button action_long_hold (unsigned int timeAhora) {
	enum pulsacion_button returnValue = pulsacion_none;
	if (timeAhora - timeAntes >= check_state) {
		if (button_estado() == button_none) {
			estadoActual = wait_trd;
		} else {
			numComprobaciones++;
			if (numComprobaciones >= 10) { // TODO
				numComprobaciones = 0;
				returnValue = pulsacionRealizada;
			}
		}
		timeAntes = timeAhora;
	}
	return returnValue;
}

/* wait_trd: Se ha levantado el botón, esperar trd ticks para volver a activarlo (rebotes salida) */
enum pulsacion_button action_wait_trd (unsigned int timeAhora) {
	if (timeAhora - timeAntes >= trd) {
		button_empezar(antirrebotes_callback);
		estadoActual = unpressed;
	}
	return pulsacion_none;
}


