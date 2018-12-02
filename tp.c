/*********************************************************************************************
* FileÂ£Âº	tp.c
* Author:	embest	
* DescÂ£Âº	LCD touch screen control function
* History:	
*********************************************************************************************/

/*--- include files ---*/
#include <string.h>
#include "tp.h"
#include "lcd.h"

static volatile int touched = 0;
static volatile ULONG pos_x, pos_y;

void TSInt(void) __attribute__((interrupt("IRQ")));

/*********************************************************************************************
* name:		TSInt
* func:		TouchScreen interrupt handler function
* para:		none
* ret:		none
* modify:
* comment:		
********************************************************************************************/
void TSInt(void)
{
    int   i;
    ULONG Pt[5];

    rINTMSK |= BIT_EINT2; // Desactivar interrupciones hasta que se vuelva a llamar a empezar

	// <X-Position Read>
	// TSPX(GPE4_Q4(+)) TSPY(GPE5_Q3(-)) TSMY(GPE6_Q2(+)) TSMX(GPE7_Q1(-))
    //       0               1                 1                0
	rPDATE=0x68;
	rADCCON=0x1<<2;			// AIN1
	
	DelayTime(1000);                // delay to set up the next channel
	for( i=0; i<5; i++ )
	{
		rADCCON |= 0x1;				// Start X-position A/D conversion
	    while( rADCCON & 0x1 );		// Check if Enable_start is low
    	while( !(rADCCON & 0x40) );	// Check ECFLG
	    Pt[i] = (0x3ff&rADCDAT);
	}
	// read X-position average value
	pos_x = (Pt[0]+Pt[1]+Pt[2]+Pt[3]+Pt[4])/5;
	
    // <Y-Position Read>
	// TSPX(GPE4_Q4(-)) TSPY(GPE5_Q3(+)) TSMY(GPE6_Q2(-)) TSMX(GPE7_Q1(+))
    //       1               0                 0                1
	rPDATE=0x98;
	rADCCON=0x0<<2;		        	// AIN0
	
	DelayTime(1000);                // delay to set up the next channel
	for( i=0; i<5; i++ )
	{
    	rADCCON |= 0x1;             // Start Y-position conversion
	    while( rADCCON & 0x1 );     // Check if Enable_start is low
    	while( !(rADCCON & 0x40) ); // Check ECFLG
	    Pt[i] = (0x3ff&rADCDAT);
	}
	// read Y-position average value
	pos_y = (Pt[0]+Pt[1]+Pt[2]+Pt[3]+Pt[4])/5; // TODO división entre 4

	touched = 1;

	rPDATE = 0xb8;                  // should be enabled	
	DelayTime(3000);                // delay to set up the next channel	

    rI_ISPC |= BIT_EINT2;            // clear pending_bit
}
			
/*********************************************************************************************
* name:		TS_init
* func:		initialize TouchScreen
* para:		none
* ret:		none
* modify:
* comment:		
*********************************************************************************************/
void TS_inicializar(void)
{
    /* enable interrupt */
	rINTMOD=0x0;
	rINTCON=0x1;
    rINTMSK |= BIT_EINT2;
    rI_ISPC |= BIT_EINT2;            // clear pending_bit
	
	// TSPX(GPE4_Q4(-)) TSPY(GPE5_Q3(-)) TSMY(GPE6_Q2(-)) TSMX(GPE7_Q1(+)) 
	//          1               1                0                 1
    rPUPE  = 0x0;	                 // Pull up
    rPDATE = 0xb8;                   // should be enabled	
    DelayTime(100); 
    
    rEXTINT |= 0x200;                // falling edge trigger
    pISR_EINT2 = (unsigned) TSInt;       // set interrupt handler
    
    rCLKCON = 0x7ff8;                // enable clock
    rADCPSR = 0x1;//0x4;             // A/D prescaler
}

void TS_empezar(void)
{
	touched = 0;
	rI_ISPC |= BIT_EINT2;              // clear pending_bit
	rINTMSK &= ~(BIT_EINT2);
}

void TS_leer(int* ready, ULONG* x, ULONG* y) {
	*ready = touched;
	if (*ready) {
		touched = 0;
		*x = pos_x;
		*y = pos_y;
	}
}

/*********************************************************************************************
* name:		TS_close TODO
* func:		close TouchScreen
* para:		none
* ret:		none
* modify:
* comment:		
*********************************************************************************************/
void TS_parar(void)
{
	/* Mask interrupt */
	rINTMSK |= BIT_EINT2;
}

