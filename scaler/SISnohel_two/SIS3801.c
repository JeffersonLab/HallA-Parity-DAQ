/************************************************************************
 *  SIS3801.c                                                           *
 *   subroutines which are used to interface with the                   *
 *   32 channel scaler card STRUCK model SIS3801                        *
 *                                                                      *  
 * Date  : January 22 99                                                *   
 * Modifications:                                                       *
 * 01/28/99    R.M.  Some renaming of address map, a few corrections,   *
 *                  replace STR->SIS, throw out old code;   NOTE:       *
 *                  I put "version 0" in SIS3801_v0.c (vers 0)          *
 *                                                                      *
 * 01/29/99    R.M.  New addressing scheme.  Seems FIFO likes to        *
 *                  be read with A32.                                   *
 *                                                                      *
 * 02/04/99    R.T.  Add routines which  test the FIFO operation        *
 *                  in the "parity mode". "testInt" and "testExt"       *
 *                  test the FIFO with internal and external pulser     *
 *                  respectively.                                       *
 *                                                                      *
 * 03/15/99    R.T.  Remove the test routine and rename some routine    *
 *                   Init -> initSIS,  etc...                           *
 *                   And make the routines to be used with more than one*
 *                   scaler.                                            *
 *                   With the PPC Board, we do not need the double      *
 *                   addressing scheme (A24 and A32 for the FIFO read)  *
 *                                                                      *
 * 03/16/99    R.T.  Add the Producer SISProducer and the interrupt     *
 *                   service routine SISInterrupt.                      *
 *                   Add the initialization of the ISR initISR_SIS      *
 *                                                                      *
 * 06/07/99    R.M.  Make one routine for SISFIFO_Read reading scaler;  *
 *                   it is shared by SISInterrupt and SISProducer.      *
 *                   This routine also calls SISintime or SISouttime    *
 *                   to load the scaler data into variable arrays,      *
 *                   depending on if the scalers are setup with in-time *
 *                   or out-of-time helicity information.               *
 *                   SISHel_Chk_Load routine added for checking helicity*
 *                   For OUT_OF_TIME_MODE, the pseudorandom helicity    *
 *                   prediction is used to check the delayed helicity   *
 *                   Added SISendrun() to flush buffers for out-of-time *
 *                   mode at end of a CODA run.                         *
 *                                                                      *
 * 7/20/99      R.T. Made the out-of-time mode work. We think.  Several *
 *                   flags added to interact with CODA end of run       *
 *                                                                      *
 * 9/25/01      R.M. Add a method Read3801(header, ichan) to simplify   *
 *                   clients.  At the moment it works in-time.  Make    *
 *                   SISProducer skip readout if CODA is running, since *
 *                   CODA's TS will do the readout using SISInterrupt   *
 *                                                                      *
 * 7/25/02      R.M. Use G0 Helicity structure, which has quartets and  *
 *                   a different random helicity routine                *
 *                                                                      *
 * 8/20/02      R.M. Massive simplification.  Eliminate SIS*buffer      *
 *                   and SIS*Cumulative, etc.  Keep SIS*Plus/Minus      *
 *                   cumulatives.  Mostly no args passed to functions,  *
 *                   rely mostly on globals.  Eliminate uneeded flags.  *
 *                   Interface to 3801 changed to                       *
 *                   int Read3801(int iscaler, int ihelicity, int ichan)*
 *                                                                      *
 * 3/05/03      R.M. Eliminate SISintime() and SISouttime(), create     *
 *                   SISDataPkg() to fill ring buffer and to package    *
 *                   the data online by helicity                        *
 *                                                                      *
 *10/22/03      R.M. Start using sysBusToLocalAddrs to get the VME      *
 *                   offsets.  Eliminated 1 of the 2 redundant scalers. *
 *                   The 2nd scaler was SIS2 and was easily turned      *
 *                   off by setting NUMSCALERS = 1 in SIS3801.h         *
 *                                                                      *
 * 1/6/06       R.M. System of globals (scaDebug, scaNbError, etc) to   *
 *                   facilitate debugging while deployed.               *
 *                                                                      *
 * 1/6/06       R.M. Let long_start_flag do the job of pointing to      *
 *                   num_read, while start_flag is a flag as before     *
 *                   (problem: when recovery started for large num_read *
 *                    start_flag would pick up a sign since it was a    *
 *                    short int !!!).                                   *
 *                                                                      *
 * 7/29/10      R.M. Updated ranBit and loadHelicity for new 30-bit     *
 *                   helicity shift register for Qweak era.             *
 *                                                                      *
 * Dec 2015     R.M.  Greatly simplifed version with no helicity        *
 *                    sorting.                                          *
 *                                                                      *
 * March 2019   C.C. Reintroduced 2nd redundant scaler implementation   *
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
#include <time.h>

//#define INTENABLE 1


/****************************************************************************/

