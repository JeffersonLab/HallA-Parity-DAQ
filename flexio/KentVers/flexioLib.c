/******************************************************************************
 *
 *  flexioLib.c   -  Driver library header file for readout of a TJNAF
 *                   Flexible Input/Output Module using a VxWorks 5.2
 *                   or later based single board computer.
 *
 *  Author: Paul King
 *          University of Maryland / TJNAF G0 Collaboration
 *          March 2001
 *
 *  Revision 1.0 - Initial revision.
 */

#include "vxWorks.h"
#include "stdio.h"
#include "logLib.h"
#include "vxLib.h"
//#include "string.h"
//#include "taskLib.h"
//#include "intLib.h"

/* Include flexIO definitions */
#include "flexioLib.h"

IMPORT  STATUS sysBusToLocalAdrs(int, char *, char **);

volatile struct flexio_struct *flexp;


/******************************************************************************
* flexioInit - Initialize Scale 32 library. Disable/Clear Counters
*
*
* RETURNS: OK, or ERROR if the scaler address is invalid.
*/

STATUS flexioInit (UINT32 addr)
{
  int res, errFlag = 0;
  int idblock1, idblock2;
  unsigned long laddr;

  /* get the FlexIO's address */
  res = sysBusToLocalAdrs(0x29,(char *) addr,(char **) &laddr);
  if (res != 0) {
      printf("scale32Init: ERROR in sysBusToLocalAdrs(0x29,0x%x,&laddr) \n",
	     addr);
      return(ERROR);
  } else {
    flexp = (struct flexio_struct *) laddr;
    /* Check if FlexIO Board exists at that address, and send reset */
    res = vxMemProbe((char *) flexp,VX_WRITE,2,(char *) FLEXIO_CSR_RESET);
    if(res < 0) {
      printf("flexioInit: ERROR: No addressable board at addr=0x%x\n",
	     (UINT32) flexp);
      flexp = NULL;
      errFlag=1;
    } else {
      flexp->csr1 = FLEXIO_CSR_RESET;
      if(flexioDebug){
	printf("flexioInit: INFO: flexp->csr1 = 0x%04x \n", flexp->csr1);
	printf("flexioInit: INFO: flexp->csr2 = 0x%04x \n", flexp->csr2);    
      }
      printf("Initialized FlexIO at address 0x%08x \n",(UINT32) flexp);
    }
  }  

  flexio_input_card = 0;
  flexio_output_card = 0;
  idblock1 = 0;
  idblock2 = 0;
  idblock1 = (flexp->csr1 & FLEXIO_CSR_ID_MASK)>>FLEXIO_CSR_ID_OFFSET;
  idblock2 = (flexp->csr2 & FLEXIO_CSR_ID_MASK)>>FLEXIO_CSR_ID_OFFSET;    

  if (idblock1 == FLEXIO_ID_EMPTY) {
    printf("flexioInit: INFO: port number 1 is empty.\n");
  }
  if (idblock2 == FLEXIO_ID_EMPTY) {
    printf("flexioInit: INFO: port number 2 is empty.\n");
  }

  if (idblock1 == FLEXIO_ID_INPUT) {
    flexio_input_card = 1;
    if (idblock2 == FLEXIO_ID_INPUT) {
      printf("flexioInit: INFO: 2 input cards.  Standard READ  call addresses card 1 only\n");
    }
  } else if (idblock2 == FLEXIO_ID_INPUT) {
    flexio_input_card = 2;
  } else {
    printf("flexioInit: INFO: No input cards. Standard READ call will fail.\n");
  }

  if (idblock1 == FLEXIO_ID_OUTPUT) {
    flexio_output_card = 1;
    if (idblock2 == FLEXIO_ID_OUTPUT) {
      printf("flexioInit: INFO: 2 output cards.  Standard WRITE  call addresses card 1 only\n");
    }
  } else if (idblock2 == FLEXIO_ID_OUTPUT) {
    flexio_output_card = 2;
  } else {
    printf("flexioInit: INFO: No output cards. Standard WRITE  call will fail.\n");
  }
    
  flexioSetDebug(0); // no verbose required by DAQ, I think.

  /* Print where we are */ 
  flexioPrintID(1);
  flexioPrintID(2);

  if(errFlag > 0) {
    printf("flexioInit: ERROR: Unable to initialize FlexIO \n");
    return(ERROR);
  } else {
    return(OK);
  }
}

