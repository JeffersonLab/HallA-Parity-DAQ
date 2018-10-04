/******************************************************************************
*
*  scalMsgLib.c  -  Driver library for readout of a Struck 7201 Multi-scaler or
*                   SIS 3801 Multi-scaler using a VxWorks 5.2 orlater based 
*                   Single Board computer. This library uses the interrupt 
*                   capabilities of the scaler to drive the readout. The user has 
*                   the option of attaching his own interrupt service routine.
*
*  Author: David Abbott 
*          Jefferson Lab Data Acquisition Group
*          January 1999
*/


#include "vxWorks.h"
#include "stdio.h"
#include "string.h"
#include "intLib.h"
#include "iv.h"
#include "logLib.h"
#include "taskLib.h"
#include "vxLib.h"
#include "msgQLib.h"

/* Include scaler definitions */
#include "scalIntLib.h"

/* define default parameters */
#define NSCALERS           32             /* Number of enabled scalers  */
#define NMESSAGES          1800           /* 30Hz for 60 Seconds        */
#define SIZEMESSAGES       4*(NSCALERS+1) /* 32 Scalers + 1 header word */
#define MAX_FIFO_ENTRIES   1024           /* for a 64K FIFO (16bit width) one can store 1024 
                                             sets of 32 scaler values */

/* External Definitions */
extern volatile struct fifo_scaler *sfs;                        /* pointer to Scaler memory map */
extern STATUS	scalIntConnect (VOIDFUNCPTR routine, int arg);
extern STATUS	scalIntInit (UINT32, UINT32, UINT32);
extern STATUS	scalIntEnable (UINT32);
extern STATUS	scalIntDisable (void);

LOCAL UINT32 msgCount     = 0;            /* Count of messages (scaler Reads put on Queue) */
LOCAL UINT32 errMsgCount  = 0;            /* Count of lost messages */
LOCAL int    maxScalers   = NSCALERS;     /* Default number of Scalers begin read */
LOCAL int    maxMsg       = NMESSAGES;    /* Default number of Messages for Queue */
LOCAL int    maxMsgSize   = SIZEMESSAGES; /* Default size of Messages */

/* Create some global variables */
typedef struct scaler_sum_data {
        unsigned long addr;            /* Base address of Scaler */
        int count;                     /* partial sum count included */
	int lost;                      /* count of partial sums not pur on MsgQ */
	unsigned long sums[32];        /* Scaler Sum data */
}SCALER_SUM_DATA;

volatile SCALER_SUM_DATA scalerSumData;
MSG_Q_ID scalMsgQID;

/*******************************************************************************
*
* scalMsgInt - 
*
*  This routine can be modified to add the code the User wishes to execute
*  upon recieving a scaler interrupt
*
*  RETURNS: N/A
*
*/
void scalMsgInt (int arg)
{
  volatile unsigned long fifoEmpty = 0;
  register jj = 0;
  int ii, errFlag = 0, stat=0;
  unsigned long tmpData[33];

  /* Check if FIFO is empty and read N scaler values out of the fifo. The argument
     to the ISR defines the number of scaler reads (or active scaler channels)
     Keep reading N scalers from FIFO until FIFO is Empty or you loop 
     MAX_FIFO_ENTRIES/2 times 
  */

  fifoEmpty = (sfs->csr)&SCAL_STATUS_FIFO_EMPTY;
  do {
    if(fifoEmpty) {
      /* FIFO is empty */
      logMsg("scalMsgInt: FIFO Empty\n",0,0,0,0,0,0);
      break;
    }
    /* logMsg("scalMsgInt: Read %d scalers\n",arg,0,0,0,0,0); */
      
    for(ii=0;ii<arg;ii++) {
      tmpData[ii+1] = sfs->fifo[0];
      scalerSumData.sums[ii] = scalerSumData.sums[ii] + tmpData[ii+1];
    }
    msgCount++;
    scalerSumData.addr = (UINT32) sfs;
    scalerSumData.count = msgCount;
    tmpData[0] = msgCount;
    
    /* 
       Check the Message Queue. If it is not full then put scaler data on
       the queue, If it is full then we are screwed and must dump the scaler 
       message. So set an error flag and dump out of the ISR so we do not
       lose any more data */

    stat = msgQSend(scalMsgQID,(char *)tmpData,sizeof(tmpData),NO_WAIT,MSG_PRI_NORMAL);
    if(stat != 0) {
      errFlag = 1;
      scalerSumData.lost += 1;
      /* logMsg("scalMsgInt: ERROR: Scaler Read (%d) lost \n",msgCount,0,0,0,0,0); */
    }

    /* Check FIFO Status again */
    fifoEmpty = (sfs->csr)&SCAL_STATUS_FIFO_EMPTY;
    jj++;
 
  } while ((fifoEmpty == 0) && (jj <= MAX_FIFO_ENTRIES>>1) && (errFlag == 0));

}


