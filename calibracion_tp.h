#ifndef __CALIBRACION_TP_H__
#define __CALIBRACION_TP_H__

/**
 * @brief Realiza el protocolo de calibración de la pantalla táctil
 *        mostrando varios sprites por pantalla y pidiendo al usuario
 *        que los pulse
 */
void calibracion_empezar();

/**
 * @brief Convierte las coordenadas (ts_x, ts_y) obtenidas al pulsar
 *        la pantalla táctil a coordenadas (lcd_x, lcd_y) de la pantalla
 */
void calibracion_convertir(int ts_x, int ts_y, int* lcd_x, int* lcd_y);

#endif /* __CALIBRACION_TP_H__ */
