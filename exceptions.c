/**
 * @file exceptions.c
 * Gestión de excepciones
 *
 * @author Victor M. Batlle <736478@unizar.es>, Diego Royo Meneses <740388@unizar.es>
 */
#include "exceptions.h"
#include "8led.h"
#include "44b.h"
#include "44blib.h"

//#define ENVIRONMENT_EMULATOR

enum exception_type { exception_undef = 0, exception_swi = 1, exception_dabort = 2 };

static volatile int error_variable = 0;
void handler(enum exception_type type, int lr) {
#if defined(ENVIRONMENT_EMULATOR)
	error_variable = 1;
#else
	D8led_inicializar();
	while(1) {
		D8led_gestionar(16);
		Delay(5000);
		D8led_gestionar(14);
		Delay(5000);
	}
#endif
}

void  __attribute__((interrupt("UNDEF"))) handler_undef(void) {
	volatile int lr;
	asm("mov %[resultado], lr" : [resultado] "=r" (lr));
	handler(exception_undef, lr - 4);
}

void  __attribute__((interrupt("SWI"))) handler_swi(void) {
	volatile int lr;
	asm("mov %[resultado], lr" : [resultado] "=r" (lr));
	handler(exception_swi, lr - 4);
}

void  __attribute__((interrupt("ABORT"))) handler_dabort(void) {
	volatile int lr;
	asm("mov %[resultado], lr" : [resultado] "=r" (lr));
	handler(exception_dabort, lr - 8);
}

void excepciones_inicializar(void) {
	pISR_UNDEF = (unsigned) handler_undef;
	pISR_SWI = (unsigned) handler_swi;
	pISR_DABORT = (unsigned) handler_dabort;
}
