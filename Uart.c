#include <string.h>
#include "..\common\44b.h"
#include "..\common\44blib.h"
#include "..\common\def.h"
#include "..\common\rtc.h"
#include "Uart.h"

#define KEY_BUFLEN 100
#define AFC_BUFLEN 0x100
#define IrDA_BUFLEN 0x100

char Uart_IntGetkey(void);
void Uart_Port(void);
void Return_Port(void);
void IrDA_Port(void);

void Uart0_TxFifoInt(void) __attribute__ ((interrupt ("IRQ")));
void Uart0_RxFifoInt(void) __attribute__ ((interrupt ("IRQ")));
void Uart0_RxFifoErrorInt(void) __attribute__ ((interrupt ("IRQ")));

void Uart0_RxInt(void) __attribute__ ((interrupt ("IRQ")));
void Uart0_TxInt(void) __attribute__ ((interrupt ("IRQ")));

void Uart1_TxFifoInt(void) __attribute__ ((interrupt ("IRQ")));
void Uart1_RxFifoInt(void) __attribute__ ((interrupt ("IRQ")));
void Uart1_RxFifoErrorInt(void) __attribute__ ((interrupt ("IRQ")));

void Uart1_RxInt(void) __attribute__ ((interrupt ("IRQ")));
void Uart1_TxInt(void) __attribute__ ((interrupt ("IRQ")));

void U1AFC_TxInt(void) __attribute__ ((interrupt ("IRQ")));
void U0AFC_RxInt(void) __attribute__ ((interrupt ("IRQ")));
void U0AFC_RxErrorInt(void) __attribute__ ((interrupt ("IRQ")));

void Exint2(void) __attribute__ ((interrupt ("IRQ")));
void Test_Done(void) __attribute__ ((interrupt ("IRQ")));
void Error(void) __attribute__ ((interrupt ("IRQ")));

void U1IrDA_TxInt(void) __attribute__ ((interrupt ("IRQ")));
void U1IrDA_RxInt(void) __attribute__ ((interrupt ("IRQ")));
void U1IrDA_RxErrorInt(void) __attribute__ ((interrupt ("IRQ")));

static unsigned char keyBuf[KEY_BUFLEN];
volatile static int IrDA_BAUD,keyBufRdPt=0;
volatile static int keyBufWrPt=0;
volatile char out=1;
static char *uart0TxStr;
static char *uart1TxStr;

volatile U32 save_UC,save_UE,save_UF,save_UPC,save_UPE,save_UPF;



char Uart_IntGetkey(void)
{
    if(keyBufRdPt==KEY_BUFLEN)
	keyBufRdPt=0;

    while(keyBufWrPt==keyBufRdPt);  //until FIFO is triggered
    return keyBuf[keyBufRdPt++];
}

////////UART 0 TEST////////////////