/******************************************************************************
 *  flexioReset
 */
void flexioReset(){
  flexp->csr1 = FLEXIO_CSR_RESET;
  flexp->csr2 = FLEXIO_CSR_RESET;
}

/******************************************************************************
 *  flexioPrintStatus
 */
void flexioPrintStatus()
{
  printf("flexioPrintStatus: INFO: flexp->csr1      = 0x%04x\n",flexp->csr1);
  flexioPrintID(1);
  printf("flexioPrintStatus: INFO: flexp->csr2      = 0x%04x\n",flexp->csr2);
  flexioPrintID(2);
  printf("flexioPrintStatus: INFO: flexp->data1     = 0x%04x\n",flexp->data1);
  printf("flexioPrintStatus: INFO: flexp->data2     = 0x%04x\n",flexp->data2);
  printf("flexioPrintStatus: INFO: flexp->interrupt = 0x%04x\n",
	 flexp->interrupt);
}

/******************************************************************************
 *  flexioPrintID
 */
void flexioPrintID(int portnum)
{
  int idblock;

  idblock = 0;
  switch (portnum){
  case 1:
    idblock = (flexp->csr1 & FLEXIO_CSR_ID_MASK)>>FLEXIO_CSR_ID_OFFSET;
    break;
  case 2:
    idblock = (flexp->csr2 & FLEXIO_CSR_ID_MASK)>>FLEXIO_CSR_ID_OFFSET;    
    break;
  default:
    printf("flexioPrintID: ERROR: port # %d is invalid, must be 1 or 2 \n",
	   portnum);
    return;
    break;
  }
  switch (idblock){
  case FLEXIO_ID_EMPTY:
    printf("flexioPrintID: INFO: port number %d is empty.\n",
	   portnum);
    break;
  case FLEXIO_ID_INPUT:
    printf("flexioPrintID: INFO: port number %d is an input card.\n",
	   portnum);
    break;
  case FLEXIO_ID_OUTPUT:
    printf("flexioPrintID: INFO: port number %d is an output card.\n",
	   portnum);
    break;
  default:
    printf("flexioPrintID: ERROR: port number %d does not have a valid ID.\n",
	   portnum);
    return;
    break;
  }

}

/******************************************************************************
 *  flexioCheckID
 */
STATUS flexioCheckID(int portnum, int porttype)
{
  int idblock;

  idblock = 0;
  switch (portnum){
  case 1:
    if(flexioDebug)
      printf("flexioCheckID: INFO: flexp->csr1 = 0x%04x\n",flexp->csr1);
    idblock = (flexp->csr1 & FLEXIO_CSR_ID_MASK)>>FLEXIO_CSR_ID_OFFSET;
    break;
  case 2:
    if(flexioDebug)
      printf("flexioCheckID: INFO: flexp->csr2 = 0x%04x\n",flexp->csr2);
    idblock = (flexp->csr2 & FLEXIO_CSR_ID_MASK)>>FLEXIO_CSR_ID_OFFSET;    
    break;
  default:
    if (flexioDebug) 
      printf("flexioCheckID: ERROR: port # %d is invalid, must be 1 or 2 \n",
	   portnum);
    return(ERROR);
    break;
  }
  
}

/******************************************************************************
 *  flexioReadData
 */
int flexioReadData()
{
  int datablock;
  int portnum;

  if(flexp == NULL) {
    if (flexioDebug) 
      if (flexioDebug) 
	printf("flexioReadData: ERROR : Flex IO board not initialized \n");
    return(-1);
  }
  if(flexio_input_card==0) {
    if (flexioDebug) 
      printf("flexioReadData: ERROR : Flex IO board has no input card\n");
    return(-1);
  }
  portnum = flexio_input_card;

  return(flexioReadDataCard(portnum));

}

/******************************************************************************
 *  flexioReadDataCard
 */
