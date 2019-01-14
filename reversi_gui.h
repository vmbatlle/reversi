/**
 * @file reversi_gui.h
 * Biblioteca para la interfaz gráfica del Reversi 8
 *
 * @author Victor M. Batlle <736478@unizar.es>, Diego Royo Meneses <740388@unizar.es>
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

/** @brief Inicializa los componentes necesarios y limpia la pantalla */
void gui_inicializar();
/** @brief Activa los posibles métodos de entrada (touchpad) */
void gui_empezar();
/** @brief Desactiva los posibles métodos de entrada (touchpad) */
void gui_parar();
/** @brief Actualiza el LCD con los datos escritos en su correspondiente buffer */
void gui_refrescar();
/** @brief Limpia el contenido de la pantalla */
void gui_limpiar_pantalla();
/** @brief Dibuja en el buffer la pantalla de bienvenida del juego */
void gui_dibujar_bienvenida();
/** @brief Dibuja en el buffer los componentes estáticos de la pantalla de juego (tablero, botones, etc.) */
void gui_dibujar_tablero_vacio();
/** @brief Dibuja en el buffer el estado de todas las fichas del tablero */
void gui_dibujar_contenido_tablero(char tablero[][8]);
/** @brief Dibuja en el buffer una ficha en su posición correspondiente */
void gui_dibujar_ficha(int fila, int columna, enum contenido_casilla casilla);
/** @brief Dibuja en el buffer el texto correspondiente a la leyenda inferior */
void gui_escribir_leyenda(char* leyenda);
/** @brief Dibuja en el buffer el texto correspondiente a los datos de profiling (parte derecha) */
void gui_escribir_profiling(unsigned long long int total,
		unsigned long long int calculos, unsigned long long int patron_volteo,
		unsigned int patron_volteo_calls);

/**
 * @brief 	Consulta el estado de la entrada del touchpad
 * @return	El tipo de toque que ha ocurrido (si ha ocurrido)
 */
enum toque_pantalla gui_touch_screen_gestionar();
/**
 * @brief 	(Función de test)
 * 			Consulta si ha sucedido algún toque en el touchpad y dibuja un sprite en el centro del toque.
 * 			De esta forma, se puede comprobar la exactitud del touchpad y la conversión de coordenadas.
 */
void gui_touch_screen_test();

#endif /* __REVERSI_GUI__ */
