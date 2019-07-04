/* scaler1.h  R.Michaels   Aug 97
   Define 3 types of scaler and total number of scalers.
   The distribution of how many of each kind and what their
   addresses is specified in scaler2.h */
/* NOTE: The SIS3800 and SIS3801 are NOT defined here... */

struct lecroy_1151_scaler {
    unsigned short reset;
    unsigned short blank1[7];
    unsigned short bim[8];
    unsigned short blank2[16];
    unsigned long preset[16];
    unsigned long scaler[16];
    unsigned short blank3[29];
    unsigned short id[3];
  };


struct str_7200_scaler {
/*  register name              offset         description  (w-write, r-read, r/w-read/write)*/
  unsigned long CSR             ; /* 00      - Status register(read)/Conrol registe(write)  */
  unsigned long IntVerR         ; /* 04      - VME IRQ and version  register (r/w) */
  unsigned long empty0          ; /* 08 not used */
  unsigned long disCntR         ; /* 0C      - Count disable register (w) */
  unsigned long empty1          ; /* 10 not used */
  unsigned long clrFIFOR        ; /* 14      - reset FIFO register (w)  (not used this version )*/
  unsigned long wrToFIFOR       ; /* 18      - write shadow to FIFO register (not used this version ) (w) */
  unsigned long empty2          ; /* 1C not used */
  unsigned long clrR            ; /* 20      - Clear all counter and overflow bits register (w) */
  unsigned long clkShadowR      ; /* 24      - clock shadow register (w) */
  unsigned long gEnCntR         ; /* 28      - global count enable register (w) */
  unsigned long gDisCntR        ; /* 2C      - global count disable register (w) */
  unsigned long bclrCntR        ; /* 30      - Broadcast:Clear all counter and overflow bits register (w) */
  unsigned long bclkShadowR     ; /* 34      - Broadcast: clock shadow register (w) */
  unsigned long bgEnCntR        ; /* 38      - Broadcast:global count enable register (w) */
  unsigned long bgDisCntR       ; /* 3C      - Broadcast: global count disable register (w) */
  unsigned long grClrR[4]       ; /* 40-4C   - clear counter group i=0:ch1-ch8; i=1:ch8-ch16 ... (w) */
  unsigned long empty3[4]       ; /* 50-5C not used */
  unsigned long resetR          ; /* 60      -  reset register (w) */
  unsigned long empty4          ; /* 64 not used */
  unsigned long testclkR        ; /* 68      - test clock register (w) */
  unsigned long empty5[5]       ; /* 6C-7C not used */
  unsigned long readFIFOR       ; /* 80      - read FIFO register (w) */
  unsigned long empty6[31]      ; /* 84-FC not used */
  unsigned long clrCntR[32]     ; /* 100-17C - clear counter i(0-31) and its overflow bit (w)   */
  unsigned long clrOverR[32]    ; /* 180-1FC - clear overflow bit of counter i(0-31) (w) */
  unsigned long readShR[32]     ; /* 200-27C - read shadow  registers (r) */
  unsigned long readCntR[32]    ; /* 280-2FC - read counter i(0-31)  registers (r) */
  unsigned long readClrCntR[32] ; /* 300-37C - read and clear counter i(0-31)  registers (r) */
  unsigned long grOverR[4]      ; /* 380-3E0 - overflow  registers i=0:ch1-ch8; i=1:ch8-ch16 ... (r) */
 };
 
struct caen_v560_scaler {
/*  register name          offset   description  (w-write, r-read, r/w-read/write)*/
  unsigned short empty1[2]; /* 00-02 - not used address */
  unsigned short IntVectR ; /* 04    - interrupt Vector register (r/w) */
  unsigned short IntLevR  ; /* 06    - interrupt Level&VETO register (r/w) */
  unsigned short EnIntR   ; /* 08    - Enable VME interrupt register (r/w) */
  unsigned short DisIntR  ; /* 0A    - Disable VME interrupt register (r/w) */
  unsigned short ClrIntR  ; /* 0C    - Clear VME interrupt register (r/w) */
  unsigned short RequestR ; /* 0E    - Request register (r/w) */
  unsigned long  CntR[16] ; /* 10-4C - Counter 0 - 15 (r) */
  unsigned short ClrR     ; /* 50    - Scale Clear register (r/w) */
  unsigned short setVETOR ; /* 52    - VME VETO set register (r/w) */
  unsigned short clrVETOR ; /* 54    - VME VETO reset register (r/w) */
  unsigned short incCntR  ; /* 56    - Scale Increase register (r/w) */
  unsigned short StatusR  ; /* 58    - Scale Status register (r/w) */
  unsigned short empty2[80]; /* 5A-F8 - not used address */
  unsigned short FixCodeR ; /* FA    - Fixed Code  register (r) */
  unsigned short ModTypeR ; /* FC    - Manufacturer&Module Type  register (r) */
  unsigned short VersionR ; /* FE    - Version&Series  register (r) */
 };
 

/* NUMSCALER = number of scaler units  
   NUMBLOCKS = number of 16-channel blocks = 
     num_1151_scalers + num_v560_scalers 
           + 2*num_str7200_scalers 
   These two must be consistent and aligned */
#define NUMSCALER 1
#define NUMBLOCKS 2

#define MAXBLOCKS 20
#define MAXSCALER 10
int vmeoff[MAXSCALER];
int vmeheader[MAXSCALER];
int scalertype[MAXSCALER];
static struct lecroy_1151_scaler *slecroy1151[MAXSCALER];
static struct str_7200_scaler *sstr7200[MAXSCALER];
static struct caen_v560_scaler *scaen560[MAXSCALER];






