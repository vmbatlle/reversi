/**
 * @file jugada_por_botones.h
 * Máquina de estados para el control de juego por botones
 *
 * @author Victor M. Batlle <736478@unizar.es>, Diego Royo Meneses <740388@unizar.es>
 */
#ifndef __JUGADA_POR_BOTONES_H__
#define __JUGADA_POR_BOTONES_H__

/*
 * Estados del autómata para gestionar la jugada por botones
 * estado_fila: Inicio de elección de fila (F)
 * estado_fila_num: Elección de fila (0-7) hasta confirmación
 * estado_columna: Inicio de elección de columna (C)
 * estado_columna_num: Elección de fila (0-7) hasta confirmación
 */
enum jugada_por_botones_estado {
	estado_fila, estado_fila_num, estado_columna, estado_columna_num, MAX_STATES_JPB=4
};

/**
 * @brief	Inicia el autómata encargado de gestionar la jugada por botones.
 */
void jugada_por_botones_iniciar(void);

/**
 * @brief 	Actualiza el autómata para la jugada por botones
 * @param ahora tiempo en ticks desde que se inició el autómata
 * @param ready Toma el valor 1 si se ha confirmado y finalizado la elección de
 * 				fila y columna, 0 si todavía no se ha terminado
 * @param fila Si ready = 1, toma el valor de la fila seleccionada (0-7)
 * @param columna Si ready = 1, toma el valor de la columna seleccionada (0-7)
 */
void jugada_por_botones_gestionar(unsigned long int ahora, int* ready, char* fila, char* columna);

#endif /* __JUGADA_POR_BOTONES_H__ */
