/******************************************************************************
*
*  caen767Lib.c  -  Driver library for readout of C.A.E.N. Model 767 TDC
*                   using a VxWorks 5.2 or later based Single Board computer. 
*
*  Author: David Abbott 
*          Jefferson Lab Data Acquisition Group
*          August 2002
*
*  Revision  1.0 - Initial Revision
*                    - Supports up to 20 CAEN Model 767s in a Crate
*                    - Programmed I/O read
*                    - Interrupts from a Single 767
*
*  Revision 1.1  - Added block reads and event flushing
*  Revision 1.1saw - Added V767Status2 call to return status 2 register
*/

#include "vxWorks.h"
#include "stdio.h"
#include "string.h"
#include "math.h"
#include "logLib.h"
#include "taskLib.h"
#include "intLib.h"
#include "iv.h"
#include "semLib.h"
#include "vxLib.h"

/* Include TDC definitions */
#include "v767Lib.h"

#define MAX_TDC_CHIPS 4

/* Include DMA Library definintions */
#include "universeDma.h"

/* Define external Functions */
IMPORT  STATUS sysBusToLocalAdrs(int, char *, char **);
IMPORT  STATUS intDisconnect(int);
IMPORT  STATUS sysIntEnable(int);
IMPORT  STATUS sysIntDisable(int);

/* Define Interrupts variables */
BOOL              v767IntRunning  = FALSE;                    /* running flag */
int               v767IntID       = -1;                       /* id number of TDC generating interrupts */
LOCAL VOIDFUNCPTR v767IntRoutine  = NULL;                     /* user interrupt service routine */
LOCAL int         v767IntArg      = 0;                        /* arg to user routine */
LOCAL int         v767IntEvCount  = 0;                        /* Number of Events to generate Interrupt */
LOCAL UINT32      v767IntLevel    = V767_VME_INT_LEVEL;       /* default VME interrupt level */
LOCAL UINT32      v767IntVec      = V767_INT_VEC;             /* default interrupt Vector */


/* Define global variables */
int Nv767 = 0;                                /* Number of TDCs in Crate */
volatile struct v767_struct *v767p[20];       /* pointers to TDC memory map */
int v767IntCount = 0;                         /* Count of interrupts from TDC */
int v767EventCount[20];                       /* Count of Events taken by TDC (Event Count Register value) */
int v767EvtReadCnt[20];                       /* Count of events read from specified TDC */

SEM_ID v767Sem;                               /* Semephore for Task syncronization */

/*******************************************************************************
*
* v767Init - Initialize v767 Library. 
*
*
* RETURNS: OK, or ERROR if the address is invalid or board is not present.
*/

STATUS 
v767Init (UINT32 addr, UINT32 addr_inc, int nadc, UINT16 crateID)
{
  int ii, res, rdata, errFlag = 0;
  int boardID = 0;
  unsigned long laddr;

  
  /* Check for valid address */
  if(addr==0) {
    printf("v767Init: ERROR: Must specify a Bus (VME-based A32/A24) address for TDC 0\n");
    return(ERROR);
  }else if(addr < 0x00ffffff) { /* A24 Addressing */
    if((addr_inc==0)||(nadc==0))
      nadc = 1; /* assume only one TDC to initialize */

    /* get the TDCs address */
    res = sysBusToLocalAdrs(0x39,(char *)addr,(char **)&laddr);
    if (res != 0) {
      printf("v767Init: ERROR in sysBusToLocalAdrs(0x39,0x%x,&laddr) \n",addr);
      return(ERROR);
    }
  }else{ /* A32 Addressing */
    if((addr_inc==0)||(nadc==0))
      nadc = 1; /* assume only one TDC to initialize */

    /* get the TDC address */
    res = sysBusToLocalAdrs(0x09,(char *)addr,(char **)&laddr);
    if (res != 0) {
      printf("v767Init: ERROR in sysBusToLocalAdrs(0x09,0x%x,&laddr) \n",addr);
      return(ERROR);
    }
  }

  Nv767 = 0;
  for (ii=0;ii<nadc;ii++) {
    v767p[ii] = (struct v767_struct *)(laddr + ii*addr_inc);
    /* Check if Board exists at that address */
    res = vxMemProbe((char *) v767p[ii],0,4,(char *)&rdata);
    if(res < 0) {
      printf("v767Init: ERROR: No addressable board at addr=0x%x\n",(UINT32) v767p[ii]);
      v767p[ii] = NULL;
      errFlag = 1;
      break;
    } else {
      /* Check if this is a Model V767 */
     /* boardID = v767p[ii]->firmwareRev; */
      boardID = V767_BOARD_ID;
      if((boardID != V767_BOARD_ID)) {
	printf(" ERROR: v767 Firmware revision does not match: %d \n",boardID);
	return(ERROR);
      }
    }
    Nv767++;
     printf("Initialized TDC ID %d at address 0x%08x \n",ii,(UINT32) v767p[ii]);
  }

  /* Initialize/Create Semephore */
  if(v767Sem != 0) {
    semFlush(v767Sem);
    semDelete(v767Sem);
  }
  v767Sem = semBCreate(SEM_Q_PRIORITY,SEM_EMPTY);
  if(v767Sem <= 0) {
    printf("v767Init: ERROR: Unable to create Binary Semephore\n");
    return(ERROR);
  }
  
  /* Set Geographic Address with ID , Reset all moduless */
  for(ii=0;ii<Nv767;ii++) {
    v767p[ii]->geoAddr = ii;
    V767_EXEC_MODULE_RESET(ii);
  }
  taskDelay(120);

  if(errFlag > 0) {
    printf("v767Init: ERROR: Unable to initialize all TDC Modules\n");
    if(Nv767 > 0)
      printf("v767Init: %d TDC(s) successfully initialized\n",Nv767);
    return(ERROR);
  } else {
    return(OK);
  }
}

/*******************************************************************************
*
*  v767GetEvcount(int id)
*
*/

int v767GetEvcount(int id) {

  return (v767p[id]->evCount)&V767_EVCOUNT_MASK;   

}


/*******************************************************************************
*
* v767Status - Gives Status info on specified TDC
*
*
* RETURNS: None
*/

