/*********************************************************************************************
* Fichero:		timer.h
* Autor:        Victor M. Batlle <736478@unizar.es>
* Descrip:		funciones de control del timer2 del s3c44b0x
* Version:		1.0.0
*********************************************************************************************/

#ifndef _TIMER_H_
#define _TIMER_H_

/*--- declaracion de funciones visibles del módulo timer.c/timer.h ---*/

/**
 * @brief Configura el temporazidor empotrado para su uso.
 */
void timer2_inicializar(void);

/**
 * @brief Inicia o reinicia la cuenta de tiempo
 */
void timer2_empezar(void);

/**
 * @brief  Consulta el tiempo transcurrido desde la 
 *         últmia invocación a @ref timer2_empezar.
 * @return Tiempo transcurrido en microsegundos.
 */
void unsigned int timer2_leer(void);

/**
 * @brief   Detiene el contador y devuelve el tiempo
 *          transcurrido desde la últmia invocación 
 *          a @ref timer2_empezar.
 * @warning El contador se parara no provocando más 
 *          interrupciones hasta que no se ponga en
 *          marcha de nuevo (@ref timer2_empezar).
 * @return  Tiempo transcurrido en microsegundos.
 */
unsigned int timer2_parar(void);
#endif /* _TIMER_H_ */
