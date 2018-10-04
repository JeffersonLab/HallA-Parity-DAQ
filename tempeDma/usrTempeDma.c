/*************************************************
 *
 *    User routines for running Tempe DMA engine
 *
 *
 *
 *
 */

#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <logLib.h>

/* Includes for sysTempeDma.c */
#include <semLib.h>
#include <cacheLib.h>
#include <iv.h>

/* timing include
#include "ppcTimer.h"
PPCTB tb1, tb2, tb3, tb4, tb5, tb6;
*/

/* Include tempe chip defines */
#include "tempe.h"

/* 64-bit address support (Tempe) */

#define MAKE64(high32,low32) (((UINT64)high32<<32)+((UINT64)low32))
#define UNMAKE64(val64,high32,low32) { high32 = (val64>>32) & 0xffffffff; \
				       low32  = (val64) & 0xffffffff; }

/* Include Base tempe DMA routines */
#include "sysTempeDma.c"


/* default settings */

LOCAL TEMPE_DMA_DESCRIPTOR dmaDescSample =
{
  0x08000000,        /* Source Address 'sourceAddr' */
  0x81000000,        /* Destination Address 'destAddr' */
  0x00010000,        /* Number of bytes to transfer 'byteCount' */
  0,		     /* 2eSST Broadcast select 'bcastSelect2esst' */

    /* Bus Attribute Structure if the VME Bus is the SOURCE of the DMA */
    {
      VME_DMA_VME,   /* Bus 'srcVmeAttr.bus' */
      VME_SST160,    /* 2eSST rate 'srcVmeAttr.vme2esstRate' */
      VME_MODE_A32,  /* address mode 'srcVmeAttr.addressMode' */
      VME_D32,       /* data width 'srcVmeAttr.dataWidth' */
      VME_BLT_OUT,   /* xfer out protocol 'srcVmeAttr.xferProtocolOut' */
      FALSE,         /* supervisor access type 'srcVmeAttr.superAccessType'*/
      TRUE	     /* program access type 'srcVmeAttr.pgmAccessType' */
    },

    /* Bus Attribute Structure if the PCI Bus is the DESTINATION of the DMA */
    {
      VME_DMA_PCI,   /* Bus 'dstVmeAttr.bus' */
      VME_SST160,    /* 2eSST rate 'dstVmeAttr.vme2esstRate' */
      VME_MODE_A32,  /* address mode 'dstVmeAttr.addressMode' */
      VME_D32,       /* data width 'dstVmeAttr.dataWidth' */
      VME_BLT_OUT,   /* xfer out protocol 'dstVmeAttr.xferProtocolOut' */
      FALSE,         /* supervisor access type 'dstVmeAttr.superAccessType'*/
      TRUE	     /* program access type 'dstVmeAttr.pgmAccessType' */
    },

  NULL	             /* Next descriptor if using Linked List mode 'next' */
};


TEMPE_DMA_ATTRIBUTES dmaAttrSample =
{
  {
    4096,	 /* PCI bus max block size 'busUsg.maxPciBlockSize' */
    0,	         /* PCI bus back-off timer (usec) 'busUsg.pciBackOffTimer' */
    2048,	 /* VME bus max block size 'busUsg.maxVmeBlockSize' */
    0,	         /* VME bus back-off timer 'busUsg.vmeBackOffTimer' */
    TRUE,        /* VME flush on aborted read? 'busUsg.vmeFlshOnAbtRead' */
    TRUE         /* PCI flush on aborted read? 'busUsg.pciFlshOnAbtRead' */
  },
  NULL	 /* User-defined completion routine 'userHandler' */
};


/* temporary just to play with srcVmeAttr.bus parameter */
void
usrVmeDmaConfig1(UINT32 busattr)
{
  if(busattr==0)      dmaDescSample.srcVmeAttr.bus = (VME_DMA_VME);
  else if(busattr==1) dmaDescSample.srcVmeAttr.bus = (VME_DMA_PCI);
  else if(busattr==2) dmaDescSample.srcVmeAttr.bus = (VME_DMA_PATTERN_WORD_INCREMENT);
  else if(busattr==3) dmaDescSample.srcVmeAttr.bus = (VME_DMA_PATTERN_WORD_NOINCREMENT);
  else if(busattr==4) dmaDescSample.srcVmeAttr.bus = (VME_DMA_PATTERN_BYTE_INCREMENT);
  else if(busattr==5) dmaDescSample.srcVmeAttr.bus = (VME_DMA_PATTERN_BYTE_NOINCREMENT);
}

