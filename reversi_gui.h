/**
 * Biblioteca para la interfaz gráfica del Reversi 8
 * @author Diego Royo Menese <>, Victor Martínez Batlle <>
 */

#ifndef __REVERSI_GUI__
#define __REVERSI_GUI__

// Estados de las casillas del tablero
enum contenido_casilla {
	CASILLA_VACIA = 0,
	FICHA_BLANCA = 1,
	FICHA_NEGRA = 2,
	FICHA_GRIS = 3
};

// Tipos de toque sobre panel táctil
enum toque_pantalla {
	toque_none = 0,
	toque_central = 1,
	toque_pasar = 2, /* botón arriba a la derecha, pasar el turno */
	toque_finalizar = 3 /* botón abajo a la derecha, finalizar la partida */
};

void gui_inicializar();
void gui_empezar();
void gui_parar();
void gui_refrescar();
void gui_limpiar_pantalla();
void gui_dibujar_bienvenida();
void gui_dibujar_tablero_vacio();
/* TODO cambiar el 8 por un enum */
void gui_dibujar_contenido_tablero(char tablero[][8]);
void gui_dibujar_ficha(int fila, int columna, enum contenido_casilla casilla);
void gui_escribir_leyenda(char* leyenda);
void gui_escribir_profiling(unsigned long long int total,
		unsigned long long int calculos, unsigned long long int patron_volteo,
		unsigned int patron_volteo_calls);

enum toque_pantalla gui_touch_screen_gestionar();
void gui_touch_screen_test();

#endif /* __REVERSI_GUI__ */
