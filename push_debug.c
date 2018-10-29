#include "push_debug.h"
#include "option.h"

enum {
	MAX_SIZE = 256
};
static volatile uint32_t *stack = (volatile uint32_t*)_ISR_STARTADDRESS-0xf00-MAX_SIZE; /*0xc7fef000*/
static uint32_t size = 0;

void push_debug(uint32_t ID_evento, uint32_t auxData){
	(*(stack + size)) = ID_evento;
	(*(stack + size + 4)) = auxData;
	size = (size + 8) % MAX_SIZE;
}