STATUS
usrVmeDmaInit(void)
{
  return(OK);
}

UINT32
usrTempeReadExcAdr()
{
  return(TEMPE_READ32(TEMPE_REG_BASE,TEMPE_VEAL));

}

UINT32
usrTempeReadDctl(int chan)
{
  if(chan==1)
    return(TEMPE_READ32(TEMPE_REG_BASE,TEMPE_DCTL1));
  else
    return(TEMPE_READ32(TEMPE_REG_BASE,TEMPE_DCTL0));
}


UINT32
usrTempeExcStatus()
{
  return(TEMPE_READ32(TEMPE_REG_BASE,TEMPE_VEAT));

}

void
usrTempeClearExc()
{

  TEMPE_WRITE32_PUSH(TEMPE_REG_BASE,TEMPE_VEAT,0x20000000);
  TEMPE_WRITE32_PUSH(TEMPE_REG_BASE,TEMPE_VEAL,0);


}


/* Print Out Program/Status info on the Tempe DMA Chip */


UINT32
usrTempeDmaStatus (UINT32 chan, int sflag)
{

  unsigned int excStatus = 0;
  unsigned int excAddrL  = 0;
  unsigned int dmastat=0;
  unsigned int dma_ctl=0;

  TEMPE_DMA_STATUS dmaStatus; /* Current status of channel */
  
  if (chan < TEMPE_DMA_CHANNEL_COUNT) {
    sysTempeDmaStatus(chan, &dmaStatus);
    dmastat = dmaStatus.dstax;
    excStatus = TEMPE_READ32(TEMPE_REG_BASE,TEMPE_VEAT);
    excAddrL  = TEMPE_READ32(TEMPE_REG_BASE,TEMPE_VEAL);
    dma_ctl   = usrTempeReadDctl(chan);
  } else {
    logMsg("ERROR: invalid channel\n",0,0,0,0,0,0);
    return(ERROR);
  }

  if(sflag) { /* Print out Status Info in readable form */
    printf("DMA Chan %d Status = 0x%08x\n",chan,dmastat);

    printf("  Source(VME) address: 0x%08x \n",dmaStatus.dcsaLx);
    printf("  Destination address: 0x%08x \n",dmaStatus.dcdaLx);
    printf("  Byte Count         : 0x%08x \n",dmaStatus.dcntx);
    printf("  Execption Status   : 0x%08x \n",excStatus);
    printf("  Bus Error   address: 0x%08x \n",excAddrL);
    printf("  DMA Control Reg    : 0x%08x \n",dma_ctl);
    printf("  VME Address Mode : %d (%s)\n",dmaDescSample.srcVmeAttr.addressMode,
	   tempeAddrModeStr[dmaDescSample.srcVmeAttr.addressMode]);
    printf("  VME Data Width   : %d (%s)\n",dmaDescSample.srcVmeAttr.dataWidth,
	   tempeDataWidthStr[dmaDescSample.srcVmeAttr.dataWidth]);
    if(dmaDescSample.srcVmeAttr.xferProtocolOut == VME_2eSST_OUT) {
      printf("  VME Transfer Mode: %d (%s - %s)\n",dmaDescSample.srcVmeAttr.xferProtocolOut,
	     tempeProtocolStr[dmaDescSample.srcVmeAttr.xferProtocolOut],
	     tempeSSTRateStr[dmaDescSample.srcVmeAttr.vme2esstRate]);
    }else{
      printf("  VME Transfer Mode: %d (%s)\n",dmaDescSample.srcVmeAttr.xferProtocolOut,
	     tempeProtocolStr[dmaDescSample.srcVmeAttr.xferProtocolOut]);
    }

    printf("  STATUS of Last Transfer:\n");
    switch(dmastat&0x1f000000) {

    case (TEMPE_DSTAx_ERR_MASK):
     printf("   Error Bit Set\n");

     if(dmastat&TEMPE_DSTAx_ERRS_MASK) {
       printf("     Source: PCI Bus\n");
     }else{
       printf("     Source: VME Bus\n");
     }

     break;
    case (TEMPE_DSTAx_ABT_MASK):
     printf("   Abort Bit Set\n");
     
     break;
    case (TEMPE_DSTAx_PAU_MASK):
     printf("   Pause Bit Set\n");
     
     break;
    case (TEMPE_DSTAx_DON_MASK):
     printf("   Done Bit Set\n");
     
     break;
    case (TEMPE_DSTAx_BSY_MASK):
     printf("   Busy Bit Set\n");
     
     break;
    default:
     printf("   NO Transfer Completed\n");

    }

    /*  ppcTimeBaseDuration(&tb5,&tb6); */

  }
  
  return (dmastat);
}


