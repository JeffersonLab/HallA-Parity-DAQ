 /************************************************************************
 *  SIS3801.h                                                           *
 *   Define the vme address of the multichannels scalers SIS3801        *
 *                                                                      *  
 *   define the memory map of the SIS3801.                              *  
 *                                                                      *  
 * Date  : R. Tieulent  March 15 99                                     *   
 * Modifications:                                                       *
 *                                                                      *  
 *  Oct 2, 2008  Version for PVDIS
 *                                                                      *
 ************************************************************************/


/* Number of scalers SIS3801 */
#define NUMSCALERS 3

/* Total number of scalers including others */
#define TOTSCALERS 4

/* Addresses on the scalers are adjusted with the thumbwheel switch.
   E.g. abc4 or ce6 are thumbwheel switch settings */
 
long SISRELADDR[NUMSCALERS]={  /* Addresses relative to A24 VME  */
  0xce1000, 0xce2000, 0xce3000 };

long SISVMEADDR[NUMSCALERS]={   /* Absolute addresses, assigned by sysBusToLocalAdrs */
  0, 0, 0  };                      /* init to zero */

/* Repository of global data */
long global_scaler_sum[32*TOTSCALERS];


/* define control registers structure*/
typedef struct fifo_scaler 
{ /*  register name              offset         description  (w-write, r-read, r/w-read/write)*/
  unsigned long csr             ; /* 00      - Status register(read)/Conrol register(write)  */
  unsigned long irq             ; /* 04      - VME IRQ control register and module identification (r/w) */
  unsigned long empty0          ; /* 08 not used */
  unsigned long cdr             ; /* 0C      - Copy disable register (w) */
  unsigned long wfifo           ; /* 10      - Write to FIFO (in FIFO test mode)*/
  unsigned long empty1          ; /* 14 not used */
  unsigned long empty2          ; /* 18 not used */
  unsigned long empty3          ; /* 1C not used */
  unsigned long clear           ; /* 20      - Clear FIFO, logic and counters (w) */
  unsigned long next            ; /* 24      - VME next clock (w) */
  unsigned long enable          ; /* 28      - Enable next clock logic (w) */
  unsigned long disable         ; /* 2C      - Disable next clock logic (w) */
  unsigned long Bclear          ; /* 30      - Broadcast: Clear FIFO, logic and counters (w) */
  unsigned long Bnext           ; /* 34      - Broadcast: VME next clock (w) */
  unsigned long Benable         ; /* 38      - Broadcast: Enable next clock logic (w) */
  unsigned long Bdisable        ; /* 3C      - Broadcast: Disable next clock logic (w) */
  unsigned long empty4[4]       ; /* 40-4C not used */
  unsigned long gEncnt1         ; /* 50      - Enable reference pulser channel 1 (w)  */
  unsigned long gDiscnt1        ; /* 54      - Disable reference pulser channel 1 (w) */
  unsigned long empty5[2]       ; /* 58-5C not used */  
  unsigned long reset           ; /* 60      - Reset register (w) */
  unsigned long empty6          ; /* 64 not used */
  unsigned long test            ; /* 68      - test pulse (generate a single pulse) (w) */
  unsigned long empty7[37]      ; /* 6C-FC not used */
  unsigned long fifo[64]        ; /* 100-1FC - read FIFO registers (w) */
 
 } SISReg;
 
