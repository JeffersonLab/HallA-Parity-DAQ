/*************************************************************************
 *
 *  vqwk.c   -  Driver library for readout of a TRIUMF VME QWEAK 8 
 *              channel 18-bit ADC using a VxWorks 5.5 or later based 
 *              single board computer.
 *
 *  Author: Paul King
 *          Ohio University / TJNAF QWEAK Collaboration
 *          December 2006
 *
 *  Revision 1.0 - Initial revision.
 *
 *           1.0a  - added reference pages for vqwkDumpStatus
 *                   changed sample_period to period_mult
 *                   to use the same jargon from the vqwk adc manual
 *                   Wednesday, March  2 17:26:12 EST 2011,jhlee
 */

#include <vxWorks.h>
#include <vxLib.h>
#include <logLib.h>

#include "vqwk_32.h"

/*  Use the universeDma include file for the DMA library definitions,
 *  even if we will end up running with the tempeDma library.  
 *  The tempeDma library has wrapper routines so the API is identical.
 */
#include "universeDma.h"



/* Define global variables */
int kReadBackwards = 0;                 /* Set to 1 for backward reading */
int kVerboseLevel  = 2;                 /* Set to 0 for only errors, 
					 * set to 1 for short status messages,
					 * and 2 for full messages.      */
int Nvqwk = 0;
struct vqwk_struct missingadc;          /* An empty ADC structure        */
struct vqwk_struct *vqwkp[20];          /* Pointers to ADC memory map    */

/* Default values to be used during initialization */
UINT8  kDefaultPeriod     = 0;          /*  40 cycles per sample          */
UINT8  kDefaultBlocks     = 4;          /*  4 blocks per gate             */
UINT16 kDefaultSamples    = 500;        /*  500 samples per block         */
UINT8  kDefaultDelay      = 20;         /*  Wait 20 samples after gate    */
UINT8  kDefaultGateFlg    = 1;          /*  External gate                 */
UINT8  kDefaultClkFlg     = 0;          /*  Internal clock                */
UINT16 kDefaultGateFreq   = 3300;       /*  Gate period is 33.00 ms       */



STATUS PrintSysBus(UINT32 addr, UINT32 am_code){
  STATUS statval;
  unsigned long laddr;
  statval = sysBusToLocalAdrs(am_code,(char *)addr,(char **)&laddr);
  printf("In the Address Modifier space, %X, the address of the module, %X, is %X.\n",
	 am_code, addr, laddr);
  return statval;
}

int vqwk_get_u8(UINT32 word, int position){
  // Grab a specific UINT8 out of a UNIT32
  if (position>=0 && position<=3){
    return ((word >> (position*8))&0xFF);
  } else {
    //  This is a bad position.
    return 0;
  }
};

int vqwk_get_u16(UINT32 word, int position){
  // Grab a specific UINT16 out of a UINT32
  if (position>=0 && position<=1){
    return ((word >> (position*16))&0xFFFF);
  } else {
    //  This is a bad position.
    return 0;
  }
};

UINT32 vqwk_set_u8(UINT32 word, int position, int newdata){
  // Set a specific UINT8 within a UNIT32
  int offset;
  UINT32 mask, invmask;
  if (position>=0 && position<=3){
    offset = position*8;
    mask = 0xFF << offset;
    invmask = 0xFFFFFFFF ^ mask;
    return ( (word&invmask) + ((newdata<<offset)&mask) );
  } else {
    //  This is a bad position.
    return word;
  }
};

UINT32 vqwk_set_u16(UINT32 word, int position, int newdata){
  // Set a specific UINT16 within a UNIT32
  int offset;
  UINT32 mask, invmask;
  if (position>=0 && position<=1){
    offset = position*16;
    mask = 0xFFFF << offset;
    invmask = 0xFFFFFFFF ^ mask;
    return ( (word&invmask) + ((newdata<<offset)&mask) );
  } else {
    //  This is a bad position.
    return word;
  }
};


void vqwkSetVerboseLevel(unsigned int verb){
  if (verb>0) kVerboseLevel=verb; 
  else kVerboseLevel=0;
};

void vqwkSetDefaultSamplePeriod(UINT8 sampleperiod){
  kDefaultPeriod = sampleperiod;
};
void vqwkSetDefaultNumberOfBlocks(UINT8 numblocks){
  kDefaultBlocks = numblocks;
};
void vqwkSetDefaultSamplesPerBlock(UINT16 samp_per_block){
  kDefaultSamples = samp_per_block;
};
void vqwkSetDefaultGateDelay(UINT8 gatedelay){
  kDefaultDelay = gatedelay;
};
void vqwkSetDefaultGateClockSources( UINT8 gateflag, UINT8 clkflag){
  kDefaultGateFlg = gateflag;
  kDefaultClkFlg  = clkflag;
};
void vqwkSetDefaultIntGateFreq(UINT16 intgatefreq){
  kDefaultGateFreq = intgatefreq;
};


