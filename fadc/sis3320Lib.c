/******************************************************************************
 *
 *  sis3320Lib.c  -  Driver library for readout of Struck 3320 Flash ADC
 *                  using a VxWorks 5.4 or later based Single Board computer. 
 *
 *  Author: Rich Holmes
 *          Syracuse University
 *          March 2006
 *
 *  Based on David Abbott's sis3300Lib.c
 *  and Struck's sis3320_single_event_sample_wrap_test1.c
 *
 *  Modifications
 * 
 *  April 2006    R. Michaels   Add sis3320Test1(), sis3320GetData(), 
 *                              sis3320IsSampling(), sis3320IsRunning(),
 *                              sis3320SetRunFlag(), sis3320GetAccum(),
 *                              sis3320DefaultSetup(), sis3320Finished()
 *                              sis3320GetBufLength(), sis3320SetDac(),
 *                              sis3320SetThresh()
 *
 * December 2007  G.Franklin   Add sis3320GetMapping();
 *                             modify debug prints in sis3320_Sum_ADC_Channel
 *
 * Dec Å¥07    R. Michaels   Deprecate old GetAccum, and SetThresh for new
 *                          version of FADC.  Old one is *_V1 (version 1).
 *                          Add sis3320SetN5N6() to set interval for accum 5,6
 *                          Add sis3320GetNumAcc() to get num entries in accumulator
 *                          Add sis3320Test2, 3, ..  for new accum.
 * Mar 2008  G. Franklin
 *                          sis3320SetupMode
 *                          sis3320StartSampling
 *                          sis3320StopSampling
 *                          sis3320SetGain
 *                          sis3320WriteAdcSPI
 *                           (also added adcSPI register to structure of
 *                                 adcGroup_struct in header file
 * May 2008 G. Franlin     reorganized
 *                         see protypes ofr documentation
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

#define HARDMAX 8

/* Include ADC definitions */
#include "sis3320.h"

/* Include DMA Library definitions */
#include "universeDma.h"
#
/* Define external Functions */
IMPORT  STATUS sysBusToLocalAdrs (int, char *, char **);
IMPORT  STATUS intDisconnect (int);
IMPORT  STATUS sysIntEnable (int);
IMPORT  STATUS sysIntDisable (int);

/* Define Interrupts variables */
BOOL              sis3320IntRunning  = FALSE; /* running flag */
int               sis3320IntID       = -1;    /* id number of ADC generating interrupts */
LOCAL VOIDFUNCPTR sis3320IntRoutine  = NULL;  /* user interrupt service routine */
LOCAL int         sis3320IntArg      = 0;     /* arg to user routine */
LOCAL int         sis3320IntEvCount  = 0;     /* Number of Events to generate Interrupt */
LOCAL UINT32      sis3320IntLevel    = SIS3320_VME_INT_LEVEL;      /* default VME interrupt level */
LOCAL UINT32      sis3320IntVec      = SIS3320_VME_INT_VEC;        /* default interrupt Vector */

/* Define global variables */
int Nsis3320 = 0;                           /* Number of ADCs in Crate */
volatile struct sis3320_struct *sis3320p[SIS3320_MAX_BOARDS];       /* pointers to ADC memory map */
int sis3320IntCount = 0;                    /* Count of interrupts from ADC */
int sis3320EventCount[SIS3320_MAX_BOARDS];  /* Count of Events taken by ADC (Event Count Register value) */
int sis3320EvtReadCnt[SIS3320_MAX_BOARDS];  /* Count of events read from specified ADC */
int s3320RunFlag = 0;
int *sis3320Data = 0;    /* Pointer to local memory for data*/

SEM_ID sis3320Sem;                          /* Semaphore for Task syncronization */
extern int s3320RunFlag;

/* Globals for read */
#define SIS3320_MAX_NUMBER_LWORDS 0x100000

unsigned int raw_adc_data[SIS3320_MAX_NUMBER_LWORDS];
unsigned int repacked_adc_data[SIS3320_MAX_NUMBER_LWORDS];

/*===========================================================================*/
/* Prototypes                                                                */
/*===========================================================================*/
/*            */
/* Data Reads */
int 
sis3320_Read_ADC_Channel (unsigned int id,
			  unsigned int adc_channel /* 0 to 7 */, 
			  unsigned int event_sample_start_addr, 
			  unsigned int event_sample_length,
			  unsigned int* uint_adc_buffer,
			  unsigned int lbuffer);

unsigned int 
sis3320_Sum_ADC_Channel (unsigned int id,
			 unsigned int adc_channel /* 0 to 7 */, 
			 unsigned int event_sample_start_addr, 
			 unsigned int event_sample_length,
			 unsigned int evc);
int sis3320ReadEvent (int id, UINT32 *data, int nsamp, int adc, int evt) ;
/* get single sample word at memory location index (lohi=0 or 1 for low or hi word*/
long sis3320GetData(int id, int adc_channel, int index, int lohi);
/* get value of accumulator which_accum */
unsigned long sis3320GetAccum(int id, int adcgr, int adc, int which_accum, int hilo);
long sis3320GetAccum_V1(int id, int adcgr, int adc, int which);  /*for old 3320 firmware*/
/* get number of samples summed into accumulator which_ccum */
long sis3320GetNumAcc(int id, int adcgr, int adc, int which_accum);
int sis3320GetBufLength(int id);
int sis3320GetActualNumEvents(int id);


/* Get Status */
int sis3320IsSampling(int id);
int sis3320IsRunning();
int sis3320SetRunFlag(int arg);
int sis3320Finished(int id);
int sis3320Busy(int id);
int sis3320DacCsrBusy(int id);   /*used for setting DAC */
/* Get Setup parameters */
int sis3320GetDac(int id, int chan);   /* get DAC setting */
int sis3320GetThresh(int id, int chan, int which_thresh); /* which_thresh= 1 or 2 */
int sis3320GetMaxSampleLength(int id);
unsigned int sis3320GetN5N6(int id, int chan);  /*Accum 5 and 6 before pre- count*/
/*                       and post-count settings (packed in 4 bytes word*/         

/* printout status and other info (Debugging tools) */
void sis3320Status (int id, int sflag); 
int sis3320PrintEvent (int id, int adc, int evt, int flag); /*flag=0 no unpacking, 1 hex, 2 for dec*/
int sis3320PrintEventDir (int id, int adc); 
void sis3320Display (int id);  /* print out configuration info */
void sis3320PrintChannelSettings(int id, int chan); /*print DAC and accumulator settings*/
 
/* Set parameters and mode  */
int sis3320SetRunFlag(int arg);   
int sis3320Init (UINT32 addr, UINT32 addr_inc, int nadc) ;
int sis3320DefaultSetup(int id);
int sis3320SetupMode(int id, int Mode);  /* Sets standard trigger modes*/
int sis3320SetGain(int id, int adc, int data);/*    accumulator setup */
int sis3320SetDac(int id, int data);        /* for compatabiilty with old code */
int sis3320SetDacChan(int id, int chan, int data);  
int sis3320SetThresh_V1(int id, int adc, int lohi, int data);
int sis3320SetThresh(
int id, int adc, int thresh1, int thresh2);
int sis3320SetN5N6(int id, int adc, int n5_before, int n5_after, int n6_before, int n6_after);
int sis3320SetEventMode(int id, int Flag);/*Flag=0 single event,=1 multiple events*/
int sis3320SetStartMode(int id, int Flag);/*Flag=0disable autostart,=1 enable*/
int sis3320SetStopMode(int id, int Flag);
int sis3320EnableUserBit(int id,int Flag);/*Flag=0 disable user bit, =1 enable*/
int sis3320SetSampleStartAddress(int id, int StartAddress);
int sis3320SetMaxSampleLength(int id, int SampleLength);
int sis3320SetMaxNumEvents(int id, int MaxNumberOfEvents);
int sis3320StopDelay (int id, unsigned long delay); /* step # samples after stop signal */
int sis3320StartDelay (int id, unsigned long delay); 
void sis3320Reset (int id); 
void sis3320Start (int id);  /* Arm Sampling Logic and wait for LEMO or software start */
/*                            (this could have been caled "Arm" */
void sis3320Stop (int id);   /* Stop Sampling */
void sis3320StopSampling (int id);   /*Stop Sampling  (same as sis3320Stop) */
void sis3320Enable (int id);
void sis3320Disable (int id);

/* interrupt and acquisition mode related routines */
int sis3320IntSet (int id, int iLevel, int iVec, int iSource);/* Set Intrpt Level, Vector,  Source */
void sis3320IntAck (int intMask);
void sis3320Int (void); /* disable interrups */
STATUS sis3320IntConnect (int id, VOIDFUNCPTR routine, int arg, UINT16 level,
			  UINT16 vector);
void sis3320IntEnable (int intMask);
void sis3320RORA();
void sis3320ROAK();
void sis3320IntDisable (int intMask);
void sis3320EventConfig (int id, unsigned long val);
unsigned int sis3320AcqConfig (int id, unsigned long val); 


/* internal  routines */
int sis3320WriteAdcSPI(int id, int adc, unsigned int spi_address, unsigned int data);
int sis3320ReadAdcSPI(int id, int adc, unsigned int* spi_address);
int sis3320SetupMapping(unsigned int id,
		      unsigned int adc,
		      unsigned int index,
		      unsigned int length,
		      unsigned int* sub_page,    /* return memory subpage*/
		      unsigned int* sub_index,  /* return index of first data word*/
		      unsigned int* sub_length,  /* return # of words this subpage*/
		      unsigned int** mem_loc);   /*starting VME memory location

/* Misc unknown */
 void sis3320Run (int id, int rflag, int whichadc); 		      

/*******************************************************************************
 *
 * sis3320Init - Initialize SIS3320 Library.
 *
 *
 * RETURNS: OK, or ERROR if the address is invalid or board is not present.
 */

STATUS
sis3320Init (UINT32 addr, UINT32 addr_inc, int nadc) 
{
  int ii, res, rdata, errFlag = 0;
  int boardID = 0;
  unsigned int laddr;
  UINT32 fooaddr;
 
  /* Check for valid address */
  if (addr == 0) 
    {
      printf ("sis3320Init: ERROR: Must specify a Bus (VME-based A32) address for ADC 0\n");
      return (ERROR);
    }
  else if (addr < 0x00ffffff) 
    { /* A24 Addressing */
      printf ("sis3320Init: ERROR: A24 Addressing not allowed for the SIS3320\n");
      return (ERROR);
    }
  else
    { /* A32 Addressing */
      if ((addr_inc == 0) || (nadc == 0)) 
	nadc = 1; /* assume only one ADC to initialize */

      /* get the ADC address */
      res = sysBusToLocalAdrs (0x09, (char *) addr, (char **) &laddr);
      printf("sysBusToLocalAdrs result 0x%x  0x%x  %d \n",addr,laddr,res);
      if (res != 0) 
	{
	  printf ("sis3320Init: ERROR in sysBusToLocalAdrs (0x0d, 0x%x, &laddr) \n",
		  addr);
	  return (ERROR);
	}
    }

  Nsis3320 = 0;
  for (ii = 0; ii < nadc; ii++) 
    {
      sis3320p[ii] = (struct sis3320_struct *) (laddr + ii * addr_inc);
      printf("Long address adc %d   laddr = 0x%x  addr_inc = 0x%x   ptr = 0x%x \n",ii,laddr,addr_inc,sis3320p[ii]);
      /* Check if Board exists at that address */
      res = vxMemProbe ((char *) &(sis3320p[ii]->id) , VX_READ, 4,
			(char *) &rdata);

      printf("Board ID:  probe result %d   rdata = 0x%x",res,rdata);
      if (res < 0) 
	{
	  printf ("sis3320Init: ERROR: No addressable board at addr = 0x%x\n",
		  (UINT32) sis3320p[ii]);
	  sis3320p[ii] = NULL;
	  errFlag = 1;
	  break;
	}
      else
	{
	  /* Check if this is a Model 3320 */
	  boardID = rdata & SIS3320_BOARD_ID_MASK;
	  if (boardID != SIS3320_BOARD_ID) 
	    {
	      printf (" ERROR: Board ID does not match: 0x%x 0x%x \n", boardID, rdata);
	      return (ERROR); 
	    }
	}
      Nsis3320++;
      printf ("Initialized ADC ID %d at address 0x%08x \n", ii,
	      (UINT32) sis3320p[ii]);
    }

  /* Initialize/Create Semaphore */
  if (sis3320Sem != 0) 
    {
      semFlush (sis3320Sem);
      semDelete (sis3320Sem);
    }
  sis3320Sem = semBCreate (SEM_Q_PRIORITY, SEM_EMPTY);
  if (sis3320Sem <= 0) 
    {
      printf ("sis3320Init: ERROR: Unable to create Binary Semaphore\n");
      return (ERROR);
    }

  /* Disable/Clear all ADCs */
  for (ii = 0; ii < Nsis3320; ii++) 
    {
      sis3320p[ii]->reset = 1;
      sis3320EventCount[ii] = 0;           /* Initialize the Event Count */
    }
  /* Initialize Interrupt variables */
  sis3320IntID = -1;
  sis3320IntRunning = FALSE;
  sis3320IntLevel = SIS3320_VME_INT_LEVEL;
  sis3320IntVec = SIS3320_VME_INT_VEC;
  sis3320IntRoutine = NULL;
  sis3320IntArg = 0;
  sis3320IntEvCount = 0;

  if (errFlag > 0) 
    {
      printf ("sis3320Init: ERROR: Unable to initialize all ADC Modules\n");
      if (Nsis3320 > 0) 
	printf ("sis3320Init: %d ADC (s) successfully initialized\n",
		Nsis3320);
      return (ERROR);
    }
  else
    {
      return (OK);
    }
}

