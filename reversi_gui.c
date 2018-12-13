/**
 * Biblioteca para la interfaz gráfica del Reversi 8
 * @author Diego Royo Menese <>, Victor Martínez Batlle <>
 */
#include "reversi_gui.h"
#include "jugada_por_pantalla.h"
#include "stdlib.h"
#include "calibracion_tp.h"
#include "lcd.h"
#include "sprites.h"
#include "Bmp.h"
#include "tp.h"

/* Opciones de dibujado del tablero */
enum {
	MARGEN_TEXTO = 2, // margen del texto de fila/columna
	BORDE_TABLERO = 2, // borde del tablero (tendrá BORDE_TABLERO + 1 píxeles)
	SIZE_X_TEXTO = 8,
	SIZE_Y_TEXTO = 16,
	// posicion X del pixel de mas arriba a la izquierda del tablero
	// sin contar el borde
	POS_X_TABLERO = SIZE_X_TEXTO + MARGEN_TEXTO + BORDE_TABLERO,
	// posicion Y del pixel de mas arriba a la izquierda del tablero
	// sin contar el borde
	POS_Y_TABLERO = SIZE_Y_TEXTO + MARGEN_TEXTO + BORDE_TABLERO,
	CASILLAS_TABLERO = 8,
	SIZE_CASILLA = 26, // tamaño de cada casilla entre las dos líneas que la separan (inclusive)
	SIZE_FICHA = 20 // tamaño del sprite de ficha
};

const STRU_BITMAP BITMAP_BLACK_TILE = {0x10, 4, SIZE_FICHA, SIZE_FICHA, TRANSPARENCY, (INT8U *)ucBlackTileMap};
const STRU_BITMAP BITMAP_WHITE_TILE = {0x10, 4, SIZE_FICHA, SIZE_FICHA, TRANSPARENCY, (INT8U *)ucWhiteTileMap};
const STRU_BITMAP BITMAP_GREY_TILE = {0x10, 4, SIZE_FICHA, SIZE_FICHA, TRANSPARENCY, (INT8U *)ucGreyTileMap};
const STRU_BITMAP BITMAP_EMPTY_TILE = {0x10, 4, SIZE_FICHA, SIZE_FICHA, TRANSPARENCY, (INT8U *)ucEmptyTileMap};
const STRU_BITMAP BITMAP_BUTTON_BACKGROUND = {0x10, 4, 80, 50, TRANSPARENCY, (INT8U *)ucButtonBackgroundMap};

void gui_inicializar() {
	Lcd_Init();
	TS_inicializar();
	gui_limpiar_pantalla();
}

void gui_empezar() {
	TS_empezar();
}

void gui_parar() {
	TS_parar();
}

void gui_refrescar() {
	Lcd_Dma_Trans();
}

void gui_limpiar_pantalla() {
	Lcd_Clr();
	Lcd_Active_Clr();
}

void gui_dibujar_bienvenida() {
	Lcd_DspAscII8x16(POS_X_TABLERO, POS_Y_TABLERO, BLACK, (INT8U*)"Toque para jugar");
}