void vqwkSetForwardsReading(){kReadBackwards=0;};
void vqwkSetBackwardsReading(){kReadBackwards=1;};

/*************************************************************************
 *
 * vqwkInit - Initialize Qweak ADC library. Disable/Clear Counters
 *
 *
 * RETURNS: OK, or ERROR if the ADC address is invalid.
 */

STATUS 
vqwkInit (UINT32 addr, UINT32 addr_inc, int nadcs)
{
  int ii, res, rdata, errFlag = 0;
  unsigned long laddr;
  
  /* Check for valid address */
  if(addr==0) {
    printf("Init: ERROR: Must specify a local VME-based (A24) address for ADC 0\n");
    return(ERROR);
  }else{
    if((addr_inc==0)||(nadcs==0))
      nadcs = 1; /* assume only one ADC to initialize */

    Nvqwk = 0;
    
    /* get the ADC's address */
    res = sysBusToLocalAdrs(0x39,(char *)addr,(char **)&laddr);
    if (res != 0) {
      printf("vqwkInit: ERROR in sysBusToLocalAdrs(0x39,0x%x,&laddr)\n",addr);
      return(ERROR);
    } else {
      for (ii=0;ii<nadcs;ii++) {
	Nvqwk++;
        vqwkp[ii] = (struct vqwk_struct *)(laddr + ii*addr_inc);
        /* Check if Board exists at that address */
        res = vxMemProbe((char *) vqwkp[ii],0,4,(char *)&rdata);
        if(res < 0) {
          printf("vqwkInit: ERROR: No addressable board at addr=0x%x\n",
		 (UINT32) vqwkp[ii]);
          /* If there is no addressable board here, connect this to 
           * the "missingadc", and keep going.
	   */
          vqwkp[ii] = &missingadc;
          errFlag++;
        } else {
	  printf("vqwkInit: Initialized ADC ID %d at address 0x%08x with default parameters\n",
		 ii, (UINT32) vqwkp[ii]);

	  /*  Let's set some things directly. */
	  vqwkSetSamplePeriod(ii, kDefaultPeriod);
	  vqwkSetNumberOfBlocks(ii,kDefaultBlocks);
	  vqwkSetSamplesPerBlock(ii,kDefaultSamples);
	  vqwkSetGateDelay(ii, kDefaultDelay);
	  vqwkSetIntGateFreq(ii,kDefaultGateFreq);
	  vqwkSetGateClockSources(ii,kDefaultGateFlg,kDefaultClkFlg);
	  
	  /*	  vqwkPrintStatus(ii);  */
	}
      }
    }
  }
  
  /* Disable/Clear all ADCs */
  for(ii=0;ii<Nvqwk;ii++) {
    if (vqwkp[ii] == &missingadc){
      //      if (kVerboseLevel>0) printf("vqwkInit: ERROR:  ADC ID %d is missing\n",ii); 
      printf("vqwkInit: ERROR:  ADC ID %d is missing\n",ii); 
      continue;
    }
    vqwkClearAllChannels(ii);
    if (kVerboseLevel>=2) vqwkPrintStatus(ii);
  }
    
  if(Nvqwk <=0 || Nvqwk == errFlag){

    return(ERROR);
  } else if (errFlag ==0) {
    printf("vqwkInit: %d ADC(s) successfully initialized\n",
	   Nvqwk);
  } else {
    printf("vqwkInit: %d ADC(s) successfully initialized; %d ADC(s) had initalization errors\n",
	   Nvqwk-errFlag, errFlag);
  }
  return(OK);
}

/*************************************************************************
 *
 * vqwkDumpStatus - Dump the status of the ADC to a buffer.
 *
 * RETURNS: Number of words written to the buffer.
 */
