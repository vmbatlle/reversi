#include "stdlib.h"

/*********************************************************************************************
* name:		strlen()
* func:		counts number of chars of a null-terminated string
* para:		s -- ASCII character string's start pointer
* ret:		length of the string
* modify:
* comment:
*********************************************************************************************/
int strlen(const char* s) {
	int i = 0; // Contador de caracteres
	const char* c = s; // Cursor
	while (*c != '\0') {
		i++;
		c++;
	}
	return i;
}

/*********************************************************************************************
* name:		reverse()
* func:		Reverse a null-terminated string
* para:		s -- ASCII character string's start pointer
* ret:		length of the string
* modify:
* comment:
*********************************************************************************************/
void reverse(char s[])
{
    int i, j;

    int length = strlen(s);
    for (i = 0, j = length - 1; i < j; i++, j--) {
    	s[j] ^= s[i];
        s[i] ^= s[j];
        s[j] ^= s[i];
    }
}

/*********************************************************************************************
* name:		itoa()
* func:		Integer to string
* para:		n -- Integer to convert
*           str -- ASCII character string's start pointer
*           max -- Max str buffer length
* ret:		length of the string
* modify:
* comment:
*********************************************************************************************/
void itoa(unsigned int n, char* s, int max) {
	unsigned int i = n;
	char* str = s;
	do {
		*str = i % 10 + '0';
		str++;
		i = i / 10;
		max--;
	} while (i > 0 && max > 1);
	*str = '\0';
	reverse(s);
}

/*********************************************************************************************
* name:		abs()
* func:		Calculates the absolute value of a given number: |n|
* para:		n -- number
* ret:		|n|
* modify:
* comment:
*********************************************************************************************/
int abs(int n) {
	if (n < 0) {
		return -n;
	} else {
		return n;
	}
}
