#ifndef __PUSH_DEBUG_H__
#define __PUSH_DEBUG_H__

#include <stdint.h>

enum evento_ID { evento_button=1 };

/**
 * @brief TODO
 */
void push_iniciar();

/**
 * @brief Apila un nuevo evento en la cima.
 * @param ID_evento Identificador de la interrupción (MSB)
 * @param auxData Información adicional
 */
void push_debug(uint8_t ID_evento, uint32_t auxData);

#endif /* __PUSH_DEBUG_H__ */
