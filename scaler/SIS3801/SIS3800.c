/************************************************************************
 *  SIS3800.c                                                           *
 *   subroutines which are used to interface with the                   *
 *   32 channel scaler card STRUCK model SIS3800                        *
 *                                                                      *  
 *   To be used when the scaler is set up as a 3800                     *
 *                                                                      *
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


/****************************************************************************/

#include "SIS3800.h" 


static struct SIS3800CSREG *scsm[NUMSCALERS+1];

/* my choice for initial csr */
unsigned long mycsr = 0x7000fd00;  


   
int initSIS3800()
{
  int isca, res;
  unsigned long laddr;

  for (isca=0; isca<NUMSCALERS; isca++){
    SISVMEADDR[isca] = 0;
    if(SISRELADDR[isca] != 0){
       res = sysBusToLocalAdrs(0x39, SISRELADDR[isca], &laddr);
       if (res != 0) {
          printf("initSIS3800: ERROR:  sysBusToLocalAdrs address %x\n",laddr);
        } else {
	  SISVMEADDR[isca] = laddr; 
	  printf("Scaler SIS3800[%d] address %x\n", isca,SISVMEADDR[isca]);
          scsm[isca] = (struct SIS3800CSREG *)SISVMEADDR[isca];
          scsm[isca]->reset=1;
          scsm[isca]->csr=mycsr;
          scsm[isca]->clear=1;
          enableClockSIS3800(isca);
    	  setControlInpModeSIS3800(1,isca);
 	  printf("Scaler SIS3800[%d] started\n", isca);
	}
    }
  }
  return 0;
}

/***************************************************************************************
 *  enableClockSIS3800(isca)            : enable next clock logic  
 *       input parameters     : isca -- number of the scaler (see the 
 *                              SIS3801.h to know which number correspond 
 *                              to which scaler. 
 *
 *       return              : 0 
 */
int enableClockSIS3800(int isca) 
{
  scsm[isca]->enclk=1;
  return 0;
 }


/***************************************************************************************
 *  disableClockSIS3800(isca)           : disable next clock logic
 *       input parameters     : isca -- number of the scaler (see the 
 *                              SIS3801.h to know which number correspond 
 *                              to which scaler. 
 *       return              : 0 
 *
 *     Notes           :  This is the state after Power on or KEY_RESET 
 */
int disableClockSIS3800(int isca) 
{
  scsm[isca]->disclk=1;
  return 0;
 }



/************************************************************************
 * 
 *  dump registers to screen
 */
int dump3800(int isca)
{
  int i,j;

  printf("ADDR(%#8x) : csr register        : %#8x \n", &scsm[isca]->csr,scsm[isca]->csr)  ; 
  printf("ADDR(%#8x) : VME IRQ control reg : %#8x \n", &scsm[isca]->IntVerR,scsm[isca]->IntVerR ) ; 

  
  printf("Here are 32 Channels:\n");  
  j=0;
  for(i=0;i<8;i++) {
    printf("%d to %d :  %d %d %d %d \n",
                 j,j+4,scsm[isca]->readCounter[j],scsm[isca]->readCounter[j+1],
                 scsm[isca]->readCounter[j+2],scsm[isca]->readCounter[j+3]) ;   
    j+=4;
  }

 return;
 }



/************************************************************************/

/* 
 *  dump registers to screen
 *  But only those registers which are readable.
 */
int dumpcsr3800(int isca)
{

  printf("ADDR(%#8x) : csr register        : %#8x \n", &scsm[isca]->csr,scsm[0]->csr)  ; 
  printf("ADDR(%#8x) : VME IRQ control reg : %#8x \n", &scsm[isca]->IntVerR,scsm[0]->IntVerR ) ; 
 }
  
/*******************************************************************************
 *   setControlInpMode(int inpmode)  : set configuration for control input pins
 *              input parameters     : inpmode - type of Mode 0-3
 *              return               : 0
 *
 *  
 */
 int setControlInpModeSIS3800(int inpmode, int isca)
 { 
  int temp;
  
  if ((inpmode>3)||(inpmode<0)) return ERROR;
  temp=inpmode<<2;
  scsm[isca]->csr=0x00000C00; /* clear Mode bits to Mode 0*/
  scsm[isca]->csr=temp;       /* set new Mode */
  return 0;
 }

 
  