void gui_dibujar_tablero_vacio() {
	/* Marco exterior del tablero con borde más gordo */
	int inner_box_length = (SIZE_CASILLA - 1) * CASILLAS_TABLERO;
	int outer_box_length = inner_box_length + 2 * BORDE_TABLERO;
	Lcd_Draw_Box_Width(POS_X_TABLERO - BORDE_TABLERO, POS_Y_TABLERO - BORDE_TABLERO,
				POS_X_TABLERO + outer_box_length - BORDE_TABLERO, POS_Y_TABLERO + outer_box_length - BORDE_TABLERO, BLACK, BORDE_TABLERO + 1);

	/* Dibujar líneas del tablero interior */
	int i;
	for (i = 1; i < CASILLAS_TABLERO; i++) {
		Lcd_Draw_HLine(POS_X_TABLERO, POS_X_TABLERO + inner_box_length,
						POS_Y_TABLERO + (SIZE_CASILLA - 1) * i, BLACK, 1);
		Lcd_Draw_VLine(POS_Y_TABLERO, POS_Y_TABLERO + inner_box_length,
						POS_X_TABLERO + (SIZE_CASILLA - 1) * i, BLACK, 1);
	}

	/*
	 * TODO: pasar los numeros magicos (+1, +4) a constantes,
	 * considerar tambien cambiar SIZE_X/Y_TEXTO a 7x9
	 * vienen dados por la fuente empleada en ascii 8x16, por ejemplo el caracter '0':
	 * ........
	 * ........
	 * ........
	 * .#####..
	 * ##...##.
	 * ##..###.
	 * ##.####.
	 * ####.##.
	 * ###..##.
	 * ##...##.
	 * ##...##.
	 * .#####..
	 * ........
	 * ........
	 * ........
	 * ........
	 */
	char digito = '1';
	/* Dibujar numeración de filas y columnas (1 - 8) */
	for (i = 0; i < CASILLAS_TABLERO; i++) {
		/* Número de fila */
		Lcd_DspAscII8x16(POS_X_TABLERO - MARGEN_TEXTO - SIZE_X_TEXTO + 1,
							POS_Y_TABLERO + (SIZE_CASILLA - 1) * i + (SIZE_CASILLA - SIZE_Y_TEXTO) / 2,
							BLACK,(INT8U*)&digito);
		/* Número de columna */
		Lcd_DspAscII8x16(POS_X_TABLERO + (SIZE_CASILLA - 1) * i + (SIZE_CASILLA - SIZE_X_TEXTO) / 2,
							POS_Y_TABLERO - MARGEN_TEXTO - SIZE_Y_TEXTO + 4,
							BLACK,(INT8U*)&digito);
		digito = digito + 1;
	}

	/* Botones para pasar turno y finalizar */
	BitmapView(230, 10, BITMAP_BUTTON_BACKGROUND);
	Lcd_DspAscII8x16(240, 20, BLACK,(INT8U*)"Pasar");
	BitmapView(230, 180, BITMAP_BUTTON_BACKGROUND);
	Lcd_DspAscII8x16(240, 190, BLACK,(INT8U*)"Terminar");

	/* Títulos para la información de profiling */
	Lcd_DspAscII8x16(230, 65, BLACK,(INT8U*)"Tiempo ejecucion");
	Lcd_DspAscII8x16(230, 80, BLACK,(INT8U*)"patron_volteo:");
	Lcd_DspAscII8x16(230, 110, BLACK,(INT8U*)"Total calculos:");
	Lcd_DspAscII8x16(230, 140, BLACK,(INT8U*)"Total ejecucion:");
}

/* TODO cambiar el 8 por un enum */
void gui_dibujar_contenido_tablero(char tablero[8][8]) {
	int x, y;
	for (x = 0; x < CASILLAS_TABLERO; x++) {
		for (y = 0; y < CASILLAS_TABLERO; y++) {
			gui_dibujar_ficha(x, y, (enum contenido_casilla) tablero[x][y]);
		}
	}
}

void gui_dibujar_ficha(int fila, int columna, enum contenido_casilla casilla) {
	if (fila >= 0 && fila < CASILLAS_TABLERO && columna >= 0 && columna < CASILLAS_TABLERO) {
		/* Calcular la posición de dibujo */
		int pos_x = POS_X_TABLERO + (SIZE_CASILLA - 1) * fila + (SIZE_CASILLA - SIZE_FICHA) / 2;
		int pos_y = POS_Y_TABLERO + (SIZE_CASILLA - 1) * columna + (SIZE_CASILLA - SIZE_FICHA) / 2;

		/* Dibujar el sprite especificado por casilla */
		switch (casilla) {
		case FICHA_BLANCA:
			BitmapView(pos_x, pos_y, BITMAP_WHITE_TILE);
			break;
		case FICHA_NEGRA:
			BitmapView(pos_x, pos_y, BITMAP_BLACK_TILE);
			break;
		case FICHA_GRIS:
			BitmapView(pos_x, pos_y, BITMAP_GREY_TILE);
			break;
		default:
			BitmapView(pos_x, pos_y, BITMAP_EMPTY_TILE);
			break;
		}
	}
}

