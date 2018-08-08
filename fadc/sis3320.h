/**************************************************************************
*
*  sis3320.h  - Header for SIS 3320 VME Flash ADC Library
*
*
*  Author: Rich Holmes
*          Syracuse University
*          March 2006
*
*  Based on David Abbott's sis3300.h and Strück's sis3320.h
*
* Version 2 accumulator stuff added 12/2007
* API register offsets added 4/2/2008  gbf
*/

#define SIS3320_MAX_BOARDS           8
#define SIS3320_MAX_ADC_CHANNELS     8
#define SIS3320_MAX_ADC_GROUPS       4
#define SIS3320_MAX_EVENTS        1024
#define ADC_2MB                2097152


/* Define Structures for access to ADC Local Memory map*/
struct adcGroup_struct
{
  volatile unsigned int eventConfig;           /* 0x0??00000 */
  volatile unsigned int sampLength;            /* 0x0??00004 */
  volatile unsigned int sampStart;             /* 0x0??00008 */
  volatile unsigned int adcInputMode;          /* 0x0??0000C */
  volatile unsigned int nextSampAdr[2];        /* 0x0??00010 */
  /* these next 2 are for vers.1 of accum (Sept 2006) */
  volatile unsigned int accumThresh1;          /* 0x0??00018 */
  volatile unsigned int accumThresh2;          /* 0x0??0001C */
  volatile unsigned int actSampVal;            /* 0x0??00020 */
  volatile unsigned int testReg;               /* 0x0??00024 */
  volatile unsigned int ddr2MemLogVerify;      /* 0x0??00028 */
  volatile unsigned int trigFlagClearCtr;      /* 0x0??0002C */
  volatile unsigned int trigReg[2][2];         /* 0x0??00030 */
  volatile unsigned int adcSPI;                /* 0x0??00040 */
  unsigned int blank2[3];                      /* 0x0??00044 */

/* Accum thresholds and N5_N6_before_after for vers. 2 of accum (Dec 2007) */
  volatile unsigned int accThresh1;            /* 0x0??00050 */
  volatile unsigned int n5n6befaft1;           /* 0x0??00054 */
  volatile unsigned int accThresh2;            /* 0x0??00058 */
  volatile unsigned int n5n6befaft2;           /* 0x0??0005C */
  unsigned int blank3[16360];                  /* 0x0??00060 */
  volatile unsigned int eventDir[2][512];      /* 0x0??10000 */
  unsigned int blank4[2079744];                /* 0x0??00000 */
};  


struct sis3320_struct
{
  volatile unsigned int csr;                   /* 0x00000000 */
  volatile unsigned int id;                    /* 0x00000004 */
  volatile unsigned int intConfig;             /* 0x00000008 */
  volatile unsigned int intControl;            /* 0x0000000C */
  volatile unsigned int acqCsr;                /* 0x00000010 */
  volatile unsigned int extStartDelay;         /* 0x00000014 */
  volatile unsigned int extStopDelay;          /* 0x00000018 */
  unsigned int blank1;                         /* 0x0000001C */
  volatile unsigned int maxNofEvReg;           /* 0x00000020 */
  volatile unsigned int actEvtCounter;         /* 0x00000024 */
  unsigned int blank2[2];                      /* 0x00000028 */
  volatile unsigned int cbltBxReg;             /* 0x00000030 */
  volatile unsigned int adcMemPageReg;         /* 0x00000034 */
  unsigned int blank3[6];                      /* 0x00000038 */
  volatile unsigned int dacCsr;                /* 0x00000050 */
  volatile unsigned int dacDataReg;            /* 0x00000054 */
  volatile unsigned int adcGainReg;            /* 0x00000058 */
  unsigned int blank4;                         /* 0x0000005C */
  volatile unsigned int xilJtagTestDataIn;     /* 0x00000060 */
  volatile unsigned int xilJtagControl;        /* 0x00000064 */
  unsigned int blank5[230];                    /* 0x00000068 */
  volatile unsigned int reset;                 /* 0x00000400 */
  unsigned int blank6[3];                      /* 0x00000404 */
  volatile unsigned int armSampLogic;          /* 0x00000410 */
  volatile unsigned int disarmSampLogic;       /* 0x00000414 */
  volatile unsigned int startSample;           /* 0x00000418 */
  volatile unsigned int stopSample;            /* 0x0000041C */
  unsigned int blank7[2];                      /* 0x00000420 */
  volatile unsigned int resetDdr2MemLogic;     /* 0x00000428 */
  unsigned int blank8[4194037];                /* 0x0000042C */
  volatile unsigned int eventConfig;           /* 0x01000000 */
  volatile unsigned int sampLength;            /* 0x01000004 */
  volatile unsigned int sampStart;             /* 0x01000008 */
  volatile unsigned int adcInputMode;          /* 0x0100000C */
  unsigned int blank9[4194300];                /* 0x01000010 */

