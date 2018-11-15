/******************************************************************************
 *
 *  vqwk.h   -  Driver library header file for readout of a TRIUMF
 *              VME QWEAK 8 channel 18-bit ADC using a VxWorks 5.5
 *              or later based single board computer.
 *
 *  Author: Paul King
 *          Ohio University / TJNAF QWEAK Collaboration
 *          December 2006
 *
 *  Revision 1.0 - Initial revision.
 */

#ifndef _VQWKLIB_
#define _VQWKLIB_

/* Define Structures for access to the VQWK module */
typedef struct vqwk_channel_struct {
  /*  Each ADC channel has a total size of 0x18 bytes.  
   *  The internal structure of the channel bank is 
   *  as follows.
   *                  Register name   Offset  Description               */
  volatile UINT32  block[4];          /* 00 - Block 1 sum               *
				       * 04 - Block 2 sum               *
				       * 08 - Block 3 sum               *
				       * 0C - Block 4 sum               */
  volatile UINT32  total;             /* 10 - Total sum                 */
  volatile UINT16  nsamples;          /* 14 - Number of A/D samples     */
  volatile UINT8   seqnum;            /* 16 - Acquisition period number */
  volatile UINT8   unused;            /* 17 - UNUSED                    */
} vqwk_channel_test;


typedef struct vqwk_struct {
  /*  register name                   Offset  Description               */
  volatile
  struct vqwk_channel_struct adc[8];  /* 00 - ADC1 data                 *
				       * 18 - ADC2 data                 *
				       * 30 - ADC3 data                 *
				       * 48 - ADC4 data                 *
				       * 60 - ADC5 data                 *
				       * 78 - ADC6 data                 *
				       * 90 - ADC7 data                 *
				       * A8 - ADC8 data                 */
  volatile UINT8  sample_period;      /* C0 - Clock cycles per sample   *
				       *      equals sample_period+40   */
  volatile UINT8  num_blocks;         /* C1 - Number of blocks per      *
				       *      gate: 1-4, default is 4   */
  volatile UINT16 samp_per_blk;       /* C2 - Samples per block         */
  volatile UINT8  gate_delay;         /* C4 - Number of clock cycles    *
				       *      delay for first sample    *
				       *      per gate                  */
  volatile UINT8  gateclk_source;     /* C5 - Selector for the gate     *
				       *      and clock source          */
  volatile UINT16 int_gate_freq;      /* C6 - Gate frequency equals     *
				       *       100kHz/int_gate_freq     */
  volatile UINT8  irq_ctr;            /* C8 - IRQ control register      */
  volatile UINT8  int_vector;         /* C9 - Interrupt vector          */
  volatile UINT8  data_ready;         /* CA - Data ready per channel    */
  volatile UINT8  firmware_revision;  /* CB - Firmware Revision Number  */
  volatile UINT32 firmware_rev_date;  /* CC - Firmware Revision Date    */
  volatile UINT8  reset_fifo_seqnum;  /* D0 - FIFO and Sequence number reset */
  volatile UINT8  unused1;            /* D1 - UNUSED                    */
  volatile UINT8  unused2;            /* D2 - UNUSED                    */
  volatile UINT8  unused3;            /* D3 - UNUSED                    */
} vqwk_test;





/* Define Masks */
const UINT32 vqwk_period_mask   = 0xff;
const UINT32 vqwk_nblocks_mask  = 0x3;
const UINT32 vqwk_blocknsamples_mask = 0xCFFF;

const UINT32 vqwk_seqnum_mask[2]    = {0x00FF0000, 0x0000FF00};  // Mask for sequence number
const UINT32 vqwk_nsamples_mask[2]  = {0x0000FFFF, 0xFFFF0000};  // Mask for number of samples
const UINT32 vqwk_seqnum_shift[2]   = {16,         8};           // Bit shift for sequence number
const UINT32 vqwk_nsamples_shift[2] = {0,          16};          // Bit shift for number of samples




/*  Access functions */
void vqwkSetVerboseLevel(unsigned int verb);
void vqwkSetDefaultSamplePeriod(UINT8 sampleperiod);
void vqwkSetDefaultNumberOfBlocks(UINT8 numblocks);
void vqwkSetDefaultSamplesPerBlock(UINT16 samp_per_block);
void vqwkSetDefaultGateDelay(UINT8 gatedelay);
void vqwkSetDefaultGateClockSources( UINT8 gateflag, UINT8 clkflag);
void vqwkSetDefaultIntGateFreq(UINT16 intgatefreq);

void vqwkSetForwardsReading();
void vqwkSetBackwardsReading();


STATUS vqwkInit (UINT32 addr, UINT32 addr_inc, int nadcs);

int vqwkDumpStatus(UINT16 index, UINT32 *data);
void vqwkPrintFirmwareVersion(UINT16 index);
void vqwkPrintStatus(UINT16 index);

void vqwkSetSamplePeriod(UINT16 index, UINT8 sampleperiod);
void vqwkSetNumberOfBlocks(UINT16 index, UINT8 numblocks);
void vqwkSetSamplesPerBlock(UINT16 index, UINT16 samp_per_block);
void vqwkSetGateDelay(UINT16 index, UINT8 gatedelay);
void vqwkSetGateClockSources(UINT16 index,  UINT8 gateflag, UINT8 clkflag);
void vqwkSetGateClockSourcesVerbose(UINT16 index, UINT8 gateflag, 
				    UINT8 clkflag, int verbose);
void vqwkSetIntGateFreq(UINT16 index, UINT16 intgatefreq);
void vqwkSetIRQRegister(UINT16 index, UINT8 irqreg);
void vqwkSetInterruptVector(UINT16 index, UINT8 intvec);

UINT8 vqwkProbeDataReady(UINT16 index);

void vqwkTakeSomeDataToScreen(UINT16 index, UINT16 numgates, UINT8 newgateclk, UINT8 readmode);

INT32  vqwkWaitForDataReady(UINT16 index, UINT8 rmask, INT32 timeout);

int  vqwkReadRegisters(UINT16 index, UINT8 rmask,  UINT32 *data);
int  vqwkReadForwards(UINT16 index, UINT8 rmask,  UINT32 *data);
int  vqwkReadBackwards(UINT16 index, UINT8 rmask,  UINT32 *data);

int  vqwkRead(UINT16 index, UINT8 rmask, UINT32 *data, INT32 timeout, INT32 verbose);

int  vqwkReadLastEvent(UINT16 index, UINT8 rmask,  UINT32 *data , INT32 timeout, INT32 verbose);

void  vqwkClearAllChannels(UINT16 index);


int  vqwkBlockRead(UINT16 index, UINT32 *data);


void setFakeData(UINT16 index, UINT32 *data);

#endif
