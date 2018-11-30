/*********************************************************************************************
* File£º	tp.H
* Author:	embest	
* Desc£º	Touch Screen define file
* History:	
*********************************************************************************************/
#include "def.h"
#include "44b.h"
#include "44blib.h"
#ifndef __TP_H__
#define __TP_H__

#endif /*__TP_H__*/

/*--- global  variables ---*/
volatile int CheckTSP,oneTouch;
unsigned int  Vx, Vy;
unsigned int  Xmax;
unsigned int  Ymax;
unsigned int  Xmin;
unsigned int  Ymin;

void TSInt(void);
void TS_inicializar(void);
void TS_empezar(void);
void TS_leer(int* ready, ULONG* x, ULONG* y);
void TS_parar(void);
//void user_irq1(void);