int vqwkDumpStatus(UINT16 index, UINT32 *data)
{
  UINT32  laddr;
  int     numwords = 0;
  UINT32  ldata1, ldata2, ldata3;
  if (index<Nvqwk && vqwkp[index]!=NULL && vqwkp[index]!=&missingadc){
    /*  This is a good ADC structure.  */
    laddr = (UINT32)vqwkp[index];

    *data++ = index;                    // Word 0
    *data++ = laddr;
    ldata1 = vqwkp[index]->c0_c1_c2;
    *data++ = vqwk_get_u8(ldata1,3);    // period_mult;    see page 22 in QWWK manual rev0
    *data++ = vqwk_get_u8(ldata1,2);    // num_blocks;     see page 22
    *data++ = vqwk_get_u16(ldata1,0);   // sample_per_blk; see page 22
    ldata2 = vqwkp[index]->c4_c5_c6;
    *data++ = vqwk_get_u8(ldata2,3);    // gate_delay;     see page 23
    *data++ = vqwk_get_u8(ldata2,2);    // gateclk_source; see page 23
    *data++ = vqwk_get_u16(ldata2,0);   // int_gate_freq;  see page 23
    ldata3 = vqwkp[index]->c8_c9_ca_cb;
    *data++ = vqwk_get_u8(ldata3,3);    // irq_ctr;        // Word 8
    *data++ = vqwk_get_u8(ldata3,2);    // int_vector;
    *data++ = vqwk_get_u8(ldata3,1);    // data_ready;
    *data++ = vqwk_get_u8(ldata3,0);    // firmware_revision;
    *data++ = vqwkp[index]->firmware_rev_date;
    *data++ = 0;
    *data++ = 0;
    *data++ = 0;                        // Word 15
    
    /* //  Reread everything for redundancy.       
     *data++ = index;                        // 16 
     *data++ = laddr;
     ldata1 = vqwkp[index]->c0_c1_c2;
     *data++ = vqwk_get_u8(ldata1,3);    // sample_period;
     *data++ = vqwk_get_u8(ldata1,2);    // num_blocks;
     *data++ = vqwk_get_u16(ldata1,0);   // samp_per_blk;
     ldata2 = vqwkp[index]->c4_c5_c6;
     *data++ = vqwk_get_u8(ldata2,3);    // gate_delay;
     *data++ = vqwk_get_u8(ldata2,2);    // gateclk_source;
     *data++ = vqwk_get_u16(ldata2,0);   // int_gate_freq;
     ldata3 = vqwkp[index]->c8_c9_ca_cb;
     *data++ = vqwk_get_u8(ldata3,3);    // irq_ctr;
     *data++ = vqwk_get_u8(ldata3,2);    // int_vector;
     *data++ = vqwk_get_u8(ldata3,1);    // data_ready;
     *data++ = vqwk_get_u8(ldata3,0);    // firmware_revision;
     *data++ = vqwkp[index]->firmware_rev_date;
     *data++ = 0;
     *data++ = 0;
     *data++ = 0;                            // 31
     */
    numwords = 16;
  }
  return numwords;
}

void vqwkPrintFirmwareVersion(UINT16 index){
  static char* months[12] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", 
			     "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};
  UINT32  laddr;
  UINT32 versioncode, datecode;
  int version, day, mon, year;

  if (index<Nvqwk && vqwkp[index]!=NULL && vqwkp[index]!=&missingadc){
    /*  This is a good ADC structure.  */
    laddr = (UINT32)vqwkp[index];
    versioncode =  vqwk_get_u8(vqwkp[index]->c8_c9_ca_cb,0);    // firmware_revision;
    version = ((versioncode>>4)&0xF)*10 + ((versioncode)&0xF);
    datecode = vqwkp[index]->firmware_rev_date;
    day  = ((datecode>>28) & 0xF)*10 
      + ((datecode>>24) & 0xF);
    year = ((datecode>>12) & 0xF)*1000
      + ((datecode>>8) & 0xF)*100
      + ((datecode>>4) & 0xF)*10 
      + ((datecode) & 0xF);
    mon  = ((datecode>>16) & 0xFF) - 0x41;  // The numeric month with JAN==0
    printf("QWEAK ADC index==%d, address==0x%lx\n",index,laddr);
    printf("\tFirmware revision ID: %d  (raw code=0x%02x)\n",version,
	   versioncode);
    printf("\tFirmware revision date: %02d %s %d  (raw code=0x%08x)\n",
	   day, months[mon], year, datecode);
  }
};



/*************************************************************************
 *
 * vqwkPrintStatus - Print the status of the ADC to the console.
 *
 * RETURNS: nothing
 */