void
v767Status( int id, int reg, int sflag)
{

  int lock;
  int drdy=0, empty=0, afull=0, bfull=0;
  int berr=0, blk=0, testmode=0, busy=0;
  int acqmode=0, drdymode=0;
  int tdcerror[4];
  UINT16 bitReg, stat1Reg, cntl1Reg, stat2Reg, cntl2Reg;
  UINT16 iLvl, iVec, evCount;

  if((id<0) || (v767p[id] == NULL)) {
    printf("v767Status: ERROR : TDC id %d not initialized \n",id);
    return;
  }

  /* read various registers */

  lock = intLock();
  bitReg = (v767p[id]->bitSet)&V767_BITSET_MASK;
  stat1Reg = (v767p[id]->status1)&V767_STATUS1_MASK;
  cntl1Reg = (v767p[id]->control1)&V767_CONTROL1_MASK;
  stat2Reg = (v767p[id]->status2)&V767_STATUS2_MASK;
  cntl2Reg = (v767p[id]->control2)&V767_CONTROL2_MASK;
  
  iLvl = (v767p[id]->intLevel)&V767_INTLEVEL_MASK;
  iVec = (v767p[id]->intVector)&V767_INTVECTOR_MASK;
  evCount  = (v767p[id]->evCount)&V767_EVCOUNT_MASK;
  intUnlock(lock);

  drdy  = stat1Reg&V767_STATUS1_DATA_READY;
  empty = stat2Reg&V767_STATUS2_EMPTY;
  afull = stat2Reg&V767_STATUS2_ALMOST_FULL;
  bfull = stat2Reg&V767_STATUS2_FULL;

  tdcerror[0] = (stat2Reg&V767_STATUS2_ERROR_0)>>12;
  tdcerror[1] = (stat2Reg&V767_STATUS2_ERROR_1)>>13;
  tdcerror[2] = (stat2Reg&V767_STATUS2_ERROR_2)>>14;
  tdcerror[3] = (stat2Reg&V767_STATUS2_ERROR_3)>>15;

  busy  = stat1Reg&V767_STATUS1_BUSY;

  berr = cntl1Reg&V767_CONTROL1_BERR_ENABLE;
  blk  = cntl1Reg&V767_CONTROL1_BLKEND;
  testmode = cntl2Reg&V767_TESTMODE;
  acqmode = cntl2Reg&V767_CONTROL2_ACQMODE_MASK;
  drdymode = (cntl2Reg&V767_CONTROL2_DREADY_MASK)>>2;

  /* print out status info */

  printf("STATUS for TDC id %d at base address 0x%x \n",id,(UINT32) v767p[id]);
  printf("---------------------------------------------- \n");

  if(iLvl>0) {
    printf(" ** Interrupts Enabled **\n");
    printf(" VME Interrupt Level: %d   Vector: 0x%x \n",iLvl,iVec);
    printf(" Interrupt Count    : %d \n",v767IntCount);
  } else {
    printf(" Interrupts Disabled\n");
    printf(" Last Interrupt Count    : %d \n",v767IntCount);
  }
  printf("\n");

  printf("  Data Status \n");
  printf("               --1--  --2--\n");
  if(bfull) {
    printf("    Status     0x%04x 0x%04x (Buffer Full)\n",stat1Reg,stat2Reg);
  } else if(afull) {
    printf("    Status     0x%04x 0x%04x (Almost Full)\n",stat1Reg,stat2Reg);
  }else if(drdy) {
    printf("    Status     0x%04x 0x%04x (Data Ready)\n",stat1Reg,stat2Reg);
  }else {
    printf("    Status     0x%04x 0x%04x (No Data)\n",stat1Reg,stat2Reg);
  }

  if(busy)
    printf("    Module Busy   : On\n");
  else
    printf("    Module Busy   : Off\n");
  
  if(evCount == 0xffff)
    printf("    Total Events  = (No Events taken)\n");
  else
    printf("    Total Events  = %d\n",evCount);


  printf("\n");

  printf("  TDC Mode/Status (control1 reg = 0x%04x)\n",cntl1Reg); 
  printf("  TDC Mode/Status (control2 reg = 0x%04x)\n",cntl2Reg); 
  printf("  TDC Mode/Status (bitset reg   = 0x%04x)\n",bitReg);

  if(testmode)
    printf("    Test Mode       : On\n");

  printf("    Aquisition Mode : %d\n",acqmode);
  printf("    Data Ready Mode : %d\n",drdymode);

  if(berr)
    printf("    Bus Errors      : On\n");
  else
    printf("    Bus Errors      : Off\n");

  if(blk)
    printf("    Block End       : On\n");
  else
    printf("    Block End       : Off\n");

  printf("    TDC Errors      : 0:%d   1:%d   2:%d   3:%d\n",
	 tdcerror[0],tdcerror[1],tdcerror[2],tdcerror[3]);

  printf("\n");

}


/*******************************************************************************
*
* v767Int - default interrupt handler
*
* This rountine handles the v767 TDC interrupt.  A user routine is
* called, if one was connected by v767IntConnect().
*
* RETURNS: N/A
*
*/

LOCAL void 
v767Int (void)
{

  UINT32 nevt=0;
  
  /* Disable interrupts */
  sysIntDisable(v767IntLevel);

  v767IntCount++;
 
  if (v767IntRoutine != NULL)  {     /* call user routine */
    (*v767IntRoutine) (v767IntArg);
  }else{
    if((v767IntID<0) || (v767p[v767IntID] == NULL)) {
      logMsg("v767Int: ERROR : TDC id %d not initialized \n",v767IntID,0,0,0,0,0);
      return;
    }

    logMsg("v767Int: Processed %d events\n",nevt,0,0,0,0,0);

  }

  /* Enable interrupts */
  sysIntEnable(v767IntLevel);

}


/*******************************************************************************
*
* v767IntConnect - connect a user routine to the v767 TDC interrupt
*
* This routine specifies the user interrupt routine to be called at each
* interrupt. 
*
* RETURNS: OK, or ERROR if Interrupts are enabled
*/

STATUS 
v767IntConnect (VOIDFUNCPTR routine, int arg, UINT16 level, UINT16 vector)
{

  if(v767IntRunning) {
    printf("v767IntConnect: ERROR : Interrupts already Initialized for TDC id %d\n",
	   v767IntID);
    return(ERROR);
  }
  
  v767IntRoutine = routine;
  v767IntArg = arg;

  /* Check for user defined VME interrupt level and vector */
  if(level == 0) {
    v767IntLevel = V767_VME_INT_LEVEL; /* use default */
  }else if (level > 7) {
    printf("v767IntConnect: ERROR: Invalid VME interrupt level (%d). Must be (1-7)\n",level);
    return(ERROR);
  } else {
    v767IntLevel = level;
  }

  if(vector == 0) {
    v767IntVec = V767_INT_VEC;  /* use default */
  }else if ((vector < 32)||(vector>255)) {
    printf("v767IntConnect: ERROR: Invalid interrupt vector (%d). Must be (32<vector<255)\n",vector);
    return(ERROR);
  }else{
    v767IntVec = vector;
  }
      
  /* Connect the ISR */
#ifdef VXWORKSPPC
  if((intDisconnect((int)INUM_TO_IVEC(v767IntVec)) != 0)) {
    printf("v767IntConnect: ERROR disconnecting Interrupt\n");
    return(ERROR);
  }
#endif
  if((intConnect(INUM_TO_IVEC(v767IntVec),v767Int,0)) != 0) {
    printf("v767IntConnect: ERROR in intConnect()\n");
    return(ERROR);
  }

  return (OK);
}


/*******************************************************************************
*
* v767IntEnable - Enable interrupts from specified TDC
*
* Enables interrupts for a specified TDC.
* 
* RETURNS OK or ERROR if TDC is not available or parameter is out of range
*/

