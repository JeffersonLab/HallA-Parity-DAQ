/*****************************************
*  AUXTB.c                               *
*  routines used to interface the        * 
*  aux timing board for                  *
*  alternate DAC outputs                 *
*                                        *
*  K.D. Paschke                Jun 2004  *
b******************************************/

#include "AUXTB.h"

/*****************************************************
*
*  Initialize the AUX HAPPEX TIME BOARD 
*  which will be used for DAC outputs only.
******************************************************/
int initAUXTB()
{
  int res;
  unsigned long laddr;

  laddr = AUXTBADDR;
  did_init_AUXTB = 1;
  printf("AUX Timing Board address = 0x%x ",laddr);
  auxtboard = (struct vme_happex_auxtb *) laddr;
  auxtboard->dac12   = 0; 
  auxtboard->dac16   = 32767;
  auxtboard->rampdelay = 0;
  auxtboard->integtime = 0;
  auxtboard->osample_w = 0;
  LAST_DAC12_AUXTB    = 0;
  LAST_DAC16_AUXTB    = 0;
  printf("HAPPEX Auxilary TIME BOARD at %x se     :\n",AUXTBADDR);
  return 1;
}


/* Dump the contents of readable registers */
int dumpRegAUXTB()
{
  if ( !did_init_AUXTB ) return errorPrintAUXTB(1);
  printf("\nDump of Readable Registers on ADC Timing Board : \n");
  printf("Ramp Delay %d (dec)  = 0x%x (hex) \n",
         auxtboard->rampdelay,auxtboard->rampdelay);
  printf("Integration Time %d (dec)  = 0x%x (hex) \n",
         auxtboard->integtime,auxtboard->integtime);
  printf("Oversampling setting %d (dec)  = 0x%x (hex) \n",
         auxtboard->osample_w,auxtboard->osample_w);
  printf("Current Oversample %d (dec)  = 0x%x (hex) \n",
         auxtboard->osample_c,auxtboard->osample_c);
  printf("DAC12 setting %d (dec)  = 0x%x (hex) \n",
    auxtboard->dac12,auxtboard->dac12); 
  printf("DAC16 settings %d (dec)  = 0x%x (hex) \n",
    auxtboard->dac16,auxtboard->dac16);
  printf("Last DAC12 %d (dec) \n",LAST_DAC12_AUXTB);
  printf("Last DAC16 %d (dec) \n",LAST_DAC16_AUXTB);
  return 1; 
}

/*****************************************************
 *  Set up the timing 
 *      input arguments:   delay = ramp delay
 *                         time = integrate time
 *      units of arguments: 2.5 microseconds
 *****************************************************/
int setTimeAUXTB(int delay, int time)
{
  if ( !did_init_AUXTB ) return errorPrintAUXTB(1);
  auxtboard->rampdelay = delay;
  auxtboard->integtime = time;    
  return 1;
}

int getRampDelayAUXTB()
{
  if ( !did_init_AUXTB ) return errorPrintAUXTB(1);
  return auxtboard->rampdelay;
}
 
int getIntTimeAUXTB()
{
  if ( !did_init_AUXTB ) return errorPrintAUXTB(1);
  return auxtboard->integtime;
}



 int getAUXFLAG(int choice)
{
  if ( !did_init_AUXTB ) return -1;
  if ( choice == 1 ) {
    return auxtboard->dac12;
  }
  if ( choice == 2 ) {
    return auxtboard->dac16;
  }
  return errorPrintAUXTB(0);
}
 

/*****************************************************
 * Set the DACS, either 12-bit or 16-bit 
 *     arguments:  choice = 1,2 (which DAC)
 *                    value = DAC setting
 *     choice = 1  -->  DAC12 is set
 *     choice = 2  -->  DAC16 is set 
 *****************************************************/
int setAUXFLAG(int choice, int value)
{
  if ( !did_init_AUXTB ) return -1;
  if ( choice == 1 ) {
    LAST_DAC12_AUXTB = value;
    auxtboard->dac12 = value;
  }
  if ( choice == 2 ) {
    LAST_DAC16_AUXTB = value;
    auxtboard->dac16 = value;
  }
  return 1;
}
 
/* error message routine */
int errorPrintAUXTB(int error) {
  if (error==1) {
    printf("\nERROR: You must first initialize the VME address !\n");
    printf("One way is to call initAUXTB() first.\n");
    printf("No action taken...\n");
    return -1;  
  }
  printf("\nERROR: Undefined error in AUXTB utilities!\n");
  return -3;  
}