  volatile struct adcGroup_struct adcG[4];     /* 0x02000000 */
/* Ea. word = 4 Bytes, and there are 8 channels * 2MByte(array) --> total 8 Mbytes */
  volatile unsigned int adcData[8*ADC_2MB];    /* 0x04000000 */   
};


#define SIS3320_BOARD_ID       0x33200000

#define SIS3320_BOARD_ID_MASK  0xffff0000
#define SIS3320_MAJOR_REV_MASK 0x0000ff00
#define SIS3320_MINOR_REV_MASK 0x000000ff


/* Define default interrupt vector/level */
#define SIS3320_VME_INT_VEC     0xba
#define SIS3320_VME_INT_LEVEL   4

/* Control/Status Register bits */
#define SIS3320_STATUS_USER_LED            0x1

#define SIS3320_SET_USER_LED               0x1

#define SIS3320_CLEAR_USER_LED         0x10000

/* Aquisition Control register bits */
#define SIS3320_ENABLE_AUTOSTART                0x10
#define SIS3320_ENABLE_MULTIEVENT               0x20
#define SIS3320_ENABLE_INT_TRIGGER_STOP         0x40

#define SIS3320_ENABLE_FP_STARTSTOP_MODE       0x100

#define SIS3320_ADC_SAMPLING_ARMED           0x10000
#define SIS3320_ADC_SAMPLING_BUSY            0x20000

#define SIS3320_DISABLE_AUTOSTART           0x100000
#define SIS3320_DISABLE_MULTIEVENT          0x200000
#define SIS3320_DISABLE_INT_TRIGGER_STOP    0x400000

#define SIS3320_DISABLE_FP_STARTSTOP_MODE  0x1000000

/* Define Bit Masks */

#define SIS3320_INT_VEC_MASK        0x000000ff
#define SIS3320_INT_LEVEL_MASK      0x00000700
#define SIS3320_INT_ENABLE_MASK     0x00000800
#define SIS3320_INT_MODE_MASK       0x00001000

#define SIS3320_INT_SOURCE_ENABLE_MASK  0x0000000f

#define SIS3320_PAGESIZE_MASK       0x0000000f
const sis3320_pagesize[16] = {0x1000000, 0x400000, 0x100000, 0x40000,
			        0x10000,   0x4000,   0x1000,   0x400, 
			          0x200,    0x100,     0x80,    0x40,
                                    0x0,      0x0,      0x0,     0x0};

#define SIS3320_CLOCKSOURCE_MASK    0x00007000
const int   sis3320_clocksource[] = {100000000, 50000000, 25000000, 12500000, 
                                    6250000, 3125000, -1, -1};
const char *sis3320_clksrc_string[] = {"100 MHz", "50 MHz", "25 MHz", "12.5 MHz", 
                                      "6.25 MHz", "3.125 MHz", "External", "P2"};

#define SIS3320_ADC_SAMPLE_OFFSET_MASK    0x00000003
#define SIS3320_ADC_ADDR_OFFSET_MASK      0x01fffffc

#define SIS3320_ADC_HIDATA_MASK           0x0fff0000
#define SIS3320_ADC_LODATA_MASK           0x00000fff
#define SIS3320_ADC_BOTHDATA_MASK         0x0fff0fff
#define SIS3320_ADC_LOWORD_MASK           0x0000ffff

#define SIS3320_EDIR_END_ADDR_MASK        0x00ffffff
#define SIS3320_EDIR_WRAP                 0x10000000
#define SIS3320_EDIR_TRIG                 0x20000000

#define SIS3320_TDIR_END_ADDR_MASK        0x0001ffff
#define SIS3320_TDIR_WRAP                 0x00080000
#define SIS3320_TDIR_TMASK                0xff000000


