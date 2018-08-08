/*****************************************
*  HAPTB.c                               *
*  routines used to interface the        * 
*  timing board for the HAPPEX ADC       *
*                                        *
*  K.D. Paschke                Sep 2002  *
*  R. Michaels, A. Vacheret    oct 2000  * 
******************************************/

#include "HAPTB_util.h"
#include "HAPTB.h"

/*****************************************************
*
*  Initialize the HAPPEX TIME BOARD by setting the 
*  ramp delay, the integration time and the oversampling 
*  to a reasonable default.
******************************************************/
int initHAPTB()
{
  int res;
  unsigned long laddr;

  if (CODA_RUN_IN_PROGRESS==1) return errorPrintHAPTB(2);
  res = sysBusToLocalAdrs(0x29,TIMEBADDR,&laddr);
  if (res != 0) {
      printf("Error in sysBusToLocalAdrs res=%d \n",res);
      printf("initHAPTB:::ERROR:::Timing Board POINTER NOT INITIALIZED");
      return -1;
  }
  printf("HAPPEX Timing Board address = 0x%x ",laddr);
  did_init_TB = 1;
  tboard = (struct vme_happex_tb *) laddr;
  tboard->dac12   = TB_DEF_DAC12; 
  tboard->dac16   = TB_DEF_DAC16;
  tboard->rampdelay = TB_DEF_RAMPDELAY;
  tboard->integtime = TB_DEF_INTEGTIME;
  tboard->osample_w = TB_DEF_OVERSAMPLE;
  LAST_DAC12_TB    = TB_DEF_DAC12;
  LAST_DAC16_TB    = TB_DEF_DAC16;
  LAST_OVERSAMPLE_TB = TB_DEF_OVERSAMPLE;
  printf("HAPPEX TIME BOARD at %x se     :\n",HAPTB_ADDR);
  printf("dac12:0x%x, dac16:0x%x\n",TB_DEF_DAC12,TB_DEF_DAC16);
  printf("ramp delay:0x%x, integrat time:0x%x, oversample:0x%x \n\n",
	 TB_DEF_RAMPDELAY,TB_DEF_INTEGTIME,TB_DEF_OVERSAMPLE);
  return 1;
}


/* Dump the contents of readable registers */
int dumpRegHAPTB()
{
  if ( !did_init_TB ) return errorPrintHAPTB(1);
  printf("\nDump of Readable Registers on ADC Timing Board : \n");
  printf("Input Data0 : %d (dec)  = 0x%x (hex) \n",
	 tboard->data0, tboard->data0);
  printf("Input Data1 : %d (dec)  = 0x%x (hex) \n",
	 tboard->data1, tboard->data1);
  printf("Ramp Delay %d (dec)  = 0x%x (hex) \n",
         tboard->rampdelay,tboard->rampdelay);
  printf("Integration Time %d (dec)  = 0x%x (hex) \n",
         tboard->integtime,tboard->integtime);
  printf("Oversampling setting %d (dec)  = 0x%x (hex) \n",
         tboard->osample_w,tboard->osample_w);
  printf("Current Oversample %d (dec)  = 0x%x (hex) \n",
         tboard->osample_c,tboard->osample_c);
  printf("DAC12 setting %d (dec)  = 0x%x (hex) \n",
    tboard->dac12,tboard->dac12); 
  printf("DAC16 settings %d (dec)  = 0x%x (hex) \n",
    tboard->dac16,tboard->dac16);
  printf("Last DAC12 %d (dec) \n",LAST_DAC12_TB);
  printf("Last DAC16 %d (dec) \n",LAST_DAC16_TB);
  printf("Last over sample setting %d (dec) \n",LAST_OVERSAMPLE_TB);
  return 1; 
}

int getOverSampleCurrentHAPTB()
{
  if ( !did_init_TB ) return -1;
  return tboard->osample_c;
}

int getOverSampleHAPTB()
{
  if ( !did_init_TB ) return -1;
  return tboard->osample_w;
}

int getDataHAPTB()
{
  if ( !did_init_TB ) return -1;
  return ((tboard->data0 & 0x1) << 1) + (tboard->data1 & 0x1);
}
 
int getRampDelayHAPTB()
{
  if ( !did_init_TB ) return -1;
  return tboard->rampdelay;
}
 
int getIntTimeHAPTB()
{
  if ( !did_init_TB ) return -1;
  return tboard->integtime;
}

 
int getDACHAPTB(int choice)
{
  if ( !did_init_TB ) return -1;
  if ( choice == 1 ) {
    return tboard->dac12;
  }
  if ( choice == 2 ) {
    return tboard->dac16;
  }
  return errorPrintHAPTB(0);
}
 

STATUS lockDACHAPTB() 
{
/*****************************************************
 * Lock the DAC setpoints, to avoid unintentional settings
 *****************************************************/
  HAPTB_DAC_LOCK = 1;
  return(OK);
}

STATUS unlockDACHAPTB() 
{
/*****************************************************
 * Lock the DAC setpoints, to avoid unintentional settings
 *****************************************************/
  HAPTB_DAC_LOCK = 0;
  return(OK);
}

/*****************************************************
 * Set the DACS, either 12-bit or 16-bit 
 *     arguments:  choice = 1,2 (which DAC)
 *                    value = DAC setting
 *     choice = 1  -->  DAC12 is set
 *     choice = 2  -->  DAC16 is set 
 *****************************************************/
int setDACHAPTB(int choice, int value)
{
  if ( !did_init_TB ) return -1;
  if (HAPTB_DAC_LOCK) return -1;
  if ( choice == 1 ) {
    LAST_DAC12_TB = value;
    tboard->dac12 = value;
  }
  if ( choice == 2 ) {
    LAST_DAC16_TB = value;
    tboard->dac16 = value;
  }
  return value;
}
 
/*****************************************************
 *  Set up the timing 
 *      input arguments:   delay = ramp delay
 *                         time = integrate time
 *      units of arguments: 2.5 microseconds
 *****************************************************/
int setTimeHAPTB(int delay, int time)
{
  if ( !did_init_TB ) return -1;
  //if (CODA_RUN_IN_PROGRESS==1) return errorPrintHAPTB(2);
  tboard->rampdelay = delay;
  tboard->integtime = time;    
  return 1;
}


/*****************************************************
 *
 *  Set up the oversampling, value = 0,1,2.... 
 *
 *****************************************************/
int setOverSampleHAPTB(int value) 
{
  if ( !did_init_TB ) return -1;
  if (CODA_RUN_IN_PROGRESS==1) return errorPrintHAPTB(2);
  LAST_OVERSAMPLE_TB = value;
  tboard->osample_w = value;
  return 1;
}


/* error message routine */
int errorPrintHAPTB(int error) {
  if (error==1) {
    printf("\nERROR: You must first initialize the VME address !\n");
    printf("One way is to call initHAPTB() first.\n");
    printf("No action taken...\n");
    return -1;  
  } else if (error=2) {
    printf("/n HAPTB ERROR::: CODA run is in progress!\n");
    printf("You should not change timing parameters during a run!\n");
    return -2;
  }
  printf("\nERROR: Undefined error in HAPTB utilities!\n");
  return -3;  
}