STATUS 
v767IntEnable (int id, UINT16 evCnt)
{

  if(v767IntRunning) {
    printf("v767IntEnable: ERROR : Interrupts already initialized for TDC id %d\n",
	   v767IntID);
    return(ERROR);
  }

  if((id<0) || (v767p[id] == NULL)) {
    printf("v767IntEnable: ERROR : TDC id %d not initialized \n",id);
    return(ERROR);
  }else{
    v767IntID = id;
  }
  
  
  sysIntEnable(v767IntLevel);   /* Enable VME interrupts */
  
  /* Zero Counter and set Running Flag */
  v767IntEvCount = evCnt;
  v767IntCount = 0;
  v767IntRunning = TRUE;
  /* Enable interrupts on TDC */
  
  return(OK);
}


/*******************************************************************************
*
* v767IntDisable - disable the TDC interrupts
*
* RETURNS: OK, or ERROR if not initialized
*/

STATUS 
v767IntDisable (int iflag)
{

  if((v767IntID<0) || (v767p[v767IntID] == NULL)) {
    logMsg("v767IntDisable: ERROR : TDC id %d not initialized \n",v767IntID,0,0,0,0,0);
    return(ERROR);
  }

  sysIntDisable(v767IntLevel);   /* Disable VME interrupts */


  /* Tell tasks that Interrupts have been disabled */
  if(iflag > 0) {
    v767IntRunning = FALSE;
  }else{
    semGive(v767Sem);
  }
  
  return (OK);
}

/*******************************************************************************
*
*  v767ReadMicro - Read Microcontroller Register
*
*
*   returns  OK or ERROR
*/
STATUS
v767ReadMicro(int id, UINT16 *data, int nwords)
{
  int ii,jj, kk=0;
  UINT16 mstatus;

  if((id<0) || (v767p[id] == NULL)) {
    logMsg("v767ReadMicro: ERROR : TDC id %d not initialized \n",id,0,0,0,0,0);
    return(ERROR);
  }

  /* default to read one dataword */
  if(nwords==0) nwords=1;

 retry:
  mstatus = (v767p[id]->microHandshake)&V767_MICRO_READOK;

  if(mstatus) {
    for (ii=0;ii<nwords;ii++) {
      jj=0;
      while(!((v767p[id]->microHandshake)&V767_MICRO_READOK)) {
	jj++;
	if(jj>1000) {
	  logMsg("v767ReadMicro: ERROR: Read Status not OK (read %d)\n",ii,0,0,0,0,0);
	  return(ERROR);
	}
      }
      taskDelay(1);
      data[ii] = v767p[id]->microReg;
    }
  }else{
    kk++;
    if(kk>=1000) {
      logMsg("v767ReadMicro: ERROR: Read Status not OK\n",0,0,0,0,0,0);
      return(ERROR);
    }else{
      goto retry;
    }
  }

  return(OK);

}

/*******************************************************************************
*
*  v767WriteMicro - Write to Microcontroller Register
*
*
*   returns  OK or ERROR
*/
STATUS
v767WriteMicro(int id, UINT16 data)
{

  int kk=0;
  volatile UINT16 mstatus=0;

  if((id<0) || (v767p[id] == NULL)) {
    logMsg("v767WriteMicro: ERROR : TDC id %d not initialized \n",id,0,0,0,0,0);
    return(ERROR);
  }

 retry:
  mstatus = (v767p[id]->microHandshake)&V767_MICRO_WRITEOK;

  if(mstatus) {
    taskDelay(1);
    v767p[id]->microReg = data;
  }else{
    kk++;
    mstatus=0;
    if(kk>=1000) {
      logMsg("v767WriteMicro: ERROR: Write Status not OK\n",0,0,0,0,0,0);
      return(ERROR);
    }else{
      goto retry;
    }
  }

  return(OK);

}

/*******************************************************************************
*
* v767TriggerConfig  - Program or Read Back TDC Trigger Matching Configuration
* 
* 
* 
* 
*
* RETURNS: OK or ERROR.
*/

STATUS
v767TriggerConfig(int id, INT16 mw, INT16 to, INT16 tl)
{

  INT16 t1, t2, t3;
  UINT16 tdata;

  if((id<0) || (v767p[id] == NULL)) {
    logMsg("v767TriggerConfig: ERROR : TDC id %d not initialized \n",id,0,0,0,0,0);
    return(ERROR);
  }

  if((mw==0)&&(to==0)&&(tl==0)) {
    /* Read and print Trigger Configuration */
    v767WriteMicro(id,GET_WINDOW_WIDTH);
    v767ReadMicro(id,&t1,1);
    v767WriteMicro(id,GET_WINDOW_OFFSET);
    v767ReadMicro(id,&t2,1);
    v767WriteMicro(id,GET_TRIGGER_LATENCY);
    v767ReadMicro(id,&t3,1);
    printf("  Match Window       = %d ns\n",(int) (t1*V767_CLOCK_PERIOD));
    printf("  Window  Offset     = %d ns\n",(int) (t2*V767_CLOCK_PERIOD));
    printf("  Trigger Latency    = %d ns\n",(int) (t3*V767_CLOCK_PERIOD));

  }else{

    v767WriteMicro(id,SET_WINDOW_WIDTH);
    tdata = (UINT16)(mw/V767_CLOCK_PERIOD);
    v767WriteMicro(id,tdata);

    taskDelay(15);

    v767WriteMicro(id,SET_WINDOW_OFFSET);
    tdata = (UINT16)(to/V767_CLOCK_PERIOD);
    v767WriteMicro(id,tdata);

    taskDelay(15);

    v767WriteMicro(id,SET_TRIGGER_LATENCY);
    tdata = (UINT16)(tl/V767_CLOCK_PERIOD);
    v767WriteMicro(id,tdata);

    taskDelay(15); 

  }


  return(OK);

}


/*******************************************************************************
*
* v767PrintEvent - Print an event from TDC to standard out. 
*
*
* RETURNS: Number of Data words read from the TDC (including Header/Trailer).
*/