/* Define some macros */
#define SIS3320_IRQ_ENABLE(id)          (sis3320p[id]->intConfig |= (SIS3320_INT_ENABLE_MASK))
#define SIS3320_IRQ_DISABLE(id)         (sis3320p[id]->intConfig &= ~(SIS3320_INT_ENABLE_MASK))

#define SIS3320_CONTROL_STATUS             0x0      /* read/write; D32 */
#define SIS3320_MODID                      0x4      /* read only; D32 */
#define SIS3320_IRQ_CONFIG                 0x8      /* read/write; D32 */
#define SIS3320_IRQ_CONTROL                0xC      /* read/write; D32 */

#define SIS3320_ACQUISTION_CONTROL        0x10      /* read/write; D32 */
#define SIS3320_START_DELAY               0x14      /* read/write; D32 */
#define SIS3320_STOP_DELAY                0x18      /* read/write; D32 */

#define SIS3320_MAX_NOF_EVENT             0x20      /* read/write; D32 */
#define SIS3320_ACTUAL_EVENT_COUNTER      0x24      /* read; D32 */

#define SIS3320_CBLT_BROADCAST_SETUP      0x30      /* read/write; D32 */
#define SIS3320_ADC_MEMORY_PAGE_REGISTER  0x34      /* read/write; D32 */

#define SIS3320_DAC_CONTROL_STATUS        0x50      /* read/write; D32 */
#define SIS3320_DAC_DATA                  0x54      /* read/write; D32 */
#define SIS3320_ADC_GAIN_CONTROL          0x58      /* read/write; D32 */

#define SIS3320_KEY_RESET                 0x400     /* write only; D32 */
#define SIS3320_KEY_ARM                   0x410     /* write only; D32 */
#define SIS3320_KEY_DISARM                0x414     /* write only; D32 */
#define SIS3320_KEY_START                 0x418     /* write only; D32 */
#define SIS3320_KEY_STOP                  0x41C     /* write only; D32 */

#define SIS3320_KEY_RESET_DDR2_LOGIC      0x428     /* write only; D32 */

#define SIS3320_EVENT_CONFIG_ALL_ADC                0x01000000    
#define SIS3320_SAMPLE_LENGTH_ALL_ADC               0x01000004    
#define SIS3320_SAMPLE_START_ADDRESS_ALL_ADC        0x01000008    
#define SIS3320_ADC_INPUT_MODE_ALL_ADC              0x0100000C    

#define SIS3320_TRIGGER_FLAG_CLR_CNT_ALL_ADC        0x0100002C      

#define SIS3320_ADC_INPUT_MODE_ADC12                0x0200000C    
#define SIS3320_ADC_INPUT_MODE_ADC34                0x0280000C    
#define SIS3320_ADC_INPUT_MODE_ADC56                0x0300000C    
#define SIS3320_ADC_INPUT_MODE_ADC78                0x0380000C    

#define SIS3320_ACTUAL_SAMPLE_ADDRESS_ADC1          0x02000010    
#define SIS3320_ACTUAL_SAMPLE_ADDRESS_ADC2          0x02000014    
#define SIS3320_ACTUAL_SAMPLE_ADDRESS_ADC3          0x02800010    
#define SIS3320_ACTUAL_SAMPLE_ADDRESS_ADC4          0x02800014    
#define SIS3320_ACTUAL_SAMPLE_ADDRESS_ADC5          0x03000010    
#define SIS3320_ACTUAL_SAMPLE_ADDRESS_ADC6          0x03000014    
#define SIS3320_ACTUAL_SAMPLE_ADDRESS_ADC7          0x03800010    
#define SIS3320_ACTUAL_SAMPLE_ADDRESS_ADC8          0x03800014    

#define SIS3320_ACTUAL_SAMPLE_VALUE_ADC12           0x02000020    
#define SIS3320_ACTUAL_SAMPLE_VALUE_ADC34           0x02800020    
#define SIS3320_ACTUAL_SAMPLE_VALUE_ADC56           0x03000020    
#define SIS3320_ACTUAL_SAMPLE_VALUE_ADC78           0x03800020

#define SIS3320_ADC_SPI_ADC12                       0x02000040
#define SIS3320_ADC_SPI_ADC34                       0x02800040
#define SIS3320_ADC_SPI_ADC56                       0x03000040
#define SIS3320_ADC_SPI_ADC78                       0x03800040

