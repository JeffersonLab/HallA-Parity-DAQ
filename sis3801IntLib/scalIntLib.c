/******************************************************************************
*
*  scalIntLib.c  -  Driver library for readout of a Struck 7201 Multi-scaler or
*                   SIS 3801 Multi-scaler using a VxWorks 5.2 orlater based 
*                   Single Board computer. This library uses the interrupt 
*                   capabilities of the scaler to drive the readout. The user has 
*                   the option of attaching his own interrupt service routine.
*
*  Author: David Abbott 
*          Jefferson Lab Data Acquisition Group
*          January 1999
*
*  Instructions:   ld < scalIntLib.o                 -  Load the driver
*                  scalIntInit (addr)                -  Setup scaler for interrupt mode.
*                                                       Link scaler interrupt with
*                                                       default isr.
*                  scalIntConnect(user_routine, arg) -  Connect a User specific
*                                                       routine to the scaler
*                                                       interrupt.
*                  scalTest(mode)                    -  Reset scaler and Initalize
*                                                       optional test/operation modes
*                  scalIntEnable(mask)               -  Enable scaler interrupts
*                  scalTrig()                        -  Trigger a software scaler read (next
*                                                       pulse)  (i.e. dump scaler to FIFO)
*                  scalIntDisable()                  -  Disable scaler interrupts
*                  scalPulse(count)                  -  Will send "count" test pulses to all
*                                                       enabled scalers if "TEST" mode is
*                                                       enabled via scalTest()
*                  scalPrint()                       -  Print out current values of
*                                                       scaler data array (after interrupt).
*                  scalIntUser(arg)                  -  Example ISR where the Scaler
*                                                       FIFO is read out and cumulative
*                                                       sums are placed in local memory.
*                  scalStatus()                      -  Print out state of Scaler in a
*                                                       semi readable form
******************************************************************************
*  Modifications:
*  2000oct30  pmk  Adding a routine to read values to a specified address
*                  (based on the "scale32Lib.c:scalRead" routine).
*                  Added routines "scalInitialize" ,"scalDisable",
*                  and "scalEnable", to be used in the CRL for the
*                  beam monitor system.
*  2000oct31  pmk  Trying to modify the scalDisable so it completely
*                  releases the module.
*                  "scalRead" now will read until the FIFO is empty;
*                  if there are more words than were expected, it will
*                  issue a warning.
*                  "scalInitialize" no longer enables the clock.
*                  "scalEnable" resets and clears the scaler before
*                  issuing a next, and enabling the clock.
*  2001may14  pmk  If there are more than 5 times the value of "arg" data
*                  words read during 'scalRead' the routine assumes that
*                  this is an overflow and empties the FIFO, without outputing
*                  the extra words.
*
*  2006apr09  pmk  If the scaler registers as FIFO_FULL durign a read, we will
*                  send a FIFO reset.
*
*/

#include "vxWorks.h"
#include "stdio.h"
#include "string.h"
#include "intLib.h"
#include "iv.h"
#include "logLib.h"
#include "taskLib.h"
#include "vxLib.h"

/* Include scaler definitions */
#include "scalIntLib.h"

#define MAXNUM_SIS3801 19


IMPORT  STATUS sysBusToLocalAdrs(int, char *, char **);
IMPORT  STATUS intDisconnect(int);
IMPORT  STATUS sysIntEnable(int);
#ifdef VXWORKSPPC
IMPORT  int sysBusIntAck(int);
#endif

STATUS	scalIntConnect (VOIDFUNCPTR routine, int arg);
STATUS	scalIntInit (UINT32, UINT32, UINT32);
STATUS	scalIntEnable (UINT32);
STATUS	scalIntDisable (void);

LOCAL BOOL        scalIntRunning  = FALSE;	              /* running flag */
LOCAL VOIDFUNCPTR scalIntRoutine  = NULL;	              /* user interrupt service routine */
LOCAL int         scalIntArg      = NULL;	              /* arg to user routine */
LOCAL UINT32      scalIntLevel    = SCAL_VME_INT_LEVEL;   /* default VME interrupt level */
LOCAL UINT32      scalIntVec      = SCAL_INT_VEC;         /* default interrupt Vector */