#include "SIS.h"
#include "SIS3801.h"

/* Externals variables used by CODA */
extern int rocActivate;   /* Means ROC is activated (set by "go") */
/*  = 0 --> not active */
/*  = +1 --> active     */
/*  = -1 --> can load words but not increment SIS counts */

extern int scaDebug;  /* to debug scalers */

struct fifo_scaler *pSIS[NUMSCALERS];
struct fifo_scaler *sfs;

/* my choice for initial CSR */
unsigned long mycsr = 0xff00fc00;  

/* SIS1 and SIS2 are two redundant scalers.  SIS2 was removed Sept 2003.
        SIS1Last  == last reading of SIS1.  similar for SIS2
        SIS1plus  == helicity plus data for SIS1.
        SIS1minus == helicity minus data for SIS1
*/

struct SISbuffer SIS1;
struct SISbuffer SIS1Last;
struct SISbuffer SIS1Previous;
struct SISbuffer SIS2;
struct SISbuffer SIS2Last;
struct SISbuffer SIS2Previous;

/*  Global variables */

int nb_error;
unsigned  long num_read[NUMSCALERS];
unsigned  long client_read;
unsigned  long num_check;
unsigned  long last_bad = 0;

int waitime;

int error_cond;           /* error condition of present reading   */


void SISInttest (int arg)
{
  int ii;

  if((sfs->csr)&FIFO_EMPTY) {
    /* FIFO is empty */
    logMsg("\nscalIntUser: FIFO Empty\n",0,0,0,0,0,0);
  } else {
    logMsg("\nscalIntUser: FIFO NOT Empty\n",0,0,0,0,0,0);
  }
}
 

/***********************************************************************
 *                                                                     *
 * SISInterrupt()   --  Interrupt service routine                      *
 *                                                                     *
 *  If the scaler FIFO becomes too full (generally > 50%, depending on *
 *  the setup), it will call this ISR which will readout the buffer.   *
 *  The same readout routines as in SISProducer are used. The idea is  *
 *  that if SISProducer is too slow, this routine will keep up with    *
 *  the FIFO.                                                          *
 *                                                                     *
 **********************************************************************/

void  SISInterrupt(int arg) {
   
  int isca;
  int debug;
  int not_empty;

  debug = arg;
   
  if(debug) logMsg("In SIS3801 Interrupt service routine\n",0,0,0,0,0,0);
 

/* Update the FIFO NOT_EMPTY flag which is 0 if all FIFO scalers are empty */      

  not_empty =0;
  for(isca=0;isca<NUMSCALERS;isca++){
      if ((SISVMEADDR[isca] != 0) &&
         ((pSIS[isca]->csr&FIFO_EMPTY) == 0))
	   not_empty++;
  }
   
  while(not_empty != 0) {

       not_empty = SISFIFO_Read();

  }

}
 
 
 
 /***************************************************************************************
 *  initISR_SIS()            : Init the interrupt service routine 
 *       input parameters    : none    
 *       return              : 0 
 */

