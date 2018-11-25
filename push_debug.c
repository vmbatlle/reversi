/**
 * @file push_debug.c
 * Funciones de control para la pila de depuración
 *
 * @author Victor M. Batlle <736478@unizar.es>, Diego Royo Meneses <740388@unizar.es>
 */
#include "push_debug.h"
#include "timer2.h"
#include "option.h"

enum {
	MAX_SIZE = 256 / 4,
};
static volatile uint32_t *stack = (volatile uint32_t*)((_ISR_STARTADDRESS)-0xf00-0x100); /*0xc7fef00*/
static uint32_t size;

void push_iniciar() {
	size = 1;
	int i;
	for (i = 1; i < MAX_SIZE; i++) {
		(*(stack - i)) = 0;
	}
	timer2_empezar();

}

void push_debug(uint8_t ID_evento, uint32_t auxData){
	(*(stack - size)) = (ID_evento << (8*3)) | (auxData & 0xFFFFFF);
	(*(stack - size - 1)) = timer2_leer();
	size = (size + 2) % MAX_SIZE;
}
