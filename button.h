#ifndef _BUTTON_H_
#define _BUTTON_H_

/* TODO cabeceras */
enum estado_button {button_none, button_iz, button_dr};

void button_iniciar(void);

void button_empezar(void (*callback)(enum estado_button));

enum estado_button button_estado(void);

/* TODO buscar un nombre mejor? y ponerlo en el .c */
void button_tratar(enum estado_button button) __attribute__((interrupt("IRQ")));

#endif /* _BUTTON_H_ */
