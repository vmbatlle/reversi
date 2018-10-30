#ifndef _BUTTON_H_
#define _BUTTON_H_

/*
 * Estado de los botones de la placa:
 * button_none: Ningún botón pulsado
 * button_iz: Botón izquierdo pulsado
 * button_dr: Botón derecho pulsado
 */
enum estado_button {button_none, button_iz, button_dr};

/* TODO: comprobar que el derecho es EINT6 y el izquierdo EINT7 */

/**
 * @brief 	Configura los botones derecho e izquierdo
 * 			(interrupciones externas EINT6/7, respectivamente)
 * 			y activa su funcionamiento
 */
void button_iniciar(void);

/**
 * @brief 	Activa el funcionamiento de los botones y configura
 * 			la función de callback.
 * @param callback 	Función de callback a llamar cuando ocurra
 * 					un evento de pulsación del botón.
 * @warning	Una vez pulsado el botón no se registrarán más pulsaciones
 * 			hasta que no se vuelva a llamar a @ref button_empezar
 */
void button_empezar(void (*callback)(enum estado_button));

/**
 * @brief 	Consulta el estado de los botones
 * @return	button_none, button_iz o button_dr
 */
enum estado_button button_estado(void);

#endif /* _BUTTON_H_ */
