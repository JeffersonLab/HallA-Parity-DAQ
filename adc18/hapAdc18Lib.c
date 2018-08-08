/* Library for JLab 18bit ADC for HAPPEX             */
/* R. Michaels, Nov 2006                             */

/*  Modifications  Oct 2009   R. Michaels

     1. adc18_DataAvail:
        Corrected logic for when data is ready or not.
        Added logMsg for various error conditions.

     2. adc18_getevtcnt:
        Change mask from FF to 3FFFFFF

     3. Add new function adc18_stop  (inverse of go).

   Apr 24, 2010

     modify the alarm about ADC18 buff full in the
     case where csr = 0xffffffff.

   Jan 4, 2016

        Add adc18_getconv and adc18_getintconv


 */



#include "vxWorks.h"
#include "stdio.h"
#include "string.h"
#include "logLib.h"
#include "taskLib.h"
#include "intLib.h"
#include "iv.h"
#include "semLib.h"
#include "vxLib.h"
#include "math.h"


#include "hapAdc18Lib.h"
#include "ADC18_cf_commands.h"


/* default ADC config bits */
#define DEFAULT_CONFIG_BITS 0xC00

static int verbose=1;

/* set the pedestal */
int adc18_setpedestal(int id, int ped1, int ped2, int ped3, int ped4);
/* set pedestal for single channel */
int adc18_setped(int id, int chan, int ped1);
/* get a pedestal */
int adc18_getpedestal(int id, int pednum);
/* Define memory state (power up condition) */
void adc18_zeromem();
/* Generic Init  (knows what to do for this crate */
int adc18_initall();
/* Initialize board pointers */
int adc18_init(int id, UINT32 adc_addr, UINT32 data_addr);
/* Get number of ADCs */
int adc18_getNum();
/* Get label of ADC #id */
int adc18_getLabel(int id);
/*Get the Conv Gain*/ 
int adc18_getconv(int id);
/*Get the Int Gain*/
int adc18_getintgain(int id);
/* Reset the ADC */
int adc18_reset(int id);
/* Soft reset of the ADC */
int adc18_softreset(int id);
/* Set timing if internal source */
int adc18_timesrc(int id, int source, int ramp, int window);
/* Set gate mode */
int adc18_gatemode(int id, int which);
/* Set DAC value (once) */
int adc18_setdac(int id, long dac_value);
/* Load 256K DAC values */
int adc18_loaddac(int id, int type);
/* Get DAC pattern (once) */
int adc18_getdac(int id);
/* Set integrator gain */
int adc18_intgain(int id, long gain);
/* Set V-to-C conversion */
int adc18_setconv(int id, long conv);
/* Set # samples */
int adc18_setsample(int id, int nsample);
/* To see if ADC is busy converting (1) or not (0) */
int adc18_getsample(int id);
/* To see if ADC is busy converting (1) or not (0) */
int adc18_Busy(int id);
/* To check if data is available (1) or not (0) */
int adc18_DataAvail(int id);
/* Print the ADC setup */
int adc18_print_setup(int id);
/* Start the DAQ sequencing */
int adc18_go(int id);
/* Stop the DAQ sequencing */
int adc18_stop(int id);
/* Get number of events (number of oversamples) */
int adc18_getevtcnt(int id);
/* Get event word count */
int adc18_getwordcnt(int id);
/* Get CRL */
long adc18_getcsr(int id);
/* Get next in FIFO */
unsigned long adc18_getdata(int id);   
/* Get all data and print it */
int adc18_testdata(int id);
/* print & decode registers and data word */
void adc18_reg_decode(int id);
void adc18_reg_status(int id);
void adc18_decode_data(int id, unsigned long data);
/* check that the ID is within range, etc */
int adc18_chkid(int id);
/* default setup */
int adc18_defaultSetup(int id);
/* interface for rapid setup */
int adc18_setup(int id, int time, int intgain, int conv);
/* to flush out buffer */
int adc18_flush(int id);
/* various test code for checking busy logic */
int adc18_testcsr1(int id);
int adc18_test2();
int adc18_oneshot();
void adc18_pulser();
int adc18_test3();
int adc18_test4();


void adc18_zeromem() {
/* This has to be done when you reboot the VME board, to define zero memory */
  int id;
  t_mode = 0;
  for (id = 0; id < ADC18_MAXBOARDS; id++) {
    adc18p[id] = 0;
    adc18_data[id] = 0;
    adc18_ok[id] = 0;
  }
}


void taskADC18_CF(long* command, long *par1, long *par2, long *par3)
{
  int junk1;
  int junk2;
  int gainbit;
  int ped;
  int chan;
  int ibrd;
  int flag;
  int pattern;
  int sample;


  switch (*command)
    {
    case ADC18_GET_NUMADC:
      *par2 = adc18_getNum();
      break;
    case ADC18_GET_LABEL:
      ibrd = *par1;
      *par2 = adc18_getLabel(ibrd);
      break;
    case ADC18_GET_CSR:
      ibrd = *par1;
      adc18_getcsr(ibrd);
      break;
    case ADC18_SET_CONV:
      ibrd = *par1;
      gainbit = *par2;
      adc18_setconv(ibrd,gainbit);
      break;
    case ADC18_SET_INT:
      ibrd = *par1;
      gainbit = *par2;
      adc18_intgain(ibrd,gainbit);
      break;
    case ADC18_SET_PED:
      ibrd = *par1;
      chan = *par2;
      ped  = *par3;
      adc18_setped(ibrd,chan,ped);
      // figure out later
      break;
    case ADC18_GET_CONV:
      ibrd = *par1;
      *par2= adc18_getconv(ibrd);
      // figure out later
      break;
    case ADC18_GET_INT:
      ibrd = *par1;
      *par2 = adc18_getintgain(ibrd);
      // figure out later
      break;
    case ADC18_GET_PED:
      ibrd = *par1;
      ped = *par2 & 0x2;
      //*par2= adc18_getped(ibrd,gainbit);
      // figure out later
      break;
    case ADC18_SET_DAC:
      ibrd = *par1;
      pattern = *par2;
      adc18_loaddac(ibrd,pattern);
      break;
    case ADC18_GET_DAC:
      ibrd = *par1;
      *par2 = adc18_getdac(ibrd);
      break;
    case ADC18_SET_SAMP:
      ibrd = *par1;
      sample = *par2;
      adc18_setsample(ibrd,sample);
      break;
    case ADC18_GET_SAMP:
      ibrd = *par1;
      *par2 = adc18_getsample(ibrd);
      //printf("sample should be: %d\n", par2);
      break;
    }
}