int flexioReadDataCard(int portnum)
{
  int datablock;

  if(flexp == NULL) {
    if (flexioDebug) 
      printf("flexioReadDataCard: ERROR : Flex IO board not initialized \n");
    return(-1);
  }

  datablock = 0;
  switch (portnum){
  case 1:
    if(flexioDebug)
      printf("flexioReadDataCard: INFO: flexp->data1 = 0x%04x\n",flexp->data1);
    datablock = flexp->data1;
    if (flexioCheckID(portnum,FLEXIO_ID_INPUT)==OK &&
	flexioIsExtLatch(portnum)==OK) flexioEnableInputLatch(portnum);
    break;
  case 2:
    if(flexioDebug)
      printf("flexioReadDataCard: INFO: flexp->data2 = 0x%04x\n",flexp->data2);
    datablock = flexp->data2;
    if (flexioCheckID(portnum,FLEXIO_ID_INPUT)==OK &&
	flexioIsExtLatch(portnum)==OK) flexioEnableInputLatch(portnum);
    break;
  default:
    if (flexioDebug) 
      printf("flexioReadDataCard: ERROR: port number %d is invalid, must be 1 or 2 \n",
	   portnum);
    break;
  }

  return(datablock);
}

/******************************************************************************
 *  flexioWriteData
 */
int flexioWriteData(int writedata)
{
  int readdata;
  int portnum;

  if(flexp == NULL) {
    if (flexioDebug) printf("flexioWriteData: ERROR : Flex IO board not initialized \n");
    return(-1);
  }
  if(flexio_output_card==0) {
    if (flexioDebug) printf("flexioReadData: ERROR : Flex IO board has no output card\n");
    return(-1);
  }
  portnum = flexio_output_card;

  return( flexioWriteDataCard(portnum, writedata) );

}

/******************************************************************************
 *  flexioGetOutput
 */
int flexioGetOutput()
{
  int readdata;
  int portnum;

  
  if(flexp == NULL) {
    if (flexioDebug) printf("flexioGetOutput: ERROR : Flex IO board not initialized \n");
    return(-1);
  }
  if(flexio_output_card==0) {
    if (flexioDebug) printf("flexioGetOutput: ERROR : Flex IO board has no output card\n");
    return(-1);
  }
  portnum = flexio_output_card;
  readdata = flexioReadDataCard(portnum);

  return( readdata );

}

/******************************************************************************
 *  flexioWriteDataCard
 */
int flexioWriteDataCard(int portnum, int writedata)
{
  int readdata;

  if(flexp == NULL) {
    if (flexioDebug) 
      printf("flexioWriteDataCard: ERROR : Flex IO board not initialized \n");
    return(-1);
  }

  if(flexioCheckID(portnum,FLEXIO_ID_OUTPUT)==ERROR) {
    if (flexioDebug) 
      printf("flexioWriteDataCard: ERROR : Port %d is not an output port.\n",
	   portnum);
    return(-1);
  }
  
  if(flexioDebug)
    printf("flexioWriteDataCard: INFO: Writing 0x%04x to port %d\n",
	   writedata, portnum);
 
  switch (portnum){
  case 1:
    flexp->data1 = writedata;
    break;
  case 2:
    flexp->data2 = writedata;
    break;
  default:
    if (flexioDebug) 
      printf("flexioWriteDataCard: ERROR: port # %d is invalid, must be 1 or 2 \n",
	   portnum);
    break;
  }

  readdata = flexioReadDataCard(portnum);

  return(readdata);
}

/******************************************************************************
 *  flexioToggleData
 */
int flexioToggleData( int bitpattern)
{
  int portnum;
  int writedata;

  if(flexp == NULL) {
    if (flexioDebug) printf("flexioToggleData: ERROR : Flex IO board not initialized \n");
    return(-1);
  }
  if(flexio_output_card==0) {
    if (flexioDebug) printf("flexioToggleData: ERROR : Flex IO board has no output card\n");
    return(-1);
  }
  portnum = flexio_output_card;

  return( flexioToggleDataCard(portnum, bitpattern) );

}

/******************************************************************************
 *  flexioToggleDataCard
 */