void
sis3320Status (int id, int sflag) 
{

  int ii;
  unsigned int sreg, iconfReg, clk, acqreg;
  int iLevel, iVec, iEnable, iMode;

  if ((id < 0) || (id >= SIS3320_MAX_BOARDS) || (sis3320p[id] == NULL)) 
    {
      printf ("sis3320Status: ERROR : ADC id %d not initialized \n", id);
      return;
    }

  sreg = sis3320p[id]->csr;
  acqreg = sis3320p[id]->acqCsr;

  iconfReg = sis3320p[id]->intConfig;
  iLevel = (iconfReg & SIS3320_INT_LEVEL_MASK) >> 8;
  iVec = (iconfReg & SIS3320_INT_VEC_MASK);
  iEnable = (iconfReg & SIS3320_INT_ENABLE_MASK);
  iMode = (iconfReg & SIS3320_INT_MODE_MASK) >> 12;

  clk = (acqreg & SIS3320_CLOCKSOURCE_MASK) >> 12;

  printf ("\nSTATUS for ADC id %d at base address 0x%x, module id 0x%x \n", id,
	  (UINT32) sis3320p[id],sis3320p[id]->id);
  printf ("------------------------------------------------ \n");

  if (iEnable) 
    {
      printf (" Interrupts Enabled  - Mode = %d (0:RORA 1:ROAK) \n", iMode);
      printf (" VME Interrupt Level: %d   Vector: 0x%x \n", iLevel, iVec);
    }
  else
    {
      printf (" Interrupts Disabled - Mode = %d (0:RORA 1:ROAK) \n", iMode);
      printf (" VME Interrupt Level: %d   Vector: 0x%x \n", iLevel, iVec);
    }
  printf ("\n");
  printf ("  MODULE ID   register = 0x%08x \n", sis3320p[id]->id);
  printf ("\n");

  printf ("  Clock Source = %s\n", sis3320_clksrc_string[clk]);
  printf ("  STATUS      register = 0x%08x \n", sreg);
  printf ("  INT CONTROL register = 0x%08x \n", sis3320p[id]->intControl);
  printf ("  ACQ CONTROL register = 0x%08x \n", acqreg);
  printf ("\n");
  printf ("  External Start Delay = %d clocks\n", sis3320p[id]->extStartDelay);
  printf ("  External Stop  Delay = %d clocks\n", sis3320p[id]->extStopDelay
);

  printf ("\n  Event Configuration Information \n");
  printf ("  ------------------------------- \n");
  for (ii = 0; ii < 4; ii++) 
    {
      printf ("  Group %d  ADC %d, %d\n", (ii) , (2 * ii) , (2 * ii+1));
      printf ("     Event Config   register = 0x%08x\n",
	      sis3320p[id]->adcG[ii].eventConfig);
      printf ("     Sample Length register = 0x%08x\n",
	      sis3320p[id]->adcG[ii].sampLength);
      printf ("     Sample Start address register = 0x%08x\n",
	      sis3320p[id]->adcG[ii].sampStart);
    }

}

/******************************************************
 *
 *   Interrupt related functions
 *
 *
 */

/* Specify the Interrupt Level, Vector, and/or Source */
int sis3320IntSet (int id, int iLevel, int iVec, int iSource) 
{
  int iVal = 0;

  if ((id < 0) || (id >= SIS3320_MAX_BOARDS) || (sis3320p[id] == NULL)) 
    {
      logMsg ("sis3320IntSet: ERROR : SIS3320 id %d not initialized \n",
	      id, 0, 0, 0, 0, 0);
      return (ERROR);
    }

  /* If interrupts are already enabled - don't touch anything */
  if ((sis3320p[id]->intConfig) & SIS3320_INT_ENABLE_MASK) 
    {
      logMsg ("sis3320IntSet: ERROR : Interrupts are already enabled\n",
	      0, 0, 0, 0, 0, 0);
      return (ERROR);
    }

  if (iLevel) 
    iVal = (iLevel << 8);
  else
    iVal = (sis3320IntLevel << 8);

  if (iVec) 
    iVal |= (iVec & 0xff);
  else
    iVal |= sis3320IntVec;

  sis3320p[id]->intConfig = iVal;

  if (iSource) 
    sis3320p[id]->intControl = (iSource & 0xf);

  sis3320IntID = id;

  return (iVal);
}

/* *********************************** */
void sis3320IntAck (int intMask) 
{

  if (sis3320IntID < 0) 
    {
      logMsg ("sis3320IntAck: ERROR : Interrupt Module ID not set\n",
	      0, 0, 0, 0, 0, 0);
      return;
    }

  /* Clear Source */
  sis3320p[sis3320IntID]->intControl = (intMask << 20);

  /* Enable interrupts */
  SIS3320_IRQ_ENABLE (sis3320IntID);

}
/* *********************************** */
void
sis3320Int (void) 
{

  /* Disable interrupts */
  sysIntDisable (sis3320IntLevel);
  SIS3320_IRQ_DISABLE (sis3320IntID);

  sis3320IntCount++;

  if (sis3320IntRoutine != NULL) 
    {     /* call user routine */
      (*sis3320IntRoutine) (sis3320IntArg);
    }
  else
    {
      if ((sis3320IntID < 0) || (sis3320p[sis3320IntID] == NULL)) 
	{
	  logMsg ("sis3320Int: ERROR : SIS3320 id %d not initialized \n",
		  sis3320IntID, 0, 0, 0, 0, 0);
	  return;
	}
    }

  /* Acknowledge interrupt - re-enable */
  SIS3320_IRQ_ENABLE (sis3320IntID);

  /* Re-Enable interrupts on CPU */
  sysIntEnable (sis3320IntLevel);

}
/* *********************************** */
STATUS
sis3320IntConnect (int id, VOIDFUNCPTR routine, int arg, UINT16 level,
		   UINT16 vector) 
{

  if (sis3320IntRunning) 
    {
      printf ("sis3320IntConnect: ERROR : Interrupts already Initialized for SIS3320 id %d\n",
	      sis3320IntID);
      return (ERROR);
    }

  sis3320IntRoutine = routine;
  sis3320IntArg = arg;

  /* Check for user defined VME interrupt level and vector */
  if (level == 0) 
    {
      sis3320IntLevel = SIS3320_VME_INT_LEVEL;  /* use default */
    }
  else if (level > 7) 
    {
      printf ("sis3320IntConnect: ERROR: Invalid VME interrupt level (%d) . Must be (1-7) \n",
	      level);
      return (ERROR);
    }
  else
    {
      sis3320IntLevel = level;
    }

  if (vector == 0) 
    {
      sis3320IntVec = SIS3320_VME_INT_VEC;   /* use default */
    }
  else if ((vector < 32) || (vector > 255)) 
    {
      printf ("sis3320IntConnect: ERROR: Invalid interrupt vector (%d) . Must be (32 < vector < 255) \n",
	      vector);
      return (ERROR);
    }
  else
    {
      sis3320IntVec = (vector & 0xf0) + 1;
    }

  /* Connect the ISR */
#ifdef VXWORKSPPC
  if ((intDisconnect ((int) INUM_TO_IVEC (sis3320IntVec)) != 0)) 
    {
      printf ("sis3320IntConnect: ERROR disconnecting Interrupt\n");
      return (ERROR);
    }
#endif
  if ((intConnect (INUM_TO_IVEC (sis3320IntVec) , sis3320Int, sis3320IntArg)) 
      != 0) 
    {
      printf ("sis3320IntConnect: ERROR in intConnect() \n");
      return (ERROR);
    }

  sis3320IntSet (id, sis3320IntLevel, sis3320IntVec, 0);
  return (OK);
}
/* *********************************** */

void
sis3320IntEnable (int intMask) 
{

  if (sis3320IntID < 0) 
    {
      logMsg ("sis3320IntEnable: ERROR : Interrupt Module ID not set\n",
	      0, 0, 0, 0, 0, 0);
      return;
    }

  if (sis3320IntRunning) 
    {
      logMsg ("sis3320IntEnable: ERROR : Interrupts already Enabled for SIS3320 id %d\n",
	      sis3320IntID, 0, 0, 0, 0, 0);
      return;
    }

  sysIntDisable (sis3320IntLevel);

  if (intMask == 0) 
    { /* Check if any sources are enabled */
      if (((sis3320p[sis3320IntID]->intControl) &
	    SIS3320_INT_SOURCE_ENABLE_MASK) == 0) 
	{
	  logMsg ("sis3320IntEnable: ERROR : No Interrupt sources are specified\n",
		  0, 0, 0, 0, 0, 0);
	  return;
	}
    }
  else
    {
      /* Enable Interrupts */
      sis3320p[sis3320IntID]->intControl = (intMask & 0xf);
    }

  SIS3320_IRQ_ENABLE (sis3320IntID);
  sis3320IntRunning = 1;
  sysIntEnable (sis3320IntLevel);

  return;
}
/* *********************************** */
void
sis3320RORA() 
{
  int temp;

  if (sis3320IntID < 0) 
    {
      logMsg ("sis3320RORA: ERROR : Interrupt Module ID not set\n",
	      0, 0, 0, 0, 0, 0);
      return;
    }

  if (sis3320IntRunning) 
    {
      logMsg ("sis3320RORA: ERROR : Interrupts Enabled for SIS3320 id %d\n",
	      sis3320IntID, 0, 0, 0, 0, 0);
      return;
    }

  temp = sis3320p[sis3320IntID]->intConfig;
  temp = temp &  (~SIS3320_INT_MODE_MASK);
  sis3320p[sis3320IntID]->intConfig = temp;

  return;
}
/* *********************************** */
void
sis3320ROAK() 
{

  if (sis3320IntID < 0) 
    {
      logMsg ("sis3320ROAK: ERROR : Interrupt Module ID not set\n",
	      0, 0, 0, 0, 0, 0);
      return;
    }

  if (sis3320IntRunning) 
    {
      logMsg ("sis3320ROAK: ERROR : Interrupts Enabled for SIS3320 id %d\n",
	      sis3320IntID, 0, 0, 0, 0, 0);
      return;
    }

  sis3320p[sis3320IntID]->intConfig |= SIS3320_INT_MODE_MASK;

  return;
}
/* *********************************** */
void
sis3320IntDisable (int intMask) 
{

  if (sis3320IntID < 0) 
    {
      logMsg ("sis3320IntEnable: ERROR : Interrupt Module ID not set\n",
	      0, 0, 0, 0, 0, 0);
      return;
    }

  sysIntDisable (sis3320IntLevel);
  SIS3320_IRQ_DISABLE (sis3320IntID);
  sis3320IntRunning = 0;

  if (intMask) 
    {
      sis3320p[sis3320IntID]->csr = (intMask & 0xf) << 16;
    }

  return;

}

/* Delcare Key Address functions */

void
sis3320Reset (int id) 
{
 
  printf("Into sis3320Reset  %d  0x%x \n",id,sis3320p[id]);

  if ((id < 0) || (id >= SIS3320_MAX_BOARDS) || (sis3320p[id] == NULL)) 
    {
      logMsg ("sis3320Reset: ERROR : ADC id %d not initialized \n",
	      id, 0, 0, 0, 0, 0);
      return;
    }
  sis3320p[id]->reset = 1;
   sis3320SetDac(0,-99999); /* reset DAC */
}

/* *********************************** */
void
sis3320Start (int id) 
/* Arm Sampling Logic and wait for LEMO or software start */
{
  if ((id < 0) || (id >= SIS3320_MAX_BOARDS) || (sis3320p[id] == NULL)) 
    {
      logMsg ("sis3320Start: ERROR : ADC id %d not initialized \n",
	      id, 0, 0, 0, 0, 0);
      return;
    }

  sis3320p[id]->armSampLogic = 1;

}
/* *********************************** */
void
sis3320Stop (int id) 
{
  if ((id < 0) || (id >= SIS3320_MAX_BOARDS) || (sis3320p[id] == NULL)) 
    {
      logMsg ("sis3320Stop: ERROR : ADC id %d not initialized \n",
	      id, 0, 0, 0, 0, 0);
      return;
    }
  sis3320p[id]->stopSample = 1;
}

/* *********************************** */
void
sis3320StartSampling (int id) 
/* Start Sampling */
{
  if ((id < 0) || (id >= SIS3320_MAX_BOARDS) || (sis3320p[id] == NULL)) 
    {
      logMsg ("sis3320Start: ERROR : ADC id %d not initialized \n",
	      id, 0, 0, 0, 0, 0);
      return;
    }

  sis3320p[id]->startSample = 1;

}
/* *********************************** */
void
sis3320StopSampling (int id) 
/* Stop Sampling */
{
  if ((id < 0) || (id >= SIS3320_MAX_BOARDS) || (sis3320p[id] == NULL)) 
    {
      logMsg ("sis3320Start: ERROR : ADC id %d not initialized \n",
	      id, 0, 0, 0, 0, 0);
      return;
    }

  sis3320p[id]->stopSample = 1;

}

/* Acquisition/Mode Functions */

/* *********************************** */
void
sis3320Enable (int id)
{
  if ((id < 0) || (id >= SIS3320_MAX_BOARDS) || (sis3320p[id] == NULL))
    {
      logMsg ("sis3320Enable: ERROR : ADC id %d not initialized \n", 
	      id, 0, 0, 0, 0, 0);
      return;
    }

  
  sis3320p[id]->armSampLogic = 1;

}

/* *********************************** */
void
sis3320Disable (int id)     
{
  if ((id < 0) || (id >= SIS3320_MAX_BOARDS) || (sis3320p[id] == NULL))
    {
      logMsg ("sis3320Disable: ERROR : ADC id %d not initialized \n", 
	      id, 0, 0, 0, 0, 0);
      return;
    }
  
  sis3320p[id]->disarmSampLogic = 1;
}

