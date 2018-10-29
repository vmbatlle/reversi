#ifndef __PUSH_DEBUG_H__
#define __PUSH_DEBUG_H__

#include <stdint.h>

/**
 * @brief Apila un nuevo evento en la cima.
 * @param ID_evento Identificador de la interrupción (MSB)
 * @param auxData Momento exacto de invocación
 */
void push_debug(uint32_t ID_evento, uint32_t auxData);

#endif /* __PUSH_DEBUG_H__ */