void Test_Uart0Fifo(void)
{
    int key;

    keyBufRdPt=keyBufWrPt=0;
    pISR_UTXD0=(unsigned)Uart0_TxFifoInt;
    pISR_URXD0=(unsigned)Uart0_RxFifoInt;
    pISR_UERR01=(unsigned)Uart0_RxFifoErrorInt;

    /*********** UART0 Tx FIFO test with interrupt ***********/     
    Uart_Printf("[Uart channel 0 tx FIFO Interrupt Test]\n");
    Uart_TxEmpty(0);	//wait until tx shifter is empty.

    uart0TxStr="123456789abcdef";//UART0 Tx FIFO interrupt test is good!!!!\r\n";
    rUFCON0=(2<<6)|(1<<4)|(6)|1;
    //FIFO trigger:tx/rx:8byte,tx/rx_fifo reset(will be cleared),FIFO enable.
    rUCON0 = 0x244;	//tx:levl,rx:edge,error int,normal*2,interrupt(Start)
    rINTMSK=~(BIT_GLOBAL|BIT_UTXD0);

    Delay(500);
    /*********** UART0 Tx FIFO test with BDMA0 ***********/ 
    Uart_Init(0,115200);
    Uart_Printf("\n[Uart0 FIFO Tx Test by BDMA0]\n");
    uart0TxStr="UART0 Tx FIFO Test by BDMA0 is good!!!!\r\n";
    Uart_TxEmpty(0);

    rUCON0=0x48;	//tx:BDMA0 rx:disable

    rBDICNT0=0;
    rBDCON0 =0x0;
    rBDISRC0=(unsigned int)uart0TxStr|(0<<30)|(1<<28);  // byte,inc
    rBDIDES0=UTXH0 |(1<<30)|(3<<28);  //L/B endian,M2IO,fix
    rBDICNT0=strlen(uart0TxStr)|(2<<30)|(1<<26)|(1<<20); //UART0,start,polling

    while(!((rBDCON0&0x30)==0x20));        
    Uart_TxEmpty(0);
    
    /*********** UART0 Rx FIFO test with interrupt ***********/ 
    rUCON0=0x245|0x80;	//tx:level,rx:edge,tx/rx:int,rcv_time_out enabled,error int enable

    Uart_Printf("\n[Uart channel 0 FIFO Rx Interrupt Test]:Type any key!!!\n");
    Uart_Printf("You have to see the typed character. To quit, press Enter key.\n");

    rINTMSK=~(BIT_GLOBAL|BIT_URXD0|BIT_UERR01);

    while( (rUFSTAT0&0xf) >0 )
        key=RdURXH0();  //To clear the Rx FIFO
   // rUERSTAT0;	        //To clear the error state

    while((key=Uart_IntGetkey())!='\r')
        Uart_SendByte(key);

    rUFCON0=(2<<6)|(1<<4)|(6)|0;  
    //FIFO trigger:tx/rx:8byte, txrx_fifo reset(will be cleared), FIFO disable.

    rINTMSK=~BIT_GLOBAL;
    rUCON0=0x45;	//rcv_time_out disabled
    Uart_Printf("\n");
    }


void Uart0_TxFifoInt(void)
{
//    rI_ISPC=BIT_UTXD0;        
    int i;
//    while( !(rUFSTAT0 & 0x200) && (*uart0TxStr != '\0') )   //until tx fifo full or end of string
    while( !(rUFSTAT0 == 16) && (*uart0TxStr != '\0') )   //until tx fifo full or end of string
    {
	rUTXH0=*uart0TxStr++;
	for(i=0;i<700;i++); //to avoid overwriting FIFO
    }

    rI_ISPC=BIT_UTXD0;
    if(*uart0TxStr == '\0')
    {
	rUCON0 &= 0x3f3;
	rI_ISPC=BIT_UTXD0;
	rINTMSK|=BIT_UTXD0;
    }
}


void Uart0_RxFifoInt(void)
{
    rI_ISPC=BIT_URXD0;
//  if(rUFSTAT0==0)
//	Uart_Printf("time out\n");
    while( (rUFSTAT0&0xf) >0 )	    //until FIFO is empty
    {
	keyBuf[keyBufWrPt++]=rURXH0;//rx buffer->keyBuf[]
	if(keyBufWrPt==KEY_BUFLEN)
	    keyBufWrPt=0;
    }
}


void Uart0_RxFifoErrorInt(void)
{
    rI_ISPC=BIT_UERR01;

    switch(rUERSTAT0)//to clear and check the status of register bits
    {
	case '1':
	    Uart_Printf("Overrun error\n");
	    break;
	case '2':
	    Uart_Printf("Parity error\n");
	    break;
	case '4':
	    Uart_Printf("Frame error\n");
	    break;
	case '8':
	    Uart_Printf("Breake detect\n");
	    break;
	default :
	    break;
    }
    while( (rUFSTAT0&0xf) >0 )
    {
	    keyBuf[keyBufWrPt++]=rURXH0;
	    if(keyBufWrPt==KEY_BUFLEN)
		    keyBufWrPt=0;
    }
}