/* Define global variables */
int Nsis3801;
volatile struct fifo_scaler *s3801p[MAXNUM_SIS3801];       /* pointer to Scaler memory map */
volatile struct fifo_scaler *sfs;       /* pointer to Scaler memory map */
volatile UINT32 scalData[32];           /* storage for Scaler data Sums */


/*******************************************************************************
*
* scalInt - default interrupt handler
*
* This rountine handles the timer interrupt.  A user routine is
* called, if one was connected by scalIntConnect().
*
* RETURNS: N/A
*
* SEE ALSO: scalIntConnect()
*/

LOCAL void scalInt (void)
    {
      int mask;

      mask = (sfs->csr)&SCAL_ENABLE_IRQ_MASK; /* Get Scaler IRQ mask */
      sfs->csr = SCAL_DISABLE_IRQ_MASK;       /* disable all interrupt sources */
      
      if (scalIntRoutine != NULL)	/* call user routine */
        (*scalIntRoutine) (scalIntArg);

#ifdef VXWORKSPPC
      sysBusIntAck(scalIntLevel);    /* For older Radstone PPC boards. */
#endif
      sfs->csr = mask;   /* Re-enable interrupts */

    }

/*******************************************************************************
*
* scalIntConnect - connect a user routine to the scaler interrupt
*
* This routine specifies the user interrupt routine to be called at each
* interrupt
*
* RETURNS: OK, or ERROR if scalInt() interrupt handler is not used.
*/

STATUS scalIntConnect 
    (
    VOIDFUNCPTR routine,	/* routine called at each timer interrupt */
    int arg		        /* argument with which to call routine */
    )
    {
      scalIntRoutine = routine;
      scalIntArg = arg;
      
      return (OK);
    }

/*******************************************************************************
*
* scalIntInit - initialize scaler for interrupts
*
*
* RETURNS: OK, or ERROR if the scaler address is invalid.
*/

STATUS scalIntInit (UINT32 addr, UINT32 level, UINT32 vector)
    {
      int res, rdata;
      unsigned long laddr;

      if (scalIntRunning) {
	printf("scalIntInit: ERROR: Scaler Interrupts are enabled. Call scalIntDisable() first!\n");
	return (ERROR);
      }

      /* Check for valid address */
      if(addr==0) {
	printf("scslIntInit: ERROR: Must specify local(A24) or CPU(A32) address for scaler\n");
	return(ERROR);
      }else if (addr > 0x00ffffff) {
	sfs = (struct fifo_scaler *)addr;
      }else{
	/* get the scaler's address */
	res = sysBusToLocalAdrs(0x39,(char *)addr,(char **)&laddr);
	if (res != 0) {
	  printf("scalIntInit: ERROR in sysBusToLocalAdrs(0x39,0x%x,&laddr) \n",addr);
	  return(ERROR);
	} else {
	  sfs = (struct fifo_scaler *)laddr;
	}	
      }
      /* Check if board really exists at the specified address */
      res = vxMemProbe((char *) sfs,0,4,(char *)&rdata);
      if(res < 0) {
	printf("scslIntInit: ERROR: No addressable board at addr=0x%x\n",(UINT32) sfs);
	return(ERROR);
      }

      /* Check for user defined VME interrupt level and vector */
      if(level == 0) {
	scalIntLevel = SCAL_VME_INT_LEVEL; /* use default */
      }else if (level > 7) {
	printf("scalIntInit: ERROR: Invalid VME interrupt level (%d). Must be (1-7)\n",level);
	return(ERROR);
      } else {
	scalIntLevel = level;
      }
      if(vector == 0) {
	scalIntVec = SCAL_INT_VEC;  /* use default */
      }else if ((vector < 32)||(vector>255)) {
	printf("scalIntInit: ERROR: Invalid interrupt vector (%d). Must be (32<vector<255)\n",vector);
	return(ERROR);
      }else{
	scalIntVec = vector;
      }
      
      /* Connect the ISR */
#ifdef VXWORKSPPC
      if((intDisconnect((int)INUM_TO_IVEC(scalIntVec)) !=0)) {
	printf("scslIntInit: ERROR disconnecting Interrupt\n");
	return(ERROR);
      }
#endif
/*
 *       intConnect(INUM_TO_IVEC(scalIntVec),scalInt,0);
 */
	printf("scslIntInit: scalInt is Connect. %d\n",intConnect(INUM_TO_IVEC(scalIntVec),scalInt,0));
#ifdef VXWORKSPPC
      intEnable(scalIntVec);            /* enable MPIC interrupts for this vector */
#endif
      sysIntEnable(scalIntLevel);   /* Enable VME interrupts */

      /* Zero out scaler Data array */
      bzero((char *)scalData,sizeof(scalData));
      
      return(OK);
    }

