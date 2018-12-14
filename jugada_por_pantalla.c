/**
 * @file jugada_por_pantalla.c
 * Máquina de estados para el control de juego por pantalla
 *
 * @author Victor M. Batlle <736478@unizar.es>, Diego Royo Meneses <740388@unizar.es>
 */
#include "jugada_por_pantalla.h"
#include "botones_antirrebotes.h"
#include "calibracion_tp.h"
#include "reversi_gui.h"
#include "8led.h"

/* TICKs de timer0 para esperar:
 *   - Timepo de cancelación: 2 segundos.
 *   - Tiempo de parpadeo: 350 ms.
 */
enum {
	RETARDO_2000_MS = 100,
	RETARDO_350_MS = 17
};

/* Estado actual de la máquina de estados */
static enum jugada_por_pantalla_estado estadoActual;

/* Acciones para cada estado del autómata */
void action_estado_bienvenida(unsigned long int ahora, char tablero[][8], int *fin, enum pulsacion_button pulsacion, enum toque_pantalla toque, int* ready, char* fila, char* columna);
void action_estado_seleccionar_movimiento(unsigned long int ahora, char tablero[][8], int *fin, enum pulsacion_button pulsacion, enum toque_pantalla toque, int* ready, char* fila, char* columna);
void action_estado_permitir_cancelar(unsigned long int ahora, char tablero[][8], int *fin, enum pulsacion_button pulsacion, enum toque_pantalla toque, int* ready, char* fila, char* columna);
void action_estado_movimiento_maquina(unsigned long int ahora, char tablero[][8], int *fin, enum pulsacion_button pulsacion, enum toque_pantalla toque, int* ready, char* fila, char* columna);
void action_estado_partida_finalizada(unsigned long int ahora, char tablero[][8], int *fin, enum pulsacion_button pulsacion, enum toque_pantalla toque, int* ready, char* fila, char* columna);
/* Tabla que relaciona los estados con sus acciones */
static void (*const tabla_estados [MAX_STATES_JPP]) (unsigned long int ahora, char tablero[][8], int *fin, enum pulsacion_button pulsacion, enum toque_pantalla toque, int* ready, char* fila, char* columna) = {
		action_estado_bienvenida, action_estado_seleccionar_movimiento,
		action_estado_permitir_cancelar, action_estado_movimiento_maquina,
		action_estado_partida_finalizada
};

void private_parpadear(unsigned long int ahora, char tablero[][8]);

static int _fila, _columna;
static unsigned int timeAntes = 0;
static unsigned int timeAntesParpadeo = 0;

void jugada_por_pantalla_iniciar(void) {
	calibracion_empezar();
	gui_empezar();
	estadoActual = estado_bienvenida;
	gui_limpiar_pantalla();
	gui_dibujar_bienvenida();
	gui_refrescar();
}

void jugada_por_pantalla_gestionar(unsigned long int ahora, char tablero[][8], int *fin, int* ready, char* fila, char* columna) {
	if (estadoActual >= 0 && estadoActual < MAX_STATES_JPP) {
		enum pulsacion_button pulsacion = antirrebotes_gestionar(ahora);
		enum toque_pantalla toque = gui_touch_screen_gestionar();
		*ready = 0;
		return tabla_estados[estadoActual](ahora, tablero, fin, pulsacion, toque, ready, fila, columna);
	}
}

/** Acciones del autómata **/

/* estado_bienvenida: Muestra una pantalla inicial con las intrucciones básicas
 * para jugar y la leyenda "Toque para jugar". */
void action_estado_bienvenida(unsigned long int ahora, char tablero[][8], int *fin,
		enum pulsacion_button pulsacion, enum toque_pantalla toque,
		int* ready, char* fila, char* columna) {
	D8led_gestionar(1);

	if (pulsacion != pulsacion_none || toque != toque_none) {
		// Saltar pantalla de bienvenida
		_fila = 0;
		_columna = 0;
		gui_limpiar_pantalla();
		gui_dibujar_tablero_vacio();
		gui_dibujar_contenido_tablero(tablero);
		gui_dibujar_ficha(_fila, _columna, FICHA_GRIS);
		gui_escribir_leyenda("Pulse para jugar");
		gui_refrescar();
		timeAntesParpadeo = ahora;
		estadoActual = estado_seleccionar_movimiento;
	}
}

/* estado_seleccionar_movimiento: Elección de número de fila y columna,
 * seleccionar la columna 0-7 con el botón izquierdo y la fila 0-7 con
 * el botón derecho. Confirmar la selección con un toque en la pantalla táctil. */