void Test_Uart0(void)
{
    int key;
    keyBufRdPt=keyBufWrPt=0;
    pISR_UTXD0=(unsigned)Uart0_TxInt;
    pISR_URXD0=(unsigned)Uart0_RxInt;
    
    /*********** UART0 Tx test with interrupt ***********/  
    Uart_Printf("[Uart channel 0 tx Interrupt Test]\n");
    Uart_TxEmpty(0); //wait until tx shifter is empty.

    uart0TxStr="UART0 Tx interrupt test is good!!!!\r\n";

    rINTMSK=~(BIT_GLOBAL|BIT_UTXD0);
//	rUCON0 &= 0x3f3;   
//	rUCON0 |= 0x4;  //needed to set the UTXD0 pending bit.
    rUCON0 = 0x244; //tx:level,rx:edge,error int,normal*2,interrupt(Start)
    Delay(3000);

    /*********** UART0 Tx test with BDMA0 ***********/ 
    rUCON0 = 0x245;//workaround

    Uart_Printf("\n[Uart0 Tx Test by BDMA0]\n");
    uart0TxStr="UART0 Tx Test by BDMA0 is good!!!!\r\n";
    Uart_TxEmpty(0);

    rUCON0=0x48;    //tx:BDMA0 rx:disable

    rBDICNT0=0x0;
    rBDCON0 =0x0;
    rBDISRC0=(unsigned int)uart0TxStr|(0<<30)|(1<<28);  // byte,inc
    rBDIDES0=UTXH0 |(1<<30)|(3<<28);	//L/B endian,M2IO,fix   
    rBDICNT0=strlen(uart0TxStr)|(2<<30)|(1<<26)|(1<<20); //UART0,

    while(!((rBDCON0&0x30)==0x20));
    Uart_TxEmpty(0);

    /*********** UART0 Rx test with interrupt ***********/
    rUCON0=0x45;    //tx:int rx:int

    Uart_Printf("\n[Uart channel 0 Rx Interrupt Test]:Type any key!!!\n");
    Uart_Printf("You will see the typed character. To quit, press Enter key.\n");
    Uart_TxEmpty(0);

    rINTMSK=~(BIT_GLOBAL|BIT_URXD0);

    keyBufRdPt=keyBufWrPt=0;
    while((key=Uart_IntGetkey())!='\r')
	    Uart_SendByte(key);

    rINTMSK=~BIT_GLOBAL;
    Uart_Printf("\n");
    }


void Uart0_RxInt(void)
{
    rI_ISPC=BIT_URXD0;

    keyBuf[keyBufWrPt++]=RdURXH0();
    if(keyBufWrPt==KEY_BUFLEN)
		keyBufWrPt=0;
}

//unsigned int txcount=0;
void Uart0_TxInt(void)
{
//    rI_ISPC=BIT_UTXD0;
//	txcount++;
    if(*uart0TxStr != '\0')
    {
	WrUTXH0(*uart0TxStr++);
	rI_ISPC=BIT_UTXD0;
    }
    else
    {
	rUCON0 &= 0x3f3;//workaround
	rI_ISPC=BIT_UTXD0;
	rINTMSK|=BIT_UTXD0;
    }
}

////////UART 1 TEST////////////////