/*******************************************************************************
*
* scalIntEnable - enable scaler interrupts
*
* This routine takes an interrupt source mask and enables interrupts
* fo the initialized scaler.
*     mask:    bit 0  IRQ on start of CIP     
*    (1-15)    bit 1  IRQ on FIFO full
*              bit 2  IRO on FIFO half full
*              bit 3  IRO on FIFO almost full
*
* RETURNS: OK, or ERROR if not intialized.
*/

STATUS scalIntEnable (UINT32 mask)
    {
      
      if((mask==0) || (mask > 0xf)) {
	printf("VME Interrupt mask=0x%x is out of range\n",mask);
	return(ERROR);
      }else{
	if(sfs) {
	  /* Program scaler to generate interrupts */
	  sfs->csr = SCAL_DISABLE_IRQ_MASK;   /* disable all interrupt sources */
	  sfs->irq = (0x800) | (scalIntLevel<<8) | scalIntVec;
	  sfs->csr = (mask<<20);
	}else{
	  printf("scalInt not Initialized. Call scalIntInit()!\n");
	  return(ERROR);
	}
      }
      return (OK);
    }

/*******************************************************************************
*
* scalIntDisable - disable the scaler interrupts
*
* RETURNS: OK, or ERROR if not initialized
*/

STATUS scalIntDisable (void)
    {
      if (sfs) {
	sfs->csr = SCAL_DISABLE_IRQ_MASK;
	scalIntRunning = FALSE;
      } else {
	printf("scalInt not Initialized. Call scalIntInit()!\n");
	return(ERROR);
      }
      
      return (OK);
    }


/*******************************************************************************
*
* scalIntUser - default user interrupt routine
*
*  This routine can be modified to add the code the User wishes to execute
*  upon recieving a scaler interrupt
*
*  RETURNS: N/A
*
*/
void scalIntUser (int arg)
{
  int ii;

  /* Check if FIFO is empty and read N scaler values out of the fifo. The argument
     to the ISR defines the number of scaler reads (or active scaler channels)
   */

  if((sfs->csr)&SCAL_STATUS_FIFO_EMPTY) {
    /* FIFO is empty */
    logMsg("scalIntUser: FIFO Empty\n",0,0,0,0,0,0);
  } else {
    /* logMsg("scalIntUser: Read %d scalers\n",arg,0,0,0,0,0); */
    for(ii=0;ii<arg;ii++) {
      scalData[ii] = scalData[ii] + sfs->fifo[0];
    }
  }
}

/******************************************************************************
* scalInitialize - Do some things at prestart
*
*  RETURNS: 
*
*/
STATUS scalInitialize (void)
{
  sfs->reset  = 1;
  /* Disable all IRQ sources, external signals, broadcast mode,
     and test modes.  Clear software disable, and input mode.   */
  sfs->csr    = 0xff00fc00;
  sfs->clear  = 1;
  /* Set the configuration for the control input pins to Mode=2 */
  sfs->csr    = SCAL_CLEAR( SCAL_INPUT_MODE_MASK );
  sfs->csr    = (0x2 << 2);
  
  return(OK);
}