void action_estado_seleccionar_movimiento(unsigned long int ahora, char tablero[][8], int *fin,
		enum pulsacion_button pulsacion, enum toque_pantalla toque,
		int* ready, char* fila, char* columna) {
	D8led_gestionar(2);

	switch (pulsacion) {
	case pulsacion_dr:
		gui_dibujar_ficha(_fila, _columna, tablero[_fila][_columna]);
		// Incrementar el número de fila seleccionado
		_fila = (_fila + 1) % 8;
		gui_dibujar_ficha(_fila, _columna, FICHA_GRIS);
		gui_refrescar();
		break;
	case pulsacion_iz:
		gui_dibujar_ficha(_fila, _columna, tablero[_fila][_columna]);
		// Incrementar el número de columna seleccionado
		_columna = (_columna + 1) % 8;
		gui_dibujar_ficha(_fila, _columna, FICHA_GRIS);
		gui_refrescar();
		break;
	case pulsacion_none:
		break;
	}

	switch (toque) {
	case toque_central:
		// Confirmar la selección de fila y columna
		gui_escribir_leyenda("Pulse para cancelar");
		gui_refrescar();
		timeAntes = ahora;
		estadoActual = estado_permitir_cancelar;
		break;
	case toque_finalizar:
		*fin = 1;
		gui_escribir_leyenda("Toque para jugar");
		gui_refrescar();
		estadoActual = estado_partida_finalizada;
		break;
	case toque_pasar:
		*ready = 1;
		*fila = 8;
		*columna =8;
		_fila = 0;
		_columna = 0;
		estadoActual = estado_movimiento_maquina;
		break;
	case toque_none:
		break;
	}

	if (*fin) {
		/* No se pueden seleccionar más movimientos */
		gui_escribir_leyenda("Toque para jugar");
		gui_refrescar();
		estadoActual = estado_partida_finalizada;
	}

	// Gestionar el parpadeo
	private_parpadear(ahora, tablero);
}

/* estado_permitir_cancelar: Permitir al usuario cancelar su movimiento durante un determinado
 * tiempo antes de confirmar su elección. */
void action_estado_permitir_cancelar(unsigned long int ahora, char tablero[][8], int *fin,
		enum pulsacion_button pulsacion, enum toque_pantalla toque,
		int* ready, char* fila, char* columna) {

	D8led_gestionar(3);
	switch (toque) {
	case toque_central:
		// Cancelar movimiento
		gui_escribir_leyenda("Pulse para jugar");
		gui_refrescar();
		estadoActual = estado_seleccionar_movimiento;
		break;
	default:
		break;
	}

	if (ahora - timeAntes > RETARDO_2000_MS) {
		// Validar movimiento
		*fila = _fila;
		*columna = _columna;
		*ready = 1;
		// Resetear los valores de fila y columna
		_fila = 0;
		_columna = 0;
		estadoActual = estado_movimiento_maquina;
	}

	// Gestionar el parpadeo
	private_parpadear(ahora, tablero); // TODO: conflicto de timeAntes entre 250 y 2000
}

/* estado_movimiento_maquina: TODO */
void action_estado_movimiento_maquina(unsigned long int ahora, char tablero[][8], int *fin,
		enum pulsacion_button pulsacion, enum toque_pantalla toque,
		int* ready, char* fila, char* columna) {
	D8led_gestionar(4);
	gui_dibujar_contenido_tablero(tablero);
	gui_escribir_leyenda("Pulse para jugar");
	gui_refrescar();
	estadoActual = estado_seleccionar_movimiento;

	if (*fin) {
		/* No se pueden seleccionar más movimientos */
		gui_escribir_leyenda("Toque para jugar");
		gui_refrescar();
		estadoActual = estado_partida_finalizada;
	}
}

/* action_estado_partida_finalizada: La partida actual ha terminado
 * por acción del usuario o por no poder realizar más movimientos */
void action_estado_partida_finalizada(unsigned long int ahora, char tablero[][8], int *fin,
		enum pulsacion_button pulsacion, enum toque_pantalla toque,
		int* ready, char* fila, char* columna) {
	D8led_gestionar(5);
	switch (pulsacion) {
	case pulsacion_iz:
	case pulsacion_dr:
		_fila = 0;
		_columna = 0;
		gui_dibujar_contenido_tablero(tablero);
		gui_dibujar_ficha(_fila, _columna, FICHA_GRIS);
		gui_escribir_leyenda("Pulse para jugar");
		gui_refrescar();
		timeAntesParpadeo = ahora;
		estadoActual = estado_seleccionar_movimiento;
		break;
	case pulsacion_none:
		break;
	}

	switch (toque){
	case toque_central:
		// Saltar pantalla de bienvenida
		_fila = 0;
		_columna = 0;
		gui_dibujar_contenido_tablero(tablero);
		gui_dibujar_ficha(_fila, _columna, FICHA_GRIS);
		gui_escribir_leyenda("Pulse para jugar");
		gui_refrescar();
		timeAntesParpadeo = ahora;
		estadoActual = estado_seleccionar_movimiento;
		break;
	default:
		break;
	}
}

/* Hace parpadear la ficha colocada en [_fila, _columna]
 * a intervalos RETARDO_250_MS de tiempo. */
void private_parpadear(unsigned long int ahora, char tablero[][8]) {
	static int status = 1;
	if (ahora - timeAntesParpadeo > RETARDO_350_MS) {
		if (status) {
			gui_dibujar_ficha(_fila, _columna, tablero[_fila][_columna]);
			status = 0;
		} else {
			gui_dibujar_ficha(_fila, _columna, FICHA_GRIS);
			status = 1;
		}
		gui_refrescar();
		timeAntesParpadeo = ahora;
	}
}