/* Function to change VME Bus attributes for the DMA transfer */

STATUS
usrVmeDmaConfig(UINT32 addrType, UINT32 dataType, UINT32 sstMode)
{


  if(addrType == VME_MODE_A16) {
    dmaDescSample.srcVmeAttr.addressMode = VME_MODE_A16;
    logMsg("VME DMA Address  Mode set for A16\n",0,0,0,0,0,0);
  } else if(addrType == VME_MODE_A24) {
    dmaDescSample.srcVmeAttr.addressMode = VME_MODE_A24;
    logMsg("VME DMA Address  Mode set for A24\n",0,0,0,0,0,0);
  } else if(addrType == VME_MODE_A32) {
    dmaDescSample.srcVmeAttr.addressMode = VME_MODE_A32;
    logMsg("VME DMA Address  Mode set for A32\n",0,0,0,0,0,0);
  } else {
    logMsg("Address Mode addrType=%d is not supported\n",addrType,0,0,0,0,0);
    return(ERROR);
  }

  if(dataType == 0)
  {
    dmaDescSample.srcVmeAttr.dataWidth = VME_D16;
    dmaDescSample.srcVmeAttr.xferProtocolOut = VME_SCT_OUT;
    logMsg("VME DMA Transfer Mode set for D16 single cycle\n",0,0,0,0,0,0);
  }
  else if(dataType == 1)
  {
    dmaDescSample.srcVmeAttr.dataWidth = VME_D32;
    dmaDescSample.srcVmeAttr.xferProtocolOut = VME_SCT_OUT;
    logMsg("VME DMA Transfer Mode set for D32 single cycle\n",0,0,0,0,0,0);
  }
  else if(dataType == 2)
  {
    dmaDescSample.srcVmeAttr.dataWidth = VME_D32;
    dmaDescSample.srcVmeAttr.xferProtocolOut = VME_BLT_OUT;
    logMsg("VME DMA Transfer Mode set for D32 block mode (BLT)\n",0,0,0,0,0,0);
  }
  else if(dataType == 3)
  {
    /* VME_D64 by default */
    dmaDescSample.srcVmeAttr.xferProtocolOut = VME_MBLT_OUT;
    logMsg("VME DMA Transfer Mode set for D64 block mode (MBLT)\n",0,0,0,0,0,0);
  }
  else if(dataType == 4)
  {
    /* VME_D64 by default */
    dmaDescSample.srcVmeAttr.xferProtocolOut = VME_2eVME_OUT;
    logMsg("VME DMA Transfer Mode set for D64 2e block mode\n",0,0,0,0,0,0);
  }
  else if(dataType == 5)
  {
    /* VME_D64 by default */
    dmaDescSample.srcVmeAttr.xferProtocolOut = VME_2eSST_OUT;
    /* Set 2eSST Transfer Mode */
    if(sstMode == VME_SST267) {
      dmaDescSample.srcVmeAttr.vme2esstRate = VME_SST267;
      logMsg("VME DMA Transfer Mode set for D64 2eSST (267MB/s) mode\n",0,0,0,0,0,0);
    } else if(sstMode == VME_SST320) {
      dmaDescSample.srcVmeAttr.vme2esstRate = VME_SST320;
      logMsg("VME DMA Transfer Mode set for D64 2eSST (320MB/s) mode\n",0,0,0,0,0,0);
    } else {
      dmaDescSample.srcVmeAttr.vme2esstRate = VME_SST160;
      logMsg("VME DMA Transfer Mode set for D64 2eSST (160MB/s) mode\n",0,0,0,0,0,0);
    }
  }
  else
  {
    logMsg("Transfer Mode dataType=%d is not supported\n",dataType,0,0,0,0,0);
    return(ERROR);
  }


  return(OK);
}