void Test_Uart1Fifo(void)
{
    int key;
    keyBufRdPt=keyBufWrPt=0;
    pISR_UTXD1=(unsigned)Uart1_TxFifoInt;
    pISR_URXD1=(unsigned)Uart1_RxFifoInt;
    pISR_UERR01=(unsigned)Uart1_RxFifoErrorInt;

    /*********** UART1 Tx FIFO test with interrupt ***********/     
    Uart_Printf("[Uart channel 1 tx FIFO Interrupt Test]\n");
    Uart_Printf("Plug the serial cable into ch1 connector!!! \n");
    Uart_Printf("Then, press any key through UART ch1.\n");
    Uart_Select(1);
    Uart_Getch();

    uart1TxStr="UART1 Tx FIFO interrupt test is good!!\r\n";
    rUFCON1=(2<<6)|(1<<4)|(6)|1;
    //FIFO trigger:tx/rx:8byte,tx/rx_fifo reset(will be cleared),FIFO enable.
    rUCON1 = 0x244;             //rx:edge,tx:level,error int,normal*2,interrupt(Start)    
    rINTMSK=~(BIT_GLOBAL|BIT_UTXD1);

    Delay(500);
    /*********** UART1 Tx FIFO test with BDMA1 ***********/ 
    Uart_Init(0,115200);
    Uart_Printf("\n[Uart1 FIFO Tx Test by BDMA1]\n");
    uart1TxStr="UART1 Tx Test by BDMA0 is good!!!!\r\n";
    Uart_TxEmpty(1);

    rUCON1=0x4c;	//tx:BDMA1 rx:disable

    rBDICNT1=0;
    rBDCON1 =0x0;
    rBDISRC1=(unsigned int)uart1TxStr|(0<<30)|(1<<28);  // byte,inc
    rBDIDES1=UTXH1 |(1<<30)|(3<<28);  //L/B endian,M2IO,fix
    rBDICNT1=strlen(uart1TxStr)|(2<<30)|(1<<26)|(1<<20); //UART1,start,polling

    while((rBDCON1&0x30)==0x10);        
    Uart_TxEmpty(1);
    
    /*********** UART1 Rx FIFO test with interrupt ***********/ 
    rUCON1=0x245|0x80;	//rx:edge,tx:level,tx/rx:int,rcv_time_out enabled,error int enable

    Uart_Printf("\n[Uart channel 1 FIFO Rx Interrupt Test]:Type any key!!!\n");
    Uart_Printf("You have to see the typed character. To quit, press Enter key.\n");

    rINTMSK=~(BIT_GLOBAL|BIT_URXD1|BIT_UERR01);
    keyBufRdPt=keyBufWrPt=0;
    while( (rUFSTAT1&0xf) >0 )
	key=RdURXH1();  //To clear the Rx FIFO
 //  	 rUERSTAT1;          //To clear the error state

    while((key=Uart_IntGetkey())!='\r')
	Uart_SendByte(key);

    rUFCON1=(2<<6)|(1<<4)|(6)|0;  
    //FIFO trigger:tx/rx:8byte, txrx_fifo reset(will be cleared), FIFO disable.

    rINTMSK=~BIT_GLOBAL;
    rUCON1=0x45;	//rcv_time_out disabled
    Uart_Printf("\n");

    Uart_Printf("Plug the serial cable into ch0 as before this test!!!\n");
    Uart_Printf("Then, press any key through UART ch 0.\n");
    Uart_Select(0);
    Uart_Getch();
    }


void Uart1_TxFifoInt(void)
{
    int i;
    while( !(rUFSTAT1 == 16) && (*uart1TxStr != '\0') )   //until tx fifo full or end of string
    {
	rUTXH1=*uart1TxStr++;
	for(i=0;i<700;i++); //to avoid overwriting FIFO
    }

    rI_ISPC=BIT_UTXD1;
    if(*uart1TxStr == '\0')
    {
	rUCON1 &= 0x3f3;
	rI_ISPC=BIT_UTXD1;
	rINTMSK|=BIT_UTXD1;
    }
}


void Uart1_RxFifoInt(void)
{
    rI_ISPC=BIT_URXD1;
//  if(rUFSTAT1==0)
//	Uart_Printf("time out\n");
    while( (rUFSTAT1&0xf) >0 )	    //until FIFO is empty
    {
	keyBuf[keyBufWrPt++]=rURXH1;//rx buffer->keyBuf[]
	if(keyBufWrPt==KEY_BUFLEN)
	    keyBufWrPt=1;
    }
}


void Uart1_RxFifoErrorInt(void)
{
    rI_ISPC=BIT_UERR01;

    switch(rUERSTAT1)//to clear and check the status of register bits
    {
	case '1':
	    Uart_Printf("Overrun error\n");
	    break;
	case '2':
	    Uart_Printf("Parity error\n");
	    break;
	case '4':
	    Uart_Printf("Frame error\n");
	    break;
	case '8':
	    Uart_Printf("Breake detect\n");
	    break;
	default :
	    break;
    }
    while( (rUFSTAT1&0xf) >0 )
    {
	keyBuf[keyBufWrPt++]=rURXH1;
	if(keyBufWrPt==KEY_BUFLEN)
	    keyBufWrPt=0;
    }
}