/******************************************************************************
* scalDisable - 
*
*  RETURNS: 
*
*/
STATUS scalDisable (void)
{
  sfs->disable = 1;     
  sfs->csr     = SCAL_CLEAR( SCAL_CSR_EXT_NEXT );
  sfs->csr     = SCAL_CLEAR( SCAL_CSR_EXT_DISABLE );
  sfs->clear  = 1;
  sfs->csr    = 0xff00fc00;
  sfs->reset  = 1;

#ifdef VXWORKSPPC
      if((intDisconnect((int)INUM_TO_IVEC(scalIntVec)) !=0)) {
	printf("scslIntInit: ERROR disconnecting Interrupt\n");
	return(ERROR);
      }
#endif
#ifdef VXWORKSPPC
      intDisable(scalIntVec);            /* enable MPIC interrupts for this vector */
#endif


  return(OK);
}

/******************************************************************************
* scalEnable - 
*
*  RETURNS: 
*
*/
STATUS scalEnable (void)
{

  //  sfs->reset   = 1;
  sfs->clear   = 1;

  sfs->next    = 1;

  /* Enable next clock logic */
  sfs->enable = 1;

  /* Enable External Next Clock */
  sfs->csr     = SCAL_CSR_EXT_NEXT;
  /* Enable External Disable */
  sfs->csr     = SCAL_CSR_EXT_DISABLE;

  return(OK);
}

/******************************************************************************
* scalRead - Read scaler values to specified address
*
*  RETURNS: number of scaler values read
*
*/
int
scalRead(int arg, UINT32 *data)
{
  int nvalues = 0;
  UINT32 localdata;

  if(((sfs->csr)&SCAL_STATUS_FIFO_EMPTY)==SCAL_STATUS_FIFO_EMPTY) {
    /* FIFO is empty */
    logMsg("scalRead: FIFO Empty\n",0,0,0,0,0,0);
  } else {
    while (((sfs->csr)&SCAL_STATUS_FIFO_EMPTY)!=SCAL_STATUS_FIFO_EMPTY){
      *data++ = sfs->fifo[0];
      nvalues++;
      if (nvalues > (5*arg))
	break;
    }
    if (nvalues > (5*arg)) {
      while (((sfs->csr)&SCAL_STATUS_FIFO_EMPTY)!=SCAL_STATUS_FIFO_EMPTY){
	localdata =  sfs->fifo[0];
      }
      *data++ = 'OVER';
      *data++ = 'FLOW';
      nvalues+=2;
    }
    if (nvalues!=arg){
      logMsg("scalRead: nvalues!=arg\n",0,0,0,0,0,0);
    }
  }

  return(nvalues);
}

/******************************************************************************
*
* scalTest - Program Scaler to test interrupts
*            mode input specifies optional test modes
*            Refer to User manual on bit enable definitions
*            for the various scaler operation modes.
*
*  RETURNS: N/A
*
*/
void scalTest(UINT32 mode)
{
  mode &= 0xff;  /* Mask the mode input */

  if(sfs) {
    printf("Reset, Clear FIFO, and Enable scaler.\n");
    sfs->reset = 1;
    sfs->clear = 1;
    sfs->enable = 1;
    if(mode) {
      sfs->csr = mode;
      printf("Program CSR for mode: 0x%x\n",mode);
    }
  }else{
    printf("Call scalIntInit() first!\n");
  }
}

/*******************************************************************************
*
* scalTrig - Trigger scaler readout (Software next Pulse) nCount times
*
*  RETURNS: N/A
*
*/
void scalTrig(int nCount)
{
  int ii;

  if(sfs) {
    sfs->next = 1;
    for(ii=1;ii<nCount;ii++) {
      taskDelay(1);
      sfs->next = 1;
    }
  }else{
    printf("Call scalIntInit() first!\n");
  }
}

