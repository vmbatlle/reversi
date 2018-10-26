/**
 * @file    timer0.h
 * @author  Victor M. Batlle <736478@unizar.es>, Diego Royo Meneses <740388@unizar.es>
 * @brief   Funciones de control del timer0 del s3c44b0x
 * @version 1.0.0
 */

#ifndef _TIMER0_H_
#define _TIMER0_H_

/*--- declaracion de funciones visibles del módulo timer.c/timer.h ---*/

/**
 * @brief Configura el temporazidor empotrado para su uso.
 */
void timer0_inicializar(void);

/**
 * @brief Inicia o reinicia la cuenta de tiempo
 */
void timer0_empezar(void);

/**
 * @brief  Consulta el tiempo transcurrido desde la 
 *         últmia invocación a @ref timer0_empezar.
 * @return Tiempo transcurrido en microsegundos.
 */
unsigned long int timer0_leer(void);

/**
 * @brief   Detiene el contador y devuelve el tiempo
 *          transcurrido desde la últmia invocación
 *          a @ref timer0_empezar.
 * @warning El contador se parara no provocando más
 *          interrupciones hasta que no se ponga en
 *          marcha de nuevo (@ref timer0_empezar).
 * @return  Tiempo transcurrido en microsegundos.
 */
unsigned long int timer0_parar(void);
#endif /* _TIMER0_H_ */