void Test_Uart1(void)
{
    int key;
    keyBufRdPt=keyBufWrPt=0;
    pISR_UTXD1=(unsigned)Uart1_TxInt;
    pISR_URXD1=(unsigned)Uart1_RxInt;
   
    /*********** UART1 Tx test with interrupt ***********/  
    Uart_Printf("[Uart channel 1 tx Interrupt Test]\n");
    Uart_Printf("Plug the serial cable into ch1 connector!!! \n");
    Uart_Printf("Then, press any key through UART ch1.\n");
    Uart_Select(1);
    Uart_Getch();

    uart1TxStr="UART1 Tx interrupt test is good!!!!\r\n";
    rINTMSK=~(BIT_GLOBAL|BIT_UTXD1);
//  rUCON1 &= 0x3f3;   
//  rUCON1 |= 0x4;  //needed to set the UTXD0 pending bit.
    rUCON1 = 0x244; //rx:edge,tx:level,error int,normal*2,interrupt(Start)
    Delay(3000);

    /*********** UART1 Tx test with BDMA1 ***********/
    rUCON1 = 0x245;

    Uart_Printf("\n[Uart1 Tx Test by BDMA1]\n");
    uart1TxStr="UART1 Tx Test by BDMA1 is good!!!!\r\n";
    Uart_TxEmpty(1);

    rUCON1=0x4c;    //tx:BDMA0 rx:disable

    rBDICNT1=0x0;
    rBDCON1 =0x0;
    rBDISRC1=(unsigned int)uart1TxStr|(0<<30)|(1<<28);  // byte,inc
    rBDIDES1=UTXH1 |(1<<30)|(3<<28);  //L/B endian,M2IO,fix   
    rBDICNT1=strlen(uart1TxStr)|(1<<31)|(1<<26)|(1<<20); //UART1,

    while(!((rBDCON1&0x30)==0x20));
    Uart_TxEmpty(1);

    /*********** UART1 Rx test with interrupt ***********/
    rUCON1=0x45;    //tx:int rx:int
    Uart_Printf("\n[Uart channel 1 Rx Interrupt Test]:Type any key!!!\n");
    Uart_Printf("You have to see the typed character. To quit, press Enter key.\n");

    rINTMSK=~(BIT_GLOBAL|BIT_URXD1);

    keyBufRdPt=keyBufWrPt=0;    
    while((key=Uart_IntGetkey())!='\r')
        Uart_SendByte(key);

    rINTMSK=~BIT_GLOBAL;
    Uart_Printf("\n");

    Uart_Printf("Plug the serial cable into ch0 as before this test!!!\n");
    Uart_Printf("Then, press any key through UART ch 0.\n");
    Uart_Select(0);
    Uart_Getch();
   
}


void Uart1_RxInt(void)
{
    rI_ISPC=BIT_URXD1;

    keyBuf[keyBufWrPt++]=RdURXH1();
    if(keyBufWrPt==KEY_BUFLEN)
	keyBufWrPt=0;
}


void Uart1_TxInt(void)
{
//    rI_ISPC=BIT_UTXD1;

    if(*uart1TxStr != '\0')
    {
	WrUTXH1(*uart1TxStr++);
	rI_ISPC=BIT_UTXD1;
    }
    else
    {
	rUCON1 &= 0x3f3;//workaround
	rI_ISPC=BIT_UTXD1;
	rINTMSK|=BIT_UTXD1;
    }
}

////////Auto Flow Control TEST(Tx)////////////////

volatile unsigned char * volatile tx0,* tx1,*tx2,tx_end=0;
volatile int i;
volatile int tx_cnt=0;
void Test_UartAFC_Tx(void)
{
    tx_cnt=0;

    tx0=(unsigned char *)malloc(AFC_BUFLEN);
    tx1=tx0;
    tx2=tx0;
    Uart_Printf("!!!tx0=0x%x\n",tx0);

    for(i=0;i<AFC_BUFLEN;i++)
	*tx1++=i;

    pISR_UTXD1=(unsigned)U1AFC_TxInt;

    
    Uart_Printf("[UART AFC Tx Test]\n");
    Uart_Printf("This test should be configured two boards.\n");
    Uart_Printf("Connect twisted(rx/tx, nCTS/nRTS) cable com2 to other board's com1 port.\n");
    Uart_Printf("Start Rx first and press any key and,.\n");
    Uart_TxEmpty(0);
    Uart_Getch();

    rUCON1=(1<<9)|(0<<8)|(0<<7)|(0<<6)|(0<<5)|(0<<4)|(1<<2)|(1);
    rUFCON1=0x0;    //FIFO disable
    rUMCON1=0x10;   //Uart1 AFC enable

    Uart_Printf("Now... Tx with AFC\n");
    Uart_TxEmpty(0);
    rINTMSK=~(BIT_GLOBAL|BIT_UTXD1);

    while(!tx_end);

    Uart_Printf("\nEnd Tx, transfer data count=%d\n",tx_cnt);

    free((void *)tx0);
    tx_end=0;
    tx_cnt=0;
}

