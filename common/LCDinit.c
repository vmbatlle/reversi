void LcdInit_4Gray160x240(void)
{
	if((U32)frameBuffer1==0)
	{
	    frameBuffer1=(unsigned int (*)[SCR_XSIZE/32])malloc(ARRAY_SIZE_MONO); 
	}
	rLCDCON1=(0)|(1<<5)|(MVAL_USED<<7)|(0x3<<8)|(0x3<<10)|(CLKVAL_MONO<<12);
	    // disable,4B_SNGL_SCAN,WDLY=8clk,WLH=8clk,
	rLCDCON2=(LINEVAL)|(HOZVAL<<10)|(10<<21);  
	    //LINEBLANK=10 (without any calculation) 
	rLCDSADDR1= (0x0<<27) | ( ((U32)frameBuffer1>>22)<<21 ) | M5D((U32)frameBuffer1>>1);
	    // monochrome, LCDBANK, LCDBASEU
	rLCDSADDR2= M5D( (((U32)frameBuffer1+(SCR_XSIZE*LCD_YSIZE/8))>>1) ) | (MVAL<<21);
	rLCDSADDR3= (LCD_XSIZE/16) | ( ((SCR_XSIZE-LCD_XSIZE)/16)<<9 );
	rLCDCON1=(1)|(1<<5)|(MVAL_USED<<7)|(0x3<<8)|(0x3<<10)|(CLKVAL_MONO<<12);
	    // enable,4B_SNGL_SCAN,WDLY=8clk,WLH=8clk,
}