int adc18_initall() {
/* Generic Init  (knows what to do for this crate) */
  int id;
  UINT32 adc_addr, data_addr;
  adc18_zeromem();
  if (NADC > ADC18_MAXBOARDS) {
    printf("adc18_initall:  Need a larger ADC_MAXBOARDS \n");
    printf("No ADCs initialized !!\n");
    return -1;
  }
  printf("(version Oct 29, 2009)  Num ADC18 = %d \n",NADC);

  for (id = 0; id < NADC; id++) {
    adc_addr = ADCADDR[id];
    data_addr = adc_addr + 0x1000;

    printf("Init ADC18 # %d   adc_addr = 0x%x    data_addr = 0x%x\n",id,adc_addr,data_addr);

    adc18_init(id, adc_addr, data_addr);
    adc18_reset(id);
    adc18_softreset(id);
  }
  return 0;
}

int adc18_init(int id, UINT32 adc_addr, UINT32 data_addr) {

/* Initialize board pointers : adc at adc_addr and data at data_addr */
/* This can be done when you reboot VME, but at least before you run */
 
  unsigned long laddr;
  int res, rdata;

  adc18_ok[id] = 0;
  if (adc18_chkid(id) == -1) return -1;
  printf("Initializing ADC18 board %d now ... \n",id);

  sysBusToLocalAdrs(0x39,adc_addr,&laddr);
  printf("ADC18 board %d  register local address = 0x%x\n",id,laddr);  
  adc18p[id] = (struct adc18_struct *)laddr;
  adc18p[id]->mod_id = id;
  res = vxMemProbe((char *) &(adc18p[id]->mod_id), VX_READ, 4, (char *)&rdata);
  if (res < 0) {
    printf("ADC18:ERR: No addressable board %d  at addr 0x%x\n",id,adc_addr);
    return -1;
  } else {
    printf("ADC18:Init: Board ID = 0x%x \n",rdata);
  }
  sysBusToLocalAdrs(0x39,data_addr,&laddr);
  printf("ADC18 board %d   data local address = 0x%x\n",id,laddr);  
  adc18_data[id] = (volatile unsigned long *)laddr;

  adc18_ok[id] = 1;
  adc18_reset(id);

  return 0;

}

int adc18_getNum() {
/* Get number of ADCs */
  return NADC;
} 

int adc18_getLabel(int id) {
/* Get label of ADC #id */
  if (id < 0 || id >= ADC18_MAXBOARDS) {
    printf("ADC18:ERR:  Board id %d is outside range !!\n",id);
    return -1;
  }
  return ADCLABEL[id];
}

int adc18_reset(int id) {
/* Clear the ADC, like a power up condition.
   In CODA do this in download.  */
  if (adc18_chkid(id) < 1) return -1;
  adc18p[id]->csr  = 0;
  adc18p[id]->csr  = 0x100;  /* hard reset */
  adc18p[id]->csr  = 0x200;  /* soft reset */
  adc18p[id]->config = 0;
  adc18p[id]->dac_value = 0;
  adc18p[id]->delay_1 = 0;
  adc18p[id]->delay_2 = 0;
  return 0;
}


int adc18_softreset(int id) {
  if (adc18_chkid(id) < 1) return -1;
  adc18p[id]->csr = 0x200;  /* soft reset */
  return 0;
}


int adc18_chkid(int id) {
/* id is the index of the board (id = 0,1,2... ) for N boards in a crate.
   This routine checks that 'id' is within range and whether
   the module was initialized and configured.
      returns: 
          -1  -->  wrong id index
           0  -->  not yet initialized
           1  -->  board was initialized (adc18_init)
           2  -->  board was configured (config nonzero)
 */
  if (id < 0 || id >= ADC18_MAXBOARDS) {
    printf("ADC18:ERR:  Board id %d is outside range !!\n",id);
    return -1;
  }
  if (adc18_ok[id] == 0) {
    printf("ADC18:chkid:  Board id %d unitiailzed. \n",id);
    return 0;
  }
  if (adc18p[id]->config != 0) return 2;  
  return 1;
}


int adc18_timesrc(int id, int source, int ramp, int window) {

// Set the timing source for ADC #id
//    source == 0 means external, 1 is internal
//    the other parameters only matter if source = internal.
//    then, ramp & window are the ramp delay and integration window.

  float set_window;

  if (adc18_chkid(id) < 1) return -1;

  if (source != 0 && source < 1) {
    printf("ADC18:ERR:  Time source may be 0 or 1 only \n");
    return -1;
  }

  if( source == 0 ) {
     t_mode = 0;			/* external timing sequence */
     printf("\nUse external timing module to generate timing sequence\n\n");
  } 
  else {
    t_mode = 1;    		/* internal timing sequence */
    printf("\nUse internal timer to generate timing sequence\n\n");
    adc18p[id]->delay_1 = ramp;		
    if( (window >= 1) && (window <= 163840) ) {
      set_window = ( ( ((float)window) + TIME_INT ) / 2.5 ) + 0.5;
    } 
    else {
      printf("!!! ERROR in input value (use default value of 100 us)\n");    
      window = 100;    
      set_window = ( ( ((float)window) + TIME_INT ) / 2.5 ) + 0.5;
    }
    printf("  Integration window = %d usec \n",window);
    printf("\n     Integration count value = %d\n", (long)set_window);    
    adc18p[id]->delay_2 = (long)set_window;
  }
  
}

long adc18_getcsr(int id) {
   
  return adc18p[id]->csr;

}


int adc18_getwordcnt(int id) {
/* Get event word count */
  int event_word_count;

  if (adc18_chkid(id) < 1) return 0;

  event_word_count = 0xFF & (adc18p[id]->evt_word_ct);
  return event_word_count;
}

int adc18_getevtcnt(int id) {
/* Get event count (number of oversamples) */
  int event_count;

  if (adc18_chkid(id) < 1) return 0;

  event_count = 0x3FFFFFF & (adc18p[id]->evt_ct);
  return event_count;
}