void U1AFC_TxInt(void)
{
    if(tx_cnt < (AFC_BUFLEN))
    {
	Uart_Printf("%d,",*tx2);
	WrUTXH1(*tx2++);
	rI_ISPC=BIT_UTXD1;
	tx_cnt++;
    }
    else
    {
	Uart_TxEmpty(1);
	rUCON1 &= 0x3f3;//workaround
	rI_ISPC=BIT_UTXD1;
	rINTMSK|=BIT_UTXD1;
	tx_end=1;
    }
}


////////Auto Flow Control TEST(Rx with FIFO)////////////////

volatile unsigned char *rx0,*rx1,*rx2,rx_end=0;
volatile int rx_cnt=0;

void Test_UartAFC_Rx(void)
{
    unsigned int i;
    unsigned int err_cnt=0;


    Uart_Printf("[UART AFC Rx Test]\n");
    Uart_Printf("This test should be configured two boards.\n");
    Uart_Printf("Connect twisted(rx/tx, nCTS/nRTS) cable com1 to other board's com2 port.\n");
    Uart_Printf("Then, change connected cable between host and com1, host to com2.\n");
    Uart_Printf("Press any key to com2 and start Rx first.\n");
    Uart_TxEmpty(0);

    Uart_Select(1);
    Uart_Getch();
    
    rx0=(unsigned char *)malloc(AFC_BUFLEN);
    rx1=rx0;
    rx2=rx0;

    pISR_URXD0=(unsigned)U0AFC_RxInt;
    pISR_UERR01=(unsigned)U0AFC_RxErrorInt;

    rx_end=0;

    rUMCON0=0x10;   //Uart0 AFC enable
    rUCON0=(1<<9)|(0<<8)|(1<<7)|(1<<6)|(0<<5)|(0<<4)|(1<<2)|(1);
    rUFCON0=(2<<6)|(1<<4)|(1<<2)|(1<<1)|(1);


    Uart_Printf("Now... Rx with AFC\n");
    Uart_TxEmpty(1);

    rINTMSK=~(BIT_GLOBAL|BIT_URXD0|BIT_UERR01);

    while(!rx_end);
    Delay(1000);

    rINTMSK|=BIT_GLOBAL;

    Uart_Printf("\nEnd Rx, receive data count=%d\n",rx_cnt);
    Uart_Printf("Now, change connected cable between host and com2, host to com1.\n");
    Uart_Printf("Then, press any key.\n");
    Uart_Init(0,115200);
    Uart_Select(0);
    Uart_Getch();

    for(i=0;i<AFC_BUFLEN;i++)
    {
	if(i-(*rx1++))
	{
	    Uart_Printf("i=%d\n",i);
	    err_cnt++;
	}
    }
    if(err_cnt)
	Uart_Printf("AFC test fail!! Error count=%d\n",err_cnt);
    else
	Uart_Printf("AFC test is good!!\n");

    free((void *)rx0);
    rx_end=0;
    rx_cnt=0;
}


void U0AFC_RxInt(void)
{
    rI_ISPC=BIT_URXD0;

    while( (rUFSTAT0 & 0x100) || ((rUFSTAT0 & 0xf) >0) )
    {
	Delay(1000);
	*rx2++=rURXH0;
	Uart_Printf("%d,",*(rx2-1));
	rx_cnt++;
    }
    if(rx_cnt == (AFC_BUFLEN))
        rx_end=1;
}


void U0AFC_RxErrorInt(void)
{
    rI_ISPC=BIT_UERR01;

    switch(rUERSTAT0)//to clear and check the status of register bits
    {
	case 1:
	    WrUTXH1('!');
	    break;
	case 2:
	    WrUTXH1('#');
	    break;
	case 4:
	    WrUTXH1('$');
	    break;
	case 8:
	    WrUTXH1('@');
	    break;
	default :
	    WrUTXH1('*');
	    break;
    }
}