/* *********************************** */
int
sis3320StopDelay (int id, unsigned long delay) 
{
  if ((id < 0) || (id >= SIS3320_MAX_BOARDS) || (sis3320p[id] == NULL)) 
    {
      logMsg ("sis3320StopDelay: ERROR : ADC id %d not initialized \n",
	      id, 0, 0, 0, 0, 0);
      return (ERROR);
    }

  /* Delay should be passed in clocks */

  sis3320p[id]->extStopDelay = delay;

  return ((int) (sis3320p[id]->extStopDelay));
}

/* *********************************** */
int
sis3320StartDelay (int id, unsigned long delay) 
{
  if ((id < 0) || (id >= SIS3320_MAX_BOARDS) || (sis3320p[id] == NULL)) 
    {
      logMsg ("sis3320StartDelay: ERROR : ADC id %d not initialized \n",
	      id, 0, 0, 0, 0, 0);
      return (ERROR);
    }

  /* Delay should be passed in clocks */

  sis3320p[id]->extStartDelay = delay;

  return ((int) (sis3320p[id]->extStartDelay));
}


/* *********************************** */
unsigned int
sis3320AcqConfig (int id, unsigned long val) 
{
  if ((id < 0) || (id >= SIS3320_MAX_BOARDS) || (sis3320p[id] == NULL)) 
    {
      logMsg ("sis3320AcqConfig: ERROR : ADC id %d not initialized \n",
	      id, 0, 0, 0, 0, 0);
      return (0xffffffff);
    }

  /* If val is 0 just return current value. Writing a 0 does nothing  */

  printf ("acqConfig sending 0x%x\n", val);
  if (val) 
    sis3320p[id]->acqCsr = val;

  return (sis3320p[id]->acqCsr);
}

/* *********************************** */
void
sis3320EventConfig (int id, unsigned long val) 
{
  if ((id < 0) || (id >= SIS3320_MAX_BOARDS) || (sis3320p[id] == NULL)) 
    {
      logMsg ("sis3320EventConfig: ERROR : ADC id %d not initialized \n",
	      id, 0, 0, 0, 0, 0);
      return;
    }

  sis3320p[id]->eventConfig = val;
}

/* Readout Functions */

#define SIS3320_MAX_PRINT_VALS    256

/* *********************************** */
int
sis3320PrintEvent (int id, int adc, int evt, int flag) 
{
  /* flag = 0 to see data without unpacking (for a single adc)
   * 1 for data unpacked as hex
   * 2 for data unpacked as dec */
  int ii;
  int grp, pagesize, tmp;
  int nsamp, e_offset = 0;
  int data, d0, d1;

  if ((id < 0) || (id >= SIS3320_MAX_BOARDS) || (sis3320p[id] == NULL)) 
    {
      logMsg ("sis3320PrintEvent: ERROR : ADC id %d not initialized \n",
	      id, 0, 0, 0, 0, 0);
      return (ERROR);
    }

  if ((adc < 0) || (adc > 7)) 
    {
      printf ("sis3320PrintEvent: ERROR: adc value %d out of range (0-7) \n",
	      adc);
      return (-1);
    }

  grp = adc >> 1;
  tmp = (sis3320p[id]->adcG[grp].eventConfig) & SIS3320_PAGESIZE_MASK;
  pagesize = sis3320_pagesize[tmp];
  e_offset = evt * pagesize;

  if (pagesize > SIS3320_MAX_PRINT_VALS) 
    {
      nsamp = SIS3320_MAX_PRINT_VALS;
      printf ("\n  Event data for ADC %d, Samples: %d (first %d values) :\n\n",
	      adc, pagesize, nsamp);
    }
  else
    {
      nsamp = pagesize;
      printf ("\n  Event data for ADC %d, Samples: %d :\n\n",
	      adc, pagesize);
    }
  for (ii = 0; ii < nsamp/2; ii++) 
    {
      data = sis3320p[id]->adcData[adc*ADC_2MB + ii+e_offset];
      d0 = (data & SIS3320_ADC_HIDATA_MASK) >> 16;
      d1 = (data & SIS3320_ADC_LODATA_MASK);
      if (flag == 1) 
	{
	  printf ("  0x%03x  0x%03x", d1, d0);
	}
      else if (flag == 2) 
	{
	  if (adc % 2) 
	    printf ("   %8d   %8d", d1, d0);
	}
      else
	{
	  if ((ii % 8) == 0) printf ("\n    ");
	  printf ("  0x%08x", data);
	}
    }
  printf ("\n");

  printf ("\n");

  return (pagesize);

}

/* *********************************** */
int
sis3320PrintEventDir (int id, int adc) 
{

  int ii;
  int grp, pagesize, tmp;
  int nevents;
  int trig, full, endAddr;

  if ((id < 0) || (id >= SIS3320_MAX_BOARDS) || (sis3320p[id] == NULL)) 
    {
      logMsg ("sis3320PrintEvent: ERROR : ADC id %d not initialized \n",
	      id, 0, 0, 0, 0, 0);
      return (-1);
    }

  if ((adc < 0) || (adc > 7)) 
    {
      printf ("sis3320PrintEvent: ERROR: adc value %d out of range (0-7) \n",
	      adc);
      return (-1);
    }

  grp = adc >> 1;
  tmp = (sis3320p[id]->adcG[grp].eventConfig) & SIS3320_PAGESIZE_MASK;
  pagesize = sis3320_pagesize[tmp];
  // For now print whole directory
  nevents = 512;

  printf ("\n  Event directory data for ADC %d, nevents %d:\n",
	  adc, nevents);

  for (ii = 0; ii < nevents; ii++) 
    {
      tmp = sis3320p[id]->adcG[grp].eventDir[adc%2][ii];
      endAddr = tmp & SIS3320_EDIR_END_ADDR_MASK;
      full = (tmp & SIS3320_EDIR_WRAP) >> 28;
      trig = (tmp & SIS3320_EDIR_TRIG) >> 29;

      printf ("      Event %4d: End Addr = 0x%08x   Wrap = %d  T1 = %d\n",
	      ii, endAddr, full, trig);

    }
  printf ("\n");

  return (nevents);
}

/* *********************************** */
int
sis3320ReadEvent (int id, UINT32 *data, int nsamp, int adc, int evt) 
{

  int ii, jj, tmp;
  int grp, pagesize, stopIndex;
  int e_offset = 0;
  unsigned int pmask;

  if ((id < 0) || (id >= SIS3320_MAX_BOARDS) || (sis3320p[id] == NULL)) 
    {
      logMsg ("sis3320ReadEvent: ERROR : ADC id %d not initialized \n",
	      id, 0, 0, 0, 0, 0);
      return (-1);
    }

  if ((adc < 0) || (adc > 7)) 
    {
      logMsg ("sis3320readEvent: ERROR: adc value %d out of range (0-7) \n",
	      adc, 0, 0, 0, 0, 0);
      return (-1);
    }

  grp = adc >> 1;
  tmp = (sis3320p[id]->adcG[grp].eventConfig) & SIS3320_PAGESIZE_MASK;
  pagesize = sis3320_pagesize[tmp];
  e_offset = evt * pagesize;
  pmask = pagesize-1;

  /* Find Stop Index so that we can properly time order the samples */
  tmp = sis3320p[id]->adcG[grp].eventDir[adc%2][evt];
  stopIndex = tmp & SIS3320_EDIR_END_ADDR_MASK;

  for (ii = 0; ii < pagesize && ii < nsamp/2; ii++) 
    {
      jj = (stopIndex + ii) & pmask;
      data[ii] = sis3320p[id]->adcData[adc*ADC_2MB + jj+e_offset];
    }

  return (pagesize);
}

/* *********************************** */
void
sis3320Display (int id)
{
  if ((id < 0) || (id >= SIS3320_MAX_BOARDS) || (sis3320p[id] == NULL)) 
    {
      printf ("sis3320Display: ERROR : ADC id %d not initialized \n", id);
      return;
    }

  printf ("FADC module %d ==========================%10u\n", id);
  printf ("csr                    0x%10x \n", sis3320p[id]->csr);
  printf ("id                     0x%10x \n", sis3320p[id]->id);
  printf ("intConfig              0x%10x \n", sis3320p[id]->intConfig);
  printf ("intControl             0x%10x \n", sis3320p[id]->intControl);
  printf ("acqCsr                 0x%10x \n", sis3320p[id]->acqCsr);
  printf ("extStartDelay          0x%10x %10u \n", 
	  sis3320p[id]->extStartDelay,sis3320p[id]->extStartDelay);
  printf ("extStopDelay           0x%10x %10u\n",
	 sis3320p[id]->extStopDelay ,sis3320p[id]->extStopDelay);
  printf ("maxNofEvReg            0x%10x %10u\n",
	  sis3320p[id]->maxNofEvReg,sis3320p[id]->maxNofEvReg);
  printf ("actEvtCounter          0x%10x %10u\n", 
	  sis3320p[id]->actEvtCounter,sis3320p[id]->actEvtCounter);
  printf ("cbltBxReg              0x%10x \n", sis3320p[id]->cbltBxReg);
  printf ("adcMemPageReg          0x%10x \n", sis3320p[id]->adcMemPageReg);
  printf ("dacCsr                 0x%10x \n", sis3320p[id]->dacCsr);
  printf ("dacDataReg             0x%10x %10u\n", 
	 sis3320p[id]->dacDataReg ,sis3320p[id]->dacDataReg);
  printf ("adcGainReg             0x%10x \n", sis3320p[id]->adcGainReg);
  printf ("xilJtagTestDataIn      0x%10x \n", sis3320p[id]->xilJtagTestDataIn);
}

/* *********************************** */
void
sis3320GrpDisplay (int id, int grp)
{
  int i;
  volatile struct adcGroup_struct* adcg;

  if ((id < 0) || (id >= SIS3320_MAX_BOARDS) || (sis3320p[id] == NULL)) 
    {
      printf ("sis3320GrpDisplay: ERROR : ADC id %d not initialized \n", id);
      return;
    }

  if ((grp < 0) || (grp > 3)) 
    {
      printf ("sis3320PrintEvent: ERROR: group value %d out of range (0-3) \n",
	      grp);
      return;
    }
  /* changed from adcG[grp-1] to adcG[grp] for consistency 7/29/08 gbf */
  adcg = &(sis3320p[id]->adcG[grp]);
  printf ("ADC group %d =====\n", grp);
  printf ("eventConfig          0x%8x\n", adcg->eventConfig);
  printf ("sampLength           0x%8x  %d\n",
	  adcg->sampLength,adcg->sampLength);
  printf ("sampStart            0x%8x\n", adcg->sampStart);
  printf ("adcInputMode         0x%8x\n", adcg->adcInputMode);
  printf ("nextSampAdr[%d]       0x%8x\n", (grp<<1)-1, adcg->nextSampAdr[0]);
  printf ("nextSampAdr[%d]       0x%8x\n", grp<<1, adcg->nextSampAdr[1]);
  printf ("actSampVal           0x%8x\n", adcg->actSampVal);
  printf ("testReg              0x%8x\n", adcg->testReg);
  printf ("ddr2MemLogVerify     0x%8x\n", adcg->ddr2MemLogVerify);
  printf ("trigFlagClearCtr     0x%8x\n", adcg->trigFlagClearCtr);
  printf ("trigReg (setup)[%d]   0x%8x\n", (grp<<1)-1, adcg->trigReg[0][0]);
  printf ("trigReg (thresh)[%d]  0x%8x\n", (grp<<1)-1, adcg->trigReg[0][1]);
  printf ("trigReg (setup)[%d]   0x%8x\n", grp<<1, adcg->trigReg[1][0]);
  printf ("trigReg (thresh)[%d]  0x%8x\n", grp<<1, adcg->trigReg[1][1]);
  printf ("actEvtCounter  = (dec) %d ", sis3320p[id]->actEvtCounter);
/*   for (i = 0; i < sis3320p[id]->actEvtCounter; ++i) */
/*     { */
/*       printf("\n i = %d \n",i); */
/*       printf ("eventDir[%d]    (dec) %d =  0x%x\n", (grp<<1)-1, adcg->eventDir[0][i],adcg->eventDir[0][i]); */
/*       printf ("eventDir[%d]    (dec) %d =  0x%x\n", grp<<1, adcg->eventDir[1][i],adcg->eventDir[1][i]); */
/*     } */
}