unsigned long adc18_getdata(int id) {
/* It's assumed the user has already called adc18_getwordcnt */
/* Then this routine gets data out of the FIFO  */
/* Usage:
/*    Let N = adc18_getwordcnt(id) 
          for (i = 0; i < N; i++) {
             mydata = adc18_getdata(id);  // pop off FIFO
          }
*/

  if (adc18_chkid(id) < 1) return -1;

  return *adc18_data[id];
}

void adc18_reg_decode(id)
{
      long value, ii;
      long divider, div_n;

      if (adc18_chkid(id) < 1) printf("ADC18:ERR: cannot decode registers\n");
	
      value = adc18p[id]->mod_id;
	
      printf("\nModule ID = %X   Version = %X\n", (value & 0x1F), ((value >> 8) & 0xFF) );

      value = adc18p[id]->config;
	
      printf("Integration gain factor (0-3) = %d\n", ((value >> 1) & 0x3) );

      printf("V-to-C conversion gain (0-15) = %d\n", ((value >> 4) & 0xF) );
		
      if( value & 0x1 )
           printf("Integration gate FORCED OPEN\n");
      else
           printf("SWITCHING integration gate\n");
	    
      if( value & 0x400 )
           printf("Raw data output\n");
      else
           printf("Raw data suppressed from output\n");
           	
      if( value & 0x800 )
           printf("DAC value output\n");
      else
           printf("DAC value suppressed from output\n");
	    	
      div_n = ( (0x3) & (value >> 8) );
      divider = 1;
      for(ii = 0; ii < div_n; ii++)
          divider = divider * 2;
	    
      printf("%d samples each for baseline & peak\n",divider);

      value = adc18p[id]->csr;
	
      if( value & 0x1 )
           printf("EVENT AVAILABLE in output buffer\n");
	    	
      if( value & 0x2 )
           printf("Output buffer EMPTY\n");
	    	
      if( value & 0x4 )
           printf("Output buffer FULL\n");
	    	
      if( value & 0x8 )
           printf("Power O.K.\n");
      else
           printf("Power NOT O.K.\n");
	    	
      if( value & 0x10 )
           printf("Channel 1 ADC BUSY asserted\n");
      if( value & 0x20 )
           printf("Channel 2 ADC BUSY asserted\n");
      if( value & 0x40 )
           printf("Channel 3 ADC BUSY asserted\n");
      if( value & 0x80 )
           printf("Channel 4 ADC BUSY asserted\n");
	    	
      value = adc18p[id]->ctrl;
       
      if( value & 0x1 )
           printf("GO asserted\n");
      else
           printf("GO NOT asserted\n");
	    
      if( value & 0x2 )
           printf("Force DAC = 0\n");
	
      if( value & 0x4 )
	{
	    printf("Self Trigger ENABLED\n");
	    value = adc18p[id]->delay_2;
	    printf("Integration Window = %.2f us\n",((float)(value & 0xFFFF)) * 2.5);
	}
	
      value = adc18p[id]->dac_value;
      printf("Current DAC value (hex) = %LX\n", (value & 0xFFFF));
       
      value = adc18p[id]->evt_ct;
      printf("Event Count = %d\n", (value & 0x3FFFFFF));
	
      value = adc18p[id]->evt_word_ct;
      printf("Event Word Count = %d\n", (value & 0xFF));

      value = adc18p[id]->pedestal[0];
      printf("Pedestal for CH 0 is %d\n", (value & 0x3FFF));
      value = adc18p[id]->pedestal[1];
      printf("Pedestal for CH 1 is %d\n", (value & 0x3FFF));
      value = adc18p[id]->pedestal[2];
      printf("Pedestal for CH 2 is %d\n", (value & 0x3FFF));
      value = adc18p[id]->pedestal[3];
      printf("Pedestal for CH 3 is %d\n", (value & 0x3FFF));

	
}

int adc18_DataAvail(int id) {
  /* To check if data are available (1) or not (0) */
  
    long csrvalue, threebits, dready;
    int csr_mask = 0x7;
    int ready_mask = 0x1;
    int debug = 0;

    csrvalue = adc18_getcsr(id);

    threebits = csrvalue & csr_mask;
    dready = csrvalue & ready_mask;

    // Logic of the 1st three bits
    // if 1stbit = 1 data are ready.  forget about 2ndbit
    // if 3rdbit = 1 the event buffer is full --> serious error
    
    //  bit=  1        2         3         result

    //        1        0         0       ok, data ready
    //        1        1         0       Bad (data ready but buffer empty ??)
    //        0        0         0       data not ready, busy condition
    //        0        1         0       data not ready
    //       any      any        1       serious error, buffer full

    if ((threebits & 0x4) != 0 && csrvalue != 0xffffffff) {
      //      logMsg("\n ADC18 ERROR: Buffer full ! Id %d Csr = 0x%x\n",id,csrvalue,0,0,0,0);
      return 3;
      }
    if (csrvalue == 0xffffffff) {
      logMsg("\n ADC18 Warning (prob. ok): Id %d Csr = 0x%x (momentarily non-addressable)\n",
                       id,csrvalue,0,0,0,0);
    } else {
      if ((threebits & 0x3) == 3) {
        logMsg("\n ADC18 ERROR:  data ready yet buffer empty ! Id %d Csr = 0x%x\n",
            id,csrvalue,0,0,0,0);
      }
    }

    if (debug) {
      logMsg("\n ADC18 DataAvail: CSR = 0x%x  threebits = 0x%x  dready = %d\n",csrvalue,threebits,dready,0,0,0);
    }

    if (dready == 1) return 1;
    return 0;

}  


int adc18_Busy(int id) {
  /* To check if the ADC is busy.  
         Returns 1 if busy                   
         Retruns 0 if not busy and event ready */

  /* To be fast I'll assume 'id' is valid (has been checked) */

      long value;
      int event_avail_flag = 0x1;
      int busy_flag = 0xf0;

      value = adc18p[id]->csr;
	
      if ( value & event_avail_flag ) return 0;  
      return 1;
}


