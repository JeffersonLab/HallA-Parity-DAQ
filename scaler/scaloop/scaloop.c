#include <vxWorks.h>
#include <types.h>
#include <vme.h>
#include <stdioLib.h>
#include <logLib.h>
#include <math.h>
#include <string.h>
#include <ioLib.h>
#include <time.h>

#define NUMSCALERS 1


#include "/adaqfs/halla/apar/devices/module_map.h"

/* global variables */
volatile struct fifo_scaler *local_pscaler[NUMSCALERS];
int FIFO_EMPTY=0x100;
int DATA_MASK=0xffffff;
int DEBUG=0;

void scaloop_init_scaler_addrs();
void scaloop_setup_scalers();  /* DONT do this except at boot */
void scaloop_flush_fifo(int isca);
void scaloop_test1(int isca);


void scalooper (void) {
   
  int TIME_DELAY=60;   /* Units are 1/60 th of sec  --> 60 = 1 sec */
  int VERBOSE=1;
  int NLOOP=20;

  int j,iloop,rdata;
  int isca;

  extern int CODA_RUN_IN_PROGRESS;

  scaloop_init_scaler_addrs();
  iloop=0;

FOREVER {
 
   iloop++;
   if (iloop > NLOOP) iloop=0;
   taskDelay(TIME_DELAY); 

   if (DEBUG) {
     logMsg("\n scaloop: Chk scalers. CODA_RUN_IN_PROGRESS = %d \n",CODA_RUN_IN_PROGRESS,0,0,0,0,0);
   }

   if ( CODA_RUN_IN_PROGRESS == 0 ) {  /* CODA not running */

     for (isca = 0; isca < NUMSCALERS; isca++) {
       scaloop_flush_fifo(isca);
     }

      if (VERBOSE && iloop==NLOOP) logMsg("\n scaloop: local flush of FIFO (its ok) \n",0,0,0,0,0,0);

   } else {     /* CODA_RUN_IN_PROGRESS==1 --> CODA is running */

      if (VERBOSE && iloop==NLOOP) logMsg("\n scaloop: FIFO flush by CODA (its ok)\n",0,0,0,0,0,0);

   }

 } /* FOREVER loop */

}

void scaloop_init_scaler_addrs() {

  /* Initialize local_pscaler array (pointers to scaler).
     Do not clear.  Clear may be done by CODA or at reboot only. */

  int scaler_base = 0xa40000;
  
  int isca, scaddr, res;
  unsigned long laddr;

  for (isca=0; isca<NUMSCALERS; isca++){

    scaddr = scaler_base + isca*0x1000;

    res = sysBusToLocalAdrs(0x39,scaddr,&laddr);
    if (res != 0) {

      printf("Error in sysBusToLocalAdrs res=%d \n",res);
      printf("ERROR:  SCALER POINTER NOT INITIALIZED");

    } else {
      printf("SIS3801 Scaler address = 0x%x\n",laddr);
      local_pscaler[isca] = (struct fifo_scaler *)laddr;
      printf("local_pscaler = 0x%x \n",local_pscaler[isca]);
    }
 
  }
}

void scaloop_setup_scalers() {

  /* To reset scalers.  Only done at reboot.  Otherwise CODA does it */

  int isca, mode, tmode;
  extern int CODA_RUN_IN_PROGRESS;

  if (CODA_RUN_IN_PROGRESS) {
    logMsg("scaloop:  Cannot setup scalers : CODA is running\n",0,0,0,0,0,0);
    return;
  }

  scaloop_init_scaler_addrs();  /* if not done already */
 
  for (isca=0; isca<NUMSCALERS; isca++){

      local_pscaler[isca]->reset=1;
      local_pscaler[isca]->csr=0xff00fc00;
      local_pscaler[isca]->clear=1;
      local_pscaler[isca]->enable=1;
      mode = 2;
      tmode = mode << 2;
      local_pscaler[isca]->csr=0x00000C00;  /* clear Mode bits to Mode 0*/
      local_pscaler[isca]->csr=tmode;       /* set new Mode */
      local_pscaler[isca]->next = 1;        /* flush FIFO */ 
      local_pscaler[isca]->csr=0x10000;     /* Enable External Next Clock */

  }

}

void scaloop_flush_fifo(int isca) {

  int j, rdata, itimeout;

  itimeout = 0;

  if (isca < 0 || isca >= NUMSCALERS) return;

  while ( ((local_pscaler[isca]->csr&FIFO_EMPTY) != FIFO_EMPTY) && itimeout++ < 100000) {

     for (j = 0; j < 32; j++) {
	rdata = ((int)local_pscaler[isca]->fifo[0])&DATA_MASK; 
#ifdef USEGLOBSCAL
	GlobalScalerIncr(isca, j, rdata);
#endif
        if (DEBUG) {
          logMsg("\n scaloop: Scaler[%d]  chan[%d]  data = %d = 0x%x = %d \n",isca,j,rdata,rdata,0,0);
        }

     }
  }
  if (DEBUG) logMsg("scaloop:  Finish loop, timeout = %d \n",itimeout,0,0,0,0,0);

}

void scaloop_test1(int isca) {


  int i,rdata;
  printf("here in test1, sca %d =  0x%x\n",isca,local_pscaler[isca]);

  i = local_pscaler[isca]->csr&FIFO_EMPTY;

  printf("test1  %d  = 0x%x \n",i,i);

  rdata = ((int)local_pscaler[isca]->fifo[0])&DATA_MASK; 

  printf("rdata %d \n",rdata);

}