void gui_escribir_profiling(unsigned long long int total,
		unsigned long long int calculos, unsigned long long int patron_volteo,
		unsigned int patron_volteo_calls) {

	// Ajustar todos los tiempos a milisegundos
	total /= 1000;
	calculos /= 1000;
	patron_volteo /= 1000;

	// Obtener strings de tiempo (convertir total de ms a s)
	char total_str[12], calculos_str[13], patron_volteo_str[13];
	itoa(total / 1000, total_str, 10);
	itoa(calculos, calculos_str, 10);
	itoa(patron_volteo, patron_volteo_str, 10);
	// Añadir ms o s al final
	int total_len = strlen(total_str);
	total_str[total_len] = 's';
	total_str[total_len + 1] = '\0';
	int calculos_len = strlen(calculos_str);
	total_str[calculos_len] = 'm';
	total_str[calculos_len + 1] = 's';
	total_str[calculos_len + 2] = '\0';
	int patron_volteo_len = strlen(patron_volteo_str);
	total_str[patron_volteo_len] = 'm';
	total_str[patron_volteo_len + 1] = 's';
	total_str[patron_volteo_len + 2] = '\0';
	// Escribir el texto por pantalla (TODO limpiar el texto anterior)
	Lcd_DspAscII8x16(230, 95, BLACK,(INT8U*)patron_volteo_str);
	Lcd_DspAscII8x16(230, 125, BLACK,(INT8U*)calculos_str);
	Lcd_DspAscII8x16(230, 155, BLACK,(INT8U*)patron_volteo_str);
	// TODO: Añadir número de invocaciones
}

void gui_escribir_leyenda(char* leyenda) {
	// Limpiar zona de pantalla asociada a la leyenda
	/*Lcd_Draw_HLine(POS_X_TABLERO, POS_X_TABLERO + (SIZE_CASILLA - 1) * CASILLAS_TABLERO,
					POS_Y_TABLERO + (SIZE_CASILLA - 1) * CASILLAS_TABLERO + BORDE_TABLERO + 1, WHITE,
					LCD_YSIZE - POS_Y_TABLERO + (SIZE_CASILLA - 1) * CASILLAS_TABLERO + BORDE_TABLERO + 1);*/
	Lcd_DspAscII8x16(POS_X_TABLERO,
						POS_Y_TABLERO + (SIZE_CASILLA - 1) * CASILLAS_TABLERO + MARGEN_TEXTO,
						BLACK,(INT8U*)leyenda);
}

enum toque_pantalla gui_touch_screen_gestionar() {
	int ready;
	ULONG ts_x, ts_y;
	TS_leer(&ready, &ts_x, &ts_y);
	if (ready) {
		int lcd_x, lcd_y;
		calibracion_convertir(ts_x, ts_y, &lcd_x, &lcd_y);
		TS_empezar(); /* volver a activar interrupciones TS */
		if (lcd_x > LCD_XSIZE / 2 - 50 && lcd_x < LCD_XSIZE / 2 + 50
			&& lcd_y > LCD_YSIZE / 2 - 50 && lcd_y < LCD_YSIZE / 2 + 50) {
			return toque_central;
		} else if (lcd_x > LCD_XSIZE - 100) {
			if (lcd_y > LCD_YSIZE / 2) {
				return toque_finalizar;
			} else {
				return toque_pasar;
			}
		} else {
			return toque_none;
		}
	} else {
		return toque_none;
	}
}

void gui_touch_screen_test() {
	int ready;
	ULONG ts_x, ts_y;
	TS_leer(&ready, &ts_x, &ts_y);
	if (ready) {
		int lcd_x, lcd_y;
		calibracion_convertir(ts_x, ts_y, &lcd_x, &lcd_y);
		TS_empezar(); /* volver a activar interrupciones TS */
		BitmapView(lcd_x - BITMAP_WHITE_TILE.usWidth / 2, lcd_y - BITMAP_WHITE_TILE.usHeight / 2, BITMAP_WHITE_TILE);
		gui_refrescar();
	}
}
