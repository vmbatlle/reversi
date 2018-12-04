/**
 * @file jugada_por_pantalla.h
 * Máquina de estados para el control de juego por pantalla
 *
 * @author Victor M. Batlle <736478@unizar.es>, Diego Royo Meneses <740388@unizar.es>
 */
#ifndef __JUGADA_POR_PANTALLA_H__
#define __JUGADA_POR_PANTALLA_H__

/*
 * Estados del autómata para gestionar la jugada por pantalla
 * estado_bienvenida: Pantalla de instrucciones
 * estado_seleccionar_movimiento: Elección de fila (0-7) y columna (0-7) hasta confirmación
 * estado_permitir_cancelar: Esperar tiempo para permitir cancelar
 * estado_partida_finalizada: Partida finalizada
 */
enum jugada_por_pantalla_estado {
	estado_bienvenida, estado_seleccionar_movimiento, estado_permitir_cancelar, estado_partida_finalizada, MAX_STATES_JPP=4
};

/**
 * @brief	Inicia el autómata encargado de gestionar la jugada por pantalla.
 */
void jugada_por_pantalla_iniciar(void);

/**
 * @brief 	Actualiza el autómata para la jugada por pantalla
 * @param ahora tiempo en ticks desde que se inició el autómata
 * @param fin Toma el valor 1 si se ha finalizado la partida
 * @param ready Toma el valor 1 si se ha confirmado y finalizado la elección de
 * 				fila y columna, 0 si todavía no se ha terminado
 * @param fila Si ready = 1, toma el valor de la fila seleccionada (0-7)
 * @param columna Si ready = 1, toma el valor de la columna seleccionada (0-7)
 */
void jugada_por_pantalla_gestionar(unsigned long int ahora, int fin, int* ready, char* fila, char* columna);

#endif /* __JUGADA_POR_PANTALLA_H__ */
