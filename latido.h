/**
 * @file    latido.h
 * @author  Victor M. Batlle <736478@unizar.es>, Diego Royo Meneses <740388@unizar.es>
 * @brief   Funciones de control del latido
 * @version 1.0.0
 */

#ifndef _LATIDO_H_
#define _LATIDO_H_

/**
 * @brief Inicializa el hardware necesario para la gestión
 *        del latido y establece los valores iniciales de
 *        estado y tiempo.
 */
void latido_inicializar();

/**
 * @brief Llamado desde el bucle de gestión de periféricos
 *        para actualizar el estado del LED que indica que
 *        el sistema funciona con normalidad.
 * @param[in] ahora Tiempo global del sistema para la iteración
 *                  actual del bucle de gestión.
 */
void latido_gestionar(unsigned long int ahora);

#endif /* _LATIDO_H_ */
