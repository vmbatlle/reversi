/**
 * @file calibracion_tp.h
 * Módulo auxiliar para la conversión de coordenadas del TP a coordenadas del LCD
 *
 * @author Victor M. Batlle <736478@unizar.es>, Diego Royo Meneses <740388@unizar.es>
 */
#ifndef __CALIBRACION_TP_H__
#define __CALIBRACION_TP_H__

/**
 * @brief Realiza el protocolo de calibración de la pantalla táctil
 *        mostrando varios sprites por pantalla y pidiendo al usuario
 *        que los pulse.
 *        IMPORTANTE: Se debe haber inicializado el LCD y TS previamente
 *        llamando a gui_empezar()
 */
void calibracion_empezar();

/**
 * @brief Convierte las coordenadas (ts_x, ts_y) obtenidas al pulsar
 *        la pantalla táctil a coordenadas (lcd_x, lcd_y) de la pantalla
 */
void calibracion_convertir(int ts_x, int ts_y, int* lcd_x, int* lcd_y);

#endif /* __CALIBRACION_TP_H__ */
