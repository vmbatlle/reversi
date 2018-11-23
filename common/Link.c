/////////////////////////////////////////////////////////////////////////

extern char Image_RO_Limit[];
extern char Image_RO_Base[];
extern char Image_RW_Limit[];
extern char Image_RW_Base[];
extern char Image_ZI_Limit[];
extern char Image_ZI_Base[];

void _Link(void)
{
    Uart_Printf("\n Miscellaneous settings of the Linker Script file(*.ld):\n");
    Uart_Printf(" Image_RO_Base  = 0x%x\n",Image_RO_Base);
    Uart_Printf(" Image_RO_Limit = 0x%x\n",Image_RO_Limit);
    Uart_Printf("       ROM code Total is used: %dK Bytes\n",\
               (Image_RO_Limit-Image_RO_Base)/1024);
    Uart_Printf(" Image_RW_Base  = 0x%x\n",Image_RW_Base);
    Uart_Printf(" Image_RW_Limit = 0x%x\n",Image_RW_Limit);
    Uart_Printf(" Image_ZI_Base  = 0x%x\n",Image_ZI_Base);
    Uart_Printf(" Image_ZI_Limit = 0x%x\n",Image_ZI_Limit);
    Uart_Printf("       RAM Total is used: %dK Bytes\n",\
               (Image_RW_Limit-Image_RW_Base)/1024+(Image_ZI_Limit-Image_ZI_Base)/1024);
}