/*******************************************************************************
*
* scalPulse - Pulse all enabled scalers (count) times
*
*  RETURNS: N/A
*
*/
void scalPulse(int count)
{
  int ii;

  if(sfs) {
    if(sfs->csr&SCAL_STATUS_TEST_MODE) {
      for(ii=0;ii<count;ii++)
	sfs->test = 1;
      /* printf("Completed %d test pulses\n",count); */
    } else {
      printf("Test mode not enabled. Call scalTest(mode)\n");
    }
  }else{
    printf("Call scalIntInit() first!\n");
  }
}


/*******************************************************************************
*
* scalPrint - Print scaler Values to Standard out
*
*  RETURNS: N/A
*
*/
void scalPrint()
{
  int ii;

  printf("   Scaler Data (Sums):\n");
  for(ii=1;ii<=32;ii+=4) {
    printf(" SCAL%02d: %10d  SCAL%02d: %10d  SCAL%02d: %10d  SCAL%02d: %10d \n",
	   ii,scalData[ii-1],ii+1,scalData[ii],ii+2,scalData[ii+1],ii+3,scalData[ii+2]);
  }

}

/*******************************************************************************
*
* scalStatus - Print status of scaler in readable form
*
*  RETURNS: N/A
*
*/
void scalStatus(int sflag)
{
  int CSR, IRQ;
  int input_mode;
  char irq_mask;
  char irq_mask_pending;
  char next_enabled[9];
  char irq_enabled[9];
  char test_enabled[9];
  char fifo_status[13];

  if(sfs) {
    CSR = sfs->csr;
    IRQ = sfs->irq;
    if (IRQ&SCAL_INT_ENABLED_MASK)
      strncpy((char *)&irq_enabled[0],"Enabled \0",9);
    else
      strncpy((char *)&irq_enabled[0],"Disabled\0",9);

    if (CSR&SCAL_STATUS_ENABLE_NEXT)
      strncpy((char *)&next_enabled[0],"Enabled \0",9);
    else
      strncpy((char *)&next_enabled[0],"Disabled\0",9);

    if (CSR&SCAL_STATUS_TEST_MODE)
      strncpy((char *)&test_enabled[0],"Enabled \0",9);
    else
      strncpy((char *)&test_enabled[0],"Disabled\0",9);

    irq_mask = (CSR&SCAL_ENABLE_IRQ_MASK)>>20;
    irq_mask_pending = (CSR&SCAL_DISABLE_IRQ_MASK)>>28;
    input_mode = (CSR&SCAL_INPUT_MODE_MASK)>>2;


    switch (CSR&SCAL_FIFO_STATUS_MASK) {
    case SCAL_STATUS_FIFO_FULL:
      strncpy((char *)&fifo_status[0],"Full        \0",13);
      break;
    case SCAL_STATUS_FIFO_ALMOST_FULL:
      strncpy((char *)&fifo_status[0],"Almost Full \0",13);
      break;
    case SCAL_STATUS_FIFO_HALF_FULL:
      strncpy((char *)&fifo_status[0],"Half Full   \0",13);
      break;
    case SCAL_STATUS_FIFO_ALMOST_EMPTY:
      strncpy((char *)&fifo_status[0],"Almost Empty\0",13);
      break;
    case SCAL_STATUS_FIFO_EMPTY:
    case (SCAL_STATUS_FIFO_EMPTY|SCAL_STATUS_FIFO_ALMOST_EMPTY):
      strncpy((char *)&fifo_status[0],"Empty       \0",13);
      break;
    default:
      strncpy((char *)&fifo_status[0],"< Half Full \0",13);
    }
    
    /* Print out results */
    printf("  Scaler Status (Addr=0x%x  ID=0x%x)\n",(int) sfs,((IRQ&SCAL_MODULE_ID_MASK)>>16));
    printf("  CSR Register = 0x%08x\n",CSR);
    printf("      FIFO Status     = %s\n",fifo_status);
    printf("      Next Logic      = %s\n",next_enabled);
    printf("      Test Mode       = %s\n",test_enabled);
    printf("      Input Mode      = %d\n",input_mode);
    printf("  IRQ Register = 0x%08x\n",IRQ);
    printf("      Interrupts          = %s\n",irq_enabled);
    printf("      VME Interrupt Level = %d\n",(IRQ&SCAL_INT_LEVEL_MASK)>>8);
    printf("      Interrupt Vector    = 0x%x\n",(IRQ&SCAL_INT_VEC_MASK));
    printf("      IRQ Source Mask     = 0x%x  IRQs pending = 0x%x\n",irq_mask,irq_mask_pending);

  }else{
    printf("Call scalIntInit() first!\n");
  }

}

