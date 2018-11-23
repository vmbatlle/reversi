#include <stdarg.h>
#include <string.h>
#include "uhal.h"
#include "44b.h"


//#######################SLT#######################
//mclk 66000000
//66000000hz/4= 16500000
///16500000/100= 165000
// 1/165000*SEC==> SEC*165

#define	_TICK	1000				//  1/1000 sec
//#define _TICK	100				//  1/100 sec
//#define _TICK	10				//  1/10 sec
#define _CLOCK	(165*(1000/_TICK))


#define MAX_TBUF	2048
static char tbuf[MAX_TBUF]= {0, };		//for uHALr_printf

//for debugging
void BreakPoint(void);
void DebugUNDEF(void);
void DebugSWI(void);
void DebugABORT(void);
void DebugFIQ(void);

//Output through LCD or serial port to print information.
extern int vsprintf(const char *format, ...);
int uHALr_printf(const char *format, ...)
{
	va_list v_list;
	char *ptr;
	int i= 0;


	va_start(v_list, format);		// Initialize variable arguments. 
	vsprintf(tbuf, format, v_list ); 
	va_end(v_list);


	ptr= tbuf;
	while( (*ptr) && i<MAX_TBUF) {
		Uart_SendByte(*ptr);
		ptr++;	i++;
	}//while

	return 1;
}//uHALr_printf


/*** Called by ARMTargetInit() ***/
//disable memory management unit.S3C44B0X无MMU功能，函数为空
void uHALr_ResetMMU(void)
{
}//uHALrResetMMU


void (*pre_isr)(void);
void *(*post_isr)(void);
//Define pre & post-process routines for Interrupt.
void uHALir_DefineIRQ(void *is, void *iq, void *n)
{
	pre_isr= (void (*)(void))is;
	post_isr= (void *(*)(void))iq;
}//uHALir_DefineIRQ


//Initialze interrupts.
void uHALr_InitInterrupts(void)
{
	// Non-vectored,IRQ disable,FIQ disable    
    rINTCON=0x0;	  

	// All=IRQ mode
    rINTMOD=0x0;	  

	// All interrupt is masked.
    rINTMSK=BIT_GLOBAL;	  

//set interrupt vector routine
//	pISR_RESET				//reserved
	pISR_UNDEF= (unsigned) DebugUNDEF;
	pISR_SWI= (unsigned) DebugSWI;
	pISR_PABORT= (unsigned) DebugABORT;
	pISR_DABORT= (unsigned) DebugABORT;
	pISR_RESERVED= (unsigned) BreakPoint;	//not used
//	pISR_IRQ= (unsigned) 0;			//reserved
	pISR_FIQ= (unsigned) DebugFIQ;
	
	pISR_ADC= (unsigned) BreakPoint;
	pISR_RTC= (unsigned) BreakPoint;
	pISR_UTXD1= (unsigned) BreakPoint;
	pISR_UTXD0= (unsigned) BreakPoint;
	pISR_SIO= (unsigned) BreakPoint;
	pISR_IIC= (unsigned) BreakPoint;
	pISR_URXD1= (unsigned) BreakPoint;
	pISR_URXD0= (unsigned) BreakPoint;
	pISR_TIMER5= (unsigned) BreakPoint;
	pISR_TIMER4= (unsigned) BreakPoint;
	pISR_TIMER3= (unsigned) BreakPoint;
	pISR_TIMER2= (unsigned) BreakPoint;
	pISR_TIMER1= (unsigned) BreakPoint;
	pISR_TIMER0= (unsigned) BreakPoint;
	pISR_UERR01= (unsigned) BreakPoint;
	pISR_WDT= (unsigned) BreakPoint;
	pISR_BDMA1= (unsigned) BreakPoint;
	pISR_BDMA0= (unsigned) BreakPoint;
	pISR_ZDMA1= (unsigned) BreakPoint;
	pISR_ZDMA0= (unsigned) BreakPoint;
	pISR_TICK= (unsigned) BreakPoint;
	pISR_EINT4567= (unsigned) BreakPoint;
	pISR_EINT3= (unsigned) BreakPoint;
	pISR_EINT2= (unsigned) BreakPoint;
	pISR_EINT1= (unsigned) BreakPoint;
	pISR_EINT0= (unsigned) BreakPoint;
	
#ifdef _ucosII_44b0
    extern void TickHandler(void);     //timer interrupt handler routine
	pISR_TIMER0= (unsigned) TickHandler;   //handler setting
#endif

}//uHALr_InitInterrupts



//Initialize timer that is used OS.
void uHALr_InitTimers(void)
{
	//dead zone=0, pre0= 100
	rTCFG0= 0x00000064;
	//all interrupt, mux0= 1/4
	rTCFG1= 0x00000001;

	//set T0 count
	rTCNTB0= _CLOCK;

	//update T0
    rTCON= 0x00000002;

	//T0,auto reload and start
	rTCON= 0x00000009;
}//uHALr_InitTimers

/*** Called by ARMTargetStart() ***/
//Request the system timer.
//return value 1:success   0:fail
int uHALr_RequestSystemTimer(void *tick, const unsigned char *str)
{
	return 1;
}//uHALr_RequestSystemTimer


//Start system timer & enable the interrupt.
void uHALr_InstallSystemTimer(void)
{
	//Non-vectored,IRQ enable,FIQ disable 
    rINTCON=0x5;

	//Non maksed TIMER0
	//#######################SLT##############################
	rINTMSK=~( BIT_TIMER0 | BIT_GLOBAL);	//Default value=0x7ffffff
}//uHALr_InstallSystemTimer

//////////////////////////////////////////////////////////////////
//routines for debugging
//////////////////////////////////////////////////////////////////
void OutDebug(unsigned int num)
{
	uHALr_printf("\r\n***STACK***:%d\r\n", num);
}//OutDebug

void BreakPoint(void)
{
	char m=200;
	uHALr_printf("!!!Enter break point.");	

	while(m) {
		Led_Display(0xf);
		Delay(1000);
		Led_Display(0x0);
		Delay(1000);
		m--;
	}//while
}//BreakPoint


//extern 
int I_COUNT= 0;		//作为控制用
void DebugUNDEF(void)
{
	uHALr_printf("!!!Enter UNDEFINED. %d\r\n", I_COUNT);	
	BreakPoint();
}


void DebugSWI(void)
{
	uHALr_printf("!!!Enter SWI. %d\r\n", I_COUNT);	
	BreakPoint();
}

void DebugABORT(void)
{
	uHALr_printf("!!!Enter ABORT %d\r\n", I_COUNT);	
	BreakPoint();
}

void DebugFIQ(void)
{
	uHALr_printf("!!!Enter FIQ. %d\r\n", I_COUNT);	
	BreakPoint();
}


