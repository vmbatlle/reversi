#ifndef _NESTED_INTERRUPTS_H_
#define _NESTED_INTERRUPTS_H_



/**
 * @brief Permite la anidación de interrupciones:
 * 		  reactiva las interrupciones (bit 7 del CPSR a 0)
 * 		  y cambia a modo SYSTEM (bits 4-0 con valor 0x1F)
 */
void __attribute__((always_inline)) enableNestedInterrupts() {
	unsigned int cpsr, spsr;
	asm("MRS %[spsr], SPSR" : [spsr] "=r" (spsr));

	asm("MRS %[cpsr], CPSR" : [cpsr] "=r" (cpsr));
	asm("ORR %[cpsr_out], %[cpsr_in], #0x1F" : [cpsr_out] "=r" (cpsr) : [cpsr_in] "r" (cpsr)); /* modo SYSTEM, bits de modo a 1 */
	asm("MSR CPSR_c, %[cpsr]" : : [cpsr] "r" (cpsr));
	asm("BIC %[cpsr_out], %[cpsr_in], #0x80" : [cpsr_out] "=r" (cpsr) : [cpsr_in] "r" (cpsr)); /* activar IRQ */
	asm("MSR CPSR_c, %[cpsr]" : : [cpsr] "r" (cpsr));

	asm("PUSH {%[spsr]}" : : [spsr] "r" (spsr)); /* guardar el estado del CPSR y SPSR en pila del modo previo */
}

void __attribute__((always_inline)) disableNestedInterrupts() {
	unsigned int cpsr, spsr;
	asm("MRS %[cpsr], CPSR" : [cpsr] "=r" (cpsr));
	asm("ORR %[cpsr_out], %[cpsr_in], #0x80" : [cpsr_out] "=r" (cpsr) : [cpsr_in] "r" (cpsr)); /* desactivar IRQ */
	asm("MSR CPSR_c, %[cpsr]" : : [cpsr] "r" (cpsr));
	asm("POP {%[spsr]}" : [spsr] "=r" (spsr));
	asm("BIC %[cpsr_out], %[cpsr_in], #0x1F" : [cpsr_out] "=r" (cpsr) : [cpsr_in] "r" (cpsr));
	asm("ORR %[cpsr_out], %[cpsr_in], #0x12" : [cpsr_out] "=r" (cpsr) : [cpsr_in] "r" (cpsr)); /* modo IRQ (0b10010) */
	asm("MSR CPSR_c, %[cpsr]" : : [cpsr] "r" (cpsr));
	asm("MSR SPSR, %[spsr]" : : [spsr] "r" (spsr)); /* recuperar el valor apilado del CPSR y SPSR */
}

#endif /* _NESTED_INTERRUPTS_H_ */
