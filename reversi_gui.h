/**
 * Biblioteca para la interfaz gráfica del Reversi 8
 * @author Diego Royo Menese <>, Victor Martínez Batlle <>
 */

#ifndef __REVERSI_GUI__
#define __REVERSI_GUI__

/* TODO merge de este enum con el de reversi_main, decidir como hacerlo */
enum contenido_casilla {
	VACIA = 0,
	BLANCA = 1,
	NEGRA = 2,
	GRIS = 3
};

void gui_inicializar();
void gui_limpiar_pantalla();
void gui_dibujar_tablero_vacio();
/* TODO cambiar el 8 por un enum */
void gui_dibujar_tablero_completo(char* tablero[][8]);
void gui_dibujar_ficha(int fila, int columna, enum contenido_casilla casilla);

#endif /* __REVERSI_GUI__ */