void
sis3320PrintChannelSettings(int id, int chan){
  /* prints out DAC and accumulator settings */
  unsigned int n5n6;
  int n5_before, n5_after,n6_before, n6_after;
  int lochan,hichan;
  int i;
  if(chan==-1){
    lochan=0;
    hichan=7;
  }else if (chan>=0 && chan<8){
    lochan=chan;
    hichan=chan;
  } else {
    return;
  }
  for( i=lochan; i<=hichan; i++){
    n5n6=sis3320GetN5N6(id,i);
    n6_after=n5n6 & 0xff;
    n5n6=n5n6>>8;
    n6_before=n5n6 & 0xff;
    n5n6=n5n6>>8;
    n5_after=n5n6 & 0xff;
    n5n6=n5n6>>8;
    n5_before=n5n6 & 0xff;
    printf("Module     %d  Channel %d\n",id,i);
    printf("DAC          %d\n", sis3320GetDac(id,i));
    printf("Threshhold 1 %d\n",sis3320GetThresh(id,i,1));
    printf("Threshhold 2 %d\n",sis3320GetThresh(id,i,2));
    printf(" Accum 5 Precount %8d    Postcount %8d\n",n5_before,n5_after);
    printf(" Accum 6 Precount %8d    Postcount %8d\n",n6_before,n6_after);
    printf("\n");
  }
  return;
}
/* *********************************** */
int
sis3320SetupMapping(unsigned int id,
		  unsigned int adc_channel,
		  unsigned int sample_index,
		  unsigned int sample_length,
		  unsigned int* sub_page_rtn,    /* rtn memory subpage*/
       	          unsigned int* sub_index_rtn,  /*rtn  address of first data word*/
		  unsigned int* sub_length_rtn, /* rtn # of samples this subpage*/
		  unsigned int** mem_loc_rtn)   /* rtn VME starting memory location*/
{
  /* returns info need to read out set of data words from FADC 
  * and sets ADC Memory Page register
   * inputs:
   * id  FADC module number
   * adc_channel  adc channel (0 - 7)
   * sample_index   index of first data sample desired
   * sample_length  number of data samples desired
   * output:
   * sub_page_rtn  use "sis3320p[id]->adcMemPageReg=sub_page" to set to correct page
   * sub_index_rtn  index of first WORD corresponding to data sample "index"
   * sub_length_rtn  Number of data WORDS (two data samples per word)
   * mem_loc_rtn    Starting VME address of data
   *
   * NOTE 1:  ADC Memory page is set for the correct 4M Samples.  This routine
   *  must be called more than once if there are more than 4M samples to be readdddd
   *
   * NOTE 2: reading out "sub_words" number of data words will either correspond
   * to readding out "length" number of samples OR it will bring us to the end
   * of the address page.  In the latter case, this routine will have to be
   * called again to get mapping for the next set of data.
   */
 
  unsigned int max_page_sample_length;
  unsigned int page_sample_length_mask;

  int sub_index;
  int sub_page;
  int sub_length;
  int rtn;
 
  if ((id < 0) || (id >= SIS3320_MAX_BOARDS) || (sis3320p[id] == NULL)) 
    {
      logMsg ("sis3320EventConfig: ERROR : ADC id %d not initialized \n",
	      id, 0, 0, 0, 0, 0);
      return;
    }
  /*
   * max_page_sample_length
   * Maximum # of samples per address space page = 4M.  Not to be
   * confused with length of wrap page or with event sample length!
   * Don't change this.
   */
  max_page_sample_length  = SIS3320_4M;  
  page_sample_length_mask = max_page_sample_length - 1;
  
  sub_index =  
    (sample_index &  page_sample_length_mask) ; /* max 32 MSample*/
  sub_page = (sample_index>>22) & 0x7;    /*upper 3 bits determine memory page */
  sub_length     =  SIS3320_4M-sample_index;  /*# remaining samples this page*/
  if(sub_length>sample_length) {
    sub_length=sample_length; /*finish with this page*/
    rtn=0;                    /*flag for last set of data*/
  } else {
    rtn=1;                    /*flag for more data after this page*/
  }
/* set page mapping and determine actual starting address*/
  sis3320p[id]->adcMemPageReg=sub_page;

  *sub_page_rtn=sub_page;
  *sub_index_rtn=sub_index;
  *sub_length_rtn=sub_length;
  *mem_loc_rtn = &(sis3320p[id]->adcData[ adc_channel*ADC_2MB+sub_index]);
  return rtn;
}

/* *********************************** */
void
testmap(int index, int length){
  unsigned int id, adc_channel;
  unsigned int sub_page, sub_index,sub_length;
  unsigned int* mem_loc;
  int rtn;
  rtn=sis3320SetupMapping(0,0,index,length,&sub_page,&sub_index,&sub_length,&mem_loc);
  printf(" index %d length %d gives:\n", index,length);
  printf("    sub_page %d  sub_index %d  sub_length %d\n",
	 sub_page,sub_index,sub_length);
  printf("    VME memory location %p\n",mem_loc);
  printf(" return %d \n", rtn);
  return;
}
/* *********************************** */
int 
sis3320_Read_ADC_Channel (unsigned int id,
			  unsigned int adc_channel /* 0 to 7 */, 
			  unsigned int event_sample_start_addr, 
			  unsigned int event_sample_length,
			  unsigned int* uint_adc_buffer,
			  unsigned int lbuffer)
{
  /*
   * Read from module id
   * adc channel adc_channel
   * starting at sample event_sample_start_addr
   * read number of samples event_sample_length
   * write to uint_adc_buffer (2 samples / lword)
   * which is of length lbuffer lwords
   * (truncate data if they don't fit)
   */

  unsigned int i;
  unsigned int data;
  unsigned int addr;
  unsigned req_nof_lwords;
  
  unsigned int max_page_sample_length;
  unsigned int page_sample_length_mask;
  unsigned int next_event_sample_start_addr;
  unsigned int rest_event_sample_length;
  unsigned int sub_event_sample_length;
  unsigned int sub_event_sample_addr;
  unsigned int sub_max_page_sample_length;
  
  unsigned int sub_page_addr_offset;
  unsigned int index_num_data;
  
  if ((id < 0) || (id >= SIS3320_MAX_BOARDS) || (sis3320p[id] == NULL)) 
    {
      logMsg ("sis3320EventConfig: ERROR : ADC id %d not initialized \n",
	      id, 0, 0, 0, 0, 0);
      return;
    }

  printf ("id = 0x%x\n", id);
  printf ("adc_channel  = 0x%x\n", adc_channel);
  printf ("event_sample_start_addr  = 0x%x\n", event_sample_start_addr);
  printf ("event_sample_length  = 0x%x\n", event_sample_length);

  /*
   * max_page_sample_length
   * Maximum # of samples per address space page = 4M.  Not to be
   * confused with length of wrap page or with event sample length!
   * Don't change this.
   */

  max_page_sample_length  = SIS3320_4M;  
  page_sample_length_mask = max_page_sample_length - 1;
  
  next_event_sample_start_addr =  
    (event_sample_start_addr &  SIS3320_ADC_ADDR_OFFSET_MASK) ; /* max 32 MSample*/
  rest_event_sample_length     =  (event_sample_length & 0x03fffffc)     ; /* max 32 MSample*/
  if (rest_event_sample_length  >= SIS3320_32M) 
    rest_event_sample_length =  SIS3320_32M;
  
  if (rest_event_sample_length  >= lbuffer*2) 
    rest_event_sample_length =  lbuffer*2;
  
  printf ("next_event_sample_start_addr = 0x%08x  rest_event_sample_length = 0x%08x \n", next_event_sample_start_addr, rest_event_sample_length);
  
  index_num_data = 0x0;
  while (rest_event_sample_length > 0)
    {
      sub_event_sample_addr =
	(next_event_sample_start_addr & page_sample_length_mask);
      sub_max_page_sample_length =
	max_page_sample_length - sub_event_sample_addr;
      if (rest_event_sample_length >= sub_max_page_sample_length) 
	sub_event_sample_length = sub_max_page_sample_length ;
      else 
	sub_event_sample_length = rest_event_sample_length ; /* - sub_event_sample_addr*/
      
      printf("The worlds most complicated crap 0x%x 0x%x \n",next_event_sample_start_addr,(next_event_sample_start_addr>>22) );

      sub_page_addr_offset = (next_event_sample_start_addr >> 22) & 0x7;
      
      printf ("sub_event_sample_addr  = 0x%08x  sub_event_sample_length  = 0x%08x \n",sub_event_sample_addr, sub_event_sample_length);
      printf ("sub_page_addr_offset   = 0x%08x   \n",sub_page_addr_offset);
      
      /* set page*/
      data = sub_page_addr_offset;
      sis3320p[id]->adcMemPageReg = data;  /* was "data" */
      printf ("Set hardware page 0x%x and 0x%x\n", data,adc_channel);

      /* read         */
      req_nof_lwords = (sub_event_sample_length) / 2 ; /* Lwords*/
      addr = (sub_event_sample_addr) / 2   ; /* Lwords*/
      
      /*      printf ("id %d adc %d addr 0x%x lwords %d\nStart at 0x%x containing 0x%x going to 0x%x containing 0x%x\n", id, adc_channel, addr, req_nof_lwords, &(sis3320p[id]->adcData[adc_channel][addr]), sis3320p[id]->adcData[adc_channel][addr], &(sis3320p[id]->adcData[adc_channel][addr+req_nof_lwords-1]), sis3320p[id]->adcData[adc_channel][addr+req_nof_lwords-1]); */

      printf("req_nof_lwords = %d\n",req_nof_lwords);

      for (i = 0; i < req_nof_lwords; i++) 
	{
	  uint_adc_buffer[i+index_num_data] = 
	    sis3320p[id]->adcData[adc_channel*ADC_2MB + addr+i];
          if (i < 20) {
               printf("DATA::::  adc_channel %d   addr %d   i=%d   index = %d   data = %d = 0x%x \n",
               adc_channel,addr,i,adc_channel*ADC_2MB + addr+i,
               sis3320p[id]->adcData[adc_channel*ADC_2MB + addr+i],
               sis3320p[id]->adcData[adc_channel*ADC_2MB + addr+i]);
	  }
	}
      
      index_num_data = index_num_data + req_nof_lwords;
      
      next_event_sample_start_addr += sub_event_sample_length    ;  
      rest_event_sample_length     -= sub_event_sample_length    ;  
      printf ("next_event_sample_start_addr = 0x%08x  rest_event_sample_length = 0x%08x \n",next_event_sample_start_addr, rest_event_sample_length);
      
      /*printf ("addr           = 0x%08x \n",addr);*/
      /*printf ("req_nof_lwords = 0x%08x \n",req_nof_lwords);*/
    }
  
  return 0;
}


/* *********************************** */
unsigned int 
sis3320_Sum_ADC_Channel (unsigned int id,
			 unsigned int adc_channel /* 0 to 7 */, 
			 unsigned int event_sample_start_addr, 
			 unsigned int event_sample_length,
			 unsigned int evc)
{
  /*
   * Sum data from module id
   * adc channel adc_channel
   * starting at sample event_sample_start_addr
   * sum number of samples event_sample_length
   * interpret like an event config register evc (for shift down
   * and/or user in; latter only works if data taken with user in to
   * data enabled)
   */

  static int debug=0;    /* to print debug stuff, set to 1 or 2 */
  unsigned int i, idx;
  unsigned int data;
  unsigned int addr;
  unsigned req_nof_lwords;
  
  unsigned int max_page_sample_length;
  unsigned int page_sample_length_mask;
  unsigned int next_event_sample_start_addr;
  unsigned int rest_event_sample_length;
  unsigned int sub_event_sample_length;
  unsigned int sub_event_sample_addr;
  unsigned int sub_max_page_sample_length;
  
  unsigned int sub_page_addr_offset;

  unsigned int sum;
  unsigned int term32,term;

  if ((id < 0) || (id >= SIS3320_MAX_BOARDS) || (sis3320p[id] == NULL)) 
    {
      logMsg ("sis3320EventConfig: ERROR : ADC id %d not initialized \n",
	      id, 0, 0, 0, 0, 0);
      return;
    }
  if(debug) {
    printf ("Sum_ADC_Channel called with id = 0x%x\n", id);
    printf ("adc_channel  = 0x%x\n", adc_channel);
    printf ("event_sample_start_addr  = 0x%x\n", event_sample_start_addr);
    printf ("event_sample_length  = 0x%x\n", event_sample_length);
  }

  /*
   * max_page_sample_length
   * Maximum # of samples per address space page = 4M.  Not to be
   * confused with length of wrap page or with event sample length!
   * Don't change this.
   */

  max_page_sample_length  = SIS3320_4M;  
  page_sample_length_mask = max_page_sample_length - 1;
  
  next_event_sample_start_addr =  
    (event_sample_start_addr &  SIS3320_ADC_ADDR_OFFSET_MASK) ; /* max 32 MSample*/
  rest_event_sample_length     =  (event_sample_length & 0x03fffffc)     ; /* max 32 MSample*/
  if (rest_event_sample_length  >= SIS3320_32M) 
    rest_event_sample_length =  SIS3320_32M;
  
  if(debug) printf ("next_event_sample_start_addr = 0x%08x  rest_event_sample_length = 0x%08x \n", next_event_sample_start_addr, rest_event_sample_length);
  
  sum = 0;
  while (rest_event_sample_length > 0)
    {
      sub_event_sample_addr =
	(next_event_sample_start_addr & page_sample_length_mask);
      sub_max_page_sample_length =
	max_page_sample_length - sub_event_sample_addr;
      if (rest_event_sample_length >= sub_max_page_sample_length) 
	sub_event_sample_length = sub_max_page_sample_length ;
      else 
	sub_event_sample_length = rest_event_sample_length ; /* - sub_event_sample_addr*/
      
      sub_page_addr_offset = (next_event_sample_start_addr >> 22) & 0x7;
      
      /*printf ("sub_event_sample_addr  = 0x%08x  sub_event_sample_length  = 0x%08x \n",sub_event_sample_addr, sub_event_sample_length);*/
      /*printf ("sub_page_addr_offset   = 0x%08x   \n",sub_page_addr_offset);*/
      
      /* set page*/
      data = sub_page_addr_offset;
      sis3320p[id]->adcMemPageReg = data;
      if(debug>1) printf ("Set hardware page 0x%x\n", data);

      /* sum         */
      req_nof_lwords = (sub_event_sample_length) / 2 ; /* Lwords*/
      addr = (sub_event_sample_addr) / 2   ; /* Lwords*/
      
      /*      printf ("id %d adc %d addr 0x%x lwords %d\nStart at 0x%x containing 0x%x going to 0x%x containing 0x%x\n", id, adc_channel, addr, req_nof_lwords, &(sis3320p[id]->adcData[adc_channel][addr]), sis3320p[id]->adcData[adc_channel][addr], &(sis3320p[id]->adcData[adc_channel][addr+req_nof_lwords-1]), sis3320p[id]->adcData[adc_channel][addr+req_nof_lwords-1]); */
      for (i = 0; i < req_nof_lwords; i++) 
	{
        idx = adc_channel*ADC_2MB + addr+i;
        if (idx < 0 || idx > HARDMAX*ADC_2MB) {
           printf("Warning(1): index violation\n");
           idx = 0;  /* memory limit on the cpu !! */
	}
	term32 = sis3320p[id]->adcData[idx];
        term = term32 & SIS3320_ADC_LODATA_MASK;

	if (!(evc & SIS3320_EVENT_CONF_ENABLE_USER_IN_ACCUM_GATE) ||
	      term & 0x8000) {
	    if (evc && SIS3320_EVENT_CONF_SHIFT_DOWN_ACCUM) {
		term = term >> 4;
	    }
	}
	sum += term;
	term = (term32 & SIS3320_ADC_HIDATA_MASK) >> 16;
	if (!(evc & SIS3320_EVENT_CONF_ENABLE_USER_IN_ACCUM_GATE) ||
	      term & 0x8000) {
	    if (evc && SIS3320_EVENT_CONF_SHIFT_DOWN_ACCUM) {
		term = term >> 4;
	    }
	}
	sum += term;
      }
      
      next_event_sample_start_addr += sub_event_sample_length    ;  
      rest_event_sample_length     -= sub_event_sample_length    ;  
      if(debug>1){
	printf ("next_event_sample_start_addr = 0x%08x  rest_event_sample_length = 0x%08x \n",next_event_sample_start_addr, rest_event_sample_length);
      
      /*printf ("addr           = 0x%08x \n",addr);*/
      /*printf ("req_nof_lwords = 0x%08x \n",req_nof_lwords);*/
      }
    }
  
  return sum;
}