int
v767PrintEvent(int id, int pflag)
{

  int nWords, evCount, trigMatch;
  UINT32 gheader, gtrailer, tmpData, dCnt;
  int chanID, dataVal, tEdge;

  if((id<0) || (v767p[id] == NULL)) {
    printf("v767PrintEvent: ERROR : TDC id %d not initialized \n",id);
    return(ERROR);
  }

  /* Check if there is a valid event */

  if((v767p[id]->status1)&V767_STATUS1_DATA_READY) {
    dCnt = 0;
    trigMatch = v767p[id]->control2&V767_CONTROL2_ACQMODE_MASK;

    if(trigMatch != V767_ACQMODE_CS) {  /* If trigger match mode then print individual event */

      /* Read Header - Get event count */
      gheader = v767p[id]->fifo;
      if((gheader&V767_DATA_TYPE_MASK) != V767_HEADER_DATA) {
	printf("v767PrintEvent: ERROR: Invalid Header Word 0x%08x\n",gheader);
	return(ERROR);
      }else{
	printf("  TDC DATA for Module %d\n",id);
	evCount = (gheader&V767_HEADER_EVCOUNT_MASK);
	dCnt++;
	printf("    Header: 0x%08x   Event Count = %d \n",gheader,evCount);
      }

      /* Loop and get data for each */
      tmpData = v767p[id]->fifo;

      while ((tmpData&V767_DATA_TYPE_MASK) == V767_DATA) {
	dCnt++;
	if ((dCnt % 5) == 0) printf("\n    ");
	printf("     0x%08x",tmpData);

	tmpData = v767p[id]->fifo;
      }
      printf("\n");

      /* Out of the Loop so last  data word should be Global EOB */
      gtrailer = tmpData;
      if((gtrailer&V767_DATA_TYPE_MASK) != V767_EOB_DATA) {
	printf("v767PrintEvent: ERROR: Invalid EOB Word 0x%08x\n",gtrailer);
	return(ERROR);
      }else{
	nWords = (gtrailer&V767_EOB_WORDCOUNT_MASK);
	dCnt++;
	printf("    EOB   : 0x%08x   Total Word Count = %d \n",gtrailer,nWords);
      }

    } else { /* Continuous Storage mode */

      tmpData = v767p[id]->data[dCnt];
      printf("  TDC Continuous Storage DATA\n");
      while ((tmpData&V767_DATA_TYPE_MASK) != V767_INVALID_DATA) {

	chanID = (tmpData&V767_CHANNEL_MASK)>>24;
	tEdge = (tmpData&V767_EDGE_MASK)>>20;
	dataVal = (tmpData&V767_DATA_MASK);

	printf("    %d   %d   %d\n",chanID, tEdge, dataVal);
	dCnt++;
	tmpData = v767p[id]->data[dCnt];	
      }
      printf("\n");

    }
    return (dCnt);

  }else{
    printf("v767PrintEvent: No data available for readout!\n");
    return(0);
  }
}

/*******************************************************************************
*
* v767ReadEvent - Read an event from TDC to a specified address. 
*
*
* RETURNS: Number of Data words read from the TDC (including Header/Trailer).
*/

int
v767ReadEvent(int id, UINT32 *tdata, int maxWords)
{

  int nWords=0, evCount, trigMatch;
  UINT32 gheader, gtrailer, tmpData, dCnt;

  if((id<0) || (v767p[id] == NULL)) {
    logMsg("v767ReadEvent: ERROR : TDC id %d not initialized \n",id,0,0,0,0,0);
    return(ERROR);
  }

  if(maxWords==0) maxWords = 1024;

  /* Check if there is a valid event */

  if((v767p[id]->status1)&V767_STATUS1_DATA_READY) {
    dCnt = 0;
    trigMatch = v767p[id]->control2&V767_CONTROL2_ACQMODE_MASK;

    if(trigMatch != V767_ACQMODE_CS) {  /* If trigger match mode then read individual event */

      /* Read Header - Get event count */
      gheader = v767p[id]->fifo;
      if((gheader&V767_DATA_TYPE_MASK) != V767_HEADER_DATA) {
	logMsg("v767ReadEvent: ERROR: Invalid Header Word 0x%08x\n",gheader,0,0,0,0,0);
	return(ERROR);
      }else{
	tdata[dCnt] = gheader;
	evCount = (gheader&V767_HEADER_EVCOUNT_MASK);
	dCnt++;
      }

      /* Loop and get data for each */
      tmpData = v767p[id]->fifo;
      while (((tmpData&V767_DATA_TYPE_MASK) == V767_DATA)&&(dCnt<maxWords)) {
	tdata[dCnt]=tmpData;
	dCnt++;
	tmpData = v767p[id]->fifo;	
      }

      /* Out of the Loop so last  data word should be Global EOB */
      gtrailer = tmpData;
      if((gtrailer&V767_DATA_TYPE_MASK) != V767_EOB_DATA) {
	logMsg("v767ReadEvent: ERROR: Invalid EOB Word 0x%08x\n",gtrailer,0,0,0,0,0);
	return(ERROR);
      }else{
	tdata[dCnt] = gtrailer;
	nWords = (gtrailer&V767_EOB_WORDCOUNT_MASK) + 2;
	dCnt++;
      }

    } else { /* Continuous Storage mode */

      tmpData = v767p[id]->fifo;
      while (((tmpData&V767_DATA_TYPE_MASK) != V767_INVALID_DATA)&&(dCnt<maxWords)) {
	tdata[dCnt]=tmpData;
	dCnt++;
	tmpData = v767p[id]->fifo;	
      }

    }

    if(nWords != dCnt) {
      logMsg("v767ReadEvent: ERROR: Word Count inconsistant: nWords = %d  Loop Count = %d\n",
	     nWords,dCnt,0,0,0,0);
      return(ERROR);
    }else{
      return (dCnt);
    }

  }else{
    return(0);
  }
}

/*******************************************************************************
*
* v767FlushEvent - Flush event/data from TDC. 
*
*
* RETURNS: Number of Data words read from the TDC.
*/

int
v767FlushEvent(int id, int fflag)
{

  int done = 0;
  UINT32 tmpData, dCnt;

  if((id<0) || (v767p[id] == NULL)) {
    logMsg("v767FlushEvent: ERROR : TDC id %d not initialized \n",id,0,0,0,0,0);
    return(-1);
  }

  /* Check if there is a valid event */

  if((v767p[id]->status2)&V767_STATUS2_EMPTY) {
    if(fflag > 0) logMsg("v767FlushEvent: Data Buffer is EMPTY!\n",0,0,0,0,0,0);
    return(0);
  }

  /* Check if Data Ready Flag is on */
  if((v767p[id]->status1)&V767_STATUS1_DATA_READY) {
    dCnt = 0;
    
    while (!done) {
      tmpData = v767p[id]->fifo;
      switch (tmpData&V767_DATA_TYPE_MASK) {
      case V767_HEADER_DATA:
	if(fflag > 0) logMsg("v767FlushEvent: Found Header 0x%08x\n",tmpData,0,0,0,0,0);
	break;
      case V767_DATA:
	break;
      case V767_EOB_DATA:
	if(fflag > 0) logMsg(" v767FlushEvent: Found Trailer 0x%08x\n",tmpData,0,0,0,0,0);
	done = 1;
	break;
      case V767_INVALID_DATA:
	if(fflag > 0) logMsg(" v767FlushEvent: Buffer Empty 0x%08x\n",tmpData,0,0,0,0,0);
	done = 1;
	break;
      default:
	if(fflag > 0) logMsg(" v767FlushEvent: Invalid Data 0x%08x\n",tmpData,0,0,0,0,0);
      }

      /* Print out Data */
      if(fflag > 1) {
	if ((dCnt % 5) == 0) printf("\n    ");
	printf("  0x%08x ",tmpData);
      }
      dCnt++;
    }
    if(fflag > 1) printf("\n");

    return (dCnt);

  }else{
    if(fflag > 0) logMsg("v767FlushEvent: Data Not ready for readout!\n",0,0,0,0,0,0);
    return(0);
  }
}