void adc18_reg_status(int id)
{
      if (adc18_chkid(id) < 1) printf("ADC18:ERR: cannot print reg status\n");
      printf("\nmod_id (hex) = %lx\n", (adc18p[id]->mod_id));
      printf("csr (hex) = %lx\n", 0xFFFF & (adc18p[id]->csr));
      printf("ctrl (hex) = %lx\n", 0xFFFF & (adc18p[id]->ctrl));
      printf("config (hex) = %lx\n", 0xFFFF & (adc18p[id]->config));
      printf("event_count = %ld\n", 0xFFFF & (adc18p[id]->evt_ct));
      printf("event_word_count = %ld\n", 0xFF & (adc18p[id]->evt_word_ct));
      printf("dac_value (hex) = %lx\n", 0xFFFF & (adc18p[id]->dac_value));
      printf("delay_1 = %ld\n", 0xFFFF & (adc18p[id]->delay_1));
      printf("delay_2 = %ld\n\n", 0xFFFF & (adc18p[id]->delay_2));
}

int adc18_gatemode(int id, int which) {
/* Set gate mode 
      which == 0   --> switch
      which == 1   --> open
*/
  long config;
  if (adc18_chkid(id) < 1) return -1;

  if (which != 0 && which < 1) {
      printf("ADC18:ERR:  Gate mode must be a zero or one \n!");
      return -1;
  }
   
  config = (0xfffffffe & adc18p[id]->config);
 
  config |= which;
  config |= DEFAULT_CONFIG_BITS;

  adc18p[id]->config = config;
  return 0;

}

int adc18_loaddac(int id, int type) {
  /* Load a pattern of 256K dac values */
  
  int i;
  int dac_min = 10000;
  int dac_max = 12000;
  int small_min = 2000;
  int small_max = 4000;
  unsigned short tmp_rand;
  float float_rand; 
  int NREPEAT = 4;
  int ncnt = 256000;
  int sign = 1;
  int dac_value = 0;
  int TRIANGLE = 0;
  int SAWTOOTH = 1;
  int CONST = 2;
  int RANDCONST = 3;
  int OFF = 3;
  
  int pattern = type;
  
  
  if (adc18_chkid(id) < 1) return -1;
  
  adc18p[id]->ctrl = 0;  /* set GO = 0 */
  
  if(verbose) printf("adc18_loaddac:  DAC pattern = %d  ncnt = %d \n",pattern, ncnt);
  
  dac_value = dac_max;

/* See the random number generator; may want to use system time, or ... */  
/* srand and rand come from vxworks lib, see stdlib.h */
  if (pattern==RANDCONST) srand(9421); 

  for (i = 0; i <= ncnt; i++) {  
    
    //for steady fluctuations
    if (pattern == TRIANGLE) {
      if ((i%NREPEAT)==0) {
	if (dac_value > dac_max) sign = -1;
	if (dac_value < dac_min) sign = 1;
	dac_value = dac_value + sign*4;
      }
    } 
    else if (pattern == SAWTOOTH) {
      //for sawtooth fluctuations
      dac_value = dac_value - sign*10;
      if (dac_value < dac_min) dac_value = dac_max;
    } 
    else if (pattern == CONST) {
      //constant value
      dac_value = 30000;
    }    
    else if (pattern == RANDCONST) {
 /* the values are random, but also constant for NREPEAT events */
 /* err, not 100% sure about this yet (Sept 13, 2016) */
      if ((i%NREPEAT)==0) {
        tmp_rand = (unsigned short) rand();
        float_rand = (float) tmp_rand/32768.;
	dac_value = small_min + (int) (small_max-small_min)*float_rand;
	/*   printf("loading dac %d %d %d \n",i,tmp_rand,dac_value); */
      }
    }    

    else {
      dac_value = 0;
    }
    
    /*   DONT WANT, I think if (i == 0) dac_value |= 0x10000;
	  if (i == ncnt) dac_value |= 0x20000; */

    adc18p[id]->dac_memory = dac_value;
    adc18p[id]->pattern = pattern;
      }
  return 0;
}



int adc18_setdac(int id, long dac_value) {
/* Set DAC value */

  if (adc18_chkid(id) < 1) return -1;

  adc18_loaddac(id,2); //CONST=2

  adc18p[id]->ctrl = 0;  /* set GO = 0 */

   /* assert first & last flags */
  adc18p[id]->dac_memory = 0x30000 | dac_value;	

  adc18p[id]->dac_value = dac_value;

  return 0;
}


int adc18_getdac(int id) {
/* Get DAC pattern */

  //if (adc18p[id]->pattern==2) return adc18p[id]->dac_value; //CONST=2
  //else 
  return adc18p[id]->pattern;
}





int adc18_getpedestal(int id, int pednum) {

  return adc18p[id]->pedestal[pednum] & 0x3FFF;

}



int adc18_setped(int id, int chan, int ped1) {
/* Set the pedestal, value between 0 and 4095 */
/* Only works before 'go' (ctrl[0] = 0) */
/* We'll needs some sort of switch here to distinguish between
   version1 (has no pedestal register) and version2 */

  if(verbose) printf("Loading ADC pedestal=%d for chan  %d successfully\n",ped1,chan);

  if (adc18_chkid(id) < 1) return -1;

  if (ped1 < 0 || ped1 > 4095) {
    printf("adc18_setpedstal::ERROR  pedestals must be 0 - 4095 \n");
    return -1;
  }


  adc18p[id]->pedestal[chan] = ped1;    
  taskDelay(1);
  adc18p[id]->csr = 0x10000;   /* load FPGA */
  taskDelay(1);



}

int adc18_setpedestal(int id, int ped1, int ped2, int ped3, int ped4) {
/* Set the pedestal, value between 0 and 4095 */
/* Only works before 'go' (ctrl[0] = 0) */
/* We'll needs some sort of switch here to distinguish between
   version1 (has no pedestal register) and version2 */

  if(verbose) printf("Loading ADC pedestals %d %d %d %d successfully\n",ped1,ped2,ped3,ped4);

  if (adc18_chkid(id) < 1) return -1;

  if (ped1 < 0 || ped1 > 4095 || ped2 < 0 || ped2 > 4095 ||
      ped3 < 0 || ped3 > 4095 || ped4 < 0 || ped4 > 4095 ) {
    if(verbose) printf("adc18_setpedstal::ERROR  pedestals must be 0 - 4095 \n");
    return -1;
  }


  adc18p[id]->pedestal[0] = ped1;    
  taskDelay(1);
    adc18p[id]->csr = 0x10000;   /* load FPGA */
  taskDelay(1);

  adc18p[id]->pedestal[1] = ped2;    
  taskDelay(1);
    adc18p[id]->csr = 0x10000;   
  taskDelay(1);

  adc18p[id]->pedestal[2] = ped3;    
  taskDelay(1);
    adc18p[id]->csr = 0x10000;   
  taskDelay(1);

  adc18p[id]->pedestal[3] = ped4;    
  taskDelay(1);

  adc18p[id]->csr = 0x10000;   


}