/* *********************************** */
void
sis3320Run (int id, int rflag, int whichadc) 
{
  unsigned int grp;

  int freeMem = 0;

  unsigned int i;
  unsigned int j;
  unsigned int ievt;

  int return_code;

  unsigned int gl_loop_counter;
 
  int iacc;
  
  unsigned int data;
  unsigned int addr;
  unsigned long numa,acc1,acc2;
  
  unsigned int ADC_InputTestMode_array[4];
  unsigned int event_sample_length;
  unsigned int event_sample_start_addr;
  
  unsigned int clock_setting;
  unsigned int poll_counter;
  
  unsigned int wrap_sample_offset;
  unsigned int wrap_read_addr_offset;
  unsigned int wrap_read_addr_mask;
  unsigned int request_nof_words;
  
  unsigned int i_adc;
  unsigned page_size_wrap;

  unsigned int nof_events;
  unsigned int evdircopy[512];
  unsigned int this_event_length;

  int nprint,MAX_PRINT;
  int i1,i2,ichan,adcval1,adcval2,adc;

  char line_in[128];

  /******************************************************************/
  
  gl_loop_counter = 0;

  MAX_PRINT=10000;

/* #define TESTADC */
#ifdef TESTADC
  // ADC test setup for 4 groups  
  printf ("ADC test mode ========================\n");
  ADC_InputTestMode_array[0] = 
    SIS3320_ENABLE_ADC_TEST_DATA | SIS3320_ADC_TEST_DATA_16BIT_MODE 
    | 0x100; 
  ADC_InputTestMode_array[1] = 
    SIS3320_ENABLE_ADC_TEST_DATA | SIS3320_ADC_TEST_DATA_16BIT_MODE 
    | 0x200; 
  ADC_InputTestMode_array[2] =  
    SIS3320_ENABLE_ADC_TEST_DATA | SIS3320_ADC_TEST_DATA_16BIT_MODE 
    | 0x400; 
  ADC_InputTestMode_array[3] =  
    SIS3320_ENABLE_ADC_TEST_DATA | SIS3320_ADC_TEST_DATA_16BIT_MODE 
    | 0x800; 
#else  
  ADC_InputTestMode_array[0] = 0; /* ADC12 no test data */
  ADC_InputTestMode_array[1] = 0; /* ADC34 no test data */
  ADC_InputTestMode_array[2] = 0; /* ADC56 no test data */
  ADC_InputTestMode_array[3] = 0; /* ADC78 no test data */
#endif

  for (i_adc = 0; i_adc < 4; i_adc++) {
    printf("Test Input %d = %d \n",i_adc,ADC_InputTestMode_array[i_adc]);
  }


  /*
   *   event_sample_length
   *
   * When sample length stop is enabled, sampling stops after this
   * many samples.
   *
   * When sample length stop is disabled, samples past this number are
   * marked with wrap bit.  If set to page length this will flag
   * samples that wrap the entire page.  Note bits 0-1 are unused.
   */

  event_sample_length = SIS3320_16M; /* Wrap half memory */
  //event_sample_length = SIS3320_512; /* try 512 */
  switch (event_sample_length)
    {
    case SIS3320_64:
      page_size_wrap = SIS3320_EVENT_CONF_PAGE_SIZE_64_WRAP;
      break;
    case SIS3320_128:
      page_size_wrap = SIS3320_EVENT_CONF_PAGE_SIZE_128_WRAP;
      break;
    case SIS3320_256:
      page_size_wrap = SIS3320_EVENT_CONF_PAGE_SIZE_256_WRAP;
      break;
    case SIS3320_512:
      page_size_wrap = SIS3320_EVENT_CONF_PAGE_SIZE_512_WRAP;
      break;
    case SIS3320_1K:
      page_size_wrap = SIS3320_EVENT_CONF_PAGE_SIZE_1K_WRAP;
      break;
    case SIS3320_4K:
      page_size_wrap = SIS3320_EVENT_CONF_PAGE_SIZE_4K_WRAP;
      break;
    case SIS3320_16K:
      page_size_wrap = SIS3320_EVENT_CONF_PAGE_SIZE_16K_WRAP;
      break;
    case SIS3320_64K:
      page_size_wrap = SIS3320_EVENT_CONF_PAGE_SIZE_64K_WRAP;
      break;
    case SIS3320_256K:
      page_size_wrap = SIS3320_EVENT_CONF_PAGE_SIZE_256K_WRAP;
      break;
    case SIS3320_1M:
      page_size_wrap = SIS3320_EVENT_CONF_PAGE_SIZE_1M_WRAP;
      break;
    case SIS3320_4M:
      page_size_wrap = SIS3320_EVENT_CONF_PAGE_SIZE_4M_WRAP;
      break;
    case SIS3320_16M:
      page_size_wrap = SIS3320_EVENT_CONF_PAGE_SIZE_16M_WRAP;
      printf("HERERERERERERE %d 0x%x \n",page_size_wrap,page_size_wrap);
      break;
    case SIS3320_32M:
      page_size_wrap = 0;
      break;
    }
  if (event_sample_length < SIS3320_32M)
    page_size_wrap |= SIS3320_EVENT_CONF_ENABLE_WRAP_PAGE_MODE;

  printf("page again........... %d 0x%x \n",page_size_wrap,page_size_wrap);

  event_sample_start_addr = 0x0;

  if ((id < 0) || (id >= SIS3320_MAX_BOARDS) || (sis3320p[id] == NULL)) 
    {
      logMsg ("sis3320Run: ERROR : ADC id %d not initialized \n",
	      id, 0, 0, 0, 0, 0);
      return;
    }

/* *********************************** */
  sis3320Reset (id);             /* Reset */
  /* Program for multi Event, External start/Stop, 200 MHz */

	  printf("Arg for AcqConfig = 0x%x\n",SIS3320_ACQ_ENABLE_LEMO_START_STOP |
			    SIS3320_ACQ_DISABLE_INTERNAL_TRIGGER |
			    SIS3320_ACQ_DISABLE_AUTOSTART |
			    SIS3320_ACQ_ENABLE_MULTIEVENT |
		 SIS3320_ACQ_SET_CLOCK_TO_200MHZ);

	  sis3320AcqConfig (id, 
			    SIS3320_ACQ_ENABLE_LEMO_START_STOP |
			    SIS3320_ACQ_DISABLE_INTERNAL_TRIGGER |
			    SIS3320_ACQ_DISABLE_AUTOSTART |
			    SIS3320_ACQ_ENABLE_MULTIEVENT |
			    SIS3320_ACQ_SET_CLOCK_TO_200MHZ);   

  /* Program for No wrap mode, Accumulator shifted down */
  printf ("eventconfig\n");
  printf("setup :  event_sample_length = 0x%x   page_size_wrap = 0x%x\n",
	 event_sample_length,page_size_wrap);

  printf("Arg for Event Config = 0x%x \n",
		      SIS3320_EVENT_CONF_ENABLE_SAMPLE_LENGTH_STOP |
		      page_size_wrap |
                      SIS3320_EVENT_CONF_ENABLE_USER_IN_ACCUM_GATE | 
		      SIS3320_EVENT_CONF_ENABLE_ACCUMULATOR |
                	 0);


    sis3320EventConfig (id, 
		      SIS3320_EVENT_CONF_ENABLE_SAMPLE_LENGTH_STOP |
		      page_size_wrap |
 		      SIS3320_EVENT_CONF_ENABLE_WRAP_PAGE_MODE |
                      SIS3320_EVENT_CONF_ENABLE_USER_IN_ACCUM_GATE | 
		      SIS3320_EVENT_CONF_ENABLE_ACCUMULATOR |
		      0);

  /* Program 2 event maximum */
  sis3320p[id]->maxNofEvReg = 2;

  /* Program Start Delay, Stop Delay (example: 110 clocks ->550nsec)  */
  sis3320StopDelay (id, 0);
  sis3320StopDelay (id, 0);
+
  /* max. Sample Length*/
  data = (event_sample_length & 0xfffffffc) - 4;

  sis3320p[id]->sampLength = data;

  printf("event sample length %d %d 0x%x\n",data,event_sample_length,data);  

  /* sample start Address*/
  data = event_sample_start_addr;
  sis3320p[id]->sampStart = data;
  
  /* ADC Input Test Mode*/
  for (grp = 0; grp < 4; ++grp)
    {
      data = ADC_InputTestMode_array[grp];
      sis3320p[id]->adcG[grp].adcInputMode = data;
      printf ("Group %d adc input mode 0x%x\n", grp, sis3320p[id]->adcG[grp].adcInputMode);
    }
  
  /* Set ADC full gain */
  data =    0 ;
  sis3320p[id]->adcGainReg = data;
  
  sis3320p[id]->armSampLogic = 1;
  while (1)
    {
      printf ("Looping...\n");
      
#ifdef TESTADC
      /* ADC Input Test Mode*/
      for (grp = 0; grp < 4; ++grp)
	{
	  data = ADC_InputTestMode_array[grp] + 
	    (gl_loop_counter & 0xfff);
	  sis3320p[id]->adcG[grp].adcInputMode = data;
/* 	  printf ("Group %d adc input mode 0x%x\n", grp, sis3320p[id]->adcG[grp].adcInputMode); */
	}
#endif

      sis3320p[id]->armSampLogic = 1;

      /* wait for stop and disarm */
      poll_counter=0;
      while ((sis3320p[id]->acqCsr & SIS3320_ADC_SAMPLING_ARMED) == 
	     SIS3320_ADC_SAMPLING_ARMED)
	{
	  poll_counter++;
	  if (poll_counter >= 500000) 
	    {
/* 	      printf ("waiting for LEMO STOP_IN pulse \n"); */
	      poll_counter=0;
	    }
	}
      
      sis3320Display (id);
      sis3320GrpDisplay (id, 1);

      /* ADC1*/
      /* read event addresses*/
      if (sis3320p[id]->adcG[0].eventConfig & SIS3320_EVENT_CONF_ENABLE_ACCUMULATOR)
	{
	  /* Accumulator mode enabled, can only read one event */
	  nof_events = (sis3320p[id]->actEvtCounter > 0);
	  evdircopy[0] = sis3320p[id]->adcG[0].nextSampAdr[0];
          printf("EVDIRCOPY[0] HERE   0x%x \n",evdircopy[0]);
	}
      else
	{
	  /* Accumulator mode disabled, read them all */
	  nof_events = sis3320p[id]->actEvtCounter;
	  for (ievt = 0; ievt < nof_events; ++ievt)
	    evdircopy[ievt] = sis3320p[id]->adcG[0].eventDir[0][ievt] & 0x1ffffff;
	}

      /* Process these events */
      for (ievt = 0; ievt < nof_events; ++ievt)
	{
	  this_event_length = evdircopy[ievt] & (event_sample_length-1);
          printf("this_event_length HERE %d %x %x %d %x \n",ievt,evdircopy[ievt],evdircopy[ievt],this_event_length,event_sample_length-1);
          printf("SECOND OPINION %d \n",sis3320GetBufLength(id));

	  event_sample_start_addr = evdircopy[ievt] - this_event_length;

	  i_adc = whichadc;

          printf("Read_ADC_Chan inputs %d  0x%x 0x%x \n",i_adc, 
		 event_sample_start_addr, this_event_length);

      
	  return_code = 
	    sis3320_Read_ADC_Channel (id,
				      i_adc, 
				      event_sample_start_addr, 
				      this_event_length,
				      raw_adc_data,
				      SIS3320_MAX_NUMBER_LWORDS);
	  if (return_code != 0) 
	    {
	      printf ("sis3320_Read_ADC_Channel:  return_code = 0x%08x at addr = 0x%08x\n",return_code,addr);
	      return;
	    }
	  
	  request_nof_words = this_event_length / 2;
	  if (request_nof_words > SIS3320_MAX_NUMBER_LWORDS)
	    request_nof_words = SIS3320_MAX_NUMBER_LWORDS;

/* 	  wrap_sample_offset =  */
/* 	    ((evdircopy[i]  ) & SIS3320_ADC_SAMPLE_OFFSET_MASK);  /\* Sample offset*\/ */
/* /\* 	  wrap_read_addr_offset =  (evdircopy[i] & SIS3320_ADC_ADDR_OFFSET_MASK) / 2; *\/ */
	  wrap_read_addr_offset = 0; /* Like this? */
	  wrap_read_addr_mask   =  request_nof_words - 1 ;
	  
/* 	  if (wrap_sample_offset == 3)  */
/* 	    { */
/* 	      for (i=0; i < request_nof_words; i++)  */
/* 		{ */
/* 		  j = (wrap_read_addr_offset + i - 1) & wrap_read_addr_mask; */
/* 		  repacked_adc_data[i] =  */
/* 		    ((raw_adc_data[j]) >> 16) & SIS3320_ADC_LODATA_MASK; */
/* 		  j = (wrap_read_addr_offset + i) & wrap_read_addr_mask; */
/* 		  repacked_adc_data[i] +=    */
/* 		    ((raw_adc_data[j]) << 16) & SIS3320_ADC_HIDATA_MASK; */
/* 		} */
/* 	    } */
	  
/* 	  else if (wrap_sample_offset == 0)  */
/* 	    { */
	  printf ("Repacking 0x%x words\n", request_nof_words);
	  for (i=0; i < request_nof_words; i++)
	    {
	      j = (wrap_read_addr_offset + i) & wrap_read_addr_mask;
	      repacked_adc_data[i] = 
		(raw_adc_data[j]) & SIS3320_ADC_BOTHDATA_MASK;
	    }
	  
/* 	    } */
	  
/* 	  else if (wrap_sample_offset == 1)  */
/* 	    { */
/* 	      for (i = 0; i < request_nof_words; i++)  */
/* 		{ */
/* 		  j = (wrap_read_addr_offset + i) & wrap_read_addr_mask; */
/* 		  repacked_adc_data[i] =    */
/* 		    ((raw_adc_data[j]) >> 16) & SIS3320_ADC_LODATA_MASK; */
/* 		  j = (wrap_read_addr_offset + i + 1) & wrap_read_addr_mask; */
/* 		  repacked_adc_data[i] += */
/* 		    ((raw_adc_data[j]) << 16) & SIS3320_ADC_HIDATA_MASK; */
/* 		} */
/* 	    } */
	  
/* 	  else if (wrap_sample_offset == 2)  */
/* 	    { */
/* 	      for (i = 0; i < request_nof_words; i++)  */
/* 		{ */
/* 		  j = (wrap_read_addr_offset + i + 1) & wrap_read_addr_mask; */
/* 		  repacked_adc_data[i] =  */
/* 		    (raw_adc_data[j]) & SIS3320_ADC_BOTHDATA_MASK; */
/* 		} */
/* 	    } */
	  
	  printf ("Event %d, %d samples  Request #words %d\n", ievt, this_event_length,request_nof_words);
	  
	  for (i = 0; i < request_nof_words-1; i += 2 )
	    {
	      if (i < 16 || i > request_nof_words-8)
		printf ("i = 0x%08x      0x%04x    0x%04x    0x%04x    0x%04x \n", 
			2*i, 
			repacked_adc_data[i] & SIS3320_ADC_LOWORD_MASK, 
			(repacked_adc_data[i] >> 16) & SIS3320_ADC_LOWORD_MASK,
			repacked_adc_data[i+1] & SIS3320_ADC_LOWORD_MASK, 
			(repacked_adc_data[i+1] >> 16) & SIS3320_ADC_LOWORD_MASK);
	    }
	  if (request_nof_words % 1)
	    {
	      i = request_nof_words - 1;
	      printf ("i = 0x%08x      0x%04x    0x%04x \n", 
		      2*i, 
		      repacked_adc_data[i] & SIS3320_ADC_LOWORD_MASK, 
		      (repacked_adc_data[i] >> 16) & SIS3320_ADC_LOWORD_MASK);
	    }

	  if (sis3320p[id]->adcG[0].eventConfig & 
	      SIS3320_EVENT_CONF_ENABLE_ACCUMULATOR)
	    printf ("ievt %d   Accumulator sum = 0x%x\n", ievt, evdircopy[ievt]);
	  
          printf("\nAccumulators 1 - 6 \n");
          for (iacc = 0; iacc < 6; iacc++) {
            numa = sis3320GetNumAcc(id, (i_adc >> 1), i_adc%2, iacc);
            acc1 = sis3320GetAccum(id, (i_adc >> 1), i_adc%2, iacc, 0);
            acc2 = sis3320GetAccum(id, (i_adc >> 1), i_adc%2, iacc, 1);
               printf("Num:  %d    Hi: 0x%x = %d      Lo = 0x%x = %d \n",numa,acc1,acc1,acc2,acc2);
	  }

	  printf ("Software sum =    0x%x\n", 
	    sis3320_Sum_ADC_Channel (id,
				     i_adc, 
				     event_sample_start_addr, 
				     this_event_length,
				     SIS3320_EVENT_CONF_SHIFT_DOWN_ACCUM));
	}
      gl_loop_counter++;
      printf (" \ngl_loop_counter = 0x%08x   \n\n", 
	      gl_loop_counter);

      nprint = request_nof_words/8;
      if (nprint > MAX_PRINT) nprint = MAX_PRINT;

      for (j = 0; j < nprint; j++) {
        i = 8*j;
	printf("%d %d  data= 0x%5x  0x%5x  0x%5x  0x%5x  0x%5x  0x%5x  0x%5x  0x%5x \n",
		  j,i,sis3320p[id]->adcData[i_adc*ADC_2MB + i],
		  sis3320p[id]->adcData[i_adc*ADC_2MB + i+1],
		  sis3320p[id]->adcData[i_adc*ADC_2MB + i+2],
		  sis3320p[id]->adcData[i_adc*ADC_2MB + i+3],
		  sis3320p[id]->adcData[i_adc*ADC_2MB + i+4],
		  sis3320p[id]->adcData[i_adc*ADC_2MB + i+5],
		  sis3320p[id]->adcData[i_adc*ADC_2MB + i+6],
		  sis3320p[id]->adcData[i_adc*ADC_2MB + i+7]);
      }

#ifdef WHATEVER
      printf("\n\nAlternative printout \n");

      nprint = request_nof_words;
      if (nprint > MAX_PRINT) nprint = MAX_PRINT;
      i1 = i_adc;  i2 = i_adc+1;
      for (adc = i1; adc < i2; adc++) { 
        for (ichan = 0; ichan < nprint; ichan++) {
	   adcval1 = sis3320GetData(id, adc, ichan, 0);
           adcval2 = sis3320GetData(id, adc, ichan, 1);
           printf("ADC %d  Read %d   Data[hi] = (dec)%d = 0x%x  ||  Data[lo] = (dec)%d = 0x%x \n", adc+1, ichan+1, adcval1, adcval1, adcval2, adcval2);
           
        }
      }

#endif


#ifdef THING1      
      for (i = 0; i < 0x800000; i += 0x40000)
	{
	  printf ("0x%10x: 0x%5x 0x%5x 0x%5x 0x%5x 0x%5x 0x%5x 0x%5x 0x%5x\n",
		  i,
		  sis3320p[id]->adcData[i_adc*ADC_2MB + i],
		  sis3320p[id]->adcData[i_adc*ADC_2MB + i+1],
		  sis3320p[id]->adcData[i_adc*ADC_2MB + i+2],
		  sis3320p[id]->adcData[i_adc*ADC_2MB + i+3],
		  sis3320p[id]->adcData[i_adc*ADC_2MB + i+4],
		  sis3320p[id]->adcData[i_adc*ADC_2MB + i+5],
		  sis3320p[id]->adcData[i_adc*ADC_2MB + i+6],
		  sis3320p[id]->adcData[i_adc*ADC_2MB + i+7]);
	}

#endif
      
      /*       break; */
      /*       taskDelay (300); */ 
    }
  

  if (freeMem) 
    free (sis3320Data);

  printf ("sis3320Run: WARN: Exiting...\n");

}