/*******************************************************************************
*
* v767ReadBlock - Read Block of events from TDC to specified address. 
*
* INPUTS:    id     - module id of TDC to access
*            data   - address of data destination
*            nwrds  - number of data words to transfer
*
* RETURNS: OK or ERROR on success of transfer.
*
*/

int
v767ReadBlock(int id, volatile UINT32 *data, int nwrds)
{

  int retVal, xferCount;
  UINT16 stat = 0;
  retVal = OK;

  if((id<0) || (v767p[id] == NULL)) {
    logMsg("v767ReadBlock: ERROR : TDC id %d not initialized \n",id,0,0,0,0,0);
    return(-1);
  }


#ifdef VXWORKSPPC
  /* Don't bother checking if there is a valid event. Just blast data out of the 
     FIFO Valid or Invalid 
     Also assume that the Universe DMA programming is setup. */

#ifdef WHENITWORKS
  retVal = sysVmeDmaSend((UINT32)data, (UINT32)(v767p[id]->data), (nwrds<<2), 0);
  if(retVal < 0) {
    logMsg("v767ReadBlock: ERROR in DMA transfer Initialization 0x%x\n",retVal,0,0,0,0,0);
    return(retVal);
  }
  /* Wait until Done or Error */
  retVal = sysVmeDmaDone(1000,1);
#endif

#else
 
  /* 68K Block 32 transfer from FIFO using VME2Chip */
  /*retVal = mvme_dma((long)data, 1, (long)(v767pl[id]->data), 0, nwrds, 1);*/

#endif

  if(retVal != 0) { /* Transfer Error */
    /* Check to see if error was generated by TDC */
    stat = (v767p[id]->bitSet)&V767_VME_BUS_ERROR;
    if((retVal>0) && (stat)) {
      v767p[id]->bitClear = V767_VME_BUS_ERROR;
      logMsg("v767ReadBlock: INFO: DMA terminated by TDC(BUS Error) - Transfer OK\n",0,0,0,0,0,0);
      xferCount = (nwrds - (retVal>>2));  /* Number of Longwords transfered */
      if ((data[xferCount-1]&V767_DATA_TYPE_MASK) == V767_EOB_DATA) {
	return(xferCount); /* Return number of data words transfered */
      } else {
	logMsg("v767ReadBlock: ERROR: Invalid Trailer data 0x%x\n",data[xferCount-1],0,0,0,0,0);
	return(xferCount);
      }
    } else {
      logMsg("v767ReadBlock: ERROR in DMA transfer 0x%x\n",retVal,0,0,0,0,0);
      return(retVal);
    }
  } else { /* Transfer OK */
    if ( ((nwrds%2)==0) && ((data[nwrds-1]&V767_DATA_TYPE_MASK) != V767_EOB_DATA) )
      logMsg("v767ReadBlock: ERROR: Invalid Trailer data 0x%x\n",data[nwrds-1],0,0,0,0,0);
    return(nwrds);
  }

}


/*******************************************************************************
*
* v767Dready - Return status of Data Ready bit in TDC
*
*
* RETURNS: 0(No Data) or 1(Data Ready) or ERROR.
*/

int
v767Dready(int id)
{

  UINT16 stat=0;


  if((id<0) || (v767p[id] == NULL)) {
    logMsg("v767Dready: ERROR : TDC id %d not initialized \n",id,0,0,0,0,0);
    return (ERROR);
  }
  
  stat = (v767p[id]->status1)&V767_STATUS1_DATA_READY;
  if(stat)
    return(1);
  else
    return(0);
}


/*******************************************************************************
*
* v767Trig         - Issue Software trigger to TDC
* v767Clear        - Clear TDC
* v767Reset        - Clear/Reset TDC
*
*
* RETURNS: None.
*/

void
v767Trig(int id)
{
  if((id<0) || (v767p[id] == NULL)) {
    logMsg("v767Trig: ERROR : TDC id %d not initialized \n",id,0,0,0,0,0);
    return;
  }
  V767_EXEC_TRIGGER(id);
}

void
v767Clear(int id)
{
  if((id<0) || (v767p[id] == NULL)) {
    logMsg("v767Clear: ERROR : TDC id %d not initialized \n",id,0,0,0,0,0);
    return;
  }
  V767_EXEC_CLEAR(id);
}

void
v767Reset(int id)
{
  if((id<0) || (v767p[id] == NULL)) {
    logMsg("v767Reset: ERROR : TDC id %d not initialized \n",id,0,0,0,0,0);
    return;
  }
  V767_EXEC_MODULE_RESET(id);

}



/*******************************************************************************
*
* v767Overflow - Return status of Gerror bit in TDC
*
*
* RETURNS: 0(No Data) or 1(Data Ready) or ERROR.
*/

int
v767Overflow(int id)
{

  UINT16 stat=0;


  if((id<0) || (v767p[id] == NULL)) {
    logMsg("v767Overflow: ERROR : TDC id %d not initialized \n",id,0,0,0,0,0);
    return (ERROR);
  }
  
  stat = (v767p[id]->status2)&V767_STATUS2_GERROR;
  if(stat)
    return(1);
  else
    return(0);
}


/*******************************************************************************
*
* v767Status2 - Return the Status 2 register
*
*
* RETURNS: 0(No Data) or 1(Data Ready) or ERROR.
*/

int
v767Status2(int id)
{

  UINT16 stat=0;


  if((id<0) || (v767p[id] == NULL)) {
    logMsg("v767Overflow: ERROR : TDC id %d not initialized \n",id,0,0,0,0,0);
    return (ERROR);
  }
  
  stat = (v767p[id]->status2)&0xffff;
  return(stat);
}

/*******************************************************************************
*
* v767SetACQMode
*
* INPUTS:    id     - module id of TDC to access
*            mode   - 0: stop_tm, 1: start_tm, 2: start_gm, 3: cs
*
* RETURNS: OK, or ERROR if not initialized
*/

STATUS
v767SetAcqMode(int id, int mode)
{


  STATUS retval=OK; 

  if((id<0) || (v767p[id] == NULL)) {
    logMsg("v767SetAcqMode: ERROR : TDC id %d not initialized \n",id,0,0,0,0,0);
    return (ERROR);
  }
  switch (mode) {
  case V767_ACQMODE_STOP_TM:
    v767WriteMicro(id,SET_ACQMODE_STOP_TM);
    taskDelay(25);
    break;
  case V767_ACQMODE_START_TM:
    v767WriteMicro(id,SET_ACQMODE_START_TM);
    taskDelay(25);
    break;
  case V767_ACQMODE_START_GM:
    v767WriteMicro(id,SET_ACQMODE_START_GM);
    taskDelay(25);
    break;
  case V767_ACQMODE_CS:
    v767WriteMicro(id,SET_ACQMODE_CS);
    taskDelay(25);
    break;
  default:
    retval = ERROR;
    break;
  }
  return (retval);
}


/*******************************************************************************
*
* v767SetDReadyMode
*
* INPUTS:    id     - module id of TDC to access
*            mode   - 0: one event, 1: almost full, 2: not empty
*
* RETURNS: OK, or ERROR if not initialized
*/

