/*****************************************
*  HAPTB.c                               *
*  routines used to interface the        * 
*  timing board for the HAPPEX ADC       *
*                                        *
*  K.D. Paschke                Sep 2002  *
*  R. Michaels, A. Vacheret    oct 2000  * 
******************************************/

#include "HAPTB.h"
#include "HAPTB_util.h"


int pickTB(int itb){
  ITB = itb;
  return ITB;
}

/*****************************************************
*
*  Initialize the HAPPEX TIME BOARD by setting the 
*  ramp delay, the integration time and the oversampling 
*  to a reasonable default.
******************************************************/
int initHAPTB()
{
  int itb = ITB;
  int res;
  unsigned long laddr;

  if (CODA_RUN_IN_PROGRESS==1) return errorPrintHAPTB(2);
 // for (itb=0; itb<NUMTB; itb++){
    TIMEBVMEADDR[itb] = 0;
    if (TIMEBRELADDR[itb] != 0){
      res = sysBusToLocalAdrs(0x29,TIMEBRELADDR[itb],&laddr);
      if (res != 0) {
        printf("Error in sysBusToLocalAdrs res=%d, addr=%d \n",res, laddr);
        printf("initHAPTB:::ERROR:::Timing Board POINTER NOT INITIALIZED");
        return -1;
      }
      else {
        TIMEBVMEADDR[itb] = laddr;
        printf("HAPPEX Timing Board address = 0x%x , 0x%x",res,laddr);
        tboard[itb] = (struct vme_happex_tb *) laddr;
        did_init_TB[itb] = 1;
        tboard[itb]->dac12   = TB_DEF_DAC12[itb]; 
        tboard[itb]->dac16   = TB_DEF_DAC16[itb];
        tboard[itb]->rampdelay = TB_DEF_RAMPDELAY[itb];
        tboard[itb]->integtime = TB_DEF_INTEGTIME[itb];
        tboard[itb]->osample_w = TB_DEF_OVERSAMPLE[itb];
        LAST_DAC12_TB[itb]    = TB_DEF_DAC12[itb];
        LAST_DAC16_TB[itb]    = TB_DEF_DAC16[itb];
        LAST_OVERSAMPLE_TB[itb] = TB_DEF_OVERSAMPLE[itb];
        printf("HAPPEX TIME BOARD at REL ADDR 0x%x :\n",TIMEBRELADDR[itb]);
        printf("dac12:0x%x, dac16:0x%x\n",TB_DEF_DAC12[itb],TB_DEF_DAC16[itb]);
        printf("ramp delay:0x%x, integrate time:0x%x, oversample:0x%x \n\n",
            TB_DEF_RAMPDELAY[itb],TB_DEF_INTEGTIME[itb],TB_DEF_OVERSAMPLE[itb]);

      }
    }
 // }
  return 1;
}


/* Dump the contents of readable registers */
int dumpRegHAPTB()
{
  int itb = ITB;
  //for (itb=0; itb<NUMTB; itb++){
    if ( !did_init_TB[itb] ) return errorPrintHAPTB(1);
    printf("\nDump of Readable Registers on ADC Timing Board number %d: \n",itb);
    printf("Input Data0 : %d (dec)  = 0x%x (hex) \n",
        tboard[itb]->data0, tboard[itb]->data0);
    printf("Input Data1 : %d (dec)  = 0x%x (hex) \n",
        tboard[itb]->data1, tboard[itb]->data1);
    printf("Ramp Delay %d (dec)  = 0x%x (hex) \n",
        tboard[itb]->rampdelay,tboard[itb]->rampdelay);
    printf("Integration Time %d (dec)  = 0x%x (hex) \n",
        tboard[itb]->integtime,tboard[itb]->integtime);
    printf("Oversampling setting %d (dec)  = 0x%x (hex) \n",
        tboard[itb]->osample_w,tboard[itb]->osample_w);
    printf("Current Oversample %d (dec)  = 0x%x (hex) \n",
        tboard[itb]->osample_c,tboard[itb]->osample_c);
    printf("DAC12 setting %d (dec)  = 0x%x (hex) \n",
        tboard[itb]->dac12,tboard[itb]->dac12); 
    printf("DAC16 settings %d (dec)  = 0x%x (hex) \n",
        tboard[itb]->dac16,tboard[itb]->dac16);
    printf("Last DAC12 %d (dec) \n",LAST_DAC12_TB[itb]);
    printf("Last DAC16 %d (dec) \n",LAST_DAC16_TB[itb]);
    printf("Last over sample setting %d (dec) \n",LAST_OVERSAMPLE_TB[itb]);
  //}
  return 1; 
}

