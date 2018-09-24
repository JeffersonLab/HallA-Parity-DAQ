/************************************************************************
 *  HAPADC.c                                                            *
 *   subroutines which are used to interface with the                   *
 *   HAPPEX ADC for test purposes                                       *
 *                                                                      *  
 * Date  : R. Michaels/ A. Vacheret  October 2000                       *   
 * Modifications:                                                       *
 *                                                                      *
 ************************************************************************/

#include <vxWorks.h>
#include <types.h>
#include <vme.h>
#include <stdioLib.h>
#include <logLib.h>
#include <math.h>
#include <string.h>
#include <ioLib.h>

#include "HAPADC.h" 


/* Definition of csr bits */
/*
    bits 0-1 = diagnostic mux (0,1,2,3 -> which ADC to send data to outputs)
    bit 2    = enable interrupts
    bit 3    = enable RNG
    bit 4    = integrator select range
    bit 5-7  = not used

   7 6 5 4 3 2 1 0 
   0 0 0 x 0 0 y z    x = hi/lo = 1/0   (z,y) = 0,1,2,3 

   example:  hi gain, mux=2 (3rd) -> mycsr = 0x12;   
*/


////////////////////////////////////////////////////////
//
//  Initialize all HAPPEX ADCs by defining their addresses
//  and setting each CSR to default values
//
////////////////////////////////////////////////////////
int initHAPADC()
{
   int vmeaddr;
   int i;
   unsigned long laddr;
   int res;
   int csrval;

   printf("Num ADC16 %d \n",NADC);

   /* ADC Pointers */
   for (i = 0; i < NADC; i++) {
     did_init[i]=-1;
     res = sysBusToLocalAdrs(0x39,ADCADDR[i],&laddr);
     printf("ADC16 # %d   Rel. addr = 0x%x    VME addr = 0x%x \n",i,ADCADDR[i],laddr);
     if (res != 0) {
       printf("Error in sysBusToLocalAdrs res=%d \n",res);
       printf("ERROR:  ADC num %d POINTER NOT INITIALIZED",i);
     } else {
       printf("HAPPEX ADC num %d address = 0x%x ",i,laddr);
       adcbrds[i] = (struct vme_happex_adc *)laddr;
       // create csr
       csrval = 0;
       csrval |= ADC_GAINBIT[i];
       csrval |= ADC_MUXBIT[i];     
       csrval |= ADC_DACBIT[i];     
       // apply default csr to this adc
       did_init[i]=writeCSRHAPADC(i,csrval);
       setDACHAPADC(i,DEF_DACVAL);
       dacvalue[i] = DEF_DACVAL;
     }
   }
   return 0;
}

////////////////////////////////////////////////////////
//
// writeCSRverboseHAPADC(int ibrd, int newcsr)
//  Argument 'newcsr' is written to the CSR of the ADC
//  with index 'ibrd'
//
////////////////////////////////////////////////////////
int writeCSRverboseHAPADC(int ibrd, int newcsr)
{
  int chkcsr,retry;
  if (ibrd>=NADC || ibrd<0) return -1;
  if (CODA_RUN_IN_PROGRESS==1) {
    printf("What, are you crazy?  There's a run in progress!");
    return -2;
  }
  chkcsr = -1;
  retry = -1;
  while (newcsr != chkcsr && retry<MAXRETRY) { 
    // Make sure the correct CSR is written
    adcbrds[ibrd]->csr = newcsr;
    chkcsr = (adcbrds[ibrd]->csr)&0x00FF;
    retry++;
  }
  if (retry<MAXRETRY) {
    printf("HAPPEX ADC %d  programmed with csr = %x, with %d retries\n",
  	   ibrd, newcsr,retry);
    return 1;
  } else {
    printf("HAPPEX ADC %d  did not accept csr write, with %d retries\n",
	   ibrd, retry);
    return -1;
  }
}