/*int adc18_reset_peds(int id) {

  int intgain, convgain, label;

  intgain=adc18_getint(id);
  convgain=adc18_getconv(id);

  label=adc18_getLabel(id);

  printf("Will now set pedestals for board ADCX%d with gain = (%d,%d)",
	 label,intgain,convgain);


	 }*/

int adc18_intgain(int id, long gain) {
/* Set integrator gain factor   */
/* note: a gain of '3' is minimum, '0' is max (in terms of result per volt) */
  long conv;
  long config;
     
  if (adc18_chkid(id) < 1) return -1;

  conv=adc18_getconv(id);
  config=adc18p[id]->config;

  adc18_reset(id);

  if(verbose) printf("adc18_intgain  adc %d   gain = %d \n",id,gain);

  if (gain < 0 || gain > 3) {
      printf("ADC18:ERR:  int_gain is outside of range. \n",id,gain);
      return -1;
  }
  config=adc18p[id]->config;
  config |= gain << 1;  //set the int gain
  config |= conv << 4;  //set the conv gain
  config |= DEFAULT_CONFIG_BITS;
  adc18p[id]->config = config;
  return 0;
}


int adc18_setconv(int id, long conv) {
/* Set V-to-C conversion */
/* The gain is linear in 'conv' */
  long intgain;
  long config;

  if (adc18_chkid(id) < 1) return -1;

  intgain=adc18_getintgain(id);
  config=adc18p[id]->config;

  adc18_reset(id);

  if(verbose) printf("adc18_setconv  adc %d   conv = %d \n",id,conv);

  if (conv < 0 || conv > 15) {
      printf("ADC18:ERR:  vc_gain is outside of range. \n");
      return -1;
  }
   
  config = adc18p[id]->config;
  config |= intgain << 1;  //set the int gain
  config |= conv << 4;  //set the conv gain
  config |= DEFAULT_CONFIG_BITS;

  adc18p[id]->config = config;
  return 0;
}
int adc18_getconv(int id) {
/* Get conversion gain present setting */

  long config;
  int conv;

  if (adc18_chkid(id) < 1) {
    printf("adc18_getconv: undefined ADC %d \n",id);
    return -1;
  }

  config = adc18p[id]->config;

  conv = ((config >> 4) & 0xF);

  printf("adc18_getconv  adc %d   conv = %d \n",id,conv);

  return conv;
}

int adc18_getintgain(int id) {
/* Get integrator gain present setting */

  long config;
  int intgain;

  if (adc18_chkid(id) < 1) {
    printf("adc18_getintgain: undefined ADC %d \n",id);
    return -1;
  }

  config = adc18p[id]->config;

  intgain = ((config >> 1) & 0x3);

  printf("adc18_getintgain  adc %d   conv = %d \n",id,intgain);

  return intgain;
}

int adc18_setsample(int id, int nsample) {
/* Set # samples on baseline and peak */

  long config;
  int n_mode;

  if (adc18_chkid(id) < 1) return -1;

  if(verbose) printf("adc18_setsample  adc %d   sample = %d \n",id,nsample);

  switch (nsample) {

    case 1:
      if(verbose) printf("Using sample by 1 \n");
      n_mode=0;
      break;
    case 2:
      if(verbose) printf("Using sample by 2 \n");
      n_mode=1;
      break;
    case 4: 
      if(verbose) printf("Using sample by 4 \n");
      n_mode=2;
      break;
    case 8:
      if(verbose) printf("Using sample by 8 \n");
      n_mode=3;
      break;
    default:
      if(verbose) printf("ADC18:ERR: nsample can only be 1, 2, 4, or 8 \n");
      return -1;

  }
   
  config = adc18p[id]->config;
 
  config |= n_mode << 8;
  config |= DEFAULT_CONFIG_BITS;

  adc18p[id]->config = config;
  adc18p[id]->sample=nsample;

  return 0;
}


int adc18_getsample(int id) {
  /* Get sample */
  
  return pow(2,((int)(adc18p[id]->config >> 8) & 0x3));
}



int adc18_go(int id) {
  // Initialize one DAQ sequence

  // ADC must be configured
   if (adc18_chkid(id) != 2) return -1;  


   adc18p[id]->ctrl = 0 | (t_mode << 2);
   adc18p[id]->ctrl = 1 | (t_mode << 2);	/* go */

   if (t_mode) adc18p[id]->csr = 0x8000;

   return 0;
}

int adc18_stop(int id) {
  // Stop the DAQ sequencing

  // ADC must be configured
   if (adc18_chkid(id) != 2) return -1;  

   adc18p[id]->ctrl = 0;

   return 0;
}


int adc18_print_setup(int id) {
/*   Print the ADC setup   */

  int i;
  long nmax;
  unsigned long rdata;

  printf("ADC id %d   chkid = %d \n",id,adc18_chkid(id));

  if (adc18_chkid(id) < 1) {
       printf("ADC is not initalized !\n");
       return -1;
  }

  adc18_reg_status(id);
  
  adc18_reg_decode(id);

  nmax = adc18_getwordcnt(id);
  printf("Num of words in ADC %d \n",nmax);
 
  if (nmax > 5000) {
       printf("WARNING: num words too big !\n");
       nmax = 5000;
  }

  for (i = 0; i < nmax; i++) {
    rdata = adc18_getdata(id);    
    printf("Data [%d]  =  %d  =  0x%x \n",i,rdata,rdata);
  }

  return 0;

}