#define SIS3320_DDR2_TEST_REGISTER_ADC12            0x02000028      
#define SIS3320_TRIGGER_FLAG_CLR_CNT_ADC12          0x0200002C      

#define SIS3320_TRIGGER_SETUP_ADC1                  0x02000030    
#define SIS3320_TRIGGER_THRESHOLD_ADC1              0x02000034    
#define SIS3320_TRIGGER_SETUP_ADC2                  0x02000038    
#define SIS3320_TRIGGER_THRESHOLD_ADC2              0x0200003C    

#define SIS3320_EVENT_DIRECTORY_ADC1                0x02010000    /* read only; D32, BLT32, MBLT64; size: 512Lwords 0x800 Bytes */
#define SIS3320_EVENT_DIRECTORY_ADC2                0x02018000    /* read only; D32, BLT32, MBLT64; size: 512Lwords 0x800 Bytes */

#define SIS3320_DDR2_TEST_REGISTER_ADC34            0x02800028      
#define SIS3320_TRIGGER_FLAG_CLR_CNT_ADC34          0x0280002C      

#define SIS3320_TRIGGER_SETUP_ADC3                  0x02800030    
#define SIS3320_TRIGGER_THRESHOLD_ADC3              0x02800034    
#define SIS3320_TRIGGER_SETUP_ADC4                  0x02800038    
#define SIS3320_TRIGGER_THRESHOLD_ADC4              0x0280003C    

#define SIS3320_EVENT_DIRECTORY_ADC3                0x02810000    /* read only; D32, BLT32, MBLT64; size: 512Lwords 0x800 Bytes */
#define SIS3320_EVENT_DIRECTORY_ADC4                0x02818000    /* read only; D32, BLT32, MBLT64; size: 512Lwords 0x800 Bytes */


#define SIS3320_DDR2_TEST_REGISTER_ADC56            0x03000028      
#define SIS3320_TRIGGER_FLAG_CLR_CNT_ADC56          0x0300002C      
#define SIS3320_TRIGGER_SETUP_ADC5                  0x03000030    
#define SIS3320_TRIGGER_THRESHOLD_ADC5              0x03000034    
#define SIS3320_TRIGGER_SETUP_ADC6                  0x03000038    
#define SIS3320_TRIGGER_THRESHOLD_ADC6              0x0300003C    
#define SIS3320_EVENT_DIRECTORY_ADC5                0x03010000    /* read only; D32, BLT32, MBLT64; size: 512Lwords 0x800 Bytes */
#define SIS3320_EVENT_DIRECTORY_ADC6                0x03018000    /* read only; D32, BLT32, MBLT64; size: 512Lwords 0x800 Bytes */


#define SIS3320_DDR2_TEST_REGISTER_ADC78            0x03800028      
#define SIS3320_TRIGGER_FLAG_CLR_CNT_ADC78          0x0380002C      
#define SIS3320_TRIGGER_SETUP_ADC7                  0x03800030    
#define SIS3320_TRIGGER_THRESHOLD_ADC7              0x03800034    
#define SIS3320_TRIGGER_SETUP_ADC8                  0x03800038    
#define SIS3320_TRIGGER_THRESHOLD_ADC8              0x0380003C    
#define SIS3320_EVENT_DIRECTORY_ADC7                0x03810000    /* read only; D32, BLT32, MBLT64; size: 512Lwords 0x800 Bytes */
#define SIS3320_EVENT_DIRECTORY_ADC8                0x03818000    /* read only; D32, BLT32, MBLT64; size: 512Lwords 0x800 Bytes */

#define SIS3320_ADC1_OFFSET                         0x04000000    
#define SIS3320_ADC2_OFFSET                         0x04800000    
#define SIS3320_ADC3_OFFSET                         0x05000000    
#define SIS3320_ADC4_OFFSET                         0x05800000    
#define SIS3320_ADC5_OFFSET                         0x06000000    
#define SIS3320_ADC6_OFFSET                         0x06800000    
#define SIS3320_ADC7_OFFSET                         0x07000000    
#define SIS3320_ADC8_OFFSET                         0x07800000    

#define SIS3320_NEXT_ADC_OFFSET                     0x00800000    