////////////////////////////////////////////////////////
//
// writeCSRHAPADC(int ibrd, int newcsr)
//  Argument 'newcsr' is written to the CSR of the ADC
//  with index 'ibrd'
//
////////////////////////////////////////////////////////
LOCAL int writeCSRHAPADC(int ibrd, int newcsr)
{
  int chkcsr,retry;
  if (ibrd>=NADC || ibrd<0) return -1;
  chkcsr = -1;
  retry = -1;
  if (CODA_RUN_IN_PROGRESS==1) return -2;
  while (newcsr != chkcsr && retry<MAXRETRY) { 
    // Make sure the correct CSR is written
    adcbrds[ibrd]->csr = newcsr;
    chkcsr = (adcbrds[ibrd]->csr)&0x00FF;
    retry++;
  }
  if (retry<MAXRETRY) {
    printf("HAPPEX ADC %d  programmed with csr = %x, with %d retries\n",
  	   ibrd, newcsr,retry);
    return 1;
  } else {
    printf("HAPPEX ADC %d  did not accept csr write, with %d retries\n",
	   ibrd, retry);
    return -1;
  }
}

////////////////////////////////////////////////////////
//  
//    dump registers to screen, 
//       if ibrd = -1, dump all registers to screen
// 
////////////////////////////////////////////////////////
int dumpHAPADC(int ibrd)
{
  int i;
  int dmux, gainbit;
  if (ibrd>=0 && ibrd<NADC) {
    if ( did_init[ibrd]==-1 ) {
      printf("Cannot dump ADC %d.  It was not initialized\n",ibrd);
      printf("Did you call initHAPADC ?\n");
      return -1;
    }
    printf("\n Dump of ADC %d registers:\n",ibrd);
    for (i=0; i<4; i++) {  
      printf("channel %d   data = %d (dec)   = 0x%x (hex) \n",
	     i,adcbrds[ibrd]->adcchan[i],adcbrds[ibrd]->adcchan[i]);
    }
    dmux = adcbrds[ibrd]->csr & 0x3;
    gainbit = adcbrds[ibrd]->csr & 0x10;
    printf("csr = 0x%x \n",adcbrds[ibrd]->csr);
    printf("rng = 0x%x \n",adcbrds[ibrd]->rng);
    printf("done = 0x%x \n",adcbrds[ibrd]->done);
    printf("Diagnostic Mux = %d \n",dmux);
    printf("Gain bit (low=0/hi=1) = %d \n",gainbit / HI_GAIN);
    return 1;
  } else if (ibrd==-1) {
    printf("\n Dump of all ADC registers:\n\n");
    for (i=0; i<NADC; i++) {
      if (!(did_init[i]==-1)) {
	dumpHAPADC(i);
	printf("\n\n");
      }
    }
    return 1;
  }
  return -1;
}

////////////////////////////////////////////////////////
//  
//    dump registers to screen, 
//       if ibrd = -1, dump all registers to screen
// 
////////////////////////////////////////////////////////
int showHAPADC(int ibrd)
{
  int i;
  int dmux, gainbit;
  if (ibrd>=0 && ibrd<NADC) {
    if ( did_init[ibrd]==-1 ) {
      printf("   ADC %d was not initialized\n",ibrd);
      return -1;
    }
    printf("\n   Configuration of ADC %d:\n",ibrd);
    dmux = adcbrds[ibrd]->csr & 0x3;
    gainbit = adcbrds[ibrd]->csr & 0x10;
    printf("csr = 0x%x \n",adcbrds[ibrd]->csr);
    printf("Diagnostic Mux = %d \n",dmux);
    printf("Gain bit (low=0/hi=1) = %d \n",gainbit / HI_GAIN);
    return 1;
  } else if (ibrd==-1) {
    printf("\n Configuration of all ADCs:\n");
    for (i=0; i<NADC; i++) {
      if (!(did_init[i]==-1)) {
	showHAPADC(i);
      }
    }
    printf("\n");
    return 1;
  }
  return -1;
}


