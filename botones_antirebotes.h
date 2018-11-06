#ifndef __BOTONES_ANTIREBOTES_H__
#define __BOTONES_ANTIREBOTES_H__

enum antirebotes_estado;
enum pulsacion_button;

/**
 * @brief 	Inicia el autómata encargado de tratar los rebotes de los botones.
 */
void antirebotes_iniciar(void);

/**
 * @brief	Actualiza el autómata para los rebotes de los botones
 * @param timeAhora tiempo en ticks desde que se inició el autómata
 * @return	pulsacion_iz si se ha pulsado el botón izquierdo,
 * 			pulsacion_dr si se ha pulsado el botón derecho,
 * 			o pulsacion_none en caso contrario.
 */
enum pulsacion_button antirebotes_gestionar(unsigned int timeAhora);

#endif /* __BOTONES_ANTIREBOTES_H__ */