char _done=0, error=0;
void Test_BDMA(void)
{
    char *_buf,i;
    char *_temp2;
    int *_temp;

    _buf=(char *)malloc(100);
    _temp=(int *)malloc(1);
    _temp2=_buf;


    rINTMSK=~(BIT_GLOBAL|BIT_BDMA0|BIT_UERR01);
    pISR_BDMA0=(unsigned)Test_Done;
    pISR_UERR01=(unsigned)Error;
    Uart_Init(0,115200);    
    Uart_Printf("[Read BDCON0 register in Rxing...]\n");
    Uart_TxEmpty(0);

    rBDISRC0=(0<<30)+(3<<28)+(int)URXH0;    //byte,inc,Rx-buf
    rBDIDES0=(1<<31)+(1<<28)+(int)_buf;	    //M2IO,fix,IISFIF
    rBDICNT0=(1<<31)+(1<<26)+(3<<22)+(1<<21)+(1<<20)+700;
    //Uart0,reserve,done_int,auto-reload/start,DMA enable,COUNT
    rBDCON0 = 0x0<<2;

    rUCON0=0x2c6;	//tx:polling rx:BDMA0    

    while(!_done)
    {
	*_temp=rBDCON0;
	if((rBDCON0 & 0xf))
	{
	    Uart_Printf("!!Error0x%x!!,",rBDCON0);
	    break;
	}
    }
    Uart_Printf("!END!\n");

    if(error)
	Uart_Printf("[rUERSTAT=0x%x]\n",rUERSTAT0);

    rINTMSK|=BIT_GLOBAL;
//    rUCON0 &= 0x3fd;//Rx disable
    rBDICNT0=0x0;   //BDMA stop

    for(i=0;i<10;i++)
	Uart_Printf("%d=0x%x,",i,*_temp2++);
    Uart_Printf("\n0x%x,",*_temp);

    free(_buf);
    _done=0;
}

void Test_Done(void)
{
    rI_ISPC=BIT_BDMA0;	//clear pending bit
    _done=1;
}

void Error(void)
{
    rI_ISPC=BIT_UERR01;
    error=1;
}


////////////IrDA////////////

void U1IrDA_TxInt(void)
{
    if(tx_cnt < (IrDA_BUFLEN))
    {
	Uart_Printf("%d,",*tx2);
	WrUTXH1(*tx2++);
	rI_ISPC=BIT_UTXD1;
	tx_cnt++;
    }
    else
    {
	Uart_TxEmpty(1);
	rUCON1 &= 0x3f3;//workaround
	rI_ISPC=BIT_UTXD1;
	rINTMSK|=BIT_UTXD1;
	tx_end=1;
    }
}


void U1IrDA_RxInt(void)
{
    rI_ISPC=BIT_URXD1;

    while( (rUFSTAT1 & 0x100) || ((rUFSTAT1 & 0xf) >0) )
    {
	*rx2=rURXH1;
	Uart_Printf("%d,",*rx2++);
	rx_cnt++;
    }
    if(rx_cnt == (IrDA_BUFLEN))
    {
    	rINTMSK|=BIT_URXD1;
        rx_end=1;
    }
}


void U1IrDA_RxErrorInt(void)
{
    rI_ISPC=BIT_UERR01;

    switch(rUERSTAT1)//to clear and check the status of register bits
    {
	case 1:
	    WrUTXH0('!');
	    break;
	case 2:
	    WrUTXH0('#');
	    break;
	case 4:
	    WrUTXH0('$');
	    break;
	case 8:
	    WrUTXH0('@');
	    break;
	default :
	    WrUTXH0('*');
	    break;
    }
}


