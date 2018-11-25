/**
 * @file exceptions.c
 * Gestión de excepciones
 *
 * @author Victor M. Batlle <736478@unizar.es>, Diego Royo Meneses <740388@unizar.es>
 */
#include "exceptions.h"
#include "options_environment.h"
#include "8led.h"
#include "44b.h"
#include "44blib.h"

#if defined(ENVIRONMENT_EMULATOR)
static int error_variable = 0;
#endif

enum exception_type { exception_undef = 0, exception_swi = 1, exception_dabort = 2 };

void handler(enum exception_type type, int lr) {
#if defined(ENVIRONMENT_EMULATOR)
	error_variable = 1;
	// Evitar la optimización de la variable por parte del compilador
	asm("" : [none] "=r" (error_variable) : );
#else
	// Alternar entre los valores E y blank en el 8led
	D8led_inicializar();
	while(1) {
		D8led_gestionar(16); // 16 = blank (no mostrar nada en el 8led)
		Delay(5000);
		D8led_gestionar(14); // 14 = 0xE (mostrar una E en el 8led)
		Delay(5000);
	}
#endif
}

// Handler para instrucción no definida
// Para probar su funcionamiento: asm volatile (".word 0xe7f000f0\n");
void  __attribute__((interrupt("UNDEF"))) handler_undef(void) {
	volatile int lr;
	asm("mov %[resultado], lr" : [resultado] "=r" (lr));
	handler(exception_undef, lr - 4); // corregir lr para que apunte a la instrucción que causa la excepción
}

// Handler para software interrupt
// Para probar su funcionamiento: asm("swi #1");
void  __attribute__((interrupt("SWI"))) handler_swi(void) {
	volatile int lr;
	asm("mov %[resultado], lr" : [resultado] "=r" (lr));
	handler(exception_swi, lr - 4); // corregir lr para que apunte a la instrucción que causa la excepción
}

// Handler para data abort
// Para probar su funcionamiento: asm("mov r0, #1"); asm("ldr r0, [r0]");
void  __attribute__((interrupt("ABORT"))) handler_dabort(void) {
	volatile int lr;
	asm("mov %[resultado], lr" : [resultado] "=r" (lr));
	handler(exception_dabort, lr - 8); // corregir lr para que apunte a la instrucción que causa la excepción
}

void excepciones_inicializar(void) {
	pISR_UNDEF = (unsigned) handler_undef;
	pISR_SWI = (unsigned) handler_swi;
	pISR_DABORT = (unsigned) handler_dabort;
}
