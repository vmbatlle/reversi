//#ifndef __BOTONES_ANTIRREBOTES_H__
//#define __BOTONES_ANTIRREBOTES_H__

/*
 * Estados del autómata para gestionar TODO
 */
enum jugada_por_botones_estado {
	estado_fila, estado_fila_num, estado_columna, estado_columna_num, MAX_STATES_JPB=4
};

/**
 * @brief TODO	Inicia el autómata encargado de tratar los rebotes de los botones.
 */
void jugada_por_botones_iniciar(void);

/**
 * @brief TODO	Actualiza el autómata para los rebotes de los botones
 * @param timeAhora tiempo en ticks desde que se inició el autómata
 * @return	pulsacion_iz si se ha pulsado el botón izquierdo,
 * 			pulsacion_dr si se ha pulsado el botón derecho,
 * 			o pulsacion_none en caso contrario.
 */
void jugada_por_botones_gestionar(unsigned long int ahora, int* ready, char* fila, char* columna);

//#endif /* __BOTONES_ANTIRREBOTES_H__ */