/*******************************************************************************
*   Read3800(header, ichan)  : Read the 3800 scalers
*              input parameters     : header = VME offset,
*                                   : ichan = channel number
*              return               : data if header & chan valid
*                                   : or zero if not valid.
*
*/

int Read3800 (int header, int ichan) {

  int isca,iscafound;

  if (ichan < 0 || ichan >= 32) return 0;

  iscafound = -1;
  for (isca = 0; isca < NUMSCALERS; isca++) {
    if (header == SISRELADDR[isca]) {
      iscafound = isca;
    }
  }

  if (iscafound == -1) return 0;

  return scsm[iscafound]->readCounter[ichan]; 

}

 


/*******************************************************************************
*   ReadSIS3800(isca)  : Read Counters (routine called by coda. 
*              input parameters     : isca, the scaler number
*              return               : 32 counter contents for each scaler
* 
*  (NOTE: this is the 'old' version of ReadSIS3800, not recommended) 
* 
*  Up to 3 scalers are definable.  The buffer "Buffer3800N[]" where
*  N = 1,2,3 is filled.  Which buffer gets filled is determined by
*  the vme addresses in SIS3800.h.  For example, if the second address
*  is the only which is different from 0, then Buffer38002 is
*  filled.  This will probably be how we run the non-helicity-gated
*  scaler (2nd in the bank of 3). 
*  
*/
void ReadSIS3800(int isca){ 
  int icha;
  
    if(SISVMEADDR[isca] != 0){
       
	 switch (isca){
	 
	  case 0:
	    for (icha=0;icha<32;icha++)
            Buffer38001[icha]=scsm[isca]->readCounter[icha];
          break;
	 
	  case 1:   
	    for (icha=0;icha<32;icha++)
	    Buffer38002[icha]=scsm[isca]->readCounter[icha];
          break;
	 
	  case 2:   
	    for (icha=0;icha<32;icha++)
            Buffer38003[icha]=scsm[isca]->readCounter[icha];
          break;
       }
  }
}

/*******************************************************************************
*   ClrSIS3800()  : Clear all Counters. 
*              input parameters     : none
*              return               : 
*
*  
*/
void ClrSIS3800(void){ 
  int isca;
  
  for (isca=0; isca<NUMSCALERS; isca++){
    if(SISVMEADDR[isca] != 0) scsm[isca]->clear=1;
  }
}

/*******************************************************************************
*   EnableBroad3800()  : Clear all Counters. 
*              input parameters     : none
*              return               : 
*
*  
*/
void EnableBroad3800(void){ 
  int isca,flag;
  
  flag=0;
  for (isca=0; isca<NUMSCALERS; isca++){
    if(SISVMEADDR[isca] != 0) {
       if(flag++==0) {
	    scsm[isca]->csr=0xc0;
	 }else{
	    scsm[isca]->csr=0x40;
	 } 
    }
  }
}

/*******************************************************************************
*   ClrBroad3800()  : Clear all Counters. 
*              input parameters     : none
*              return               : 
*
*  
*/
void ClrBroad3800(void){ 

static short *clear3800;
   
   clear3800 = 0xc1cb0030;
   *clear3800 = 1;
  
}

/*******************************************************************************
*   EnableClock3800()  : Clear all Counters. 
*              input parameters     : none
*              return               : 
*
*  
*/
void EnableClock3800(void){ 

static short *enableclock;
   
    enableclock= 0xc1cb0038;
   *enableclock= 1;
  
}

/*******************************************************************************
*   DisableClock3800()  : Clear all Counters. 
*              input parameters     : none
*              return               : 
*
*  
*/
void DisableClock3800(void){ 

static short *disableclock;
   
    disableclock= 0xc1cb003c;
   *disableclock= 1;
  
}