/* *********************************** */
int sis3320IsSampling(int id) {
  if ((id < 0) || (id >= SIS3320_MAX_BOARDS) || (sis3320p[id] == NULL)) 
    {
      printf ("sis3320Status: ERROR : ADC id %d not initialized \n", id);
      return 0;
    }
  return (sis3320p[id]->acqCsr)&0x20000;
}

/* *********************************** */
int sis3320IsRunning() {
  return s3320RunFlag;
}

/* *********************************** */
int sis3320SetRunFlag(int arg) {
  s3320RunFlag = arg;
  return 0;
}

/* *********************************** */
int sis3320Finished(id) {
  /* returns 0 --> finished,  non-zero --> not finished */

  int poll_counter, status;
  int ldebug = 0;
  status = 0;

  if ((id < 0) || (id >= SIS3320_MAX_BOARDS) || (sis3320p[id] == NULL)) 
    {
      printf ("sis3320Finished: ERROR : ADC id %d not initialized \n", id);
      return 0;
    }
  if (ldebug) printf ("Test of finished.  busy =  %d   acqCsr = 0x%x,
            armed = %d \n",
       sis3320Busy(id), sis3320p[id]->acqCsr, 
	      ((sis3320p[id]->acqCsr & SIS3320_ADC_SAMPLING_ARMED) == 0) ); 

  if (sis3320Busy(id)) return 0;

  return ( (sis3320p[id]->acqCsr & SIS3320_ADC_SAMPLING_ARMED) == 0); 
}


int sis3320Busy(id) {
  if ((id < 0) || (id >= SIS3320_MAX_BOARDS) || (sis3320p[id] == NULL)) 
    {
      printf ("sis3320Finished: ERROR : ADC id %d not initialized \n", id);
      return 0;
    }
  return (sis3320p[id]->acqCsr & SIS3320_ADC_SAMPLING_BUSY); 
}

/* *********************************** */
int sis3320GetBufLength(int id) {

  int event_sample_length, evdir, buf_len;

  /* assume accumulator mode is enabled ... */

  if ((id < 0) || (id >= SIS3320_MAX_BOARDS) || (sis3320p[id] == NULL)) 
    {
      printf ("sis3320GetBufLength: ERROR : ADC id %d not initialized \n", id);
      return 0;
    }


  event_sample_length = SIS3320_16M; /* Wrap half memory */
  evdir = sis3320p[id]->adcG[0].nextSampAdr[0];
  buf_len = evdir & (event_sample_length-1);

  return buf_len;


}


/* *********************************** */
long sis3320GetData(int id, int adc, int index, int lohi) {

  int jj, tmp, idx;
  int grp, pagesize, stopIndex;
  int tdata;
  int e_offset=0;
  unsigned int pmask;

  if ((id < 0) || (id >= SIS3320_MAX_BOARDS) || (sis3320p[id] == NULL)) 
    {
      printf ("sis3320GetData: ERROR : ADC id %d not initialized \n", id);
      return 0;
    }

  if((adc<0)||(adc>7)) {
    logMsg("s3320GetData: ERROR: adc value %d out of range (0-7)\n",adc,0,0,0,0,0);
    return(-1);
  }

  sis3320p[id]->adcMemPageReg = 8;
  idx = adc*ADC_2MB + index;
  
  if (idx < 0 || idx > HARDMAX*ADC_2MB) {
       printf("Warning(3): Index violation\n");
       return 0;  /* memory limit on the cpu !! */
  }

  tdata = sis3320p[id]->adcData[idx];

    
  if (lohi == 1) {
      return (tdata & SIS3320_ADC_HIDATA_MASK) >> 16;
  } else {
      return (tdata & SIS3320_ADC_LODATA_MASK);
  }


}

/* *********************************** */
int sis3320DefaultSetup(int id) {

  /* Default setup for ADC, for the time being (April 27, 2006) */
 
  unsigned int data;
  unsigned int event_sample_length;
  
  if ((id < 0) || (id >= SIS3320_MAX_BOARDS) || (sis3320p[id] == NULL)) 
    {
      printf ("sis3320DefaultSetup: ERROR : ADC id %d not initialized \n", id);
      return 0;
    }

  printf("Arg for Acq Config = 0x%x \n", SIS3320_ACQ_ENABLE_LEMO_START_STOP |
			 SIS3320_ACQ_DISABLE_INTERNAL_TRIGGER |
		         SIS3320_ACQ_DISABLE_AUTOSTART |
		         SIS3320_ACQ_ENABLE_MULTIEVENT |
                 	 SIS3320_ACQ_SET_CLOCK_TO_250MHZ);   

     sis3320AcqConfig (id,  SIS3320_ACQ_ENABLE_LEMO_START_STOP |
			 SIS3320_ACQ_DISABLE_INTERNAL_TRIGGER |
		         SIS3320_ACQ_DISABLE_AUTOSTART |
		         SIS3320_ACQ_ENABLE_MULTIEVENT |
		         SIS3320_ACQ_SET_CLOCK_TO_250MHZ);   

     printf("page size = 0x%x\n",SIS3320_EVENT_CONF_ENABLE_WRAP_PAGE_MODE);

      printf("Arg for Event Config = 0x%x\n",
   		      SIS3320_EVENT_CONF_ENABLE_WRAP_PAGE_MODE |
    	              SIS3320_EVENT_CONF_ENABLE_ACCUMULATOR );
                                    

    sis3320EventConfig (id, 
		      SIS3320_EVENT_CONF_ENABLE_WRAP_PAGE_MODE |
		      SIS3320_EVENT_CONF_ENABLE_ACCUMULATOR |
		      SIS3320_EVENT_CONF_PAGE_SIZE_16M_WRAP |
		      0);

    /*  What it was ............
    sis3320EventConfig (id, 
		      SIS3320_EVENT_CONF_ENABLE_SAMPLE_LENGTH_STOP |
   		      SIS3320_EVENT_CONF_ENABLE_WRAP_PAGE_MODE |
                      SIS3320_EVENT_CONF_ENABLE_USER_IN_ACCUM_GATE |
		      SIS3320_EVENT_CONF_SHIFT_DOWN_ACCUM |
		      SIS3320_EVENT_CONF_ENABLE_ACCUMULATOR ); 
    */


    /*    sis3320StartDelay(id, 0);
	  sis3320StopDelay(id, 0); */


  /* Program 1 event maximum */
    sis3320p[id]->maxNofEvReg = 1;


  /* max. Sample Length*/  
    event_sample_length = SIS3320_16M; /* Wrap half memory */
    data = (event_sample_length & 0xfffffffc) - 4;
    
/* try this */
/*    data = 0xfffffc; */

    printf("Sample length = %d = 0x%x \n",data,data);
    sis3320p[id]->sampLength = data;


 /* sample start Address*/
    sis3320p[id]->sampStart = 0;

  /* 0 for full gain, 1 for half */
    data = 0;
    sis3320p[id]->adcGainReg = data;

    /* clear thresholds */
   sis3320SetThresh(0,0,-1,0);


  /* Give an offset to the signals */
    data =50000;
    sis3320SetDac(0, data);   /* Note, 50000 gives pedestal ~3600 */
                              /* 40600 is approximately the midpoint*/


}


