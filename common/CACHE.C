#include <string.h>
#include "..\common\44b.h"
#include "..\common\44blib.h"

void MarchCMinus32(int address,int unit,unsigned int pattern);
void MarchCMinusM_10(int address,int unit,unsigned int pattern,unsigned int mask);
void _MarchSub1(unsigned int *pt,int unit,unsigned int pattern,int incdec);
void _MarchSub1_10(unsigned int *pt,int unit,unsigned int pattern,int incdec,unsigned int mask);
int marchError;

void FlushCache(void)			   
{					   
    int i,saveSyscfg;
    
    saveSyscfg=rSYSCFG;

    rSYSCFG=SYSCFG_0KB; 		      
    for(i=0x10002000;i<0x10004800;i+=16)    
    {					   
		*((int *)i)=0x0;		   
    }
				  
    rSYSCFG=saveSyscfg; 			    
}					   

int Test_Cache(void)
{
    int saveSyscfg;
    Uart_Printf("Cache(Internal RAM) Cell Test by March C-\n");
	
    saveSyscfg=rSYSCFG;
    marchError=0;

    //00->01->11<->10

    rSYSCFG=SYSCFG_0KB;  //WB_off,Cache_off,I_RAM 8KB,stall disable
    //set0=0x10000000~0x100007ff	      
    //set1=0x10000800~0x10000fff	      
    //set2=0x10001000~0x100017ff	      
    //set3=0x10001800~0x10001fff	      
    
    MarchCMinus32(0x10000000,511,0x00000000); 
    MarchCMinus32(0x10000000,511,0x0f0f0f0f);
    MarchCMinus32(0x10000000,511,0x33333333);
    MarchCMinus32(0x10000000,511,0x55555555);
    MarchCMinus32(0x10000000,511,0xaaaaaaaa);
    Uart_Printf("Set0 is tested!\n");

    MarchCMinus32(0x10000800,511,0x00000000);
    MarchCMinus32(0x10000800,511,0x0f0f0f0f);
    MarchCMinus32(0x10000800,511,0x33333333);
    MarchCMinus32(0x10000800,511,0x55555555);
    MarchCMinus32(0x10000800,511,0xaaaaaaaa);
    Uart_Printf("Set1 is tested!\n");
	
    MarchCMinus32(0x10001000,511,0x00000000);
    MarchCMinus32(0x10001000,511,0x0f0f0f0f);
    MarchCMinus32(0x10001000,511,0x33333333);
    MarchCMinus32(0x10001000,511,0x55555555);
    MarchCMinus32(0x10001000,511,0xaaaaaaaa);
    Uart_Printf("Set2 is tested!\n");

    MarchCMinus32(0x10001800,511,0x00000000);
    MarchCMinus32(0x10001800,511,0x0f0f0f0f);
    MarchCMinus32(0x10001800,511,0x33333333);
    MarchCMinus32(0x10001800,511,0x55555555);
    MarchCMinus32(0x10001800,511,0xaaaaaaaa);
    Uart_Printf("Set3 is tested!\n");
	
    MarchCMinusM_10(0x10002000,127,0x00000000,0x1ffff);
    MarchCMinusM_10(0x10002000,127,0x0000ffff,0x1ffff);
    MarchCMinusM_10(0x10002000,127,0x00ff00ff,0x1ffff);
    MarchCMinusM_10(0x10002000,127,0x0f0f0f0f,0x1ffff);
    MarchCMinusM_10(0x10002000,127,0x33333333,0x1ffff);
    MarchCMinusM_10(0x10002000,127,0x55555555,0x1ffff);
    Uart_Printf("TagRAM0 is tested!\n");

    MarchCMinusM_10(0x10002800,127,0x00000000,0x1ffff);
    MarchCMinusM_10(0x10002800,127,0x0000ffff,0x1ffff);
    MarchCMinusM_10(0x10002800,127,0x00ff00ff,0x1ffff);
    MarchCMinusM_10(0x10002800,127,0x0f0f0f0f,0x1ffff);
    MarchCMinusM_10(0x10002800,127,0x33333333,0x1ffff);
    MarchCMinusM_10(0x10002800,127,0x55555555,0x1ffff);
    Uart_Printf("TagRAM1 is tested!\n");

    MarchCMinusM_10(0x10003000,127,0x00000000,0x1ffff);
    MarchCMinusM_10(0x10003000,127,0x0000ffff,0x1ffff);
    MarchCMinusM_10(0x10003000,127,0x00ff00ff,0x1ffff);
    MarchCMinusM_10(0x10003000,127,0x0f0f0f0f,0x1ffff);
    MarchCMinusM_10(0x10003000,127,0x33333333,0x1ffff);
    MarchCMinusM_10(0x10003000,127,0x55555555,0x1ffff);
    Uart_Printf("TagRAM2 is tested!\n");

    MarchCMinusM_10(0x10003800,127,0x00000000,0x1ffff);
    MarchCMinusM_10(0x10003800,127,0x0000ffff,0x1ffff);
    MarchCMinusM_10(0x10003800,127,0x00ff00ff,0x1ffff);
    MarchCMinusM_10(0x10003800,127,0x0f0f0f0f,0x1ffff);
    MarchCMinusM_10(0x10003800,127,0x33333333,0x1ffff);
    MarchCMinusM_10(0x10003800,127,0x55555555,0x1ffff);
    Uart_Printf("TagRAM3 is tested!\n");

    MarchCMinusM_10(0x10004000,127,0x00000000,0xf);	   
    MarchCMinusM_10(0x10004000,127,0x0000ffff,0xf);
    MarchCMinusM_10(0x10004000,127,0x00ff00ff,0xf);
    MarchCMinusM_10(0x10004000,127,0x0f0f0f0f,0xf);
    MarchCMinusM_10(0x10004000,127,0x33333333,0xf);
    MarchCMinusM_10(0x10004000,127,0x55555555,0xf);
    Uart_Printf("LRU is tested!\n");	    

    Uart_Printf("4KB internal SRAM test.\n");

    rSYSCFG=SYSCFG_4KB;
    MarchCMinus32(0x10001000,511,0x00000000);
    MarchCMinus32(0x10001000,511,0x0f0f0f0f);
    MarchCMinus32(0x10001000,511,0x33333333);
    MarchCMinus32(0x10001000,511,0x55555555);
    MarchCMinus32(0x10001000,511,0xaaaaaaaa);
    Uart_Printf("Set2 is tested at 4KB!\n");

    MarchCMinus32(0x10001800,511,0x00000000);
    MarchCMinus32(0x10001800,511,0x0f0f0f0f);
    MarchCMinus32(0x10001800,511,0x33333333);
    MarchCMinus32(0x10001800,511,0x55555555);
    MarchCMinus32(0x10001800,511,0xaaaaaaaa);
    Uart_Printf("Set3 is tested at 4KB!\n");

    rSYSCFG=saveSyscfg;     //wr_on,Cache 8KB+I_RAM 0KB,stall disable.

    //cache flush is needed....

    if(marchError==0)
		return 1; 
	else 
		return 0;
}

