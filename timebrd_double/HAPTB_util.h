/*******************************************
*  HAPTB_util.h                            *
*  header for HAPTB_util, defines          *
*       variables and parameters           *
*                                          *
*    KDP   Dec 2001                        *
********************************************/

#include <vxWorks.h>

/*time board base address (hardware switch: on=0, LSB=12) */
#define TIMEBADDR 0xb4b0

// GLOBALS 
int CODA_RUN_IN_PROGRESS =0;
extern int ITB = 0;

volatile struct vme_happex_tb *tboard[NUMTB]; 

// LOCALS
LOCAL int TB_DEF_DAC12[NUMTB] = {0,0};
LOCAL int TB_DEF_DAC16[NUMTB] = {0,0};
LOCAL int TB_DEF_RAMPDELAY[NUMTB] = {0,0};   
LOCAL int TB_DEF_INTEGTIME[NUMTB] = {13200,13200};
LOCAL int TB_DEF_OVERSAMPLE[NUMTB] = {0,0};
LOCAL int did_init_TB[NUMTB] = {0,0};

LOCAL int LAST_DAC12_TB[NUMTB];
LOCAL int LAST_DAC16_TB[NUMTB];
LOCAL int LAST_OVERSAMPLE_TB[NUMTB];
LOCAL int HAPTB_DAC_LOCK[NUMTB];