void
vqwkPrintStatus(UINT16 index)
{
  UINT32  laddr;
  int period;
  float totaltime;
  UINT32 ldata1, ldata2, ldata3;
  int gateclk;

  if (index<Nvqwk && vqwkp[index]!=NULL && vqwkp[index]!=&missingadc){
    /*  This is a good ADC structure.  */
    laddr = (UINT32)vqwkp[index];
    printf("QWEAK ADC index==%d, address==0x%lx\n",index,laddr);
    
    ldata1 = vqwkp[index]->c0_c1_c2;
    ldata2 = vqwkp[index]->c4_c5_c6;
    ldata3 = vqwkp[index]->c8_c9_ca_cb;

    printf("Variable dump:\n\tclk \tnblk \tnsamp\tdelay\tgateclk\tfreq\ti_cr\ti_vec\tready\trevnum\n\t0x%02x \t0x%02x \t0x%04x \t0x%02x \t0x%02x \t0x%04x \t0x%02x \t0x%02x \t0x%02x \t0x%02x\n", 
	   vqwk_get_u8(ldata1,3), vqwk_get_u8(ldata1,2), vqwk_get_u16(ldata1,0),
	   vqwk_get_u8(ldata2,3), vqwk_get_u8(ldata2,2), vqwk_get_u16(ldata2,0),
	   vqwk_get_u8(ldata3,3), vqwk_get_u8(ldata3,2), vqwk_get_u8(ldata3,1), vqwk_get_u8(ldata3,0)
	   );


    /*     printf("Register dump:\n\t 0x%08x\t 0x%08x\t0x%08x\n",
           *((UINT32*)(laddr+0xC0)),
           *((UINT32*)(laddr+0xC4)),
           *((UINT32*)(laddr+0xC8)));
     */
      
    /*     printf("Register dump:\n\t 0x%04x 0x%04x\t 0x%04x 0x%04x\t0x%04x 0x%04x\n", */
    /* 	   *((UINT16*)(laddr+0xC0)), */
    /* 	   *((UINT16*)(laddr+0xC2)), */
    /* 	   *((UINT16*)(laddr+0xC4)), */
    /* 	   *((UINT16*)(laddr+0xC6)), */
    /* 	   *((UINT16*)(laddr+0xC8)), */
    /* 	   *((UINT16*)(laddr+0xCA))); */
    

/*     printf("\tsamp_per_blk=0x%x\n\tgate_delay=0x%x\n\tgateclk_source=0x%x\n", */
/* 	   vqwkp[index]->samp_per_blk, vqwkp[index]->gate_delay,  */
/* 	   vqwkp[index]->gateclk_source); */
/*     printf("\tint_gate_freq=0x%x\n\tirq_ctr=0x%x\n\tint_vector=0x%x\n", */
/* 	   vqwkp[index]->int_gate_freq, vqwkp[index]->irq_ctr,  */
/* 	   vqwkp[index]->int_vector); */
/*     printf("\tdata_ready=0x%x\n",vqwkp[index]->data_ready); */

    gateclk = vqwk_get_u8(ldata2,2);
    printf("\tGate/Clock register == 0x%x\n", gateclk);

    if ((gateclk & 0x1) != 0x1){
      printf("\tUsing INTERNAL 20 MHz CLOCK.\n");
    } else {
      printf("\tUsing EXTERNAL CLOCK\n");
    }
    if ((gateclk & 0x2) != 0x2){
      float freq = 100.0/(vqwk_get_u16(ldata2,0));
      printf("\tUsing INTERNAL GATE, frequency = %f kHz (register=0x%x).\n",
	     freq, vqwk_get_u16(ldata2,0));
    } else {
      printf("\tUsing EXTERNAL GATE.\n");
    }
    printf("\tAcquisition starts %d clock cycles after the gate begins\n",
	   vqwk_get_u8(ldata2,3));
    printf("\tThere are %d blocks of %d samples/block for each gate.\n",
	   (vqwk_get_u8(ldata1,2) + 1), vqwk_get_u16(ldata1,0));
    
    period = 40 + vqwk_get_u8(ldata1,3);
    printf("\tThe sample period is %d clock cycles (register=0x%x).\n",
	   period, vqwk_get_u8(ldata1,3));

    /*  totaltime is based on a 20 MHz clock, so 50 ns per pulse.   */
    totaltime = 2.5e-6 + 50.0e-9 * ( (period * vqwk_get_u16(ldata1,0)
				      * (vqwk_get_u8(ldata1,2) + 1))
				     + vqwk_get_u8(ldata2,3));
    printf("\n\tThe total delay+acquisition time per gate is %f s.\n",
	   totaltime);

    if (vqwk_get_u8(ldata3,3) & 0x08 == 0x0){
      printf("\n\tVME Interrupt requests are disabled.\n");
    } else {
      printf("\n\tVME Interrupt requests are enabled, with IRQ level of %d\n",
	     (vqwk_get_u8(ldata3,3)&0x7));
    }
    printf("\tThe Interrupt Vector is 0x%x.\n",vqwk_get_u8(ldata3,2));
    
    if (vqwk_get_u8(ldata3,1) == 0){
      printf("\n\tAll ADC FIFOs are empty.\n");
    } else {
      printf("\n\tThere is data in ADC FIFOs: ... ");
    }
  }
}



/*************************************************************************
 *
 * vqwkSetSamplePeriod - Set the ADC to ...
 *
 *
 * RETURNS: nothing
 */