int flexioToggleDataCard(int portnum, int bitpattern)
{
  int readdata;
  int writedata;

  if(flexp == NULL) {
    if (flexioDebug) 
      printf("flexioToggleData: ERROR : Flex IO board not initialized \n");
    return(-1);
  }

  if(flexioCheckID(portnum,FLEXIO_ID_OUTPUT)==ERROR) {
    if (flexioDebug) 
      printf("flexioToggleData: ERROR : Port %d is not an output port.\n",
	   portnum);
    return(-1);
  } else {
    readdata   = flexioReadDataCard(portnum);
    //    writedata  = (readdata | bitpattern) - (readdata & bitpattern);
    writedata  = (readdata ^ bitpattern);
    readdata   = flexioWriteDataCard(portnum,writedata);
    
    return(readdata);
  }
}

/******************************************************************************
 *  flexioGetOutputBit
 */
int flexioGetOutputBit(int bitloc)
{
  int portnum;

  if(flexp == NULL) {
    if (flexioDebug) printf("flexioGetOutputBit: ERROR : Flex IO board not initialized \n");
    return(-1);
  }
  if(flexio_output_card==0) {
    if (flexioDebug) printf("flexioGetOutputBit: ERROR : Flex IO board has no output card\n");
    return(-1);
  }
  portnum = flexio_output_card;

  return( flexioGetBitCard(portnum, bitloc) );

}

/******************************************************************************
 *  flexioGetBitCard
 */
int flexioGetBitCard(int portnum, int bitloc)
{
  int readdata;
  int readbit;
  int bitmask;

  bitmask = 0x1 << (bitloc-1);

  if(flexp == NULL) {
    if (flexioDebug) 
      printf("flexioGetOutputBit: ERROR : Flex IO board not initialized \n");
    return(-1);
  }

  readdata   = flexioReadDataCard(portnum);
  readbit = (readdata & bitmask) >> (bitloc-1);
    
  return(readbit);
}



/******************************************************************************
 *  flexioWriteChan
 */
int flexioWriteChan(int newbit, int chan)
{
  int portnum;
  int mask;

  if(flexp == NULL) {
    if (flexioDebug) printf("flexioWriteMask: ERROR : Flex IO board not initialized \n");
    return(-1);
  }
  if(flexio_output_card==0) {
    if (flexioDebug) printf("flexioWriteMask: ERROR : Flex IO board has no output card\n");
    return(-1);
  }
  mask = 0;
  //if (chan>0 && chan<=16)  mask = pow(2,chan-1); Bryan Moffit spotted a roblem with using pow(#,#). pow(#,#) always returned 33! See HALOG Entry Num 285859 for details. 

  if (chan>0 && chan<=16)  mask = 1<<(chan-1);    
  
  portnum = flexio_output_card;

  return( flexioWriteMaskCard(portnum, newbit, mask) );

}

/******************************************************************************
 *  flexioWriteMask
 */
int flexioWriteMask(int newbit, int bitpattern)
{
  int portnum;
  int writedata;

  if(flexp == NULL) {
    if (flexioDebug) printf("flexioWriteMask: ERROR : Flex IO board not initialized \n");
    return(-1);
  }
  if(flexio_output_card==0) {
    if (flexioDebug) printf("flexioWriteMask: ERROR : Flex IO board has no output card\n");
    return(-1);
  }
  portnum = flexio_output_card;

  return( flexioWriteMaskCard(portnum, newbit, bitpattern) );

}

/******************************************************************************
 *  flexioWriteMaskCard
 */
int flexioWriteMaskCard(int portnum, int newbit, int bitpattern)
{
  int readdata;
  int writedata;
  int compmask;

  if(flexp == NULL) {
    if (flexioDebug) 
      printf("flexioWriteMask: ERROR : Flex IO board not initialized \n");
    return(-1);
  }

  if(flexioCheckID(portnum,FLEXIO_ID_OUTPUT)==ERROR) {
    if (flexioDebug) 
      printf("flexioWriteMask: ERROR : Port %d is not an output port.\n",
	   portnum);
    return(-1);
  }

  readdata   = flexioReadDataCard(portnum);
  if (newbit) {
    writedata = readdata | bitpattern;
  } else {
    compmask = 0xff ^ bitpattern;
    writedata = readdata & compmask;
  }
  
  readdata   = flexioWriteDataCard(portnum,writedata);
  
  return(readdata);

}