/******************************************************************************
*
* usrVme2MemDmaStart - starts copying data from VMEbus memory to local one
*/

STATUS
usrVme2MemDmaStart(int chan, UINT32 localVmeAddr, UINT32 localAddr, UINT32 nbytes)
{

  TEMPE_DMA_DESCRIPTOR dmaDesc; /* descriptor */
  TEMPE_DMA_ATTRIBUTES dmaAttr; /* attributes */
  TEMPE_DMA_STATUS     dmaStat; /* DMA status */

  /* Setup Descriptor */
  dmaDesc = dmaDescSample;
  dmaDesc.sourceAddr = localVmeAddr;  /* map lower 32 bit addr onto 64 bit field */
  dmaDesc.destAddr = localAddr;       /* map lower 32 bit addr onto 64 bit field */
  dmaDesc.byteCount = nbytes;

  /* Setup Attributes */
  dmaAttr = dmaAttrSample;
  dmaAttr.userHandler = (INT_HANDLER) WAIT_FOREVER; /* Don't return till DMA done */

  /* Clear VME Exception Status */
  TEMPE_WRITE32_PUSH(TEMPE_REG_BASE,TEMPE_VEAT,0x20000000)

  /* GO */
    /*  logMsg("Start DMA (src, dst, cnt): 0x%08x 0x%08x %\n",
	 (UINT32) localVmeAddr,
	 (UINT32) localAddr,
	 (UINT32) nbytes,0,0,0); */

  if(sysTempeDmaStart(chan, &dmaDesc, &dmaAttr, &dmaStat) == ERROR)
  {
    logMsg("DMA ERROR: channel %d : Start error\n",chan,0,0,0,0,0);
    return(ERROR);
  }

  /*
  ppcTimeBaseDuration(&tb1,&tb2);
  ppcTimeBaseDuration(&tb2,&tb3);
  ppcTimeBaseDuration(&tb3,&tb4);
  */

  return(OK);
}

/******************************************************************************
*
* usrMem2VmeDmaStart - starts copying data from local memory to VMEBus memory
*/

STATUS
usrMem2VmeDmaStart(int chan, UINT32 localVmeAddr, UINT32 localAddr, UINT32 nbytes)
{

  TEMPE_DMA_DESCRIPTOR dmaDesc; /* descriptor */
  TEMPE_DMA_ATTRIBUTES dmaAttr; /* attributes */
  TEMPE_DMA_STATUS     dmaStat; /* DMA status */
  addressMode_t tempAddrMode;

  /* Setup Descriptor */
  dmaDesc = dmaDescSample;
  /* Define transfer direction */
  dmaDesc.srcVmeAttr.bus = VME_DMA_PCI;
  dmaDesc.dstVmeAttr.bus = VME_DMA_VME;
  /* Swap Defined Address Modes */
  tempAddrMode = dmaDesc.srcVmeAttr.addressMode;
  dmaDesc.srcVmeAttr.addressMode = dmaDesc.dstVmeAttr.addressMode;
  dmaDesc.dstVmeAttr.addressMode = tempAddrMode;
  /* Set Addresses */
  dmaDesc.sourceAddr = localAddr;  /* map lower 32 bit addr onto 64 bit field */
  dmaDesc.destAddr = localVmeAddr;       /* map lower 32 bit addr onto 64 bit field */
  dmaDesc.byteCount = nbytes;

  /* Setup Attributes */
  dmaAttr = dmaAttrSample;
  dmaAttr.userHandler = (INT_HANDLER) WAIT_FOREVER; /* Don't return till DMA done */

  /* Clear VME Exception Status */
  TEMPE_WRITE32_PUSH(TEMPE_REG_BASE,TEMPE_VEAT,0x20000000)

  /* GO */
    /*  logMsg("Start DMA (src, dst, cnt): 0x%08x 0x%08x %\n",
	 (UINT32) localVmeAddr,
	 (UINT32) localAddr,
	 (UINT32) nbytes,0,0,0); */

  if(sysTempeDmaStart(chan, &dmaDesc, &dmaAttr, &dmaStat) == ERROR)
  {
    logMsg("DMA ERROR: channel %d : Start error\n",chan,0,0,0,0,0);
    return(ERROR);
  }

  /*
  ppcTimeBaseDuration(&tb1,&tb2);
  ppcTimeBaseDuration(&tb2,&tb3);
  ppcTimeBaseDuration(&tb3,&tb4);
  */

  return(OK);
}




