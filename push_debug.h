/**
 * @file push_debug.h
 * Funciones de control para la pila de depuración
 *
 * @author Victor M. Batlle <736478@unizar.es>, Diego Royo Meneses <740388@unizar.es>
 */
#ifndef __PUSH_DEBUG_H__
#define __PUSH_DEBUG_H__

#include <stdint.h>

enum evento_ID { evento_button=1, evento_timer0=2 };

/**
 * @brief Inicializa la pila de debug, inicia a 0 todos los
 * 		  valores almacenados en esta.
 */
void push_iniciar();

/**
 * @brief Apila un nuevo evento en la cima.
 * @param ID_evento Identificador de la interrupción (MSB)
 * @param auxData Información adicional
 */
void push_debug(uint8_t ID_evento, uint32_t auxData);

#endif /* __PUSH_DEBUG_H__ */
