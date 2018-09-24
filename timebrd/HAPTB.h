/*******************************************
*  HAPTB.h                                 *
*  define the time board address map       *     
*                                          * 
*                                          *
*    K.D. Paschke                Sep 2002  *
*    A. Vacheret, R. Michaels.  oct 2000   * 
********************************************/

/* Structure to define memory address map of the time board*/
struct vme_happex_tb {
/* register name          byte offset|| description  */
  volatile unsigned char  data0;      /*  $0  || read only, 0: Data0  */
  volatile unsigned char  data1;      /*      ||            1: Data1  */
           unsigned short dummy1;     /*  $2  || unused */
           unsigned short dummy2;     /*  $4  || unused */
  volatile unsigned short dac12;      /*  $6  || 12-bit DAC value*/   
  volatile unsigned short dac16;      /*  $8  || 16-bit DAC value */
  volatile unsigned short rampdelay;  /*  $A  ||  ramp delay  */   
  volatile unsigned short integtime;  /*  $C  ||  integration time*/   
  volatile unsigned char  osample_c;  /*  $E  || -Currentoversample : read 8-15 */
  volatile unsigned char  osample_w;  /*      || -oversample setting: write 0-7 */
};