/* define sample clock */
#define SIS3320_ACQ_SET_CLOCK_TO_250MHZ             0x70008000  
#define SIS3320_ACQ_SET_CLOCK_TO_200MHZ             0xf0000000  /* default after Reset  */
#define SIS3320_ACQ_SET_CLOCK_TO_100MHZ             0x60001000
#define SIS3320_ACQ_SET_CLOCK_TO_50MHZ              0x50002000
#define SIS3320_ACQ_SET_CLOCK_TO_LEMO_CLOCK_IN      0x10006000
#define SIS3320_ACQ_SET_CLOCK_TO_P2_CLOCK_IN        0x00007000

#define SIS3320_ACQ_DISABLE_LEMO_START_STOP         0x01000000
#define SIS3320_ACQ_ENABLE_LEMO_START_STOP          0x00000100

#define SIS3320_ACQ_DISABLE_INTERNAL_TRIGGER        0x00400000
#define SIS3320_ACQ_ENABLE_INTERNAL_TRIGGER         0x00000040

#define SIS3320_ACQ_DISABLE_MULTIEVENT              0x00200000
#define SIS3320_ACQ_ENABLE_MULTIEVENT               0x00000020

#define SIS3320_ACQ_DISABLE_AUTOSTART               0x00100000
#define SIS3320_ACQ_ENABLE_AUTOSTART                0x00000010

/* bits of SIS3320_Event Configuration register ; D-register*/
/* Note: changed from Strück version by prepending "SIS3320_". */
#define SIS3320_EVENT_CONF_ENABLE_SAMPLE_LENGTH_STOP        0x20
#define SIS3320_EVENT_CONF_ENABLE_WRAP_PAGE_MODE            0x10

#define SIS3320_EVENT_CONF_PAGE_SIZE_16M_WRAP               0x0
#define SIS3320_EVENT_CONF_PAGE_SIZE_4M_WRAP                0x1
#define SIS3320_EVENT_CONF_PAGE_SIZE_1M_WRAP                0x2
#define SIS3320_EVENT_CONF_PAGE_SIZE_256K_WRAP              0x3

#define SIS3320_EVENT_CONF_PAGE_SIZE_64K_WRAP               0x4
#define SIS3320_EVENT_CONF_PAGE_SIZE_16K_WRAP               0x5
#define SIS3320_EVENT_CONF_PAGE_SIZE_4K_WRAP                0x6
#define SIS3320_EVENT_CONF_PAGE_SIZE_1K_WRAP                0x7

#define SIS3320_EVENT_CONF_PAGE_SIZE_512_WRAP               0x8
#define SIS3320_EVENT_CONF_PAGE_SIZE_256_WRAP               0x9
#define SIS3320_EVENT_CONF_PAGE_SIZE_128_WRAP               0xA
#define SIS3320_EVENT_CONF_PAGE_SIZE_64_WRAP                0xB

/* These are sizes of pages (in samples) enabled by above.  (32M
   corresponds to entire memory, page wrapping disabled).  *=2 to get
   size in bytes, /=2 to get size in lwords.
 
   Added by RSH 3/06 */
#define SIS3320_32M           0x2000000
#define SIS3320_16M           0x1000000
#define SIS3320_4M            0x400000
#define SIS3320_1M            0x100000
#define SIS3320_256K          0x40000
#define SIS3320_64K           0x10000
#define SIS3320_16K           0x4000
#define SIS3320_4K            0x1000
#define SIS3320_1K            0x400
#define SIS3320_512           0x200
#define SIS3320_256           0x100
#define SIS3320_128           0x80
#define SIS3320_64            0x40

#define SIS3320_EVENT_CONF_ENABLE_USER_IN_TO_DATA          0x1000
#define SIS3320_EVENT_CONF_ENABLE_USER_IN_ACCUM_GATE       0x0400
#define SIS3320_EVENT_CONF_SHIFT_DOWN_ACCUM                0x0200
#define SIS3320_EVENT_CONF_ENABLE_ACCUMULATOR              0x0100

#define SIS3320_ENABLE_ADC_TEST_DATA                       0x010000
#define SIS3320_ADC_TEST_DATA_16BIT_MODE                   0x000000
#define SIS3320_ADC_TEST_DATA_32BIT_MODE                   0x100000