int initISR_SIS()
{
  int isca;
#ifdef INTENABLE

  for (isca=0; isca<NUMSCALERS; isca++){
     if(SISRELADDR[isca] != 0){
	 scalIntInit(SISRELADDR[isca]);
	 scalIntConnect(SISInterrupt,DEBUG_INT);
	 scalIntEnable(INTERRUPT_FLAG);
	 printf("Interrupt Service routine initialized using scaler %d\n",isca);
	 break;
     }
  }
#endif
  return 0;
}

  
/***************************************************************************************
 *  initSIS()                : Init SIS3801 cards
 *       input parameters    : none    
 *       return              : 0 
 */

int initSIS()
{
  int isca, res;
  unsigned long laddr;

  for (isca=0; isca<NUMSCALERS; isca++){
    SISVMEADDR[isca] = 0;
    if(SISRELADDR[isca] != 0){
       res = sysBusToLocalAdrs(0x39, SISRELADDR[isca], &laddr);
       if (res != 0) {
          printf("initSIS3801: ERROR:  sysBusToLocalAdrs address %x\n",laddr);
        } else {
	  SISVMEADDR[isca] = laddr; 
          pSIS[isca] = (struct fifo_scaler *)SISVMEADDR[isca];
          pSIS[isca]->reset=1;
          pSIS[isca]->csr=mycsr;
          pSIS[isca]->clear=1;
          enableClockSIS(isca);
	  setControlInpMode(2,isca);
          enableExtNextSIS(isca);
          enableExtDisSIS(isca);
          printf("initSIS in SIS3801[%d] at addr %x \n", isca,SISVMEADDR[isca]);  
	}
     }
  }

  return 0;
}


/***************************************************************************************
 *  clrAllCntSIS()           : clear all counters , and overflow bit
 *       input parameters    : none    
 *       return              : 0 
 */

int clrAllCntSIS()
{
  int isca;
  for (isca=0;isca<NUMSCALERS; isca++) {
	if(SISVMEADDR[isca] != 0) pSIS[isca]->clear=1;
  }
  return 0;
 }


/*********************************************************************
 *  vmeNext3801(isca)                : VME next clock (twice)
 *       input parameters     : isca -- number of the scaler 
 *  

 *          return           : 0
 */
int vmeNext3801(int isca)
{
  pSIS[isca]->next=1;
  return 0;
} 


/*********************************************************************
 *  dump registers to screen
 *  But only those registers which are readable.
 */
int dumpCSR(int isca)
{

  printf("ADDR(%#8x) : CSR register        : %#8x \n", &sfs->csr,sfs->csr)  ; 
  printf("ADDR(%#8x) : VME IRQ control reg : %#8x \n", &sfs->irq,sfs->irq ) ; 
printf("  -------\n");
  printf("ADDR(%#8x) : CSR register        : %#8x \n", &pSIS[isca]->csr,pSIS[isca]->csr)  ; 
  printf("ADDR(%#8x) : VME IRQ control reg : %#8x \n", &pSIS[isca]->irq,pSIS[isca]->irq ) ; 
 }
  
/*******************************************************************************
 *   setControlInpMode(inpmode,isca)  : set configuration for control input pins
 *              input parameters     : inpmode -- type of Mode 0-3
 *                                   : isca -- number of the scaler (see the 
 *                                     SIS3801.h to know which number correspond 
 *                                     to which scaler. 
 *              return               : 0
 *
 */
 int setControlInpMode(int inpmode, int isca)
 { 
  int temp;
  
  if ((inpmode>3)||(inpmode<0)) return ERROR;
  temp=inpmode<<2;
  pSIS[isca]->csr=0x00000C00; /* clear Mode bits to Mode 0*/
  pSIS[isca]->csr=temp;       /* set new Mode */
  return 0;
 }



  
  

/***************************************************************************************
 *  enableClockSIS(isca)            : enable next clock logic  
 *       input parameters     : isca -- number of the scaler (see the 
 *                              SIS3801.h to know which number correspond 
 *                              to which scaler. 
 *
 *       return              : 0 
 */