/* *********************************** */
int sis3320SetupMode(int id,int Mode) {
  /* gbf feb 29, 2008 */
  /* based on DefaultSetup, but allows multiple predefined modes of running*/
  /* Mode 1 for long buffers, lemo start and stop */
  /* Mode 2 for short wrapped buffers, autostart, lemo stop */
 
  unsigned int registerbits;
  unsigned int data;
  unsigned int event_sample_length;
  printf("Setting FADC DAQ MODE %d \n",Mode);
  if ((id < 0) || (id >= SIS3320_MAX_BOARDS) || (sis3320p[id] == NULL)) 
    {
      printf ("sis3320SetupMode: ERROR : ADC id %d not initialized \n", id);
      return 0;
    }
  if(Mode==1) {
    registerbits= SIS3320_ACQ_ENABLE_LEMO_START_STOP |
			 SIS3320_ACQ_DISABLE_INTERNAL_TRIGGER |
		         SIS3320_ACQ_DISABLE_AUTOSTART |
		         SIS3320_ACQ_ENABLE_MULTIEVENT |
		         SIS3320_ACQ_SET_CLOCK_TO_250MHZ;   
    printf("Arg for Acq Config = 0x%x \n", registerbits);
    sis3320AcqConfig (id, registerbits);

    registerbits=SIS3320_EVENT_CONF_ENABLE_WRAP_PAGE_MODE |
      SIS3320_EVENT_CONF_ENABLE_ACCUMULATOR |
      SIS3320_EVENT_CONF_PAGE_SIZE_16M_WRAP;
    printf("page size = 0x%x\n",SIS3320_EVENT_CONF_ENABLE_WRAP_PAGE_MODE);
    printf("Arg for Event Config = 0x%x\n",registerbits);
    sis3320EventConfig (id,  registerbits) ;

    sis3320p[id]->maxNofEvReg = 1; /* Program 1 event maximum */
                                   /* max. Sample Length*/  
    event_sample_length = SIS3320_16M; /* Wrap half memory */
    data = (event_sample_length & 0xfffffffc) - 4;
    printf("Sample length = %d = 0x%x \n",data,data);
    sis3320p[id]->sampLength = data;
    sis3320p[id]->sampStart = 0; /* sample start Address  */
  }else if (Mode==2){
    /* small memory wrap mode with autostart */
    /* Data type 2 */
    registerbits= SIS3320_ACQ_ENABLE_LEMO_START_STOP |
			 SIS3320_ACQ_DISABLE_INTERNAL_TRIGGER |
		         SIS3320_ACQ_DISABLE_AUTOSTART |
		         SIS3320_ACQ_ENABLE_MULTIEVENT |
		         SIS3320_ACQ_SET_CLOCK_TO_250MHZ;   
    printf("Arg for Acq Config = 0x%x \n", registerbits);
    sis3320AcqConfig (id, registerbits);

    registerbits=SIS3320_EVENT_CONF_ENABLE_WRAP_PAGE_MODE |
      SIS3320_EVENT_CONF_ENABLE_ACCUMULATOR |
      SIS3320_EVENT_CONF_PAGE_SIZE_512_WRAP;
    printf("page size = 0x%x\n",SIS3320_EVENT_CONF_ENABLE_WRAP_PAGE_MODE);
    printf("Arg for Event Config = 0x%x\n",registerbits);
    sis3320EventConfig (id,  registerbits) ;

    sis3320p[id]->maxNofEvReg = 1; /* Program 1 event maximum */
                                   /* max. Sample Length*/  
    event_sample_length = SIS3320_512; /* Wrap size */
    data = (event_sample_length & 0xfffffffc) - 4;
    printf("Sample length = %d = 0x%x \n",data,data);
    sis3320p[id]->sampLength = data;
    sis3320p[id]->sampStart = 0; /* sample start Address*/
  } else if(Mode==3) {
    /* Multievent mode */
    registerbits= SIS3320_ACQ_ENABLE_LEMO_START_STOP |
      SIS3320_ACQ_DISABLE_INTERNAL_TRIGGER |
		         SIS3320_ACQ_DISABLE_AUTOSTART |
		         SIS3320_ACQ_ENABLE_MULTIEVENT |
		         SIS3320_ACQ_SET_CLOCK_TO_250MHZ;   
    printf("Arg for Acq Config = 0x%x \n", registerbits);
    sis3320AcqConfig (id, registerbits);

 /*    registerbits=SIS3320_EVENT_CONF_ENABLE_WRAP_PAGE_MODE | */
/*       SIS3320_EVENT_CONF_ENABLE_ACCUMULATOR | */
/*       SIS3320_EVENT_CONF_PAGE_SIZE_16M_WRAP; */
    registerbits=
      SIS3320_EVENT_CONF_ENABLE_ACCUMULATOR |
      SIS3320_EVENT_CONF_PAGE_SIZE_16M_WRAP;
    printf("page size = 0x%x\n",SIS3320_EVENT_CONF_ENABLE_WRAP_PAGE_MODE);
    printf("Arg for Event Config = 0x%x\n",registerbits);
    sis3320EventConfig (id,  registerbits) ;

    sis3320p[id]->maxNofEvReg = 1; /* Default event maximum */
    //   event_sample_length = SIS3320_16M;      /*default max sample length*/
    event_sample_length = 800;
    data = (event_sample_length & 0xfffffffc) - 4;
    printf("Sample length = %d = 0x%x \n",data,data);
    sis3320p[id]->sampLength = data;
    sis3320p[id]->sampStart = 0; /* sample start Address  */
  }
  return 0;
}

/* *********************************** */
int sis3320SetEventMode(int id, int Flag) {
  /* Flag=0 for single event mode
     =1 for multi event mode */
  if(Flag==0){
    sis3320AcqConfig(id, SIS3320_DISABLE_MULTIEVENT);
  } else if (Flag==1) {
    sis3320AcqConfig(id, SIS3320_ENABLE_MULTIEVENT);
  }
  return 0;
}
int sis3320StartMode(int id, int Flag){
  /* Flag=0 for disable autostart 
     = 1 for enable autostart */
  if(Flag==0){
    sis3320AcqConfig(id, SIS3320_DISABLE_AUTOSTART);
  } else if (Flag==1) {
    sis3320AcqConfig(id, SIS3320_ENABLE_AUTOSTART);
  }
  return 0;
}
int sis3320SetSampleStartAddress(int id, int StartAddress){
  if ((id < 0) || (id >= SIS3320_MAX_BOARDS) || (sis3320p[id] == NULL)) 
    {
      printf ("sis3320GetAccum_V1: ERROR : ADC id %d not initialized \n", id);
      return 0;
    }
  sis3320p[id]->sampStart=StartAddress;
}
int sis3320SetStopMode(int id,int Flag){
  /* Flag=0 for disable Sample Length stop*/
  /* Flag=1 to enable Sample Length stop*/
  int tmp;
  volatile struct adcGroup_struct* adcg;
  if ((id < 0) || (id >= SIS3320_MAX_BOARDS) || (sis3320p[id] == NULL)) 
    {
      printf ("sis3320GetAccum_V1: ERROR : ADC id %d not initialized \n", id);
      return 0;
    }
  adcg = &(sis3320p[id]->adcG[0]);
  tmp=adcg->eventConfig;
  if(Flag==0){
    sis3320p[id]->eventConfig = (tmp & 0xffdf);     /* clear stop bit*/
  } else {
    sis3320p[id]->eventConfig = (tmp | 0x20);     /* set stop bit*/
  }
  return 0;
}
int sis3320EnableUserBit(int id,int Flag){
  /* Flag=0 for disable user input bit*/
  /* Flag=1 to enable input user bit*/
  int tmp;
  volatile struct adcGroup_struct* adcg;
  if ((id < 0) || (id >= SIS3320_MAX_BOARDS) || (sis3320p[id] == NULL)) 
    {
      printf ("sis3320GetAccum_V1: ERROR : ADC id %d not initialized \n", id);
      return 0;
    }
  adcg = &(sis3320p[id]->adcG[0]);
  tmp=adcg->eventConfig;
  if(Flag==0){
    sis3320p[id]->eventConfig = (tmp & 0xefff);     /* clear bit 12*/
  } else {
    sis3320p[id]->eventConfig = (tmp | 0x1000);     /* set bit 12*/
  }
  return 0;
}

int sis3320SetWrapPageSize(int id, int WrapSizeCode){
  /* WrapSizeCode=0  to 11 */
  /* 0  16M Samples
   * 1   4M
   * 2   1M
   * 3 256k
   * 4  64k
   * 5  16k
   * 6  4k
   * 7  1k
   * 8  512
   * 9  256
   *10  128
   *11  64
   */
  int tmp;
   volatile struct adcGroup_struct* adcg;
   if ((id < 0) || (id >= SIS3320_MAX_BOARDS) || (sis3320p[id] == NULL)) 
    {
      printf ("sis3320GetAccum_V1: ERROR : ADC id %d not initialized \n", id);
      return 0;
    }
  /* you can write to all 4 groups at once, but only read from 1 */
  adcg = &(sis3320p[id]->adcG[0]);  /*get current settings for group 0*/
  tmp=adcg->eventConfig;   /* get current settings*/
  tmp= tmp&0xfffffff0;
  tmp= tmp |( WrapSizeCode &0xf);   /* new lowest 4 bits are wrap size code*/
  sis3320p[id]->eventConfig = tmp;
  return 0;
}

int sis3320SetMaxSampleLength(int id, int SampleLength){
  /* sets sample length. 
     used if "enable Max Sample Count stop" is enabled */
  if ((id < 0) || (id >= SIS3320_MAX_BOARDS) || (sis3320p[id] == NULL)) 
    {
      printf ("sis3320GetAccum_V1: ERROR : ADC id %d not initialized \n", id);
      return 0;
    }
  sis3320p[id]->sampLength=((SampleLength-4)&0xfffffc);
  return 0;
}
int sis3320GetMaxSampleLength(int id){
  /* gets sample length. 
     used if "enable Max Sample Count stop" is enabled */
  if ((id < 0) || (id >= SIS3320_MAX_BOARDS) || (sis3320p[id] == NULL)) 
    {
      printf ("sis3320GetAccum_V1: ERROR : ADC id %d not initialized \n", id);
      return 0;
    }
  return (sis3320p[id]->sampLength)+4;
}
int sis3320SetMaxNumEvents(int id,int MaxNumberOfEvents){
  /* set the maximum number of events for multiple event mode */
  if ((id < 0) || (id >= SIS3320_MAX_BOARDS) || (sis3320p[id] == NULL)) 
    {
      printf ("sis3320GetAccum_V1: ERROR : ADC id %d not initialized \n", id);
      return 0;
    }
  sis3320p[id]->maxNofEvReg=MaxNumberOfEvents;
  return 0;
}


/* *********************************** */
long sis3320GetAccum_V1(int id, int adcgr, int adc, int which) {
  /* version 1 (V1) for the 2006 version of FADC */
  int i,j;
  volatile struct adcGroup_struct* adcg;

  if ((id < 0) || (id >= SIS3320_MAX_BOARDS) || (sis3320p[id] == NULL)) 
    {
      printf ("sis3320GetAccum_V1: ERROR : ADC id %d not initialized \n", id);
      return 0;
    }
    if((adcgr<0)||(adcgr>3)) {
      logMsg("s3320GetAccum_V1: ERROR: adc group %d out of range (0-3)\n",adcgr,0,0,0,0,0);
      return(-1);
    }
    if (adc < 0 || adc > 1) {
      printf("ss3320GetAccum_V1: need to pick 0 or 1 for adc idx\n");
      return -1;
    }
    if (which < 0 || which > 1) {
      printf("ss3320GetAccum_V1: need to pick 0 or 1 for which accum\n");
      return -1;
    }

    adcg = &(sis3320p[id]->adcG[adcgr]);

    return adcg->eventDir[adc][which]; 

};


/* *********************************** */
unsigned long sis3320GetAccum(int id, int adcgr, int adc, int which, int hilo) {
  /*  New version, Dec 2007
      which = 0-5  (for 6 accumulators)
      hilo ==  0=bits 32-36,   1 = lower 32 bits */
  int i,j;
  unsigned long temp,value;
  volatile struct adcGroup_struct* adcg;

  if ((id < 0) || (id >= SIS3320_MAX_BOARDS) || (sis3320p[id] == NULL)) 
    {
      printf ("sis3320GetAccum: ERROR : ADC id %d not initialized \n", id);
      return 0;
    }
    if((adcgr<0)||(adcgr>3)) {
      logMsg("s3320GetAccum: ERROR: adc group %d out of range (0-3)\n",adcgr,0,0,0,0,0);
      return(-1);
    }
    if (adc < 0 || adc > 1) {
      printf("ss3320GetAccum: need to pick 0 or 1 for adc idx\n");
      return -1;
    }
    if (which < 0 || which > 5) {
      printf("ss3320GetAccum: need to pick 0 - 5 for which accum index\n");
      return -1;
    }
    if (hilo < 0 || hilo > 1) {
      printf("ss3320GetAccum: need to pick 0 - 1 for hilo \n");
      return -1;
    }

    adcg = &(sis3320p[id]->adcG[adcgr]);

    temp = adcg->eventDir[adc][1+(2*which)+hilo]; 

    if (hilo == 0) {
      value = ((temp & 0xf0000000) >> 28);
    } else {
      value = temp;
    }

   return value;

};