////////////////////////////////////////////////////////
// 
//  insert data into 6 consectutive memory locations, used for readout
//     IMPORTANT: interupt service routines (ISR) cannot produce any
//                screen i/o
//
////////////////////////////////////////////////////////
unsigned long* readoutHAPADC(unsigned long* dabufp, int ibrd)
{
  int j;
  int timeout, donetest;

  if (did_init[ibrd]!=-1) {
//    timeout = 0;
//    donetest = 0;
//      while ( donetest ) {
//        // CSRbit 1=>  1=not.done,0=done: this is opposite to description from 
//        // Wilson's thesis, so maybe this is just a timing thing.
//        donetest = (adcbrds[ibrd]->done & 2);  
//        if (timeout++ >= ADC_TIMEOUT) break;
//      }
//    if (timeout < ADC_TIMEOUT) {
      /* Reshuffle the ADC channels into a more logical order and remap to avoid the gap */
      *dabufp++ = (adcbrds[ibrd]->adcchan[0]+0x8000) & 0xffff;
      *dabufp++ = (adcbrds[ibrd]->adcchan[2]+0x8000) & 0xffff;
      *dabufp++ = (adcbrds[ibrd]->adcchan[1]+0x8000) & 0xffff;
      *dabufp++ = (adcbrds[ibrd]->adcchan[3]+0x8000) & 0xffff;
      *dabufp++ = adcbrds[ibrd]->csr & 0xffff;
      *dabufp++ = dacvalue[ibrd] & 0xffff;       /* adc->dac is write only */
      return dabufp;
//    } 
  }
  // if haven't returned, then not initialized or timeout
  for (j = 0; j < 6; j++) *dabufp++ = -1;
  return dabufp;
}

////////////////////////////////////////////////////////
// 
//  get the present dac value 
//
////////////////////////////////////////////////////////
int getDACHAPADC(int ibrd) {
  if (ibrd < 0 || ibrd >= NADC) return 0;
  return dacvalue[ibrd];
} 

////////////////////////////////////////////////////////
// 
//  get the present adc data, using the "logical" 
//  remapping of channels mentioned in readoutHAPADC
//
////////////////////////////////////////////////////////
long getOneADC16(int ibrd, int ichan) {
  int remap[4];
  if (ibrd < 0 || ibrd >= NADC) return 0;
  if (ichan < 0 || ichan > 3) return 0;
  remap[0] = 0;  remap[1]=2;  remap[2]=1;  remap[3]=3;
  return (adcbrds[ibrd]->adcchan[remap[ichan]]+0x8000) & 0xffff;
}

////////////////////////////////////////////////////////
// 
//  set noise dac on a single adc
//   if dac bit is off, only default DAC value is applied!
//
////////////////////////////////////////////////////////
int setDACHAPADC(int ibrd, int newval)
{
  int shortdelay;

  if (did_init[ibrd]==-1) {
    return -1;
  } 
  // if dac_noise is off, don't change the dac value
  if (ADC_DACBIT[ibrd]==DAC_OFF) newval = DEF_DACVAL;  
  dacvalue[ibrd] = newval;
  adcbrds[ibrd]->dac = dacvalue[ibrd];
  shortdelay = adcbrds[ibrd]->csr; // Need for DACnoise to work on ALL ADCs.
  // this is a curious thing.  It appears that the write buffer is not
  // necessarily flushed, but the READ forces a flush of the bus i/o buffer.
  // so a READ is necessary to be sure that the WRITE takes... 
}

////////////////////////////////////////////////////////
// 
//  setAllDACBitHAPADC(int flag)
//    flag = 1 -> DAC forced ON
//    flag = 0 -> DAC forced OFF
//
////////////////////////////////////////////////////////
int setAllDACBitHAPADC(int flag)
{
  int i;
  if (CODA_RUN_IN_PROGRESS==1) return -2;
  for (i=0;i<NADC;i++) setDACBitHAPADC(i, flag);
  return 1;
}      