void Test_Uart1IrDA_Tx(void)
{
    int i, j;
    tx_cnt=0;
    tx0=(unsigned char *)malloc(IrDA_BUFLEN);
    tx1=tx0;
    tx2=tx0;
    Uart_Printf("!!!tx0=0x%x\n",tx0);
    
    Uart_Select(0);
    
    for(i=0;i<IrDA_BUFLEN;i++)
	*tx1++=i;
        
    pISR_UTXD1=(unsigned)U1IrDA_TxInt;
    
    Uart_Printf("\nSelect the baud rate\n");
    Uart_Printf("1)9600    2)19200    3)38400    4)57600    5)115200\n"); 
    j=Uart_Getch();
    switch(j)
    {
	case '1':
            IrDA_BAUD=9600;
        break;
	
	case '2':
            IrDA_BAUD=19200;
        break;
	
	case '3':
            IrDA_BAUD=38400;
        break;
	
	case '4':
            IrDA_BAUD=57600;
        break;
	
	case '5':
            IrDA_BAUD=115200;
        break;
	
	default:
	break;
    }

    rUBRDIV1=( (int)(MCLK/16./IrDA_BAUD + 0.5) -1 );
    Uart_Printf("rUBRDIV1=%d\n", rUBRDIV1);

    rULCON1=(1<<6)|3;
    rUCON1=0;

    Uart_Printf("[UART IrDA Tx Test]\n");
    Uart_Printf("This test should be configured two boards.\n");
    Uart_Printf("Start Rx first and press any key and...\n");
    Uart_TxEmpty(0);
    Uart_Getch();

    
    rUCON1=(1<<9)|(1<<8)|(0<<7)|(0<<6)|(0<<5)|(0<<4)|(1<<2)|(1);
    //Tx&Rx:Level,Timeout off,Rx err off,Loop-back off,send break off,int,int
    rUFCON1=(1<<6)|(1<<4)|(1<<2)|(1<<1)|(0);	
    Uart_Printf("Now... Tx with IrDA\n");
    Uart_TxEmpty(0);
    rINTMSK=~(BIT_GLOBAL|BIT_UTXD1);

    while(!tx_end);
    Uart_Printf("\nEnd Tx, transfer data count=%d\n",tx_cnt);
    while(rUFSTAT1 & 0x2f0);
    Uart_TxEmpty(1);    
  
    Uart_Printf("\nTx is Done. Press any key to quit!!!\n");
    Uart_Getch();
    Uart_Init(0,115200);
 
}

void Test_Uart1IrDA_Rx(void)
{
    unsigned int i, j, err_cnt;
    rx2=(unsigned char *)malloc(IrDA_BUFLEN);
    rx1=rx2;   
    Uart_Select(0);
        
    pISR_URXD1=(unsigned)U1IrDA_RxInt;
    pISR_UERR01=(unsigned)U1IrDA_RxErrorInt;

    Uart_Printf("\nSelect the baud rate\n");
    Uart_Printf("1)9600    2)19200    3)38400    4)57600    5)115200\n"); 
    j=Uart_Getch();
    switch(j)
    {
	case '1':
            IrDA_BAUD=9600;
        break;
	
	case '2':
            IrDA_BAUD=19200;
        break;
	
	case '3':
            IrDA_BAUD=38400;
        break;
	
	case '4':
            IrDA_BAUD=57600;
        break;
	
	case '5':
            IrDA_BAUD=115200;
        break;
	
	default:
	break;
    }

    rUBRDIV1=( (int)(MCLK/16./IrDA_BAUD + 0.5) -1 );
    Uart_Printf("rUBRDIV1=%d\n", rUBRDIV1);
    
    rUCON1=0;
    rULCON1=(1<<6)|3;

    Uart_Printf("[UART IrDA Rx Test]\n");
    Uart_Printf("This test should be configured two boards.\n");
    Uart_Printf("Press any key to start Rx and then Start Tx....\n");
    Uart_Getch();

    rUCON1=(1<<9)|(1<<8)|(1<<7)|(1<<6)|(0<<5)|(0<<4)|(1<<2)|(1);
//    rUFCON1=0;	//FIFO Disable
    rUFCON1=(2<<6)|(1<<4)|(1<<2)|(1<<1)|(1);	//FIFO Enable
    rx_end=0;
    err_cnt=0;
    Uart_Printf("Now... Rx with IrDA\n");
    Uart_TxEmpty(0);
     rINTMSK=~(BIT_GLOBAL|BIT_URXD1|BIT_UERR01);

    while(!rx_end);
  
    Uart_Printf("\nEnd Rx, receive data count=%d\n",rx_cnt);
    
    for(i=0;i<IrDA_BUFLEN;i++)
    {

	if(i-(*rx1++))
	{
	    Uart_Printf("i=%d\n",i);
	    err_cnt++;
	}
    }
    if(err_cnt)
	Uart_Printf("IrDA test fail!! Error count=%d\n",err_cnt);
    else
	Uart_Printf("IrDA test is good!!\n");


    Uart_Init(1,115200);   
    Uart_Init(0,115200);

}