int adc18_setup(int id, int time, int intgain, int conv) {

  //adc18_init(id,0xed000,0xed100);  /* VME address of board */

  if (time < 0 || time > 163840) {
    printf("ERROR:  time must be 0 to 163840 usec \n");
    return -1;
  }
  if (intgain < 0 || intgain > 3) {
    printf("ERROR:  integain must be 0 to 3 \n");
    return -1;
  }
  if (conv < 0 || conv > 15 ) {
    printf("ERROR:  V-to-C conversion must be 0 to 15 \n");
    return -1;
  }
   
  adc18_reset(id);    
  adc18_timesrc(id,0,0, time);  
  adc18_gatemode(id,1);            
  adc18_intgain(id,intgain);  
  adc18_setconv(id,conv);

  adc18_print_setup(id);
}  


int adc18_defaultSetup(int id) {

  adc18_reset(id);
    
  adc18_timesrc(id,0,0,30000);     
  adc18_gatemode(id,1);            
  adc18_intgain(id,3);  
  adc18_setconv(id,0);
  adc18_setsample(id,0);

  adc18_print_setup(id);

  return 0;
}

void adc18_decode_data(int id, unsigned long data)
{
    unsigned long module_id, event_number, ch_number, divider, div_n, data_type;
    unsigned long diff_value, sample_number, raw_value, dac_value;
    long sign, difference, ii;
    double diff_avg;
    int lprint = 1;
		
    if( data & 0x80000000 ) {		/* header */
        module_id = (0x1F) & (data >> 26);
        event_number = data & 0x3FFFFFF;
        if( lprint )
          printf("\n**** HEADER (%LX)  ID = %LX  event number = %d\n", 
          		  data, module_id, event_number);

      } else {		 /* data */

        ch_number = (0x3) & (data >> 29);
        div_n = ( (0x3) & (data >> 25) );
        divider = 1;
        for(ii = 0; ii < div_n; ii++)
              divider = divider * 2;
        data_type = (0x7) & (data >> 22 );
          
        if( data_type == 4 ) {	/* DAC value */
            dac_value = 0xFFFF & data;
	    if( lprint )	        
	     	printf("     DAC (%LX)  value = %d\n", data, dac_value);	                   
	}
	    	        
	if( data_type == 0 ) {
	     diff_value = (0x1FFFFF) & data;
	     if( data & 0x200000 ) {
	        sign = -1;
	        difference = sign * ((~diff_value & 0x1FFFFF) + 1);	    /* 2's complement */
	     } else {
	        sign = 1;
	        difference = diff_value;
	     }
	     diff_avg = ((float)difference)/((float)divider);
	     if( lprint ) printf("     DIFF(peak-base) (%LX)  CH = %d  DIV = %d  SIGN = %d  DIFF/DIV = %.3f\n", 
	    	  data, ch_number, divider, sign, diff_avg);
	     
              } else if( data_type == 1 ) {

	         sample_number = (0xF) & (data >> 18);
	         raw_value = 0x3FFFF & data;
	         if( lprint ) printf("     RAW(base) (%LX)  CH = %d  sample = %d  value = %d\n", 
	    		      data, ch_number, sample_number, raw_value);	                   
	    	} else if( data_type == 2 ) {
	          sample_number = (0xF) & (data >> 18);
	          raw_value = 0x3FFFF & data;
	          if( lprint ) printf("     RAW(peak) (%LX)  CH = %d  sample = %d  value = %d\n", 
	    		     	data, ch_number, sample_number, raw_value);	                   
	    	}
      }
}

int adc18_flush(int id) {
  /* purpose: to flush the buffer */
  /* This ensures the 1st bit of CSR is zero */

  int i, timeout;
  unsigned long rdata;

  i = 0;
  timeout = 1000000;  /* max number of words allowed */

  if (adc18_chkid(id) < 1) {
       printf("ADC is not initalized !\n");
       return -1;
  }

  while (adc18_DataAvail(id)) {
      rdata = adc18_getdata(id);
      if (i++ > timeout) {
	printf("adc18_flush::ERROR:  Too much data %d %d  ?!\n",i,timeout);
        return -1;
      }
  }

}


int adc18_testdata(int id) {

  /* This is an example of how a trigger routine might work.
     Get one event, then print it out.  (A trigger routine
     would not print but only put data to a buffer.) */

  int i;
  long nmax;
  unsigned long rdata;

  if (adc18_chkid(id) < 1) {
       printf("ADC is not initalized !\n");
       return -1;
  }

  adc18_go(id);

  nmax = adc18_getwordcnt(id);
  printf("Num of words in ADC %d \n",nmax);
 
  if (nmax > 5000) {
       printf("WARNING: num words too big !\n");
       nmax = 5000;
  }

  for (i = 0; i < nmax; i++) {
    rdata = adc18_getdata(id);    
    printf("Data [%d]  =  %d  =  0x%x \n",i,rdata,rdata);
    adc18_decode_data(id, rdata);
  }

  return 0;

}


int adc18_testcsr1(int id) {
 int i=0;
 int j=0;
 int k=0;
 int l;
 int c;
 int nmax=0;
 int rdata;

 adc18_defaultSetup(id);

 c=adc18_getcsr(id);
 printf("c = %d \n",c);  /* this was printf(c) which led to compiler warning */

 while(i<10 && !(k>100)){
   if(j=0) adc18_go(id);

   if((adc18_getcsr(id))%2==1){ //if there is an event
     adc18_getcsr(id);
     nmax = adc18_getwordcnt(id);
     for (l = 0; l < nmax; l++) {
      rdata = adc18_getdata(id);    
      printf("Data [%d]  =  %d  =  0x%x \n",i,rdata,rdata);
     }
     i++;
     j=0;
     adc18_getcsr(id);
   }

   else{
     j=1;
     k++;
   }

 }
 adc18_getcsr(id);
 printf("this sucks more");
 return 0;
}