void vqwkSetSamplePeriod(UINT16 index, UINT8 sampleperiod)
{
  UINT32 ldata1;
  if (index<Nvqwk && vqwkp[index]!=NULL && vqwkp[index]!=&missingadc){
    /*  This is a good ADC structure.  */
    ldata1 = vqwkp[index]->c0_c1_c2;
    vqwkp[index]->c0_c1_c2 = vqwk_set_u8(ldata1,3,sampleperiod);
    ldata1 = vqwkp[index]->c0_c1_c2;
    /*     printf("\tldata1==0x%x \tvqwk_get_u8(ldata1,3)==0x%x \tvqwk_get_u8(ldata1,2)==0x%x \tvqwk_get_u8(ldata1,1)==0x%x \tvqwk_get_u8(ldata1,0)==0x%x \tvqwk_get_u16(ldata1,1)==0x%x \tvqwk_get_u16(ldata1,0)==0x%x\n", */
    /* 	   ldata1,  */
    /* 	   vqwk_get_u8(ldata1,3), vqwk_get_u8(ldata1,2),  */
    /* 	   vqwk_get_u8(ldata1,1), vqwk_get_u8(ldata1,0),  */
    /* 	   vqwk_get_u16(ldata1,1), vqwk_get_u16(ldata1,0) */
    /* 	   ); */
    /* if (kVerboseLevel>0) 
      printf("\tThe sample period is %d clock cycles (register=0x%x).\n",
      sampleperiod+40, vqwk_get_u8(ldata1,3)); */
  }
}

/*************************************************************************
 *
 * vqwkSetNumberOfBlocks - Set the ADC to ...
 *
 *
 * RETURNS: nothing
 */
void vqwkSetNumberOfBlocks(UINT16 index, int numblocks)
{
  UINT32 ldata1;
  if (numblocks<0 || numblocks>4){
    printf("\tThe number of blocks must be between 1 and 4.\n");
  } else if (index<Nvqwk && vqwkp[index]!=NULL && vqwkp[index]!=&missingadc){
    /*  This is a good ADC structure.  */
    ldata1 = vqwkp[index]->c0_c1_c2;
    vqwkp[index]->c0_c1_c2 = vqwk_set_u8(ldata1,2,(numblocks-1));
    ldata1 = vqwkp[index]->c0_c1_c2;
    /* if (kVerboseLevel>0) 
      printf("\tThe number of blocks is %d.\n", (vqwk_get_u8(ldata1,2))+1);
    */
  }
}

/*************************************************************************
 *
 * vqwkGetNumberOfBlocks - Get the ADC to ...
 *
 *
 * RETURNS: int
 */
int vqwkGetNumberOfBlocks(UINT16 index)
{
  UINT32 ldata1;
   if (index<Nvqwk && vqwkp[index]!=NULL && vqwkp[index]!=&missingadc){
    /*  This is a good ADC structure.  */
    ldata1 = vqwkp[index]->c0_c1_c2;
    return vqwk_get_u8(ldata1,2)+1;
  }
}


/*************************************************************************
 *
 * vqwkSetSamplesPerBlock - Set the ADC to ...
 *
 *
 * RETURNS: nothing
 */
void vqwkSetSamplesPerBlock(UINT16 index, UINT16 samp_per_blk)
{
  UINT32 ldata1;
  if (samp_per_blk==0 || samp_per_blk>16383){
    printf("\tThe number of samples per block should be between 1 and 16383.\n");
  } else if (index<Nvqwk && vqwkp[index]!=NULL && vqwkp[index]!=&missingadc){
    /*  This is a good ADC structure.  */
    ldata1 = vqwkp[index]->c0_c1_c2;
    vqwkp[index]->c0_c1_c2 = vqwk_set_u16(ldata1,0,samp_per_blk);
    ldata1 = vqwkp[index]->c0_c1_c2;
    /*if (kVerboseLevel>0) 
      printf("\tThe number of samples per blocks is %d.\n",  vqwk_get_u16(ldata1,0));
    */
  }
}

/*************************************************************************
 *
 * vqwkGetSamplesPerBlock - get vqwk parameter ...
 *
 *
 * RETURNS: int
 */
int vqwkGetSamplesPerBlock(UINT16 index)
{
  UINT32 ldata1;
  if (index<Nvqwk && vqwkp[index]!=NULL && vqwkp[index]!=&missingadc){
    /*  This is a good ADC structure.  */
    ldata1 = vqwkp[index]->c0_c1_c2;
    return vqwk_get_u16(ldata1,0);   // sample_per_blk; see page 22
   }
}


/*************************************************************************
 *
 * vqwkSetGateDelay - Set the ADC to ...
 *
 *
 * RETURNS: nothing
 */
void vqwkSetGateDelay(UINT16 index, UINT8 gatedelay)
{
  UINT32 ldata2;
  if (gatedelay==0){
    printf("\tThe gate delay should be at least 1 cycle.\n");
  } else  if (index<Nvqwk && vqwkp[index]!=NULL && vqwkp[index]!=&missingadc){
    /*  This is a good ADC structure.  */
    ldata2 = vqwkp[index]->c4_c5_c6;
    vqwkp[index]->c4_c5_c6 = vqwk_set_u8(ldata2,3,gatedelay);
    ldata2 = vqwkp[index]->c4_c5_c6;

    /*
    if (kVerboseLevel>0) 
    printf("\tThe acquisition will begin %d clock cycles after the gate.\n", vqwk_get_u8(ldata2,3));
    */
  }
}

