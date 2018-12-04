/**
 * @file jugada_por_pantalla.c
 * Máquina de estados para el control de juego por pantalla
 *
 * @author Victor M. Batlle <736478@unizar.es>, Diego Royo Meneses <740388@unizar.es>
 */
#include "jugada_por_pantalla.h"
#include "botones_antirrebotes.h"
#include "reversi_gui.h"

/* TICKs de timer0 para esperar:
 *   - Timepo de cancelación: 2 segundos.
 *   - Tiempo de parpadeo: 250 ms.
 */
enum {
	RETARDO_2000_MS = 0, //TODO: Ajustar
	RETARDO_250_MS = 0 //TODO: Ajustartoque_pantalla
};

enum toque_pantalla {
	toque_none = 0,
	toque_central = 1
};

/* Estado actual de la máquina de estados */
static enum jugada_por_pantalla_estado estadoActual;

/* Acciones para cada estado del autómata */
void action_estado_bienvenida(unsigned long int ahora, int fin, enum pulsacion_button pulsacion, enum toque_pantalla toque, int* ready, char* fila, char* columna);
void action_estado_seleccionar_movimiento(unsigned long int ahora, int fin, enum pulsacion_button pulsacion, enum toque_pantalla toque, int* ready, char* fila, char* columna);
void action_estado_permitir_cancelar(unsigned long int ahora, int fin, enum pulsacion_button pulsacion, enum toque_pantalla toque, int* ready, char* fila, char* columna);
void action_estado_partida_finalizada(unsigned long int ahora, int fin, enum pulsacion_button pulsacion, enum toque_pantalla toque, int* ready, char* fila, char* columna);
/* Tabla que relaciona los estados con sus acciones */
static void (*const tabla_estados [MAX_STATES_JPP]) (unsigned long int ahora, int fin, enum pulsacion_button pulsacion, enum toque_pantalla toque, int* ready, char* fila, char* columna) = {
		action_estado_bienvenida, action_estado_seleccionar_movimiento,
		action_estado_permitir_cancelar, action_estado_partida_finalizada
};

void private_parpadear(unsigned long int ahora);

static int _fila, _columna;
static unsigned int timeAntes = 0;

void jugada_por_pantalla_iniciar(void) {
	estadoActual = estado_bienvenida;
	// TODO: Limpiar pantalla y mostrar mensaje bienvenida
}

void jugada_por_pantalla_gestionar(unsigned long int ahora, int fin, int* ready, char* fila, char* columna) {
	if (estadoActual >= 0 && estadoActual < MAX_STATES_JPP) {
		enum pulsacion_button pulsacion = antirrebotes_gestionar(ahora);
		enum toque_pantalla toque = toque_none; //gui_touch_screen_gestionar(ahora);
		*ready = 0;
		return tabla_estados[estadoActual](ahora, fin, pulsacion, toque, ready, fila, columna);
	}
}

/** Acciones del autómata **/

/* estado_bienvenida: Muestra una pantalla inicial con las intrucciones básicas
 * para jugar y la leyenda "Toque la pantalla para jugar". */
void action_estado_bienvenida(unsigned long int ahora, int fin,
		enum pulsacion_button pulsacion, enum toque_pantalla toque,
		int* ready, char* fila, char* columna) {

	int fire_transition = 0;

	switch (pulsacion) {
	case pulsacion_iz:
	case pulsacion_dr:
		fire_transition = 1;
		break;
	case pulsacion_none:
		break;
	}
	switch (toque){
	case toque_central: // TODO: Definir área de interacción.
		fire_transition = 1;
		break;
	case toque_none:
		break;
	}

	if (fire_transition) {
		// Saltar pantalla de bienvenida
		_fila = 0;
		_columna = 0;
		// TODO: Dibujar tablero inicial
		estadoActual = estado_seleccionar_movimiento;
	}
}

/* estado_seleccionar_movimiento: Elección de número de fila y columna,
 * seleccionar la columna 0-7 con el botón izquierdo y la fila 0-7 con
 * el botón derecho. Confirmar la selección con un toque en la pantalla táctil. */
void action_estado_seleccionar_movimiento(unsigned long int ahora, int fin,
		enum pulsacion_button pulsacion, enum toque_pantalla toque,
		int* ready, char* fila, char* columna) {

	switch (pulsacion) {
	case pulsacion_dr:
		// TODO: Borrar ficha anterior
		// Incrementar el número de fila seleccionado
		_fila = (_fila + 1) % 8;
		// TODO: Dibujar ficha
		break;
	case pulsacion_iz:
		// TODO: Borrar ficha anterior
		// Incrementar el número de columna seleccionado
		_columna = (_columna + 1) % 8;
		// TODO: Dibujar ficha
		break;
	case pulsacion_none:
		break;
	}

	switch (toque) {
	case toque_central:
		// Confirmar la selección de fila y columna

		// TODO: Mostrar leyenda de confirmación
		timeAntes = ahora;
		estadoActual = estado_permitir_cancelar;
		break;
	case toque_none:
		break;
	}

	if (fin) {
		/* No se pueden seleccionar más movimientos */
		estadoActual = estado_partida_finalizada;
	}

	// Gestionar el parpadeo
	private_parpadear(ahora);
}

/* estado_permitir_cancelar: Permitir al usuario cancelar su movimiento durante un determinado
 * tiempo antes de confirmar su elección. */
void action_estado_permitir_cancelar(unsigned long int ahora, int fin,
		enum pulsacion_button pulsacion, enum toque_pantalla toque,
		int* ready, char* fila, char* columna) {

	switch (toque) {
	case toque_central:
		// Cancelar movimiento
		estadoActual = estado_seleccionar_movimiento;
		break;
	case toque_none:
		break;
	}

	if (ahora - timeAntes > RETARDO_2000_MS) {
		// Validar movimiento
		*fila = _fila;
		*columna = _columna;
		*ready = 1;
		estadoActual = estado_seleccionar_movimiento;
	}

	// Gestionar el parpadeo
	private_parpadear(ahora); // TODO: conflicto de timeAntes entre 250 y 2000
}

/* action_estado_partida_finalizada: La partida actual ha terminado
 * por acción del usuario o por no poder realizar más movimientos */
void action_estado_partida_finalizada(unsigned long int ahora, int fin,
		enum pulsacion_button pulsacion, enum toque_pantalla toque,
		int* ready, char* fila, char* columna) {
	switch (pulsacion) {
	case pulsacion_iz:
	case pulsacion_dr:
		_fila = 0;
		_columna = 0;
		// TODO: Dibujar tablero inicial
		estadoActual = estado_seleccionar_movimiento;
		break;
	case pulsacion_none:
		break;
	}

	switch (toque){
	case toque_central: // TODO: Definir área de interacción.
		// Saltar pantalla de bienvenida
		_fila = 0;
		_columna = 0;
		// TODO: Dibujar tablero inicial
		estadoActual = estado_seleccionar_movimiento;
		break;
	case toque_none:
		break;
	}
}

/* Hace parpadear la ficha colocada en [_fila, _columna]
 * a intervalos RETARDO_250_MS de tiempo. */
void private_parpadear(unsigned long int ahora) {
	static int status = 0;
	if (ahora - timeAntes > RETARDO_250_MS) {
		if (status) {
			// TODO: Ocultar ficha
			status = 0;
		} else {
			// TODO: Mostrar ficha
			status = 1;
		}
		timeAntes = ahora;
	}
}