int adc18_test2() {
/* Bob's test of busy logic for id=0 */
/* this seems to work for internal timing but not quite right
   for external timing (see test3) */

  long csr; 
  int ramp, window, iinput;
  int ncnt,i,nloop,iev,rdata;  
  int timesrc;  /* 0 = external, 1 = internal */

/* this is like defaultSetup but with possible change in time source */

/*  adc18_init(0,0xed000,0xed100); */ /* VME address of board */
  adc18_reset(0);

  printf("What timing source ?  Enter 0 or 1  (0=ext, 1=int) \n");
  scanf("%d",&iinput);
  if (iinput == 0) {
    timesrc = 0;
  } else {
    timesrc = 1;
  }
    
  if (timesrc == 0) {
    printf("Using external timing board \n");
    adc18_timesrc(0,0,0,0);     /* External timing source */
  } else {
    printf("Using internal timing \nEnter ramp delay \n");
    scanf("%d",&ramp);
    printf("Enter window \n");
    scanf("%d",&window);
    printf("ramp = %d   window = %d \n",ramp,window);
    adc18_timesrc(0,1,ramp,window); /* Internal time; ramp & window are last 2 params */  
  }

  adc18_gatemode(0,1);            
  adc18_intgain(0,3);  
  adc18_setconv(0,0);

  /*  adc18_print_setup(0); */

  /* make sure the buffer is flushed */
  adc18_flush(0);

  csr=adc18_getcsr(0);
  printf("\n ******************** \n\n Initial CSR  = 0x%x\n",csr);

  for (iev = 0; iev < 100000; iev++) {

    printf("Enter any number when ready to get an event \n");
    scanf("%d",&iinput);

    adc18_softreset(0);

    adc18_go(0);

    csr=adc18_getcsr(0);
    printf("\n Before busy loop, CSR  = 0x%x\n",csr); 

    for (nloop = 0; nloop < 500000; nloop++) {
       /* WARNING: dont want the printf, it screws up the timing calib */
       /*       csr=adc18_getcsr(0);  */
       /*       printf("\n Busy loop %d    CSR  = 0x%x\n",nloop,csr); */
       if ( !adc18_Busy(0) ) break;
    }

    csr=adc18_getcsr(0);
    printf("\n After busy loop, loop size = %d   CSR  = 0x%x\n",nloop,csr);

    ncnt = adc18_getwordcnt(0);
    printf("Num of data in buffer = %d \n",ncnt);

    for (i = 0; i < ncnt; i++) {
      /*      printf("Data point # %d    Enter any number to continue \n",i);
      scanf("%d",&iinput);
      */ 

      rdata = adc18_getdata(0);
      csr = adc18_getcsr(0);

      printf("Data = 0x%x     Csr = 0x%x \n",rdata,csr);
 
    }

    csr=adc18_getcsr(0);
    printf("\n After we got all the data, CSR  = 0x%x\n",csr);

    printf("\n\n ---> Want to repeat the cycle ?  (yes=1, no=0) \n");
    scanf("%d",&iinput);

    if (iinput != 1) return 0;
  }  


}     

int adc18_oneshot() {
/* Purpose : to use the TIR board to send a single pulse out.
   This will be used to pulse the timing board for tests */

  int i,j;
  unsigned int tAddr, tirVersion;
  int oport_data, stat;
  unsigned long laddr;
  unsigned short rval;
  struct vme_tir {
      unsigned short tir_csr;
      unsigned short tir_vec;
      unsigned short tir_dat;
      unsigned short tir_oport;
      unsigned short tir_iport;
  };
  struct vme_tir *tir;

  tAddr = 0x0ed0;     /* address of TIR */
  oport_data = 2;     /* this is the bit(s) to put out on TIR */

  stat = 0;
  stat = sysBusToLocalAdrs(0x29,tAddr,&laddr);
  if (stat != 0) {
       printf("tirInit: ERROR: Error in sysBusToLocalAdrs res=%d \n",stat);
       return -1;
  } else {
    /* printf("TIR address = 0x%x\n",laddr); */
       tir = (struct vme_tir *)laddr;
  }
  stat = vxMemProbe((char *)laddr,0,2,(char *)&rval);
  if (stat != 0) {
      printf("tirInit: ERROR: TIR card not addressable\n");
      return -1;
  } else {
      tir->tir_csr = 0x80;
      tirVersion = (unsigned int)rval;
     /*  printf("tirInit: tirVersion = 0x%x\n",tirVersion); */
  }

  /* Toggle the output */
  tir->tir_oport = 0;               /*  off  */
  tir->tir_oport = oport_data;      /*  on   */
  for (i=0; i<10000; i++) {
    j = i*2;   /* do nothing, wait loop (5000 --> 50 usec) */
  }
  printf("i am here \n");
  tir->tir_oport = 0;               /*  off  */   

  return 0;
}

void adc18_pulser(int npulse) {
  /* to check out adc18_oneshot */
  int i,k,m;
  for (i=0; i< npulse; i++) {
    for (k = 0; k<50000; k++) {
      m = k*2;  /* wait loop */
    }
    adc18_oneshot();
  }
}