/*************************************************************************
 *
 * vqwkGetGateDelay - Get the ADC to ...
 *
 *
 * RETURNS: int
 */
 
int vqwkGetGateDelay(UINT16 index)
{
  UINT32 ldata2;
   if (index<Nvqwk && vqwkp[index]!=NULL && vqwkp[index]!=&missingadc){
    /*  This is a good ADC structure.  */
    ldata2 = vqwkp[index]->c4_c5_c6;
    return vqwk_get_u8(ldata2,3);   // sample_per_blk; see page 22

  }
}

/*************************************************************************
 *
 * vqwkSetGateClockSources - Set the ADC to use the internal or external
 *                          sources for the GATE and the SYSTEM_CLOCK.
 *
 * RETURNS: nothing
 */
void vqwkSetGateClockSources(UINT16 index, UINT8 gateflag, UINT8 clkflag)
{
  vqwkSetGateClockSourcesVerbose(index, gateflag, clkflag, 0);
};

void vqwkSetGateClockSourcesVerbose(UINT16 index, UINT8 gateflag, 
				    UINT8 clkflag, int verbose)
{
  UINT32 ldata2;
  UINT8 source = 0x00;
  float freq;
  if (gateflag !=0)     source |= 0x02;
  if (clkflag !=0)      source |= 0x01;
  if (index<Nvqwk && vqwkp[index]!=NULL && vqwkp[index]!=&missingadc){
    /*  This is a good ADC structure.  */
    ldata2 = vqwkp[index]->c4_c5_c6;
    vqwkp[index]->c4_c5_c6 = vqwk_set_u8(ldata2,2,source);
    if (verbose>0){
      ldata2 = vqwkp[index]->c4_c5_c6;
      source = vqwk_get_u8(ldata2,2);
      if ((source & 0x1) == 0){
	printf("\tUsing INTERNAL 20 MHz CLOCK.\n");
      } else {
	printf("\tUsing EXTERNAL CLOCK\n");
      }
      if ((source & 0x2) == 0){
	printf("\tUsing INTERNAL GATE.\n");
      } else {
	printf("\tUsing EXTERNAL GATE.\n");
      }
    }
  }
}


/*************************************************************************
 *
 * vqwkSetIntGateFreq - Set the ADC to ...
 *
 *
 * RETURNS: nothing
 */
void vqwkSetIntGateFreq(UINT16 index, UINT16 intgatefreq)
{
  UINT32 ldata2;
  float freq;
  if (index<Nvqwk && vqwkp[index]!=NULL && vqwkp[index]!=&missingadc){
    ldata2 = vqwkp[index]->c4_c5_c6;
    vqwkp[index]->c4_c5_c6 = vqwk_set_u16(ldata2,0,intgatefreq);
    ldata2 = vqwkp[index]->c4_c5_c6;
    freq = 100.0/(intgatefreq);
    /*
    if (kVerboseLevel>0) 
      printf("\tInternal gate frequency = %f kHz (register=0x%x).\n",
	     freq, vqwk_get_u16(ldata2,0));
    */
  }
}

/*************************************************************************
 *
 * vqwkSetIRQRegister - Set the ADC to ...
 *
 *
 * RETURNS: nothing
 */
void vqwkSetIRQRegister(UINT16 index, UINT8 irqreg)
{
}

/*************************************************************************
 *
 * vqwkSetInterruptVector - Set the ADC to ...
 *
 *
 * RETURNS: nothing
 */
void vqwkSetInterruptVector(UINT16 index, UINT8 intvec)
{
}


UINT8 vqwkProbeDataReady(UINT16 index)
{
  UINT8 dataflag = 0;
  if (index<Nvqwk && vqwkp[index]!=NULL && vqwkp[index]!=&missingadc){
    dataflag = vqwk_get_u8(vqwkp[index]->c8_c9_ca_cb,1);
  }
  return dataflag;
}