/* *********************************** */
long sis3320GetNumAcc(int id, int adcgr, int adc, int which) {
  /*  Get the number of values in accum # which
      which = 0-5  (for 6 accumulators)
      hilo ==  0=bits 32-36,   1 = lower 32 bits */
  int i,j;
  long temp,value;
  volatile struct adcGroup_struct* adcg;

  if ((id < 0) || (id >= SIS3320_MAX_BOARDS) || (sis3320p[id] == NULL)) 
    {
      printf ("sis3320GetAccum: ERROR : ADC id %d not initialized \n", id);
      return 0;
    }
    if((adcgr<0)||(adcgr>3)) {
      logMsg("s3320GetAccum: ERROR: adc group %d out of range (0-3)\n",adcgr,0,0,0,0,0);
      return(-1);
    }
    if (adc < 0 || adc > 1) {
      printf("ss3320GetAccum: need to pick 0 or 1 for adc idx\n");
      return -1;
    }
    if (which < 0 || which > 5) {
      printf("ss3320GetAccum: need to pick 0 - 5 for which accum index\n");
      return -1;
    }

    adcg = &(sis3320p[id]->adcG[adcgr]);

    temp = adcg->eventDir[adc][1+(2*which)]; 

    value = temp & 0xffffff;

    return value;

};


/* *********************************** */
int sis3320SetDac(int id, int data) {
  /* this version sets all 8 DACs to the same value */
  int i,j;
  unsigned int ldata = data;

  if ((id < 0) || (id >= SIS3320_MAX_BOARDS) || (sis3320p[id] == NULL)) 
    {
      printf("sis3320SetDac: ERROR : ADC id %d not initialized \n", id);
      return 0;
    }
  if (data < -999) {
     printf("sis3320: Reset DAC \n");
     sis3320p[id]->dacCsr = 3;  /* reset */
     return 0;
  }
  for( i=0; i<8; i++){
    sis3320p[id]->dacDataReg = ldata;
    sis3320p[id]->dacCsr = 1+(i<<4);  /* write data to register*/
    while( sis3320DacCsrBusy(id) ){
    }
    /*taskDelay(1*60);*/
    sis3320p[id]->dacCsr = 2+(i<<4) ;  /* load data into DAC*/
    while( sis3320DacCsrBusy(id) ){ /* wait for end of busy bit */
    }
  }

};
/* *********************************** */
int sis3320SetDacChan(int id, int chan, int data) {
  /* this version sets DAC for individual channels
  sets all channels to value "data" if chan=-1 */
  int i,j;
  unsigned int ldata = data;
  int lochan,hichan;

  if ((id < 0) || (id >= SIS3320_MAX_BOARDS) || (sis3320p[id] == NULL)) 
    {
      printf("sis3320SetDac: ERROR : ADC id %d not initialized \n", id);
      return 0;
    }
  if (data < -999) {
     printf("sis3320: Reset DAC \n");
     sis3320p[id]->dacCsr = 3;  /* reset */
     return 0;
  }
  if(chan==-1){
    lochan=0;
    hichan=7;
  }else if (chan>=0 && chan<8){
    lochan=chan;
    hichan=chan;
  } else {
    return 0;
  }
  for( i=lochan; i<=hichan; i++){
    printf("setting DAC chan %d to %d\n",i,ldata);
    sis3320p[id]->dacDataReg = ldata;
    sis3320p[id]->dacCsr = 1+(i<<4);  /* write data to register*/
    while( sis3320DacCsrBusy(id) ){
    }
    sis3320p[id]->dacCsr = 2+(i<<4) ;  /* load data into DAC*/
    while( sis3320DacCsrBusy(id)){ /* wait for end of busy bit */
    }
  }
  return 0;
};
/* ********************************** */
int sis3320DacCsrBusy(int id){
  return ((sis3320p[id]->dacCsr)&0x8000)==0x8000;
}


/* *********************************** */
int sis3320GetDac(int id, int chan) {
  /* returns Dac setting */
  int i,j;
  unsigned int ldata;
  int lobits, hibits;
  if ((id < 0) || (id >= SIS3320_MAX_BOARDS) || (sis3320p[id] == NULL)) 
    {
      printf("sis3320GetDac: ERROR : ADC id %d not initialized \n", id);
      return 0;
    }
  
  sis3320p[id]->dacCsr = 1 +(chan<<4);  /* Needed to set DAC channel*/
  while( sis3320DacCsrBusy(id)){
  }
  ldata=sis3320p[id]->dacDataReg;
  while( sis3320DacCsrBusy(id)){
  }
  lobits= ldata&0xffff;
  hibits= (ldata>>16)&0xffff;
  
  return hibits;
};


/* *********************************** */
int sis3320SetThresh_V1(int id, int adc, int lohi, int data) {

  /* version 1 (V1) for 2006 version of FADC board */

  int grp, thresh1, thresh2;

  if ((id < 0) || (id >= SIS3320_MAX_BOARDS) || (sis3320p[id] == NULL)) 
    {
      printf("sis3320SetThresh_V1: ERROR : ADC id %d not initialized \n", id);
      return 0;
    }
 
  
  grp = adc >> 1;
   
  thresh1 = sis3320p[id]->adcG[grp].accumThresh1;
  thresh2 = sis3320p[id]->adcG[grp].accumThresh2;

  if (lohi == -1 ) { 

    printf("Clearing thesholds (V1) \n");
    sis3320p[id]->adcG[grp].accumThresh1 = 0;
    sis3320p[id]->adcG[grp].accumThresh2 = 0;

    return 0;

  }

  if (lohi == 0 ) {

    printf("Setting low thresholds to %d \n",data);
 
    thresh1 |= 0x1000;
    thresh1 |= data;
    sis3320p[id]->adcG[grp].accumThresh1 = thresh1;
    thresh2 |= 0x1000;
    thresh2 |= data;
    sis3320p[id]->adcG[grp].accumThresh2 = thresh2;

  }

  if (lohi == 1 ) {

    printf("(V1) Setting hi thresholds to %d \n",data);
 
    thresh1 |= 0x10000000;
    thresh1 |= data<<16;
    sis3320p[id]->adcG[grp].accumThresh1 = thresh1;
    thresh2 |= 0x10000000;
    thresh2 |= data<<16;
    sis3320p[id]->adcG[grp].accumThresh2 = thresh2;

  }

  printf("SetThresh_V1:  Thresholds settings = 0x%x  and 0x%x \n",
	 sis3320p[id]->adcG[grp].accumThresh1,
	 sis3320p[id]->adcG[grp].accumThresh2);

  return 1;

}

/* *********************************** */
int sis3320SetN5N6(int id, int chan, int n5_before, int n5_after, int n6_before, int n6_after) {
  /* Set the N5_before, N5_after, N6_before, N6_after variables for
     accumulators 5 and 6.
     Does this for all channels if chan=-1 */

  int grp;
  int lochan, hichan;
  int i;
  unsigned int dataword;
  if ((id < 0) || (id >= SIS3320_MAX_BOARDS) || (sis3320p[id] == NULL)) 
    { 
      printf("sis3320SetN5N6: ERROR : ADC id %d not initialized \n", id);
      return 0;
    }
  if(chan==-1){
    lochan=0;
    hichan=7;
  }else if (chan>=0 && chan<8){
    lochan=chan;
    hichan=chan;
  } else {
    printf("illegal channel in call to sis3320SetN5N6\n");
    return -1;
  }
  if (n5_before == -1 ) { 
    dataword=0;
    if(chan=-1) {
      printf("Clearing N5, N6 variables for all channels\n");
    }else {
      printf("Clearing N5, N6 variables for channel chan \n");
    }
  }else {
    dataword= ((n5_before & 0xff)<<24) + ((n5_after & 0xff)<<16) + 
      ((n6_before & 0xff) << 8) + (n6_after & 0xff);
  } 
  for(i=lochan; i<=hichan; i++){
    printf("Set N5N6 for channel %d to %x\n",i,dataword);
    grp = i>> 1;
    if( (i & 0x1) ==0){
      sis3320p[id]->adcG[grp].n5n6befaft1 = dataword;  /*even numbered channels*/
    }else{
      sis3320p[id]->adcG[grp].n5n6befaft2 = dataword;  /*odd numbered channels*/
    }
  }
  return 1;
}
/* *********************************** */
unsigned int sis3320GetN5N6(int id, int chan){
  int grp;
  unsigned int dataword;
  if(chan>=0 & chan<8){
    grp=chan>>1;
    if( (chan&0x1)==0){
      dataword=sis3320p[id]->adcG[grp].n5n6befaft1;
    }else{
      dataword=sis3320p[id]->adcG[grp].n5n6befaft2;
    }
    return dataword;
  }else {
    return 0;
  }
}


/* *********************************** */
int sis3320SetThresh(int id, int adc, int thresh1, int thresh2) {
  /* Version for Dec 2007 version of the FADC board */

  int grp,oddeven;
  unsigned int ldata;
  
  if ((id < 0) || (id >= SIS3320_MAX_BOARDS) || (sis3320p[id] == NULL)) 
    { 
      printf("sis3320SetThresh: ERROR : ADC id %d not initialized \n", id);
      return 0;
    }
 
  grp = adc >> 1;
  oddeven=adc & 0x1;

  if ( (thresh1 > 4095) || (thresh2 > 4095) )  {
    printf("sis3320SetThresh:: warning:  thresholds cannot be > 4095 \n");
  }
   
  if (thresh1 == -1 ) { 

    printf("Clearing thesholds \n");
  if( oddeven==0){
    sis3320p[id]->adcG[grp].accThresh1=0;
  }else {
    sis3320p[id]->adcG[grp].accThresh2=0;
  }
    return 0;

  }

  ldata = ((thresh1 & 0xfff) << 16) + (thresh2 & 0xfff) ;
  if(oddeven==0){
    sis3320p[id]->adcG[grp].accThresh1=ldata;
  }else {
    sis3320p[id]->adcG[grp].accThresh2=ldata;
  }
  return 1;

}
/* *********************************** */
int sis3320SetGain(int id, int adc, int data) {
  //sets "CML" bit
  //replaces old sis3320 instruction with 250MHz model
  //sis3320p[id]->adcGainReg = data; /* 0 for full gain, 1 for half */
  // adc= channel number (0 to 7) or =-1 to set all 8 channels
  int adclo;
  int adchi;
  int adc_channel;
  if(adc==-1){
    adclo=0;
    adchi=7;
  }else {
    adclo=adc;
    adchi=adc;
  }
  for(adc_channel=adclo; adc_channel<=adchi; adc_channel++){
    if(data==1){
      sis3320WriteAdcSPI(id,adc_channel,0xf,0x2);  //set CML  bit in register
    } else {
      sis3320WriteAdcSPI(id,adc_channel,0xf,0x0);  //clear CML bit in register
    }
    sis3320WriteAdcSPI(id,adc_channel,0xff,0x1);  //issue update command
  }
  return 0;
}

   

/* *********************************** */
int sis3320GetThresh(int id, int chan, int which) {
  /* Obtain threshold (which = 1,2) */

  int grp,oddeven;
  int thresh;
  unsigned int ldata;
  if ((id < 0) || (id >= SIS3320_MAX_BOARDS) || (sis3320p[id] == NULL)) 
    { 
      printf("sis3320SetThresh: ERROR : ADC id %d not initialized \n", id);
      return 0;
    }
 
  grp = chan >> 1;
  oddeven=chan & 0x1;
  if(oddeven==0){
    ldata=sis3320p[id]->adcG[grp].accThresh1;
  }else{
    ldata=sis3320p[id]->adcG[grp].accThresh2;
  }
  if (which == 1) {
    thresh = (ldata>>16) & 0xfff;
  } else { 
    thresh = ldata & 0xfff;
  }
  return thresh;
}
/* *********************************** */
int sis3320WriteAdcSPI(int id, int adc, unsigned int spi_addr, unsigned int data){
  /* set ADC Serial Buss registers.  Needed for Version 2 gain control gbf*/
  int adc_channel;
  int which_one;
  int group;
  unsigned int vme_addr;
  unsigned int vme_data;
  unsigned int tmp;
  volatile struct adcGroup_struct* adcg;
  adc_channel= adc & 0x7;   //0 to 7 are valid
  group= adc_channel >>1;   // 0 and 1 are pair 0, 2 and 3 are pair 2, etc.
  which_one= adc_channel & 0x1;  //lowest bit is which of channel within pair
  adcg=&sis3320p[id]->adcG[group];
  vme_data= (data&0xff)+((spi_addr<<8)& 0x1fff00);
//bit22=1  ADC channel 2,4,6,8(adc=1,3,5,7)
  if(which_one==1) vme_data=vme_data + 0x400000;
  adcg->adcSPI=vme_data;
  while( adcg->adcSPI& 0x800000){
    //wait for busy bit to clear
    //    printf("adcSPI register busy \n");
  }

  return 0;
}
int sis3320GetActualNumEvents(int id){
  if ((id < 0) || (id >= SIS3320_MAX_BOARDS) || (sis3320p[id] == NULL)) 
    { 
      printf("sis3320SetThresh: ERROR : ADC id %d not initialized \n", id);
      return 0;
    }
  return sis3320p[id]->actEvtCounter;
}

