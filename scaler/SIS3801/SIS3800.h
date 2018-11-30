/************************************************************************
 *  SIS3800.h                                                           *
 *   Define the vme address of the multichannels scalers SIS3800        *
 *                                                                      *  
 *                                                                      *
 ************************************************************************/


/* Number of scalers of type SIS3800 */
#define NUMSCALERS 1

/* Addresses on the scalers are adjusted with the thumbwheel switch.
   E.g. ab1 or ce6 are thumbwheel switch settings */
 
long SISRELADDR[NUMSCALERS]={  /* Addresses relative to A24 VME  */
  0xab1000 };

long SISVMEADDR[NUMSCALERS]={   /* Absolute addresses, assigned by sysBusToLocalAdrs */
  0, };                    /* init to zero */


/* define control registers structure*/
typedef struct SIS3800CSREG 
{ /*  register name              offset         description  (w-write, r-read, r/w-read/write)*/
  unsigned long csr             ; /* 00      - Status register(read)/Conrol register(write)  */
  unsigned long IntVerR         ; /* 04      - VME IRQ control register and module identification (r/w) */
  unsigned long empty0          ; /* 08 not used */
  unsigned long disCntR         ; /* 0C      - Copy disable register (w) */
  unsigned long empty1[4]       ; /* 10-1C not used */
  unsigned long clear           ; /* 20      - Clear all counters and overflow bits (w) */
  unsigned long vmeclk          ; /* 24      - VME next clock (w) */
  unsigned long enclk           ; /* 28      - Global count enable (w) */
  unsigned long disclk          ; /* 2C      - Global count disable (w) */
  unsigned long bclear          ; /* 30      - Broadcast: Clear FIFO, logic and counters (w) */
  unsigned long bvmeclk         ; /* 34      - Broadcast: VME next clock (w) */
  unsigned long benclk          ; /* 38      - Broadcast: Global count enable (w) */
  unsigned long bdisclk         ; /* 3C      - Broadcast: Global count Disable  (w) */
  unsigned long clear1_8        ; /* 40      - clear counter group and overflow channel 1-8 (w) */
  unsigned long clear9_16       ; /* 44      - clear counter group and overflow channel 9-16 (w) */
  unsigned long clear17_24      ; /* 48      - clear counter group and overflow channel 17-24 (w) */
  unsigned long clear25_32      ; /* 4C      - clear counter group and overflow channel 25-32 (w) */
  unsigned long gEncnt1         ; /* 50      - Enable reference pulser channel 1 (w)  */
  unsigned long gDiscnt1        ; /* 54      - Disable reference pulser channel 1 (w) */
  unsigned long empty2[2]       ; /* 58-5C not used */  
  unsigned long reset           ; /* 60      - Reset register (w) */
  unsigned long empty3          ; /* 64 not used */
  unsigned long testclkR        ; /* 68      - test pulse (generate a single pulse) (w) */
  unsigned long empty4[37]      ; /* 6C-FC not used */
  unsigned long clrCount[32]    ; /* 100-17C - clear counter N and its overflow bit (w) */
  unsigned long clrOverCount[32]; /* 180-1FC - clear overflow bit of counter N (w) */
  unsigned long readShadow[32]  ; /* 200-27C - Read Shadow register (does not initiate clock shadow) (r) */
  unsigned long readCounter[32] ; /* 280-2FC - Read counter (initiates clock shadow also) (r) */
  unsigned long readClrCount[32]; /* 300-37C - Read and clear all counters (r) */
  unsigned long readOver1_8     ; /* 380     - Overflow register channel 1-8 (r) */
  unsigned long empty5[7]       ; /* 384-39C not used */
  unsigned long readOver9_16    ; /* 3A0     - Overflow register channel 9-16 (r) */
  unsigned long empty6[7]       ; /* 384-39C not used */
  unsigned long readOver17_24   ; /* 3C0     - Overflow register channel 17-24 (r) */
  unsigned long empty7[7]       ; /* 384-39C not used */
  unsigned long readOver25_32   ; /* 3E0     - Overflow register channel 25-32 (r) */

 } SISReg;
 
unsigned long Buffer38001[32];
unsigned long Buffer38002[32];
unsigned long Buffer38003[32];



