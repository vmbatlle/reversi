/**
 * @file jugada_por_botones.c
 * Máquina de estados para el control de juego por botones
 *
 * @author Victor M. Batlle <736478@unizar.es>, Diego Royo Meneses <740388@unizar.es>
 */
#include "jugada_por_botones.h"
#include "botones_antirrebotes.h"
#include "8led.h"

/* Estado actual de la máquina de estados */
static enum jugada_por_botones_estado estadoActual;

/* Acciones para cada estado del autómata */
void action_estado_fila(enum pulsacion_button pulsacion, int* ready, char* fila, char* columna);
void action_estado_fila_num(enum pulsacion_button pulsacion, int* ready, char* fila, char* columna);
void action_estado_columna(enum pulsacion_button pulsacion, int* ready, char* fila, char* columna);
void action_estado_columna_num(enum pulsacion_button pulsacion, int* ready, char* fila, char* columna);
/* Tabla que relaciona los estados con sus acciones */
static void (*const tabla_estados [MAX_STATES_JPB]) (enum pulsacion_button, int*, char*, char*) = {
		action_estado_fila, action_estado_fila_num, action_estado_columna, action_estado_columna_num
};

static int _fila, _columna;

void jugada_por_botones_iniciar(void) {
	estadoActual = estado_fila;
	D8led_gestionar(15); /* 15 = 0xF */
}

void jugada_por_botones_gestionar(unsigned long int ahora, int* ready, char* fila, char* columna) {
	if (estadoActual >= 0 && estadoActual < MAX_STATES_JPB) {
		enum pulsacion_button pulsacion = antirrebotes_gestionar(ahora);
		*ready = 0;
		return tabla_estados[estadoActual](pulsacion, ready, fila, columna);
	}
}

/** Acciones del autómata **/

/* estado_fila: Inicio de la elección de fila, se muestra una F por el 8led
 * hasta que se pulsa el botón izquierdo y se elige la fila 0 */
void action_estado_fila (enum pulsacion_button pulsacion, int* ready, char* fila, char* columna) {
	switch (pulsacion) {
	case pulsacion_iz:
		// Seleccionar la fila 0
		_fila = 0;
		D8led_gestionar(_fila);
		estadoActual = estado_fila_num;
		break;
	case pulsacion_dr:
		break;
	case pulsacion_none:
		break;
	}
}

/* estado_fila_num: Elección de número de fila, seleccionar el número 0-7 con el botón izquierdo
 * y confirmar la selección con el botón derecho */
void action_estado_fila_num (enum pulsacion_button pulsacion, int* ready, char* fila, char* columna) {
	switch (pulsacion) {
	case pulsacion_iz:
		// Incrementar el número de fila seleccionado
		_fila = (_fila + 1) % 8;
		D8led_gestionar(_fila);
		break;
	case pulsacion_dr:
		// Confirmar la selección de fila y pasar a seleccionar la columna
		D8led_gestionar(12); /* 12 = 0xC */
		_columna = 0;
		estadoActual = estado_columna;
		break;
	case pulsacion_none:
		break;
	}
}

/* estado_columna: Inicio de la elección de columna, se muestra una C por el 8led
 * hasta que se pulsa el botón izquierdo y se elige la columna 0 */
void action_estado_columna (enum pulsacion_button pulsacion, int* ready, char* fila, char* columna) {
	switch (pulsacion) {
	case pulsacion_iz:
		// Seleccionar la columna 0
		_columna = 0;
		D8led_gestionar(_columna);
		estadoActual = estado_columna_num;
		break;
	case pulsacion_dr:
		break;
	case pulsacion_none:
		break;
	}
}

/* estado_columna_num: Elección de número de columna, seleccionar el número 0-7 con el botón izquierdo
 * y confirmar la selección con el botón derecho */
void action_estado_columna_num (enum pulsacion_button pulsacion, int* ready, char* fila, char* columna) {
	switch (pulsacion) {
	case pulsacion_iz:
		// Incrementar el número de columna seleccionado
		_columna = (_columna + 1) % 8;
		D8led_gestionar(_columna);
		break;
	case pulsacion_dr:
		// Devolver los valores de fila y columna seleccionados
		*ready = 1;
		*fila = _fila;
		*columna = _columna;

		// Pasar a seleccionar fila de nuevo
		D8led_gestionar(15); /* 15 = 0xF */
		estadoActual = estado_fila;
		break;
	case pulsacion_none:
		break;
	}
}