void vqwkTakeSomeDataToScreen(UINT16 index, UINT16 numgates, UINT8 newgateclk, UINT8 readmode)
{
  UINT32 counter;
  INT32  timer;
  UINT32 adcdata[50];
  UINT32 numvalues;

  UINT32 oldldata2;

  UINT16 ii;

  UINT32 seqnum_mask = vqwk_seqnum_mask[1];
  UINT32 seqnum_shift = vqwk_seqnum_shift[1];
  UINT32 nsamples_mask = vqwk_nsamples_mask[1];
  UINT32 nsamples_shift = vqwk_nsamples_shift[1];
  
  oldldata2 = vqwkp[index]->c4_c5_c6;
  vqwkp[index]->c4_c5_c6 = vqwk_set_u8(oldldata2,2,newgateclk);

  counter = 0;
  

  while (counter < numgates){

    printf("Wait for data to be ready...  ");
    timer = vqwkWaitForDataReady(index, 0xFF, 600000);
    if (timer<0) {
       printf("Error code==%d\n",timer);
    } else {
       printf("Data is ready after %d cycles.  Data_ready==%x\n",timer, vqwkProbeDataReady(0));
    }

    printf("Read everything after waiting for a new gate\n");
    if (readmode==0)
      numvalues = vqwkReadRegisters(index, 0xFF, adcdata);
    else
      numvalues = vqwkBlockRead(index, adcdata);

    counter++;
    if (numvalues < 8*6){
    } else {
      printf("        Block1     Block2     Block3     Block4     1+2+3+4    BlockSum   NSamp  Seqnum\n");
      for (ii=0; ii<8; ii++){
	printf("ADC %d:  0x%08lx 0x%08lx 0x%08lx 0x%08lx 0x%08lx 0x%08lx %6d %3d\n",
	       ii+1, 
	       adcdata[ii*6 + 0],
	       adcdata[ii*6 + 1],
	       adcdata[ii*6 + 2],
	       adcdata[ii*6 + 3],
	       (adcdata[ii*6 + 0]+adcdata[ii*6 + 1]+adcdata[ii*6 + 2]+adcdata[ii*6 + 3]),
	       adcdata[ii*6 + 4],
	       (adcdata[ii*6 + 5]&nsamples_mask)>>nsamples_shift,
	       (adcdata[ii*6 + 5]&seqnum_mask)>>seqnum_shift);
      }
      printf("\n");
    }

    //  Read everything again.
    printf("Second reading without waiting for another gate.  Data_ready==%x\n",
	   vqwkProbeDataReady(index));
    if (readmode==0)
      numvalues = vqwkReadRegisters(index, 0xFF, adcdata);
    else
      numvalues = vqwkBlockRead(index, adcdata);

    counter++;
    if (numvalues < 8*6){
    } else {
      printf("        Block1     Block2     Block3     Block4     1+2+3+4    BlockSum   NSamp  Seqnum  Word6\n");
      for (ii=0; ii<8; ii++){
	printf("ADC %d:  0x%08lx 0x%08lx 0x%08lx 0x%08lx 0x%08lx 0x%08lx %6d %3d  0x%08lx\n",
	       ii+1, 
	       adcdata[ii*6 + 0],
	       adcdata[ii*6 + 1],
	       adcdata[ii*6 + 2],
	       adcdata[ii*6 + 3],
	       (adcdata[ii*6 + 0]+adcdata[ii*6 + 1]+adcdata[ii*6 + 2]+adcdata[ii*6 + 3]),
	       adcdata[ii*6 + 4],
	       (adcdata[ii*6 + 5]&nsamples_mask)>>nsamples_shift,
	       (adcdata[ii*6 + 5]&seqnum_mask)>>seqnum_shift,
	       adcdata[ii*6 + 5]);
      }
      printf("\n");
    }



  }
  vqwkp[index]->c4_c5_c6 = oldldata2;
}

INT32  vqwkWaitForDataReady(UINT16 index, UINT8 rmask, INT32 timeout){
  INT32 cycles;
  UINT8 dataready;
 
  cycles = -1;
  if (index<Nvqwk && vqwkp[index]!=NULL && vqwkp[index]!=&missingadc
      && rmask!=0){

    cycles = 0;
    
    if (timeout>0) {
      while (cycles<timeout && dataready!=rmask) { 
	dataready = (vqwkProbeDataReady(index)) & rmask;
	cycles++;
      }
      if (cycles >= timeout){
	cycles *= -1;
      }
    } else {
      while (cycles<timeout && dataready!=rmask) { 
	dataready = (vqwkProbeDataReady(index)) & rmask;
	cycles++;
      }
    }
  }
  return cycles;
};

int  vqwkReadRegisters(UINT16 index, UINT8 rmask,  UINT32 *data){
  int status;
  if (kReadBackwards==1){
    status = vqwkReadBackwards(index, rmask, data);
  } else {
    status = vqwkReadForwards(index, rmask, data);
  }
  return status;
};

int  vqwkReadForwards(UINT16 index, UINT8 rmask,  UINT32 *data){
  int ii, jj;
  int nvalues;
  UINT32 tmpval;
  nvalues = 0;
  if (index<Nvqwk && vqwkp[index]!=NULL && vqwkp[index]!=&missingadc){
    for (ii=0; ii<8; ii++){
      if(((rmask)&(1<<ii)) > 0) {
	for (jj=0; jj<4; jj++){
	  *data++ = vqwkp[index]->adc[ii].block[jj];
	  nvalues++;
	}
	*data++ = vqwkp[index]->adc[ii].total;
	nvalues++;
        *data++ = vqwkp[index]->adc[ii].nsamples_seqnum;
	nvalues++;
      }
    }
  }
  return nvalues;  
};