int getOverSampleCurrentHAPTB()
{
  int itb = ITB;
  if ( !did_init_TB[itb] ) return -1;
  return tboard[itb]->osample_c;
}

int getOverSampleHAPTB()
{
  int itb = ITB;
  if ( !did_init_TB[itb] ) return -1;
  return tboard[itb]->osample_w;
}

int getDataHAPTB()
{
  int itb = ITB;
  if ( !did_init_TB[itb] ) return -1;
  return ((tboard[itb]->data0 & 0x1) << 1) + (tboard[itb]->data1 & 0x1);
}
 
int getRampDelayHAPTB()
{
  int itb = ITB;
  if ( !did_init_TB[itb] ) return -1;
  return tboard[itb]->rampdelay;
}
 
int getIntTimeHAPTB()
{
  int itb = ITB;
  if ( !did_init_TB[itb] ) return -1;
  return tboard[itb]->integtime;
}

 
int getDACHAPTB(int choice)
{
  int itb = ITB;
  if ( !did_init_TB[itb] ) return -1;
  if ( choice == 1 ) {
    return tboard[itb]->dac12;
  }
  if ( choice == 2 ) {
    return tboard[itb]->dac16;
  }
  return errorPrintHAPTB(0);
}
 

STATUS lockDACHAPTB() 
{
  int itb = ITB;
/*****************************************************
 * Lock the DAC setpoints, to avoid unintentional settings
 *****************************************************/
  HAPTB_DAC_LOCK[itb] = 1;
  return(OK);
}

STATUS unlockDACHAPTB() 
{
  int itb = ITB;
/*****************************************************
 * Lock the DAC setpoints, to avoid unintentional settings
 *****************************************************/
  HAPTB_DAC_LOCK[itb] = 0;
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
  int itb = ITB;
  if ( !did_init_TB[itb] ) return -1;
  if (HAPTB_DAC_LOCK[itb]) return -1;
  if ( choice == 1 ) {
    LAST_DAC12_TB[itb] = value;
    tboard[itb]->dac12 = value;
  }
  if ( choice == 2 ) {
    LAST_DAC16_TB[itb] = value;
    tboard[itb]->dac16 = value;
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
  int itb = ITB;
  if ( !did_init_TB[itb] ) return -1;
  //if (CODA_RUN_IN_PROGRESS==1) return errorPrintHAPTB(2);
  tboard[itb]->rampdelay = delay;
  tboard[itb]->integtime = time;    
  return 1;
}


/*****************************************************
 *
 *  Set up the oversampling, value = 0,1,2.... 
 *
 *****************************************************/
int setOverSampleHAPTB(int value) 
{
  int itb = ITB;
  if ( !did_init_TB[itb] ) return -1;
  if (CODA_RUN_IN_PROGRESS==1) return errorPrintHAPTB(2);
  LAST_OVERSAMPLE_TB[itb] = value;
  tboard[itb]->osample_w = value;
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


///////////////////////////////////////////////
//  HAPTB_config.c                           //
//  interface between configuration socket   //
//  server and the HAPPEX timing board       //
//  kdp, sep 2002                            //
///////////////////////////////////////////////

#include "HAPTB_cf_commands.h"

void taskHAPTB_CF(long* command, long *par1, long *par2, long *par3)
{
  int itb = ITB;
  int junk;
  switch (*command)
    {
    case HAPTB_GET_DATA:
      switch (*par1)
	{
        case HAPTB_RD:
	  *par2 = getRampDelayHAPTB();
	  break;
        case HAPTB_IT:
	  *par2 = getIntTimeHAPTB();
	  break;
	case HAPTB_OS:
	  *par2 = getOverSampleHAPTB();
	  break;
	}
      break;
    case HAPTB_SET_DATA:
      switch (*par1)
	{
        case HAPTB_RD:
	  //  int setTimeHAPTB(int delay, int time)
	  junk = getIntTimeHAPTB();
	  *command = setTimeHAPTB(*par2,junk);
	  break;
        case HAPTB_IT:
	  //  int setTimeHAPTB(int delay, int time)
	  junk = getRampDelayHAPTB();
	  *command = setTimeHAPTB(junk,*par2);
	  break;
	case HAPTB_OS:
	  *command = setOverSampleHAPTB(*par2);
	  break;
	}
      break;
    }
}