int enableClockSIS(int isca) 
{
  pSIS[isca]->enable=1;
  return 0;
 }


/***************************************************************************************
 *  disableClockSIS(isca)           : disable next clock logic
 *       input parameters     : isca -- number of the scaler (see the 
 *                              SIS3801.h to know which number correspond 
 *                              to which scaler. 
 *       return              : 0 
 *
 *     Notes           :  This is the state after Power on or KEY_RESET 
 */
int disableClockSIS(int isca) 
{
  pSIS[isca]->disable=1;
  return 0;
 }

/*******************************************************************************
 *   enableExtNextSIS(isca)  : Enable External Next Clock
 *       input parameters     : isca -- number of the scaler 
 *  
 *              return               : 0
 *
 */
 int enableExtNextSIS(int isca)
 { 
  pSIS[isca]->csr= 0x10000; 
  return 0;
 }


/*******************************************************************************
 *   disableExtNextSIS(isca)  : Disable External Next Clock 
 *       input parameters     : isca -- number of the scaler 
 *  
 *              return               : 0
 *
 */
 int disableExtNextSIS(int isca)
 { 
  pSIS[isca]->csr= 0x1000000; 
  return 0;
 }

 /*******************************************************************************
 *   enableExtDisSIS(isca)  : Enable External Disable
 *       input parameters     : isca -- number of the scaler 
 *  
 *              return               : 0
 *  
 */
 int enableExtDisSIS(int isca)
 { 
  pSIS[isca]->csr=0x40000; 
  return 0;
 }


/*******************************************************************************
 *   disableExtDisSIS(isca)  : Disable External Disable 
 *       input parameters     : isca -- number of the scaler 
 *  
 *              return               : 0
 *
 */
 int disableExtDisSIS(int isca)
 { 
  pSIS[isca]->csr=0x4000000; 
  return 0;
 }



/***********************************************************************
 * InitBuffer3801()   --  Initialize the buffer to zero                    
 *                                                                     
 *
 */

void InitBuffer3801(void){

  int i;
  bzero((char *)&SIS1,       sizeof(SIS1));
  bzero((char *)&SIS2,       sizeof(SIS2));
  bzero((char *)&SIS1Last,       sizeof(SIS1Last));
  bzero((char *)&SIS2Last,       sizeof(SIS2Last));
  bzero((char *)&SIS1Previous,   sizeof(SIS1Previous));
  bzero((char *)&SIS2Previous,   sizeof(SIS2Previous));
  bzero((char *)&num_read,       sizeof(num_read));

}



/* To dump the last SIS buffer.  For debugging. */

void dumpLastBuffer() 
{
        int isca,i,loc;

        printf("\n -------- Dump of Last Reading \n");

        for (i=0;i<NUM_CHANNEL;i++) 
            printf("SIS1 Last. data [ %d ] = %d\n",
      	         i,(SIS1Last.data[i]&DATA_MASK));

        if (NUMSCALERS >= 2) {
        for (i=0;i<NUM_CHANNEL;i++) 
             printf("SIS2 Last. data [ %d ] = %d\n",
    	          i,(SIS2Last.data[i]&DATA_MASK));
	}
}




/***********************************************************************
 * runStartClrSIS()   --  Initialize the buffer, the counters, and send  
 *                        a VME Next Clock.    
 *     note : Routine used at the beginnig of the run.
 */

void runStartClrSIS(void){

   int isca;

   for (isca=0;isca<NUMSCALERS;isca++) {
      if(SISVMEADDR[isca] != 0){ 
	   disableExtNextSIS(isca);
// FIXME CAMERON what is this?
	     /*pSIS[isca]->clear=1;*/ 
      }
   }

   InitBuffer3801();

   for (isca=0;isca<NUMSCALERS;isca++) {
      if(SISVMEADDR[isca] != 0){ 
	   vmeNext3801(isca);
           enableExtNextSIS(isca);
       }
   }


 /*   printf("Start a new run \n"); */
}