/**********************************************************************************************
 *  sis3801InitializeMany
 *
 */

STATUS sis3801InitializeMany(UINT32 baseaddr, UINT32 addrinc, int nscaler)
{
  int isca;
  int addr;
  int errorcount;

  errorcount = 0;
  Nsis3801 = 0;
  for (isca=0; isca<nscaler; isca++){
    Nsis3801++;
    addr = baseaddr + (isca * addrinc);
    if (scalIntInit(addr, 0, 0)==OK){
      printf("sis3801InitializeMany:  SIS3801 module %d at addr=0x%08x initialized.\n",Nsis3801,(int) sfs);
      s3801p[isca] = sfs;
    } else {
      errorcount++;
      printf("sis3801InitializeMany:  SIS3801 module %d at laddr=0x%08x doesn't initialize.\n",
	     Nsis3801,addr);
      s3801p[isca] = NULL;
    }
  }
  /*  Now do the initialization. */
  for (isca=0; isca<Nsis3801; isca++){
    if (s3801p[isca]){
      s3801p[isca]->reset  = 1;
      /* Disable all IRQ sources, external signals, broadcast mode,
	 and test modes.  Clear software disable, and input mode.   */
      s3801p[isca]->csr    = 0xff00fc00;
      s3801p[isca]->clear  = 1;
      /* Set the configuration for the control input pins to Mode=2 */
      s3801p[isca]->csr    = SCAL_CLEAR( SCAL_INPUT_MODE_MASK );
      s3801p[isca]->csr    = (0x2 << 2);
    }
  }
  if (errorcount>0){
    printf("sis3801InitializeMany:  There were %d errors initializing the SIS3801 modules.\n",
	   errorcount);
    return(ERROR);
  }

  return(OK);
}

/******************************************************************************
* sIS3801DisableMany - 
*
*  RETURNS: 
*
*/
STATUS sis3801DisableMany (void)
{
  int isca;

  for (isca=0; isca<Nsis3801; isca++){
    s3801p[isca]->disable = 1;     
    s3801p[isca]->csr     = SCAL_CLEAR( SCAL_CSR_EXT_NEXT );
    s3801p[isca]->csr     = SCAL_CLEAR( SCAL_CSR_EXT_DISABLE );
    s3801p[isca]->clear  = 1;
    s3801p[isca]->csr    = 0xff00fc00;
    s3801p[isca]->reset  = 1;
  }

#ifdef VXWORKSPPC
    if((intDisconnect((int)INUM_TO_IVEC(scalIntVec)) !=0)) {
      printf("sis3801DisableMany:  Error disconnecting Interrupt\n");
      return(ERROR);
      }
#endif
#ifdef VXWORKSPPC
    intDisable(scalIntVec);            /* enable MPIC interrupts for this vector */
#endif
  

  return(OK);
}

/******************************************************************************
* sis3801EnableMany - 
*
*  RETURNS: 
*
*/
STATUS sis3801EnableMany (void)
{
  int isca;

  for (isca=0; isca<Nsis3801; isca++){

    //  s3801p[isca]->reset   = 1;
    s3801p[isca]->clear   = 1;

    /* Enable next clock logic */
    s3801p[isca]->enable = 1;
  
    /* Enable External Next Clock */
    s3801p[isca]->csr     = SCAL_CSR_EXT_NEXT;
    /* Enable External Disable */
    s3801p[isca]->csr     = SCAL_CSR_EXT_DISABLE;

    /* Send one software "next clock". */
    s3801p[isca]->next    = 1;

  }

  return(OK);
}