STATUS
v767SetDReadyMode(int id, int mode)
{

  STATUS retval=OK; 

  if((id<0) || (v767p[id] == NULL)) {
    logMsg("v767SetDReadyMode: ERROR : TDC id %d not initialized \n",id,0,0,0,0,0);
    return (ERROR);
  }
  switch (mode) {
  case V767_DREADY_MODE_1EVENT:
    v767WriteMicro(id,SET_DREADY_MODE_1EVENT);
    taskDelay(25);
    break;
  case V767_DREADY_MODE_ALMOST_FULL:
    v767WriteMicro(id,SET_DREADY_MODE_ALMOST_FULL);
    taskDelay(25);
    break;  
  case V767_DREADY_MODE_NOT_EMPTY:
    v767WriteMicro(id,SET_DREADY_MODE_NOT_EMPTY);
    taskDelay(25);
    break;
  default:
    retval = ERROR;
    break;
  }
  return (retval);
}

/*******************************************************************************
*
* v767BusErrEnable
*
* INPUTS:    id     - module id of TDC to access
*
* RETURNS: OK, or ERROR if not initialized
*/

STATUS
v767BusErrEnable(int id)
{

  if((id<0) || (v767p[id] == NULL)) {
    logMsg("v767BusErrEnable: ERROR : TDC id %d not initialized \n",id,0,0,0,0,0);
    return (ERROR);
  }

  (v767p[id]->control1)|=V767_CONTROL1_BERR_ENABLE;

  return OK;
}


/*******************************************************************************
*
* v767BusErrDisable
*
* INPUTS:    id     - module id of TDC to access
*
* RETURNS: OK, or ERROR if not initialized
*/

STATUS
v767BusErrDisable(int id)
{

  if((id<0) || (v767p[id] == NULL)) {
    logMsg("v767BusErrDisable: ERROR : TDC id %d not initialized \n",id,0,0,0,0,0);
    return (ERROR);
  }

  (v767p[id]->control1)&=V767_CONTROL1_BERR_DISABLE;

  return OK;
}


/*******************************************************************************
*
* v767AddrRotary
*
* INPUTS:    id     - module id of TDC to access
*
* RETURNS: OK, or ERROR if not initialized
*/

STATUS
v767AddrRotary(int id)
{

  if((id<0) || (v767p[id] == NULL)) {
    logMsg("v767AddrRotary: ERROR : TDC id %d not initialized \n",id,0,0,0,0,0);
    return (ERROR);
  }

  (v767p[id]->bitClear)|=V767_SELADDR_MASK;

  return OK;
}


/*******************************************************************************
*
* v767SetBLKEndMode
*
* INPUTS:    id     - module id of TDC to access
*            mode   - 0: Block ends when buffer is completly empty 1: Block ends after first EOB
*
* RETURNS: OK, or ERROR if not initialized
*/

STATUS
v767SetBLKEndMode(int id, int mode)
{

  STATUS retval=OK; 

  if((id<0) || (v767p[id] == NULL)) {
    logMsg("v767SetBLKEndMode : ERROR : TDC id %d not initialized \n",id,0,0,0,0,0);
    return (ERROR);
  }
  switch (mode) {
  case 0:
    (v767p[id]->control1)&=(V767_CONTROL1_NOBLKEND);
    break;
  case 1:
    (v767p[id]->control1)|=(V767_CONTROL1_BLKEND);
    break;
  default:
    retval = ERROR;
    break;
  }
  return (retval);
}

/*******************************************************************************
*
* v767AutoloadEnable
*
* INPUTS:    id     - module id of TDC to access
*
* RETURNS: OK, or ERROR if not initialized
*/

STATUS
v767AutoloadEnable(int id)
{

  if((id<0) || (v767p[id] == NULL)) {
    logMsg("v767AutoloadEnable: ERROR : TDC id %d not initialized \n",id,0,0,0,0,0);
    return (ERROR);
  }

  v767WriteMicro(id,ENABLE_AUTO_LOAD);
  taskDelay(15);

  return OK;
}

/*******************************************************************************
*
* v767AutoloadDisable
*
* INPUTS:    id     - module id of TDC to access
*
* RETURNS: OK, or ERROR if not initialized
*/

STATUS
v767AutoloadDisable(int id)
{

  if((id<0) || (v767p[id] == NULL)) {
    logMsg("v767AutoloadDisable: ERROR : TDC id %d not initialized \n",id,0,0,0,0,0);
    return (ERROR);
  }

  v767WriteMicro(id,DISABLE_AUTO_LOAD);
  taskDelay(15);

  return OK;
}

/*******************************************************************************
*
* v767SaveConfig
*
* INPUTS:    id     - module id of TDC to access
*
* RETURNS: OK, or ERROR if not initialized
*/

STATUS
v767SaveConfig(int id)
{

  if((id<0) || (v767p[id] == NULL)) {
    logMsg("v767SaveConfig: ERROR : TDC id %d not initialized \n",id,0,0,0,0,0);
    return (ERROR);
  }

  v767WriteMicro(id,SAVE_CONFIG);
  taskDelay(100);

  return OK;
}

/*******************************************************************************
*
* v767SetEdgeDetectionMode
*
* INPUTS:    id     - module id of TDC to access
*            mode   - 0: rising edge, 1: falling edge, 6: both edges on all channels
*            The TDC supports more modes, which are not yet implemented
*
* RETURNS: OK, or ERROR if not initialized
*/

STATUS
v767SetEdgeDetectionMode(int id, int mode)
{

  STATUS retval=OK; 

  if((id<0) || (v767p[id] == NULL)) {
    logMsg("v767SetedgeDetectionMode: ERROR : TDC id %d not initialized \n",id,0,0,0,0,0);
    return (ERROR);
  }
  switch (mode) {
  case 0:
    v767WriteMicro(id,EDGEDET_RISE_ALL);
    taskDelay(15);
    break;
  case 1:
    v767WriteMicro(id,EDGEDET_FALL_ALL);
    taskDelay(15);
    break;
  case 6:
    v767WriteMicro(id,EDGEDET_BOTH_ALL);
    taskDelay(15);
    break;
  default:
    retval = ERROR;
    break;
  }
  return (retval);
}

/*******************************************************************************
*
* v767GetEdgeDetectionMode
*
* INPUTS:    id     - module id of TDC to access
*
* RETURNS: -1 if not initialized, 100 if non standard, or mode as in SetEdgeDetection
*/

int
v767GetEdgeDetectionMode(int id)
{

  UINT16 t1,t2,t3;
  int retval=-1; 

  if((id<0) || (v767p[id] == NULL)) {
    logMsg("v767GetedgeDetectionMode: ERROR : TDC id %d not initialized \n",id,0,0,0,0,0);
    return (retval);
  }

  v767WriteMicro(id,EDGEDET_READ);
  v767ReadMicro(id,&t1,1);
  v767ReadMicro(id,&t2,1);
  v767ReadMicro(id,&t3,1);

  printf(" Even  Channels : %d \n", (int) (t1&3) );
  printf(" Odd   Channels : %d \n", (int) (t2&3) );
  printf(" Start Channel  : %d \n", (int) (t3&3) );

  taskDelay(15);

  if (((t1&3)==1)&&((t2&3)==1)) {
    retval=0;
  } else if (((t1&3)==2)&&((t2&3)==2)) {
    retval=1;
  } else if (((t1&3)==3)&&((t2&3)==3)) {
    retval=6;
  } else {
    retval=100;
  }

  return (retval);
}