////////////////////////////////////////////////////////
// 
//  setDACBitHAPADC(int ibrd, int flag)
//    flag = 1 -> DAC forced ON
//    flag = 0 -> DAC forced OFF
//    else : no change
//
////////////////////////////////////////////////////////
int setDACBitHAPADC(int ibrd, int flag)
{
  int csrval;
  if (ibrd<0 || ibrd>=NADC) return -1;
  if (CODA_RUN_IN_PROGRESS==1) return -2;
  if (flag==1)  ADC_DACBIT[ibrd] = DAC_ON;
  if (flag==0)  ADC_DACBIT[ibrd] = DAC_OFF;
  // create csr
  csrval = 0;
  csrval |= ADC_GAINBIT[ibrd];
  csrval |= ADC_MUXBIT[ibrd];     
  csrval |= ADC_DACBIT[ibrd];     
  setDACHAPADC(ibrd,DEF_DACVAL);
  return writeCSRHAPADC(ibrd,csrval);
}


////////////////////////////////////////////////////////
// 
//  usesDACHAPADC(int ibrd)
//    returns 1 if this adc uses dac noise, 0 otherwise
//
////////////////////////////////////////////////////////
int usesDACHAPADC(int ibrd)
{
  if (ibrd<0 || ibrd>=NADC) return -1;
  if (ADC_DACBIT[ibrd]==DAC_ON) return 1;
  return 0;
}

////////////////////////////////////////////////////////
// 
//  setGainBitHAPADC(int ibrd, int flag)
//    flag = 1 -> high gain
//    flag = 0 -> lo gain
//    else : no change
//
////////////////////////////////////////////////////////
int setGainBitHAPADC(int ibrd, int flag)
{
  int csrval;
  if (ibrd<0 || ibrd>=NADC) return -1;
  if (CODA_RUN_IN_PROGRESS==1) return -2;
  if (flag==1)  ADC_GAINBIT[ibrd] = HI_GAIN;
  if (flag==0)  ADC_GAINBIT[ibrd] = LO_GAIN;
  // create csr
  csrval = 0;
  csrval |= ADC_GAINBIT[ibrd];
  csrval |= ADC_MUXBIT[ibrd];     
  csrval |= ADC_DACBIT[ibrd];     
  return writeCSRHAPADC(ibrd,csrval);
}

////////////////////////////////////////////////////////
// 
//  setAllGainBitHAPADC(int flag)
//    flag = 1 -> high gain
//    flag = 0 -> lo gain
//    else : no change
//
////////////////////////////////////////////////////////
int setAllGainBitHAPADC(int ibrd, int flag)
{
  int i;
  if (CODA_RUN_IN_PROGRESS==1) return -2;
  for (i=0;i<NADC;i++) setGainBitHAPADC(i, flag);
  return 1;
}      




////////////////////////////////////////////////////////
//  
//    return csr of a specific adc, 
//       necessary for config interface
//       if ibrd = -1, dump all registers to screen
// 
////////////////////////////////////////////////////////
long getCSRHAPADC(int ibrd)
{
  if (ibrd>=0 && ibrd<NADC && did_init[ibrd]!=-1 ) return adcbrds[ibrd]->csr;
  return -1;
}

////////////////////////////////////////////////////////
//  
//    return number of ADCs
//       necessary for config interface
// 
////////////////////////////////////////////////////////
int getNumHAPADC()
{
  return NADC;
}

////////////////////////////////////////////////////////
//  
//    return label of an ADC
//       necessary for config interface
// 
////////////////////////////////////////////////////////
int getLabelHAPADC(int ibrd)
{
  if (ibrd>=0 && ibrd<NADC && did_init[ibrd]!=-1) return ADCLABEL[ibrd];
  return -1;
}