/******************************************************************************
* sis3801ReadOneModule - Read scaler values to specified address
*
*  RETURNS: number of scaler values read
*
*/
int  sis3801ReadOneModule(int isca, int arg, UINT32 *data)
{
  int nvalues = 0;
  int i;
  UINT32 localdata;

  if (s3801p[isca]){
    if(((s3801p[isca]->csr)&SCAL_STATUS_FIFO_EMPTY)==SCAL_STATUS_FIFO_EMPTY) {
      /* FIFO is empty */
      logMsg("scalRead: FIFO Empty\n",0,0,0,0,0,0);
    } else if (((s3801p[isca]->csr)&SCAL_STATUS_FIFO_FULL)==SCAL_STATUS_FIFO_FULL){
      /* FIFO is FULL */
      logMsg("scalRead: FIFO Full; try to read 32 words, then clear it!\n",0,0,0,0,0,0);
      for (i=0; i<32; ++i){
	*data++ = s3801p[isca]->fifo[0];
	nvalues++;
      }
      /*  Send the clear signal to the FIFO. */
      s3801p[isca]->clear = 1;
      if(((s3801p[isca]->csr)&SCAL_STATUS_FIFO_EMPTY)==SCAL_STATUS_FIFO_EMPTY) {
	logMsg("scalRead: FIFO is empty; we have successfully cleared it\n",0,0,0,0,0,0);
      } else {
	logMsg("scalRead: FIFO is NOT empty; if this message reoccurs, you have a problem.  END THE RUN.\n",0,0,0,0,0,0);
      }
    } else {
      while (((s3801p[isca]->csr)&SCAL_STATUS_FIFO_EMPTY)!=SCAL_STATUS_FIFO_EMPTY){
	*data++ = s3801p[isca]->fifo[0];
	nvalues++;
	if (nvalues > (5*arg))
	  break;
      }
      if (nvalues > (5*arg)) {
	/* while (((s3801p[isca]->csr)&SCAL_STATUS_FIFO_EMPTY)!=SCAL_STATUS_FIFO_EMPTY){
	 *  localdata =  s3801p[isca]->fifo[0];
	 * }
	 */
	/*  Send the clear signal to the FIFO. */
	s3801p[isca]->clear = 1;
	*data++ = 'OVER';
	*data++ = 'FLOW';
	nvalues+=2;
      }
      if (nvalues!=arg){
	logMsg("scalRead: nvalues!=arg\n",0,0,0,0,0,0);
      }
    }
  } else {
    logMsg("scalRead: Module not initialized\n",0,0,0,0,0,0);
  }

  return(nvalues);
}

/******************************************************************************
* sis3801ReadOneModule - Read scaler values to specified address
*
*  RETURNS: number of scaler values read
*
*/
int  sis3801PrintOneModule(int isca, int arg)
{
  int nvalues = 0;
  UINT32 localdata;

  if (s3801p[isca]){
    /*  The module is here! */
    if(((s3801p[isca]->csr)&SCAL_STATUS_FIFO_EMPTY)==SCAL_STATUS_FIFO_EMPTY) {
      /* FIFO is empty */
      logMsg("scalRead: FIFO Empty\n",0,0,0,0,0,0);
    } else {
      while (((s3801p[isca]->csr)&SCAL_STATUS_FIFO_EMPTY)!=SCAL_STATUS_FIFO_EMPTY){
	printf("%10ld\t",s3801p[isca]->fifo[0]);
	nvalues++;
	if (nvalues > (5*arg))
	  break;
      }
      if (nvalues > (5*arg)) {
	while (((s3801p[isca]->csr)&SCAL_STATUS_FIFO_EMPTY)!=SCAL_STATUS_FIFO_EMPTY){
	  localdata =  s3801p[isca]->fifo[0];
	}
	printf("OVERFLOW");
	nvalues+=2;
      }
      if (nvalues!=arg){
	logMsg("scalRead: nvalues!=arg\n",0,0,0,0,0,0);
      }
    }
  } else {
    printf("sis3801PrintOneModule:  Module %d is not initialized.\n",isca);
  }

  return(nvalues);
}