int  vqwkReadBackwards(UINT16 index, UINT8 rmask,  UINT32 *data){
  int ii, jj;
  int nvalues;
  UINT32 tmpval;
  nvalues = 0;
  if (index<Nvqwk && vqwkp[index]!=NULL && vqwkp[index]!=&missingadc){
    for (ii=0; ii<8; ii++){
      if(((rmask)&(1<<ii)) > 0) {
	data += 5;
        *data-- = vqwkp[index]->adc[ii].total;
	nvalues++;
        for (jj=3; jj>=0; jj--){
          *data-- = vqwkp[index]->adc[ii].block[jj];
          nvalues++;
        }
        data += 6;
        *data++ = vqwkp[index]->adc[ii].nsamples_seqnum;
        nvalues++;
      }
    }
  }
  return nvalues;
};


int  vqwkRead(UINT16 index, UINT8 rmask,  UINT32 *data , INT32 timeout, INT32 verbose){
  int ii, jj;
  int ready;
  int nvalues;
  UINT32 tmpval;

  if (index<Nvqwk && vqwkp[index]!=NULL && vqwkp[index]!=&missingadc){
    ready = vqwkWaitForDataReady(index, rmask, timeout);
    if (ready <0){
      if (verbose == 1)
	printf("No data was available after %d cycles.\n",-1*ready);
    } else {
      nvalues = vqwkReadRegisters(index, rmask, data);
    }
  }
  return nvalues;
};


int  vqwkReadLastEvent(UINT16 index, UINT8 rmask,  UINT32 *data , INT32 timeout, INT32 verbose){
  int nvalues;
  int counter;
  nvalues = counter = 0;
  while (vqwkProbeDataReady(index) & rmask !=0){
    vqwkReadRegisters(index, rmask, data);
    counter++;
    if (counter>timeout) break;
  }
  return nvalues;
}


void  vqwkClearAllChannels(UINT16 index){
  if (index<Nvqwk && vqwkp[index]!=NULL && vqwkp[index]!=&missingadc){
    /*  This is a good ADC structure.
     *  Writing to this register clears the FIFO and resets the sequence
     *  number for all channels
     */
    vqwkp[index]->reset_fifo_seqnum = 0xFFFFFFFF;
  }
}


int  vqwkBlockRead(UINT16 index, UINT32 *data){
  /*  This routine will always readout all eight channels in the ADC,
   *  using DMA calls.
   *  It does not check for valid data, and it does not check that
   *  the DMA programming has been done; both should be verified
   *  by the calling routines.
   */
  int nwrds = 48; //  There are 6 words per channel & 8 channels.
  volatile unsigned int *laddr;  // local pointer to the output data buffer
  int retVal, xferCount;
  int dummy=0;

  if (index>Nvqwk || vqwkp[index]==NULL || vqwkp[index]==&missingadc){
    logMsg("vqwkBlockRead: ERROR : ADC in slot %d is not initialized \n",index,0,0,0,0,0);
    return(ERROR);
  }
  if(data==NULL) {
    logMsg("vqwkBlockRead: ERROR: Invalid Destination address\n",0,0,0,0,0,0);
    return(ERROR);
  }

  /* Check for 8 byte boundary for address - insert dummy word*/
  if((unsigned long) (data)&0x7) {
    *data = 0xf0f0f0f0;
    dummy = 1;
    laddr = (data + 1);
  } else {
    dummy = 0;
    laddr = data;
  }
  retVal = sysVmeDmaSend((UINT32)laddr, (unsigned int)(vqwkp[index]), (nwrds<<2), 0);
  if(retVal |= 0) {
    logMsg("vqwkBlockRead: ERROR in DMA transfer Initialization 0x%x\n",retVal,0,0,0,0,0);
    return(retVal);
  }
  /* Wait until Done or Error */
  retVal = sysVmeDmaDone(10000,1);
  
  if(retVal >= 0) {
    xferCount = (nwrds - (retVal>>2) + dummy);  /* Number of Longwords transfered */
    return(xferCount); /* Return number of data words transfered */
  } else {  /* Error in DMA */
    logMsg("vqwkBlockRead: ERROR: sysVmeDmaDone returned an Error\n",0,0,0,0,0,0);
    return(retVal);
  }
  /*  If we get here, then there has been some problem, but it hasn't
   *  been caught yet.
   */
  logMsg("vqwkBlockRead: ERROR: unrecognized error\n",0,0,0,0,0,0);
  return(ERROR);
}




void setFakeData(UINT16 index, UINT32 *data) {
  /*  This will generate a ADC module worth of fake data.  */
  int ii,jj,tot;
  for(ii=0;ii<8;ii++) { 	   /* run over all channels in the adc. */
    int blknum = 4;	           /* get the total number of blocks */
    tot=0;
    for(jj=0;jj<blknum;jj++) {	   /* run over all data stored in channel */
      *data++=jj;
      tot+=jj;
    }
    /* set the total */
    *data++=tot; 		
    /* Make the Sequence number and store. */
    *data++= 0;
    
  }
  
  return;
}