void MarchCMinus32(int address,int unit,unsigned int pattern)
{
    int i;
    unsigned int *tempPt;
    
    tempPt=(unsigned int *)address;
    
    for(i=0;i<=unit;i++)
    {
	//Uart_Printf("P=%x",pattern);
		*tempPt++=pattern;
	//Uart_Printf("A=%x,P=%x\n",tempPt,*tempPt);
    }
    
    _MarchSub1((unsigned int *)address,unit,~pattern,1);
    _MarchSub1((unsigned int *)address,unit,pattern,1);
    _MarchSub1((unsigned int *)address+unit,unit,~pattern,-1);
    _MarchSub1((unsigned int *)address+unit,unit,pattern,-1); 
    
    tempPt=(unsigned int *)address;
    for(i=0;i<=unit;i++)
    {
	if(*tempPt!=pattern)
	{
	    marchError=1;
	    Uart_Printf("ERROR0:%x\n",tempPt);
	}
	tempPt++;
    }
    //Uart_Printf("RP=%x,RA=%x\n",*tempPt,tempPt);
}

void MarchCMinusM_10(int address,int unit,unsigned int pattern,unsigned int mask)
{
    int i;
    unsigned int *tempPt;
    
    tempPt=(unsigned int *)address;
    
    for(i=0;i<=unit;i++)
    {
	//Uart_Printf("P=%x",pattern);
	//    *tempPt++=pattern;
	//Uart_Printf("A=%x,P=%x\n",tempPt,*tempPt);
	*tempPt=pattern;
	tempPt+=0x4;
    }
    
    _MarchSub1_10((unsigned int *)address,unit,~pattern,4,mask);
    _MarchSub1_10((unsigned int *)address,unit,pattern,4,mask);
    _MarchSub1_10((unsigned int *)address+(unit*4),unit,~pattern,-4,mask);
    _MarchSub1_10((unsigned int *)address+(unit*4),unit,pattern,-4,mask); 
    
    tempPt=(unsigned int *)address;
    for(i=0;i<=unit;i++)
    {
    
	if( *tempPt!=(pattern&mask) )
	{
	    marchError=1;
	    Uart_Printf("ERROR0:%x\n",tempPt);
	}
	tempPt+=0x4;
    }
    //Uart_Printf("RP=%x,RA=%x\n",*tempPt,tempPt);
}

void _MarchSub1(unsigned int *pt,int unit,unsigned int pattern,int incdec)
{
    unsigned int i,rp;

    for(i=0;i<=unit;i++)
    {
	rp=*pt;
	//Uart_Printf("RP=%x",*pt);
	*pt=pattern;
	//Uart_Printf("j=%x,p=%x,pt=%x\n",rp,(~pattern),pt);
	if(rp!=(~pattern))
	{
	    marchError=1;
	    Uart_Printf("ERROR1:A=%x,RP=%x,WP=%x \n",pt,rp,(~pattern));
	}
	pt+=incdec;
    }
}

void _MarchSub1_10(unsigned int *pt,int unit,unsigned int pattern,int incdec,unsigned int mask)
{
    unsigned int i,rp;
    unsigned int *pt2=pt;
    
    for(i=0;i<=unit;i++)
    {
	rp=*pt;
	//Uart_Printf("RP=%x",*pt);
	*pt=pattern;
	//Uart_Printf("j=%x,p=%x,pt=%x\n",rp,(~pattern),pt);
	
	if( rp!=(mask&(~pattern)) )
	{
	    marchError=1;
	    Uart_Printf("ERROR1:A=%x,RP=%x,WP=%x pt2=%x \n",pt,rp,(~pattern),pt2);
	}
	pt+=incdec;
    }
}

