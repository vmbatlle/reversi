#ifndef _NESTED_INTERRUPTS_H_
#define _NESTED_INTERRUPTS_H_

/* Protocolos para permitir la anidación de interrupciones IRQ dentro de una
 * rutina de servicio. */

/**
 * @brief Permite la anidación de interrupciones:
 * 		   - Reactiva las interrupciones IRQ (bit 7 del CPSR a 0)
 * 		   - Cambia a modo SYSTEM (bits 4-0 con valor 0x1F)
 * 		  y guarda el estado del SPSR en pila
 */
void __attribute__((always_inline)) enableNestedInterrupts() {
	unsigned int cpsr, spsr;
	// Leer el valor del SPSR
	asm("MRS %[spsr], SPSR" : [spsr] "=r" (spsr));

	// Guardar el valor del CPSR,
	// cambiar a modo SYSTEM (CPSR[4:0] = 0b11111, 0x1F)
	// y reactivar las interrupciones IRQ (CPSR[7] = 0)
	asm("MRS %[cpsr], CPSR" : [cpsr] "=r" (cpsr));
	asm("ORR %[cpsr_out], %[cpsr_in], #0x1F" : [cpsr_out] "=r" (cpsr) : [cpsr_in] "r" (cpsr));
	asm("MSR CPSR_c, %[cpsr]" : : [cpsr] "r" (cpsr)); /* modo SYSTEM, bits de modo = 0x1F */
	asm("BIC %[cpsr_out], %[cpsr_in], #0x80" : [cpsr_out] "=r" (cpsr) : [cpsr_in] "r" (cpsr));
	asm("MSR CPSR_c, %[cpsr]" : : [cpsr] "r" (cpsr)); /* activar IRQ */

	// Guardar el estado del SPSR en la pila del modo SYSTEM (compartida con modo USER)
	// Ha tenido que ser inicializada previamente (en este caso, 44binit.asm)
	asm("PUSH {%[spsr]}" : : [spsr] "r" (spsr));
}

/**
 * @brief Recupera el valor del SPSR almacenado y
 * 		  desactiva las interrupciones anidadas:
 * 		   - Desactiva de nuevo las interrupciones IRQ (bit 7 del CPSR a 0)
 * 		   - Cambia a modo IRQ (bits 4-0 del CPSR con valor 0x12)
 */
void __attribute__((always_inline)) disableNestedInterrupts() {
	unsigned int cpsr, spsr;
	// Desactivar interrupciones (CPSR[7] = 1)
	asm("MRS %[cpsr], CPSR" : [cpsr] "=r" (cpsr));
	asm("ORR %[cpsr_out], %[cpsr_in], #0x80" : [cpsr_out] "=r" (cpsr) : [cpsr_in] "r" (cpsr));
	asm("MSR CPSR_c, %[cpsr]" : : [cpsr] "r" (cpsr)); /* desactivar IRQ */

	// Rescatar el valor del SPSR almacenado en la pila del modo SYSTEM
	asm("POP {%[spsr]}" : [spsr] "=r" (spsr));

	// Cambiar a modo IRQ (CPSR[4:0] = 0b10010, 0x12)
	asm("BIC %[cpsr_out], %[cpsr_in], #0x1F" : [cpsr_out] "=r" (cpsr) : [cpsr_in] "r" (cpsr));
	asm("ORR %[cpsr_out], %[cpsr_in], #0x12" : [cpsr_out] "=r" (cpsr) : [cpsr_in] "r" (cpsr));
	asm("MSR CPSR_c, %[cpsr]" : : [cpsr] "r" (cpsr)); /* modo IRQ (0b10010) */

	// Devolver el SPSR a su valor original (antes de iniciar las interrupciones anidadas)
	asm("MSR SPSR, %[spsr]" : : [spsr] "r" (spsr));
}

#endif /* _NESTED_INTERRUPTS_H_ */