STATUS
usrTempeDmaDone(UINT32 chan)
{
  unsigned int excStatus = 0;
  unsigned int excAddrL  = 0;
  int byteCount = 0;

  TEMPE_DMA_STATUS dmaStatus; /* Current status of channel */

  
  if (chan >= TEMPE_DMA_CHANNEL_COUNT) {
    logMsg("ERROR: invalid channel\n",0,0,0,0,0,0);
    return(ERROR);
  }

  dmaStatus.dstax = 0;
  while(((dmaStatus.dstax)&(0x1e000000))==0)
  {
    sysTempeDmaStatus(chan, &dmaStatus);
  }
  /*
  The status is read from the Tempe chip at the time of the call to
  sysTempeDmaStatus().  Included in the status are values of the
  channels status register, current source address, current destination
  address, current link address, remaining byte count and an upper and
  lower timebase register value.
  */

  /*
    BOOL   dmaDone;    TRUE when DMA is done, successfully or with error 
    UINT32 dstax;	   Offset to Tempe status register 
    UINT32 dcsaUx;     Offset to Tempe Current Source Addr (upper) 
    UINT32 dcsaLx;     Offset to Tempe Current Source Addr (lower) 
    UINT32 dcdaUx;     Offset to Tempe Current Destination Addr (upper) 
    UINT32 dcdaLx;     Offset to Tempe Current Destination Addr (lower) 
    UINT32 dclaUx;	   Offset to Tempe Current Link Addr (upper) 
    UINT32 dclaLx;	   Offset to Tempe Current Link Addr (lower) 
    UINT32 dcntx;	   Offset to Tempe Count (byte) 
    UINT32 timeBaseU;  Timebase upper register 
    UINT32 timeBaseL;  Timebase lower register 
  */

  /*
  logMsg("dmaStatus: 0x%08x : Done:%d  Final Byte Count %d - Final Address (src, dest) 0x%08x 0x%08x\n",
	 dmaStatus.dstax, dmaStatus.dmaDone,dmaStatus.dcntx, dmaStatus.dcsaLx,dmaStatus.dcdaLx,0);
  */

  if(dmaStatus.dmaDone == 0) {  /* There was an Error */

    /* Check VME Exception Status and address and Clear*/
    excStatus = TEMPE_READ32(TEMPE_REG_BASE,TEMPE_VEAT);
    excAddrL  = TEMPE_READ32(TEMPE_REG_BASE,TEMPE_VEAL);
    TEMPE_WRITE32_PUSH(TEMPE_REG_BASE,TEMPE_VEAT,0x20000000)
 
    if(excStatus&0x80080000) {  /* VME Bus Error */
      byteCount = (dmaStatus.dcsaLx - excAddrL) + dmaStatus.dcntx; /* This is the number BYTES NOT Transfered*/
      return(byteCount);
    }else{
      logMsg("DMA Terminated by Unknown Exception. dmaStatus: 0x%08x  Exception(adr,status): 0x%08x,0x%08x\n",
	     dmaStatus.dstax,excAddrL,excStatus,0,0,0);
      return(ERROR);
    }
  }

  return(0);

}


