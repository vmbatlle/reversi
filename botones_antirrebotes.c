/**
 * @file botones_antirrebotes.c
 * Gestión de rebotes para los botones de la placa
 *
 * @author Victor M. Batlle <736478@unizar.es>, Diego Royo Meneses <740388@unizar.es>
 */
#include "botones_antirrebotes.h"
#include "button.h"
#include "timer0.h"

/* Estado actual de la máquina de estados */
static enum antirrebotes_estado estadoActual;

/* trp y trd en ticks (espera para los rebotes de entrada / salida) */
static const unsigned int trp = 7; // Retardo para rebotes de entrada (ticks timer0), 134ms
static const unsigned int trd = 1; // Retardo para rebotes de salida (ticks timer0), 6ms

/* Comprobaciones mientras el botón se encuentra pulsado */
static const unsigned int check_state = 1; 	// Tiempo entre comprobaciones mientras el botón
											// se encuentra pulsado (ticks timer0), 20ms
static int numComprobaciones = 0; 	// Número de comprobaciones realizadas mientras
									// el botón se encontraba pulsado (estados {short|long}_hold)
// Si las comprobaciones (check_state) se hacen cada 20ms, número de comprobaciones
// necesarias hasta alcanzar 500 ms y 200 ms
enum {
	COMPROBACIONES_500_MS = 25,
	COMPROBACIONES_200_MS = 10
};

/* Acciones para cada estado del autómata */
enum pulsacion_button action_unpressed (unsigned int);
enum pulsacion_button action_wait_trp (unsigned int);
enum pulsacion_button action_pressed (unsigned int);
enum pulsacion_button action_short_hold (unsigned int);
enum pulsacion_button action_long_hold (unsigned int);
enum pulsacion_button action_wait_trd (unsigned int);
/* Tabla que relaciona los estados con sus acciones */
static enum pulsacion_button (*const tabla_estados [MAX_STATES_BA]) (unsigned int) = {
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

/* Inicializa la gestión del botón */
void antirrebotes_iniciar(void) {
	estadoActual = unpressed;
	button_empezar(antirrebotes_callback);
}

/* Llamado desde el bucle de gestión de periféricos para desencadenar
 * las acciones necesarias en la máquina de estados. */
enum pulsacion_button antirrebotes_gestionar(unsigned int timeAhora) {
	if (estadoActual >= 0 && estadoActual < MAX_STATES_BA) {
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

/* short_hold: Se ha presinado un botón y se ha devuelto el valor de la pulsación realizada,
 * esperar a que se levante o a que se mantenga pulsado durante más de 500ms */
enum pulsacion_button action_short_hold (unsigned int timeAhora) {
	if (timeAhora - timeAntes >= check_state) {
		if (button_estado() == button_none) {
			estadoActual = wait_trd;
		} else {
			numComprobaciones++;
			if (numComprobaciones >= COMPROBACIONES_500_MS) {
				numComprobaciones = 0;
				estadoActual = long_hold;
			}
		}
		timeAntes = timeAhora;
	}
	return pulsacion_none;
}

/* long_hold: Se ha presinado un botón y se ha mantenido durante 500ms,
 * volver a marcar el mismo valor de pulsación cada 200ms que siga pulsado hasta que se levante */
enum pulsacion_button action_long_hold (unsigned int timeAhora) {
	enum pulsacion_button returnValue = pulsacion_none;
	if (timeAhora - timeAntes >= check_state) {
		if (button_estado() == button_none) {
			estadoActual = wait_trd;
		} else {
			numComprobaciones++;
			if (numComprobaciones >= COMPROBACIONES_200_MS) {
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