int adc18_test3(int rannum) {
/* This is for external timing only -- test of busy logic for id=0 */

  long csr; 
  int ramp, window, iinput;
  int ncnt,i,nloop,iev,rdata,dac16val;  

/* ADC18: this is like defaultSetup, for external timing  */

/*  adc18_init(0,0xed000,0xed100); */ /* VME address of board */
  adc18_reset(0);

  printf("test3: Using external timing board ... \n");
  adc18_timesrc(0,0,0,0);     /* External timing source */
  adc18_gatemode(0,1);            
  adc18_intgain(0,3);  
  adc18_setconv(0,0);

  /*  adc18_print_setup(0); */

  /* make sure the buffer is flushed */
  adc18_flush(0);

  //rannum must be between 0 and ~7,000
    dac16val = rannum+33000;
    
    setDACHAPTB(2,dac16val);

  csr=adc18_getcsr(0);
  printf("\n ******************** \n\n Initial CSR  = 0x%x\n",csr);

  for (iev = 0; iev < 100000; iev++) {

    printf("Enter any number whint adc18_getconv(ibrd,gainbit);
/*Get the Conv Gain*/ 
int adc18_getintgain(ibrd);en ready to get an event \n");
    scanf("%d",&iinput);

    adc18_softreset(0);

    adc18_go(0);

    adc18_oneshot();  /* get one trigger */

    csr=adc18_getcsr(0);
    printf("\n Before busy loop, CSR  = 0x%x\n",csr); 

    for (nloop = 0; nloop < 500000; nloop++) {
       /* WARNING: dont want the printf, it screws up the timing calib */
       /*       csr=adc18_getcsr(0);  */
       /*       printf("\n Busy loop %d    CSR  = 0x%x\n",nloop,csr); */
       if ( !adc18_Busy(0) ) break;
    }

    csr=adc18_getcsr(0);
    printf("\n After busy loop, loop size = %d   CSR  = 0x%x\n",nloop,csr);

    ncnt = adc18_getwordcnt(0);
    printf("Num of data in buffer = %d \n",ncnt);

    for (i = 0; i < ncnt; i++) {
      /*      printf("Data point # %d    Enter any number to continue \n",i);
      scanf("%d",&iinput);
      */ 

      rdata = adc18_getdata(0);
      csr = adc18_getcsr(0);

      printf("Data = 0x%x     Csr = 0x%x \n",rdata,csr);
 
    }

    csr=adc18_getcsr(0);
    printf("\n After we got all the data, CSR  = 0x%x\n",csr);

    printf("\n\n ---> Want to repeat the cycle ?  (yes=1, no=0) \n");
    scanf("%d",&iinput);

    if (iinput != 1) return 0;
  }  


}     


int adc18_test4(int sample, int oversample) {
/* Similar to test3, but using oversampling */

/* I'm too tired to continue now ... but here are some notes on
   how to proceed.

   Seems that "event count" nevt increments by number of oversamples
   on each MPS.  Also "event word count" nword is always 14 if there
   is one base & peak sample (this will grow with more samples
   of base,peak).  And the DataAvail becomes zero after 14, not
   after 14*N_oversample.  Hmmm...  Ed J. says one should read out
   N_oversample*nword and check header.  E.g. the event # in header
   should increment.  Header should have valid module id, if id=31
   it is invalid.  Invalid also has event=0 in header.   Whew ! */

  while (1){

  long csr; 
  int ramp, window, iinput;
  int nevt,nword,i,nloop,iev,ievloop,iword;
  int hdr_evt, hdr_modid;
  int oldevnum;
  unsigned long rdata;  
  int dataavail;
  int fullwindow;
  int count;
  int data;


/* ADC18: this is like defaultSetup, for external timing  */

  adc18_init(0,0xed000,0xed100);  /* VME address of board */
  adc18_reset(0);

  printf("test4: External timing with oversampling ... \n");
  adc18_timesrc(0,0,0,0);     /* External timing source */
  adc18_gatemode(0,1);            
  adc18_intgain(0,3);  
  adc18_setconv(0,0);
  adc18_setsample(0,sample);  /* 2nd arg is 1,2,4, or 8 */

  /* units: 2.5 usec */
  ramp = 400;
  fullwindow= 13000;
/* oversample=0 means no oversampling, `oversample N' means N+1 samples */  
  window=fullwindow/(oversample+1);

  printf("Ramp = %d    Window = %d    Oversample = %d \n",ramp,window,oversample);

  setTimeHAPTB(ramp,window);
  setOverSampleHAPTB(oversample); 

  /*  adc18_print_setup(0); */

  /* make sure the buffer is flushed */
  adc18_flush(0);

  csr=adc18_getcsr(0);
  printf("\n ******************** \n\n Initial CSR  = 0x%x\n",csr);

  oldevnum=0;
  count=0;

  //  for (iev = 0; iev < 10; iev++) { /*this is what was causing the test to run continuously previously i<10000*/

    printf("Enter any number when ready to get an event \n");
    scanf("%d",&iinput);

    adc18_softreset(0);

    adc18_go(0);

    adc18_oneshot();  /* get one trigger */

    csr=adc18_getcsr(0);
    printf("\n Before busy loop, CSR  = 0x%x\n",csr); 

    for (nloop = 0; nloop < 500000; nloop++) {
       /* WARNING: dont want the printf, it screws up the timing calib */
       /* csr=adc18_getcsr(0);  
	  printf("\n Busy loop %d    CSR  = 0x%x\n",nloop,csr); */
       if ( !adc18_Busy(0) ) break;
    }

 
    csr=adc18_getcsr(0);
    printf("\n After busy loop, loop size = %d   CSR  = 0x%x\n",nloop,csr);

    nevt = adc18_getevtcnt(0);
    nword = adc18_getwordcnt(0);
    printf("Num of events %d   oldevnum %d    num words %d \n",nevt,oldevnum,nword);

    while(adc18_DataAvail(0)){

	 printf("Evloop %d   Nword %d  Enter any number to continue \n",ievloop,iword);
	   scanf("%d",&iinput);
      
        rdata = adc18_getdata(0);
        csr = adc18_getcsr(0);
       	printf("Data = 0x%x     Csr = 0x%x \n",rdata,csr);
	
	count++;

	//	adc18_decode_data(0,rdata);

        if (rdata & 0x80000000) {
        
           hdr_modid = ((rdata & 0x7c000000)>>26);
           hdr_evt = (rdata & 0x3ffffff);
	   printf("Header in loop= 0x%x    modid = %d = 0x%x    event = %d 0x%x \n",
                 rdata,hdr_modid,hdr_modid,hdr_evt,hdr_evt);
        }

	if(count>50000)break;

      oldevnum = nevt;
      /************************************************************************************************************************
      rdata = adc18_getdata(0);/*something should fail( because the data should be flushed) here dont know what should fail...*
      csr = adc18_getcsr(0);
      printf("Data = 0x%x     Csr = 0x%x \n",rdata,csr);
      adc18_decode_data(0,rdata);

      hdr_modid = ((rdata & 0x7c000000)>>26);
      hdr_evt = (rdata & 0x3ffffff);
      printf("Header = 0x%x    modid = %d = 0x%x    event = %d 0x%x \n",rdata,hdr_modid,hdr_modid,hdr_evt,hdr_evt);
      ***************************************************************************************************************************/

    }

    printf("Counter=0x%x \n",count);
    data=(oversample+1)*(nword);
    if (count!=data)	{/*makes sure the "correct" amount of data was retrieved*/
      printf("\n bad \n");
    }

    csr=adc18_getcsr(0);
    printf("\n After we got all the data, CSR  = 0x%x\n",csr);
    scanf("%d",&iinput);
    printf("\n\n ---> Want to repeat the cycle1 ?  (yes=1, no=0) \n");
    scanf("%d",&iinput);

    if (iinput != 1) break;
}
return 0;
}