/* Other Utility routines to tweak AM codes for existing windows */
/*  Valid Outbound window id (owid)
        1 -> A32
        2 -> A24
        3 -> A16
    Valid amcodes = 0x10 - 0x1f (User)
      or 0 will revert back to normal AM code for the specified window
*/
unsigned int
sysTempeSetAM(unsigned int owid, unsigned int amcode)
{
  unsigned int reg=0;
  unsigned int am, pgm, sup;

  if((owid<1)||(owid>3)) 
    return(0);

  /* Create the proper bits to rewrite the Attribute register */ 
  if(amcode == 0) {
    sup = 0; 
    pgm = 0;
    am = 3 - owid;
  } else if ((amcode<0x10)||(amcode>0x1f)) {  /* Out of range */
    return(0);
  } else {
    switch (amcode) {
    case 0x10:
      am = 8; sup = 0; pgm = 0;
      break;
    case 0x11:
      am = 8; sup = 0; pgm = 1;
      break;
    case 0x12:
      am = 8; sup = 1; pgm = 0;
      break;
    case 0x13:
      am = 8; sup = 1; pgm = 1;
      break;
    case 0x14:
      am = 9; sup = 0; pgm = 0;
      break;
    case 0x15:
      am = 9; sup = 0; pgm = 1;
      break;
    case 0x16:
      am = 9; sup = 1; pgm = 0;
      break;
    case 0x17:
      am = 9; sup = 1; pgm = 1;
      break;
    case 0x18:
      am = 10; sup = 0; pgm = 0;
      break;
    case 0x19:
      am = 10; sup = 0; pgm = 1;
      break;
    case 0x1a:
      am = 10; sup = 1; pgm = 0;
      break;
    case 0x1b:
      am = 10; sup = 1; pgm = 1;
      break;
    case 0x1c:
      am = 11; sup = 0; pgm = 0;
      break;
    case 0x1d:
      am = 11; sup = 0; pgm = 1;
      break;
    case 0x1e:
      am = 11; sup = 1; pgm = 0;
      break;
    case 0x1f:
      am = 11; sup = 1; pgm = 1;
    }

  }

  switch (owid) {
  case 1: /* A32 */
    reg = (TEMPE_READ32(TEMPE_REG_BASE,TEMPE_OTAT1))&0xffffffc0;
    reg |= (sup<<5)|(pgm<<4)|(am);
    TEMPE_WRITE32_PUSH(TEMPE_REG_BASE,TEMPE_OTAT1,reg);
    break;
  case 2: /* A24 */
    reg = (TEMPE_READ32(TEMPE_REG_BASE,TEMPE_OTAT2))&0xffffffc0;
    reg |= (sup<<5)|(pgm<<4)|(am);
    TEMPE_WRITE32_PUSH(TEMPE_REG_BASE,TEMPE_OTAT2,reg);
    break;
  case 3: /* A16 */
    reg = (TEMPE_READ32(TEMPE_REG_BASE,TEMPE_OTAT3))&0xffffffc0;
    reg |= (sup<<5)|(pgm<<4)|(am);
    TEMPE_WRITE32_PUSH(TEMPE_REG_BASE,TEMPE_OTAT3,reg);
  }

  return (reg);
} 




/* Wrappers for backward compatibility */

STATUS 
sysVmeDmaSend (
    UINT32 locAdrs,   /* PCI Bus Local Address */
    UINT32 vmeAdrs,   /* VME Bus local Address */
    int size,       /* Size of the DMA Transfer in bytes*/
    BOOL toVme    /* Direction of the DMA  */
   )
{
  
  if(toVme) {
    return(usrMem2VmeDmaStart(0,vmeAdrs,locAdrs,size));
  } else {
    return(usrVme2MemDmaStart(0,vmeAdrs,locAdrs,size));
  }

}

int
sysVmeDmaDone(int pcnt, int pflag)
{
  
  return(usrTempeDmaDone(0));

}

void
sysVmeDmaShow()
{
  /* print out status of DMA channel 0 */
  usrTempeDmaStatus(0,1);

}

STATUS
sysVmeDmaSet(UINT32 type, UINT32 pval)
{

  unsigned int addrType, dataType;

  /* Only deal with VME Address and Data capabilities
     ie type = 11 (address width) and type = 12 (Data width)
     Return ERROR for all other options */

  if((type != 11)&&(type != 12))
    return (ERROR);


  if(type == 11) { /* Address Width  0 (A16), 1 (A24), or 2 (A32) */
    if((pval >= 0)&&(pval <3))
      addrType = pval;  
    else
      return(ERROR);

    /* Get Data Type from current val */
    dataType = dmaDescSample.srcVmeAttr.dataWidth + dmaDescSample.srcVmeAttr.xferProtocolOut ;
  }

  if(type == 12) { /* Data Type  3 (D32), 4 (BLK32), or 5 (MBLK) */
    if((pval > 2)&&(pval < 6))
      dataType = pval-2;  /* dataType defined differently for Tempe */
    else
      return(ERROR);

    /* Get Address mode from current val */
    addrType = dmaDescSample.srcVmeAttr.addressMode;

  }

  usrVmeDmaConfig(addrType, dataType, VME_SST160);

  return(OK);
}
