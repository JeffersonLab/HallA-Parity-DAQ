/* Flags and structures for SIS380* scaler */

/* If 4 FIFO half full flag, if 1 FIFO not empty */
#define INTERRUPT_FLAG 4

/* Delay(sec) = TIME_DELAY_SIS/60.  (this param is usually 3)*/
#define TIME_DELAY_SIS 3

/* Debug flags 1=Debug Mode, 0=quiet mode */
#define DEBUG1 0    /* raw printouts of buffers */
#define DEBUG2 0    /* interaction with CODA */
#define DEBUG3 0    /* loading and checking helicity */
#define DEBUG4 0    /* same as 3, but only if shreg_init_flag=1 */
#define DEBUG5 0
#define DEBUG_INT 0 /* debug of interrupt service routine */

#define OUT_OF_TIME_MODE  0  /* In-time(0) or out-of-time(1) helicity */

#define NUM_CHANNEL 32
#define RING_DEPTH  500    /* depth of ring buffer */
#define SHREG_NBIT   24    /* number of bits in shift register */
#define ROLLOVER  370000000  /* When num_read reset to 1 (~1week at 600 Hz)*/
#define NREADTOL 1  /* num_read of scalers must be within this tolerance */
#define NDELAY 2          /* Number of quads the helicity is delayed.  Cannot be
                            zero.  If you want zero, set OUT_OF_TIME_MODE=0. */
#define NRING_CHANNEL 32  /* Number of channels stored in a ring; is <= NUM_CHANNEL */
#define EFLAG -1

#define FIFO_EMPTY 0x100
#define HELICITY_MASK 0x40000000
#define DATA_MASK 0xffffff
#define UPBIT_MASK 0xff000000
#define QRT_MASK 0x80000000

struct SISbuffer{
  long numread;
  long hbit;
  long upbit;
  long qrt;
  long data[NUM_CHANNEL];
};

struct SISring{
  long clock;
  long qrt_hel;  /* qrt in bit 2,  helicity in bit 1 */
  long data[NRING_CHANNEL];
};





