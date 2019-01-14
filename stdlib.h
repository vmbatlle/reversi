/**
 * @file stdlib.h
 * Implementación de varias funciones de la biblioteca estándar de C
 *
 * @author Victor M. Batlle <736478@unizar.es>, Diego Royo Meneses <740388@unizar.es>
 */
#ifndef __STDLIB_H__
#define __STDLIB_H__

int abs(int n);
int strlen(const char* s);
void reverse(char s[]);
void itoa(unsigned int n, char * str, int max);

#endif /* __STDLIB_H__ */