/***********************************************************************
 * WaitAjust()   --  Ajust the Delay time                     
 *                                                                     
 *
 */

void WaitAjust(void){

   if ((2*num_read[0]>=num_check)&&(waitime>0)) {
      waitime--;
   } else if (3*num_read[0]<num_check){
      waitime++;
   }

   if(waitime<TIME_DELAY_SIS-2) waitime = TIME_DELAY_SIS-2;
   if(waitime<1) waitime = TIME_DELAY_SIS;
   if(waitime>TIME_DELAY_SIS+2) waitime = TIME_DELAY_SIS+2;
   if (rocActivate > 0) waitime = 30;

}


/***********************************************************************
 *                                                                     *
 * SISProducer()   --  Read the FIFO and put the data in the shared    *
 *	                	  Memory.                              *
 *                                                                     *
 *     This routine is spawned when the vxWorks kernel boots up.  It   *
 *     runs continuously, updating the Cumulative counts and the ring  *
 *     buffer of scaler data.  Thus, even though the scaler is run as  *
 *     a 3801 (with FIFO), the data appears like a reading of a 3800   *
 *     (traditional scaler) but with a time lag due to waitime         *
 *                                                                     *
 ***********************************************************************/

void SISProducer (void) {
   
  int i,j,k,n;
  int isca;
  int not_empty=0;
  static int cnt=0;

/* Initialize the buffer to zero */

   runStartClrSIS();
 
   waitime=TIME_DELAY_SIS;


FOREVER {
 
 WaitAjust();
   
 taskDelay(waitime); 

 if ( rocActivate == 0 ) {
     
   num_check++;
   
   if (DEBUG1) {
     logMsg("\n We enter in the %dth pass\n",num_check,0,0,0,0,0);
     logMsg("And the CSR is : %x\n",sfs->csr,0,0,0,0,0);
   }
	
/* Update the FIFO NOT_EMPTY flag which is 0 if all FIFO scalers are empty */      

    not_empty =0;
    for(isca=0;isca<NUMSCALERS;isca++){
      if ((SISVMEADDR[isca] != 0) && 
         ((pSIS[isca]->csr&FIFO_EMPTY) == 0))
	   not_empty++;
    }
   

   while(not_empty != 0) {
	

	if (DEBUG1) 
          logMsg("Num_read = %d, Num_check = %d\n",num_read[0],num_check,0,0,0,0);
      

/* Disable the interrupt during the read of the FIFO */ 	   

#ifdef INTENABLE
  	scalIntDisable(); 	   
#endif
  	   
        not_empty = SISFIFO_Read();

/* Enable the interrupt after the read of the FIFO */ 	   

#ifdef INTENABLE
	scalIntEnable(INTERRUPT_FLAG); 	   
#endif
  	   
 
        if (DEBUG1) {
            dumpLastBuffer();
	}


   } /* End While not empty */

 } /* End of !rocActivate */

} /* End FOREVER */


}


/************************************************************************
*                                                                       *
*   Read3801    R. Michaels   Sept 2001                                 *
*                                                                       *
*   Purpose:  Provides and interface to the data for clients which      *
*             is simpler than addressing external structures            *
*   Input args:                                                         *
*        iscaler = 0 or 1  for the 2 scalers involved                   *
*        ichan  = channel number                                        *
         if chan = 100 return the number of reads                       *
*   Return value:                                                       *
*        data, if the inputs are valid                                  *
*        zero, if the inputs are not valid                              *
*                                                                       *
************************************************************************/

int Read3801(int iscaler, int ichan) {

  if (iscaler == 0) {
    if (ichan >= 0 && ichan < NUM_CHANNEL) {
      return SIS1.data[ichan];
    }
    if (ichan == 100) {
      return SIS1.numread;
    }
  }
  if (iscaler == 1) {
    if (ichan >= 0 && ichan < NUM_CHANNEL) {
      return SIS2.data[ichan];
    }
    if (ichan == 100) {
      return SIS2.numread;
    }
  }
  return 0;
}