/*******************************************************************************
*
* scalMsgInit - 
*
*  Initialize the Message Queue and Scaler Sum Data areas.
*  
*
*  RETURNS: 0 if Initialization fails otherwise it Returns
*           the message queue ID.
*
*/
UINT32 scalMsgInit(UINT32 addr, int nScal, int nMsg, int sizeMsg)
{
  int stat1;

  /* Intialize Scaler */
  if(sfs) {
    if((sfs->csr)&SCAL_ENABLE_IRQ_MASK) {
      printf("scalMsgInit: ERROR: Scaler is initialized and Enabled\n
                 Call scalIntDisable() first.\n");
      return(0);
    }
  }
  if(addr) {
    if((stat1 = scalIntInit(addr,0,0)) != 0) {
      printf("scalMsgInit: ERROR: scalIntInit(%d,0,0) failed\n",addr);
      return(0);
    }
  }

   
  /* Connect interrupt */
  maxScalers = (nScal == 0) ? NSCALERS : nScal;    /* use default of nScal = 0*/
  scalIntConnect((VOIDFUNCPTR)scalMsgInt,maxScalers);

  /* Create Message Queue */
  maxMsg  = (nMsg == 0) ? NMESSAGES : nMsg;
  maxMsgSize = (sizeMsg == 0) ? SIZEMESSAGES: sizeMsg;
  if(scalMsgQID != 0) {
    printf("scalMsgInit: WARN: Deleting old Message Queue and Creating New\n");
    msgQDelete(scalMsgQID);
  }
  scalMsgQID = msgQCreate(maxMsg,maxMsgSize,MSG_Q_FIFO);
  if(scalMsgQID == 0) {
    printf("scalMsgInit: ERROR: msgQCreate(%d,%d,0) failed\n",maxMsg,maxMsgSize);
    return(0);
  }

  /* Zero Structure for holding Scaler sums */
  bzero((char *)&scalerSumData, sizeof(scalerSumData));
  msgCount = 0;
  errMsgCount = 0;

  return((UINT32)scalMsgQID);  
}

/*******************************************************************************
*
* scalMsgPrint - Print scaler info to Standard out
*
*  RETURNS: N/A
*
*/
void scalMsgPrint(int mflag)
{
  int ii;

  msgQShow(scalMsgQID,mflag);

  printf("   Scaler Data (Sums)  for scaler at address = 0x%x\n",scalerSumData.addr);
  printf("     Partial Sum Count = %d  Partial Sums lost = %d\n",scalerSumData.count,scalerSumData.lost);
  for(ii=1;ii<=8;ii++) {
    printf(" SCAL%02d: %10d  SCAL%02d: %10d  SCAL%02d: %10d  SCAL%02d: %10d \n",
	   ii,scalerSumData.sums[ii-1],
           ii+8,scalerSumData.sums[ii+7],
           ii+16,scalerSumData.sums[ii+15],
           ii+24,scalerSumData.sums[ii+23]);
  }

}

/*******************************************************************************
*
* scalMsgEnable - Zero Counters , Enable Scaler interrupts
*
*  RETURNS: N/A
*
*/
STATUS scalMsgEnable(UINT32 mask)
{
  int ii, cnt, stat;
  UINT32 tmp[NSCALERS+1];

  if((sfs == NULL) || (scalMsgQID == 0)) {
    printf("scalMsgEnable: ERROR: call scalMsgInit() first...\n");
    return(ERROR);
  }

  if((sfs->csr)&SCAL_ENABLE_IRQ_MASK) {
    printf("scalMsgEnable: ERROR: Scaler is initialized and Enabled\n
                 Call scalIntDisable() first.\n");
    return(ERROR);
  }

  
  /* Zero Message Queue and counters */
  bzero((char *)&scalerSumData, sizeof(scalerSumData));
  msgCount = 0;
  errMsgCount = 0;

  if((cnt = msgQNumMsgs(scalMsgQID)) > 0) {
    for(ii=0;ii<cnt;ii++) {
      stat = msgQReceive(scalMsgQID,(char *)tmp,sizeof(tmp),WAIT_FOREVER);
      if(stat != 0) {
	printf("scalMsgEnable: ERROR: msgQReceive() failed\n");
	return(ERROR);
      }
    }
  }

  /* Enable Interrupts */
  scalIntEnable(mask);

  return(OK);
}

/*******************************************************************************
*
* scalMsgRead - Read Message from Scaler Queue. Throw data away.
*               if rFlag = 1   Print number of messages read from the Queue
*                  rFlag > 1   Print out contents of each message.
*
*  RETURNS: ERROR if read fails or Number of messges read from the queue.
*
*/
int scalMsgRead(int rFlag)
{
  int ii, jj, cnt, stat;
  UINT32 tmp[NSCALERS+1];

  if(scalMsgQID == 0) {
    printf("scalMsgRead: ERROR: Scaler Message Queue not Available\n");
    return(ERROR);
  }

  if((cnt = msgQNumMsgs(scalMsgQID)) > 0) {
    for(ii=0;ii<cnt;ii++) {
      stat = msgQReceive(scalMsgQID,(char *)tmp,sizeof(tmp),WAIT_FOREVER);
      if(stat < 0) {
	printf("scalMsgRead: ERROR: msgQReceive() failed (stat = %d)\n",stat);
	return(ERROR);
      }
      if(rFlag > 1) {
	printf("Scaler Event Number %d\n",tmp[0]);
	for (jj=1;jj<=NSCALERS;jj+=8) {
	  printf("      %08x %08x %08x %08x %08x %08x %08x %08x\n",
		 tmp[jj],tmp[jj+1],tmp[jj+2],tmp[jj+3],
                 tmp[jj+4],tmp[jj+5],tmp[jj+6],tmp[jj+7]);
	}
      }
    }
  }

  if(rFlag == 1) printf("scalMsgRead: Read %d messages\n",cnt);

  return(cnt);
}