/*******************************************************************************
*
* v767OverlapTrigEnable
*
* INPUTS:    id     - module id of TDC to access
*
* RETURNS: OK, or ERROR if not initialized
*/

STATUS
v767OverlapTrigEnable(int id)
{

  if((id<0) || (v767p[id] == NULL)) {
    logMsg("v767OverlapTrigEnable: ERROR : TDC id %d not initialized \n",id,0,0,0,0,0);
    return (ERROR);
  }

  v767WriteMicro(id,ENABLE_OVL_TRG);
  taskDelay(15);

  return OK;
}

/*******************************************************************************
*
* v767OverlapTrigDisable
*
* INPUTS:    id     - module id of TDC to access
*
* RETURNS: OK, or ERROR if not initialized
*/

STATUS
v767OverlapTrigDisable(int id)
{

  if((id<0) || (v767p[id] == NULL)) {
    logMsg("v767OverlapTrigDisable: ERROR : TDC id %d not initialized \n",id,0,0,0,0,0);
    return (ERROR);
  }

  v767WriteMicro(id,DISABLE_OVL_TRG);
  taskDelay(15);

  return OK;
}

/*******************************************************************************
*
* v767ChannelMask
*
* INPUTS:    id     - module id of TDC to access
*            tdata  - array of 8 UINT16 words,
*                     bits set to 0 turn of the corresponding TDC channel
*
* RETURNS: OK, or ERROR if not initialized
*/

STATUS
v767ChannelMask(int id,UINT16 *tdata)
{

  int i;

  if((id<0) || (v767p[id] == NULL)) {
    logMsg("v767ChannelMask: ERROR : TDC id %d not initialized \n",id,0,0,0,0,0);
    return (ERROR);
  }

  v767WriteMicro(id,WRITE_ENABLE_PATTERN);
  for (i=0; i<8; i++) {
    taskDelay(15);
    v767WriteMicro(id,tdata[i]);
  }

  taskDelay(15);

  return OK;
}


/*******************************************************************************
*
* v767Rates
*
* INPUTS:    id     - module id of TDC to access
*
* RETURNS: OK, or ERROR if not initialized
* 
* Prints a map of rates per channel
* Caution: this routine does change the programming of the TDC
*          the TDC needs to be reprogrammed afterwards
*/

STATUS
v767Rates(int id)
{

  int i,nwire;
  int nchip;
  int noverflows, ntries;
  int nhits[128], ntrig[128], flag[128];
  unsigned short channelmask[8];

  if((id<0) || (v767p[id] == NULL)) {
    logMsg("v767Rates: ERROR : TDC id %d not initialized \n",id,0,0,0,0,0);
    return (ERROR);
  }

  printf("v767Rates \n");
  printf("========= \n\n");
  printf("Warning: the TDC will be reprogrammed, previous settings will be lost. \n");
  printf("Warning: before running CODA again, \n ");
  printf("         you have to reset and download the configuration again.\n");

  printf("\nResetting TDC\n");

  v767Reset(id); 
  for (i=0;i<8;i++) {
    channelmask[i] = 0xffff;
  }
  for (i=0;i<128;i++) { nhits[i]=0; ntrig[i]=0; flag[i]=1; }
  noverflows=0;
  ntries=0;
  taskDelay(100);
  v767SetAcqMode(id,0);
  v767SetDReadyMode(id,0); 
  v767SetEdgeDetectionMode(id,0);
  v767OverlapTrigDisable(id);
  v767TriggerConfig(id,10000,-10100,0);
  v767ChannelMask(id,channelmask); 
  v767Clear(id);

  printf("Trying standard procedure on all channels\n");

  if (v767RateLoop(id, &ntries, 100000, &noverflows, 10, nhits)!=OK) return ERROR;

  for (i=0;i<128;i++) {
    ntrig[i]+=ntries;
  }

  if (ntries<100000) {

    for (nchip=0;nchip<4;nchip++) {
      for (i=0;i<8;i++) {
	channelmask[i] = 0x0;
      }
      channelmask[2*nchip]=0xffff;
      channelmask[2*nchip+1]=0xffff;
      v767ChannelMask(id,channelmask); 

      printf("Some Channels are noisy: Trying Chip %d only \n",nchip);

      if (v767RateLoop(id, &ntries, 20000, &noverflows, 10, nhits)!=OK) return ERROR;
      
      for (i=32*nchip;i<32*nchip+32;i++) {
	ntrig[i]+=ntries;
      }

      if (ntries<20000) {
	printf("Chip %d has noisy channels, going wire by wire \n",nchip);
	for (nwire=32*nchip;nwire<32*nchip+32;nwire++) {
	  if (nwire<32*nchip+16) {
	    channelmask[2*nchip]= 1 << (nwire % 16) ;
	    channelmask[2*nchip+1]=0x0;
	  } else {
	    channelmask[2*nchip]=0x0;
	    channelmask[2*nchip+1]= 1 << (nwire % 16) ;
	  }
	  v767ChannelMask(id,channelmask); 
	  printf("Some Channels are noisy: Trying Channel %d only \n",nwire);
	  if (v767RateLoop(id, &ntries, 2000, &noverflows, 10, nhits)!=OK) return ERROR;
      	  ntrig[nwire]+=ntries;
	  if (ntries<2000) {
	    flag[nwire]=0;
	  }
	}
      }

    }

  }

  printf("V767Rate: done\n\n");
  for (i=0;i<128;i++) { 
    if (flag[i]==1) {
      printf(" Ch %3d : %8f +- %8f ;",i,(1.e5*nhits[i]/ntrig[i]),(sqrt(nhits[i])/ntrig[i]*1.e5) );
    } else {
      printf(" Ch %3d : HIGHHIGH ++ HIGHHIGH ;",i);
    }
    if ((i % 2)==1) printf("\n") ;
  }


  return OK;
}


