/*********************************************************************************************
* File£º	light.c
* Author:	embest	
* Desc£º	control board's two LEDs on or offf
* History:	
*********************************************************************************************/

/*--- include files ---*/
#include "44b.h"
#include "44blib.h"

/*--- global variables ---*/
int led_state;							/* LED status	*/

/*--- function declare ---*/
void Led_Test();						/* LED test		*/
void leds_on();							/* all leds on  */
void leds_off();						/* all leds off */
void led1_on();							/* led 1 on 	*/
void led1_off();						/* led 1 off 	*/
void led2_on();							/* led 2 on 	*/
void led2_off();						/* led 2 off 	*/
//void Led_Display(int LedStatus);		/* led control 	*/

/*--- function code---*/
/*********************************************************************************************
* name:		Led_Test
* func:		leds test funciton
* para:		none
* ret:		none
* modify:
* comment:		
*********************************************************************************************/
void Led_Test()
{
	/* 1 on -> 2 on -> all on -> 2 off -> 1 off */
	leds_off();
	Delay(1000);
	led1_on();
	Delay(1000);
	led1_off();
	led2_on();
	Delay(1000);
	leds_on();
	Delay(1000);
	led2_off();
	Delay(1000);
	led1_off();
}

/*********************************************************************************************
* name:		leds_on
* func:		all leds on
* para:		none
* ret:		none
* modify:
* comment:		
*********************************************************************************************/
void leds_on()
{
	Led_Display(0x3);
}

/*********************************************************************************************
* name:		leds_off
* func:		all leds off
* para:		none
* ret:		none
* modify:
* comment:		
*********************************************************************************************/
void leds_off()
{
	Led_Display(0x0);
}

/*********************************************************************************************
* name:		led1_on
* func:		led 1 on
* para:		none
* ret:		none
* modify:
* comment:		
*********************************************************************************************/
void led1_on()
{
	led_state = led_state | 0x1;
	Led_Display(led_state);
}

/*********************************************************************************************
* name:		led1_off
* func:		led 1 off
* para:		none
* ret:		none
* modify:
* comment:		
*********************************************************************************************/
void led1_off()
{
	led_state = led_state & 0xfe;
	Led_Display(led_state);
}

/*********************************************************************************************
* name:		led2_on
* func:		led 2 on
* para:		none
* ret:		none
* modify:
* comment:		
*********************************************************************************************/
void led2_on()
{
	led_state = led_state | 0x2;
	Led_Display(led_state);
}

/*********************************************************************************************
* name:		led2_off
* func:		led 2 off
* para:		none
* ret:		none
* modify:
* comment:		
*********************************************************************************************/
void led2_off()
{
	led_state = led_state & 0xfd;
	Led_Display(led_state);
}

#define _LIB_LED_off // _LIB_LED_off -- don't use LIB settings.
#ifndef _LIB_LED_off
/*********************************************************************************************
* name:		Led_Display
* func:		Led Display control function
* para:		LedStatus -- led's status
* ret:		none
* modify:
* comment:		
*********************************************************************************************/
void Led_Display(int LedStatus)
{
	led_state = LedStatus;
	
	if((LedStatus&0x01)==0x01)
		rPDATB=rPDATB&0x5ff;
	else
		rPDATB=rPDATB|0x200;
	
	if((LedStatus&0x02)==0x02)
		rPDATB=rPDATB&0x3ff;
	else
		rPDATB=rPDATB|0x400;
}
#endif