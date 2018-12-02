#include "calibracion_tp.h"
#include "reversi_gui.h"
#include "tp.h"
#include "lcd.h"
#include "Bmp.h"

/* Sprite de cruz empleado en la calibración */
const INT8U ucCrossTileMap[] = {
BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK,
BLACK, BLACK, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, BLACK, BLACK,
BLACK, WHITE, BLACK, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, BLACK, WHITE, BLACK,
BLACK, WHITE, WHITE, BLACK, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, BLACK, WHITE, WHITE, BLACK,
BLACK, WHITE, WHITE, WHITE, BLACK, WHITE, WHITE, WHITE, WHITE, WHITE, BLACK, WHITE, WHITE, WHITE, BLACK,
BLACK, WHITE, WHITE, WHITE, WHITE, BLACK, WHITE, WHITE, WHITE, BLACK, WHITE, WHITE, WHITE, WHITE, BLACK,
BLACK, WHITE, WHITE, WHITE, WHITE, WHITE, BLACK, WHITE, BLACK, WHITE, WHITE, WHITE, WHITE, WHITE, BLACK,
BLACK, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, BLACK, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, BLACK,
BLACK, WHITE, WHITE, WHITE, WHITE, WHITE, BLACK, WHITE, BLACK, WHITE, WHITE, WHITE, WHITE, WHITE, BLACK,
BLACK, WHITE, WHITE, WHITE, WHITE, BLACK, WHITE, WHITE, WHITE, BLACK, WHITE, WHITE, WHITE, WHITE, BLACK,
BLACK, WHITE, WHITE, WHITE, BLACK, WHITE, WHITE, WHITE, WHITE, WHITE, BLACK, WHITE, WHITE, WHITE, BLACK,
BLACK, WHITE, WHITE, BLACK, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, BLACK, WHITE, WHITE, BLACK,
BLACK, WHITE, BLACK, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, BLACK, WHITE, BLACK,
BLACK, BLACK, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, BLACK, BLACK,
BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK
};
const STRU_BITMAP BITMAP_CROSS = {0x10, 4, 15, 15, TRANSPARENCY, (INT8U *)ucCrossTileMap};

enum {
	LCD_WIDTH = 320, // Tamaño del LCD LCD_WIDTH x LCD_HEIGHT
	LCD_HEIGHT = 240,
	N_TESTS_CALIBRACION = 4, // Número de tests empleados en la calibración
	SPRITE_MARGIN = 15 // Margen a dejar con el borde de la pantalla al dibujar sprites
};

// Indica si ya se ha realizado la calibración o no (0: no, 1: sí)
static int esta_calibrado = 0;
/* Resultados de la calibración:
 * Por ejemplo, la esquina superior izquierda del touchpad corresponde
 * coordenadas (left, top) */
static ULONG left, top, right, bottom;

void calibracion_empezar() {

	/* Posiciones en el LCD donde se colocarán los sprites de ayuda
	 * a la calibración (centro del sprite):
	 * Arriba izquierda, abajo izquierda, arriba derecha, abajo derecha */
	static const int test_lcd_x[N_TESTS_CALIBRACION] =
		{ SPRITE_MARGIN, SPRITE_MARGIN, LCD_WIDTH - SPRITE_MARGIN, LCD_WIDTH - SPRITE_MARGIN };
	static const int test_lcd_y[N_TESTS_CALIBRACION] =
		{ SPRITE_MARGIN, LCD_HEIGHT - SPRITE_MARGIN, SPRITE_MARGIN, LCD_HEIGHT - SPRITE_MARGIN };
	// Resultados obtenidos de cada uno de los test en coordenadas del TS
	ULONG ts_x[N_TESTS_CALIBRACION], ts_y[N_TESTS_CALIBRACION];

	// Realizar N_TESTS_CALIBRACION para calibrar la pantalla
	int i = 0;
	for (i = 0; i < N_TESTS_CALIBRACION; i++) {
		// Dibujar sprite "cross" en una pantalla vacía
		gui_limpiar_pantalla();
		BitmapView(	test_lcd_x[i] - BITMAP_CROSS.usWidth / 2,
					test_lcd_y[i] - BITMAP_CROSS.usHeight / 2,
					BITMAP_CROSS );
		Lcd_Dma_Trans();

		// Obtener resultados del TS
		TS_empezar();
		int ready = 0;
		while (!ready) {
			TS_leer(&ready, &ts_x[i], &ts_y[i]);
		}
		TS_parar();
		Delay(1000); // esperar para evitar toques seguidos
	}

	/*
	 * Almacenar los resultados para conversiones posteriores:
	 * Obtener coordenadas del touchpad según resultados de tests 0-3
	 *   /|\-   top   -
	 *    |0           2|
	 * left             right
	 *    |1           3|
	 *    X-   bottom  ->
	 */
	left = (ts_x[0] + ts_x[1]) / 2;
	top = (ts_y[0] + ts_y[2]) / 2;
	right = (ts_x[2] + ts_x[3]) / 2;
	bottom = (ts_y[1] + ts_y[3]) / 2;
	esta_calibrado = 1; // marcar que se ha realizado la calibración

	// Limpiar la pantalla finalmente
	gui_limpiar_pantalla();

}

// TODO hacer que devuelva 1 si estaba calibrado y 0 si no?
void calibracion_convertir(int ts_x, int ts_y, int* lcd_x, int* lcd_y) {
	if (esta_calibrado) {
		// Invertir el eje Y, no hace falta invertir el X
		*lcd_x = (ts_x - left) * LCD_WIDTH / (right - left);
		*lcd_y = (top - ts_y) * LCD_HEIGHT / (top - bottom);
	} else {
		*lcd_x = -1;
		*lcd_y = -1;
	}
}
