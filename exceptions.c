/**
 * @file exceptions.c
 * Gestión de excepciones
 *
 * @author Victor M. Batlle <736478@unizar.es>, Diego Royo Meneses <740388@unizar.es>
 */
#include "exceptions.h"
#include "44b.h"

#define ENVIRONMENT_EMULATOR

static volatile int error_variable = 0;
void  __attribute__((interrupt("IRQ"))) handler(void) {
#if defined(ENVIRONMENT_EMULATOR)
	error_variable = 1;
#else
	while(1) {
		/* TODO: parpadear 8led */
	}
#endif
}

void excepciones_inicializar(void) {
	pISR_UNDEF = (unsigned) handler;
	pISR_SWI = (unsigned) handler;
	pISR_DABORT = (unsigned) handler;
}