/******************************************************************************
 *  flexioSetDebug
 *    simply sets vxworks flag for output from otherwise quiet routines
 *    some routines must be quiet by default for DAQ
 */
STATUS flexioSetDebug(int flag)
{
  if (flag==1) {
    flexioDebug=1;
  }  else {
    flexioDebug=0;
  }
  return(flexioDebug);
}


/******************************************************************************
 *  flexioSetOutputMode
 */
STATUS flexioSetOutputMode(int portnum, int outputmode)
{
  if(flexp == NULL) {
    printf("flexioSetOutputMode: ERROR : Flex IO board not initialized \n");
    return(ERROR);
  }
  if(flexioCheckID(portnum,FLEXIO_ID_OUTPUT)==ERROR) {
    printf("flexioSetOutputMode: ERROR : Port %d is not an output port.\n",
	   portnum);
    return(ERROR);
  }
  return(OK);
}



/******************************************************************************
 *  flexioEnableInputLatch
 */
STATUS flexioEnableInputLatch(int portnum)
{
  int localmask;

  if(flexp == NULL) {
    printf("flexioIsExtLatch: ERROR : Flex IO board not initialized \n");
    return(ERROR);
  }
  if(flexioCheckID(portnum,FLEXIO_ID_INPUT)==ERROR) {
    printf("flexioIsExtLatch: ERROR : Port %d is not an input port.\n",
	   portnum);
    return(ERROR);
  }

  localmask = FLEXIO_CSR_IN_STROBE | FLEXIO_CSR_IN_LATCH |
    FLEXIO_CSR_IN_ACT_STR;
  if (flexioSetCSRMask(portnum,localmask)==ERROR){
    return(ERROR);
  } else {
    return( flexioIsExtLatch(portnum) );
  }
}

/******************************************************************************
 *  flexioIsExtLatch
 */
STATUS flexioIsExtLatch(int portnum)
{
  int localmask;

  if(flexp == NULL) {
    printf("flexioIsExtLatch: ERROR : Flex IO board not initialized \n");
    return(ERROR);
  }
  if(flexioCheckID(portnum,FLEXIO_ID_INPUT)==ERROR) {
    printf("flexioIsExtLatch: ERROR : Port %d is not an input port.\n",
	   portnum);
    return(ERROR);
  }

  localmask = FLEXIO_CSR_IN_STROBE | FLEXIO_CSR_IN_LATCH;
  if (flexioCheckCSRMask(portnum,localmask)==ERROR){
    return(ERROR);
  } else {
    return(OK);
  }
}

/******************************************************************************
 *  flexioCheckCSRMask
 */
STATUS flexioCheckCSRMask(int portnum, int csrmask)
{
  int isamatch;

  if(flexp == NULL) {
    printf("flexioCheckCSRMask: ERROR : Flex IO board not initialized \n");
    return(ERROR);
  }

  isamatch = FALSE;
  switch (portnum){
  case 1:
    if ( (flexp->csr1 & csrmask) == csrmask)  isamatch = TRUE;
    break;
  case 2:
    if ( (flexp->csr2 & csrmask) == csrmask)  isamatch = TRUE;
    break;
  default:
    printf("flexioCheckCSRMask: ERROR: port # %d is invalid, must be 1 or 2 \n",
	   portnum);
    break;
  }

  if (isamatch==TRUE){
    return(OK);
  } else {
    return(ERROR);
  }
}

/******************************************************************************
 *  flexioSetCSRMask
 */
STATUS flexioSetCSRMask(int portnum, int csrmask)
{
  if(flexp == NULL) {
    printf("flexioSetCSRMask: ERROR : Flex IO board not initialized \n");
    return(ERROR);
  }

  switch (portnum){
  case 1:
    flexp->csr1 = csrmask;
    break;
  case 2:
    flexp->csr2 = csrmask;
    break;
  default:
    printf("flexioSetCSRMask: ERROR: port # %d is invalid, must be 1 or 2 \n",
	   portnum);
    break;
  }

  if (flexioCheckCSRMask(portnum,csrmask)==ERROR){
    return(ERROR);
  } else {
    return(OK);
  }
}


/******************************************************************************
 *  flexio
 */
//STATUS flexio
//{
//}

/******************************************************************************
 */