/************************************************************************
*                                                                       *
*   SISFIFO_Read    R. Michaels   June 1999                             *
*                                                                       *
*   Purpose:  To read the FIFO, determine the helicity, check the       *
*   helicity, and assign data to proper location in scaler banks.       *
*                                                                       *
*   Return value:  not_empty  = 1 means FIFO not empty                  *
*                               0 means FIFO is empty                   *
*                                                                       *
************************************************************************/

int SISFIFO_Read (void) {

    int i,j,k,isca,not_empty; 

/* Get the the data from the FIFO */		

    for (isca=0;isca<NUMSCALERS;isca++){
       if((SISVMEADDR[isca] != 0) && 
         ((pSIS[isca]->csr&FIFO_EMPTY) != FIFO_EMPTY)){

	 if (DEBUG3) logMsg("FIFO not empty\n",0,0,0,0,0,0);

     	   if (++num_read[isca]>ROLLOVER ) {
  	       logMsg("WARNING: number of reads is rolling over\n",0,0,0,0,0,0);
               num_read[isca] = 1;
   	   }

	   switch (isca) {
		
	     case 0: 
	       for(i=0;i<NUM_CHANNEL;i++) {
		 SIS1Last.data[i] = pSIS[isca]->fifo[0];
		 if ((SIS1Last.data[i] & DATA_MASK) == DATA_MASK){
		   logMsg("Error 0xffffff: %x, %i, %x \n",SIS1Last.data[i],i,SIS1Last.data[i] & DATA_MASK,0,0,0);
		 }
	       }
	       SIS1Last.numread   = num_read[isca];
	       break;
	     
	    case 1: 
	       for(i=0;i<NUM_CHANNEL;i++) {
		 SIS2Last.data[i] = pSIS[isca]->fifo[0];
		 if ((SIS2Last.data[i] & DATA_MASK) == DATA_MASK){
		   logMsg("Error 0xffffff: %x, %i, %x \n",SIS2Last.data[i],i,SIS2Last.data[i] & DATA_MASK,0,0,0);
		 }
	       }
	       SIS2Last.numread   = num_read[isca];
	       break;

         
            default:
	   }
       } else {

	 if (DEBUG3) logMsg("FIFO is empty !!\n",0,0,0,0,0,0);

       }
    }
   

  /* Keep the cumulative counts in memory, depending on in-time
     or out-of-time mode */

    SISDataPkg();


   if (DEBUG5) {
     logMsg("Helicity bit %d   chan 31 hel %d\n",SIS1Last.hbit,SIS1Last.data[31]&DATA_MASK,0,0,0,0);
   }



/* Update the FIFO NOT_EMPTY flag which is 0 if all FIFO scalers are empty */      
    not_empty = 0;
    for(isca=0;isca<NUMSCALERS;isca++){
       if ((SISVMEADDR[isca] != 0) 
         & ((pSIS[isca]->csr&FIFO_EMPTY) == 0))
            not_empty++;
    }

    return not_empty;

}     


/*************************************************************
*                                                            *
*  SISDataPkg     R. Michaels     Mar 2003                   *
*                                                            *
*  To load the cumulative counts.
*  This version: no helicity sorting  (2015)
*                                                            * 
*************************************************************/ 

int SISDataPkg()
{

   int i;
   static int nint=0;

   nint++;

   if  (rocActivate == 0) {

        SIS1.numread  = SIS1Last.numread;
        SIS2.numread  = SIS2Last.numread;

	for (i=0;i<NUM_CHANNEL;i++) { 
           SIS1.data[i]  += (SIS1Last.data[i]&DATA_MASK);
           SIS1Previous.data[i] = (SIS1Last.data[i]&DATA_MASK);
           SIS2.data[i]  += (SIS2Last.data[i]&DATA_MASK);
           SIS2Previous.data[i] = (SIS2Last.data[i]&DATA_MASK);
	}
   }

   return 0;
}





