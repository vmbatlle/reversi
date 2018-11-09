#ifndef __BOTONES_ANTIRREBOTES_H__
#define __BOTONES_ANTIRREBOTES_H__

/*
 * Estados del autómata para gestionar los rebotes de los botones:
 * unpressed: Ningún botón se ha presionado
 * wait_trp: Se ha presionado un botón, esperar trp ticks (rebotes entrada)
 * pressed: Se ha presionado un botón, devolver el valor correspondiente de pulsacion_button
 * wait_release: Se ha presinado un botón y se ha devuelto el valor, esperar a que se levante ese botón
 * wait_trd: Se ha levantado el botón, esperar trd ticks para volver a activarlo (rebotes salida)
 */
enum antirrebotes_estado {
	unpressed, wait_trp, pressed, short_hold, long_hold, wait_trd, MAX_STATES_BA=6
};

/*
 * Posibles salidas del autómata:
 * pulsacion_none: No se ha detectado ninguna pulsación
 * pulsacion_iz: Pulsación del botón izquierdo
 * pulsacion_dr: Pulsación del botón derecho
 */
enum pulsacion_button {
	pulsacion_none, pulsacion_iz, pulsacion_dr
};

/**
 * @brief 	Inicia el autómata encargado de tratar los rebotes de los botones.
 */
void antirrebotes_iniciar(void);

/**
 * @brief	Actualiza el autómata para los rebotes de los botones
 * @param timeAhora tiempo en ticks desde que se inició el autómata
 * @return	pulsacion_iz si se ha pulsado el botón izquierdo,
 * 			pulsacion_dr si se ha pulsado el botón derecho,
 * 			o pulsacion_none en caso contrario.
 */
enum pulsacion_button antirrebotes_gestionar(unsigned int timeAhora);

#endif /* __BOTONES_ANTIRREBOTES_H__ */
