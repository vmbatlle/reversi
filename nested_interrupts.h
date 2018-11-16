#ifndef _NESTED_INTERRUPTS_H_
#define _NESTED_INTERRUPTS_H_



/**
 * @brief Permite la anidación de interrupciones:
 * 		  reactiva las interrupciones (bit 7 del CPSR a 0)
 * 		  y cambia a modo SYSTEM (bits 4-0 con valor 0x1F)
 */
void __attribute__((always_inline)) enableNestedInterrupts() {
	asm("MRS r1, SPSR");

	asm("MRS r0, CPSR");
	asm("MOV r2, r0");
	asm("BIC r0, #0x80"); /* activar IRQ */
	asm("ORR r0, #0x1F"); /* modo SYSTEM, bits de modo a 1 */
	asm("MSR CPSR_c, r0");

	asm("PUSH {r1, r2}"); /* guardar el estado del CPSR y SPSR en pila del modo previo */
}

void __attribute__((always_inline)) disableNestedInterrupts() {
	asm("POP {r0, r1}");
	asm("MSR SPSR, r0"); /* recuperar el valor apilado del CPSR y SPSR */
	asm("MSR CPSR, r1");
}

#endif /* _NESTED_INTERRUPTS_H_ */
