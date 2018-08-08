/*******************************************
*  HAPTB_util.h                            *
*  header for HAPTB_util, defines          *
*       variables and parameters           *
*                                          *
*    KDP   Dec 2001                        *
********************************************/

#include <vxWorks.h>

/*time board base address (hardware switch: on=0, LSB=12) */
#define TIMEBADDR 0xf0f0

// GLOBALS 
int CODA_RUN_IN_PROGRESS =0;

volatile struct vme_happex_tb *tboard; 

// LOCALS
LOCAL int TB_DEF_DAC12   =    0;
LOCAL int TB_DEF_DAC16   =    0;
LOCAL int TB_DEF_RAMPDELAY =  0;   
LOCAL int TB_DEF_INTEGTIME = 13200;
LOCAL int TB_DEF_OVERSAMPLE=    0;

LOCAL int did_init_TB  = 0;
LOCAL int LAST_DAC12_TB;
LOCAL int LAST_DAC16_TB;
LOCAL int LAST_OVERSAMPLE_TB;
LOCAL int HAPTB_ADDR;
LOCAL int HAPTB_DAC_LOCK;