STATUS
v767RateLoop(int id, int* ntries, int maxtries, int* noverflows, int maxoverflows, int* nhits)
{
  int i,chan;
  int nDataWords;
  UINT32 databuffer[1024];

  *ntries=0;
  *noverflows=0;

  while ((*ntries<maxtries)&&(*noverflows<maxoverflows)) {

    V767_EXEC_TRIGGER(id);
    i=0;
    while ((!(v767Dready(id)))&&(i<1000)) {
      i++;
    }
    if (v767Overflow(id)!=0) {
      (*noverflows)++;
      v767Clear(id);
    } else {    
      if (v767Dready(id)) {
	(*ntries)++;
	nDataWords = v767ReadEvent(id,databuffer,1024);
	if ( (databuffer[0]&0x600000) != 0x400000 ) {
	  printf("No Header Word: Exiting\n");
	  return ERROR;
	}
	if ( (nDataWords<1024) && ((databuffer[nDataWords-1]&0x600000) != 0x200000 )) {
	  printf("No EOB Word: Exiting\n");
	  return ERROR;
	}
	for (i=1; i<nDataWords-1; i++) {
	  if (  (databuffer[i]&0x600000)==0 ) {
	    chan = (databuffer[i]&0x7f000000)>>24  ; 
	    nhits[chan]++;
	  } else {
	    printf("Data Word Invalid: Exiting");
	  return ERROR;
	  }
	}
      } else {
	printf("No data: Exiting\n");
	return ERROR;
      }
    }
  }
  return OK;
}

void mytest1() {

   UINT32 buf[1200];
   int i,evcnt,nData,itime,maxtime;

   maxtime=100;   /* each is 10 usec --> 100 = 1 msec */

/* Download phase --- initialize */

   v767Init(0x08000000,0,1,0);

   v767SetAcqMode(0,1);       /* 2nd arg is the mode */

   v767SetEdgeDetectionMode(0,0);

   /* 2nd arg = width, 3rd = offset (nsec), 4th=latency */
   v767TriggerConfig(0,5000,-200,0);
   v767Status(0,0,0); 
   v767TriggerConfig(0,0,0,0); /* Print current trigger config */

 
 /* End download.  Now readout as in trigger routine */


   v767SetDReadyMode(0,0);

   taskDelay(20);

   itime=0;
   while( v767Dready(0) != 1 ) {
     if(itime++ > maxtime) break;
   }
   printf("time for Dready = %d \n",itime);
   if (itime == maxtime) printf("ERROR: timed out for Dready \n");
   
   evcnt =  v767GetEvcount(0);

   printf("\n event count %d = 0x%x\n",evcnt,evcnt);

   if (evcnt != 0) { 

      nData = v767ReadEvent(0,buf,1024);

    /* local print */
     printf("\n\n $$$$$$$$$$$$ \n  nData = %d \n  Buffer: \n",nData);
     for (i = 0; i < nData; i++) {
       printf("Raw Data[%d] = 0x%x  = (dec)%d \n",i,buf[i],buf[i]);
     }


   /* let the library interpret data */
     v767PrintEvent(0,0);

     //     v767Clear(0);

   }

}

void mytest2() {

   int i,itime,maxtime,rawdata;

   maxtime=100;   /* each is 10 usec --> 100 = 1 msec */

/* Download phase --- initialize */

   v767Init(0x08000000,0,1,0);

   v767Reset(0);
   taskDelay(2*60);// 4 sec waiting

   v767SetAcqMode(0,0);       /* 2nd arg is the mode */

   v767SetEdgeDetectionMode(0,0);

   /* 2nd arg = width, 3rd = offset (nsec), 4th=latency */
   v767TriggerConfig(0,500,-100,0);
   v767Status(0,0,0); 
   v767TriggerConfig(0,0,0,0); /* Print current trigger config */

 
 /* End download.  Now readout as in trigger routine */

   v767SetDReadyMode(0,0);

   printf("Ok, go send one event !!\n");

//   itime=0;
//    while( v767Dready(0) != 1 ) {
//     if(itime++ > maxtime) break;
//   }
    // wait a bit more
    taskDelay(8*60);

    printf("Proceeding to readout .... \n");
   
   /* print the fifo -- whatever is there or not */

    //   for (i = 0; i < 10; i++) {

    //     rawdata = v767p[0]->fifo;

    //     printf("Raw fifo data[%d] = %d = 0x%x\n",i,rawdata,rawdata);

    //   }

   /* let the library interpret data */
     v767PrintEvent(0,0);


}


void mytest3() {

   unsigned long tdcdata[1024];
   int j,itimeout,MAX_TIME,nDataWords;

   MAX_TIME=5000;

   v767Init(0x08000000,0,1,0);

   v767Reset(0);
   taskDelay(2*60);// 4 sec waiting
   v767SetAcqMode(0,0);       /* 2nd arg is the mode */

   v767SetEdgeDetectionMode(0,0);

   /* 2nd arg = width, 3rd = offset (nsec), 4th=latency */
   v767TriggerConfig(0,5000,-1000,0);
   v767Status(0,0,0); 
   v767TriggerConfig(0,0,0,0); /* Print current trigger config */

 
 /* End download.  Now readout as in trigger routine */

   v767SetDReadyMode(0,0);

   printf("Ok, go send one event !!\n");

   taskDelay(5*60);

   v767Status2(0);      

   itimeout=0;
   while(v767Dready(0) != 1) if (itimeout++ > MAX_TIME) break;

   nDataWords = 0;
   if (itimeout < MAX_TIME) {
      nDataWords = v767ReadEvent(0,tdcdata,1024);
   } 
   if(nDataWords <= 0) printf("ERROR: timed out !!! \n");
   printf("nDataWords = %d \n",nDataWords);

   for (j = 0; j<nDataWords; j++) {
     printf("Data %d   = 0x%x\n",j,tdcdata[j]);
   }

   while(v767Dready(0)) v767Clear(0);

   printf("\n ALL DONE !\n");
}

void mytest4() {

   unsigned long tdcdata[1024];
   int j,itimeout,MAX_TIME,nDataWords;
   int status2,ievent;
   int chan, tdata;

   MAX_TIME=500000;

   v767Init(0x08000000,0,1,0);
   v767Reset(0);
   taskDelay(2*60);// 4 sec waiting



   v767TriggerConfig(0,5000,-1000,0);
   v767Status(0,0,0); 


   v767TriggerConfig(0,0,0,0); /* Print current trigger config */

 
 /* End download.  Now readout as in trigger routine */

   /* simulated DAQ with several events */

   //for (ievent = 0; ievent<100; ievent++) {
   for (ievent = 0; ievent<10; ievent++) {

     printf("\n\nDoing event %d\n",ievent);

     while(v767Dready(0)) v767Clear(0);

     status2 = v767Status2(0);      

     itimeout=0;

     printf("Status2 = 0x%x    Dready %d \n",status2,v767Dready(0));

     while(v767Dready(0) != 1) if (itimeout++ > MAX_TIME) break;

     printf("timeout %d     Dready %d \n",itimeout,v767Dready(0));

     nDataWords = 0;
     if (itimeout < MAX_TIME) {
        nDataWords = v767ReadEvent(0,tdcdata,1024);
     } 
     if(nDataWords <= 0) printf("ERROR: timed out !!! \n");
     printf("nDataWords = %d \n",nDataWords);

     for (j = 0; j<nDataWords; j++) {
       chan = tdcdata[j]>>24;
       tdata = tdcdata[j]&0xfffff;
       printf("Data %d   = 0x%x   chan = %d   raw data = %d\n",j,tdcdata[j],chan,tdata);
     }
  
     while(v767Dready(0)) v767Clear(0);

   }

   printf("\n ALL DONE !\n");
}
