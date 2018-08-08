/******************************************************************************
*
*  v767Lib.h  -  Driver library header file for readout of a C.A.E.N. multiple
*                Model 775 TDCs using a VxWorks 5.2 or later based single board 
*                computer.
*
*  Author: David Abbott 
*          Jefferson Lab Data Acquisition Group
*          November 2003
*/


#define V767_MAX_CHANNELS   128
#define V767_MAX_WORDS_PER_EVENT  34

/* Define a Structure for access to TDC*/
struct v767_struct {
  volatile unsigned long  fifo;
  volatile unsigned short geoAddr;
  volatile unsigned short bitSet;
  volatile unsigned short bitClear;
  volatile unsigned short intLevel;
  volatile unsigned short intVector;
  volatile unsigned short status1;
  volatile unsigned short control1;
  volatile unsigned short ader32;
  volatile unsigned short ader24;
  volatile unsigned short mcstBaseAddr;
  volatile unsigned short SSReset;
  unsigned short blank0[3];
  volatile unsigned short mcstCtrl;
  unsigned short blank1[19];
  volatile unsigned short status2;
  volatile unsigned short control2;
  volatile unsigned short evCount;
  volatile unsigned short clearCount;
  volatile unsigned short microHandshake;
  volatile unsigned short microReg;
  volatile unsigned short clear;
  volatile unsigned short testH;
  volatile unsigned short testL;
  volatile unsigned short trigger;
  unsigned short blank2[2002];
  volatile unsigned short cr[2048];
  volatile unsigned long data[14335];
};

#define V767_BOARD_ID   0x01e6

/* Define default interrupt vector/level */
#define V767_INT_VEC      0xbb
#define V767_VME_INT_LEVEL   4

/* Register Bits */
#define V767_VME_BUS_ERROR           0x8

#define V767_STATUS1_DATA_READY      0x1
#define V767_STATUS1_GDATA_READY     0x2
#define V767_STATUS1_BUSY            0x4
#define V767_STATUS1_GBUSY           0x8
#define V767_STATUS1_TERM_ON        0x40
#define V767_STATUS1_TERM_OFF       0x80

#define V767_CONTROL1_BLKEND         0x4
#define V767_CONTROL1_NOBLKEND      0xfffb
#define V767_CONTROL1_RESET         0x10
#define V767_CONTROL1_BERR_ENABLE   0x20
#define V767_CONTROL1_BERR_DISABLE  0xffdf


#define V767_STATUS2_EMPTY           0x1
#define V767_STATUS2_FULL            0x2
#define V767_STATUS2_ALMOST_FULL     0x4
#define V767_STATUS2_GERROR          0x8 
#define V767_STATUS2_ERROR_0      0x1000
#define V767_STATUS2_ERROR_1      0x2000
#define V767_STATUS2_ERROR_2      0x4000
#define V767_STATUS2_ERROR_3      0x8000

#define V767_ACQMODE_START_TM        0x1
#define V767_ACQMODE_STOP_TM         0x0
#define V767_ACQMODE_START_GM        0x2
#define V767_ACQMODE_CS              0x3
#define V767_DREADY_MODE_1EVENT      0x0
#define V767_DREADY_MODE_ALMOST_FULL 0x1
#define V767_DREADY_MODE_NOT_EMPTY   0x2
#define V767_TESTMODE               0x10

#define V767_SELADDR_MASK           0x10

#define V767_DATA                0x00000000
#define V767_HEADER_DATA         0x00400000
#define V767_EOB_DATA            0x00200000
#define V767_INVALID_DATA        0x00600000


/* Register Masks */
#define V767_DATA_TYPE_MASK  0x00600000
#define V767_CHANNEL_MASK    0x7f000000
#define V767_EDGE_MASK       0x00100000
#define V767_DATA_MASK       0x000fffff

#define V767_HEADER_EVCOUNT_MASK    0x00000fff
#define V767_HEADER_GEOADDR_MASK    0xf8000000
#define V767_EOB_STATUS_MASK        0x07000000
#define V767_EOB_WORDCOUNT_MASK     0x0000ffff
#define V767_EOB_GEOADDR_MASK       0xf8000000



#define V767_GEOADDR_MASK           0x001f
#define V767_BITSET_MASK            0x0098
#define V767_MCSTBASEADDR_MASK      0x00ff
#define V767_INTLEVEL_MASK          0x0007
#define V767_INTVECTOR_MASK         0x00ff
#define V767_STATUS1_MASK           0x00cf
#define V767_CONTROL1_MASK          0x0034
#define V767_ADER32_MASK            0x00ff
#define V767_ADER24_MASK            0x00ff
#define V767_STATUS2_MASK           0xf00f
#define V767_CONTROL2_MASK          0x001f
#define V767_CONTROL2_ACQMODE_MASK  0x0003
#define V767_CONTROL2_DREADY_MASK   0x000c
#define V767_EVCOUNT_MASK           0x03ff

#define V767_MICRO_WRITEOK       0x0002
#define V767_MICRO_READOK        0x0001


/* Micro Controller OPCodes */
#define V767_CLOCK_PERIOD            25      /* in nanoseconds */

#define SET_WINDOW_WIDTH         0x3000
#define GET_WINDOW_WIDTH         0x3100
#define SET_WINDOW_OFFSET        0x3200
#define GET_WINDOW_OFFSET        0x3300
#define SET_TRIGGER_LATENCY      0x3400
#define GET_TRIGGER_LATENCY      0x3500

#define SET_ACQMODE_STOP_TM      0x1000
#define SET_ACQMODE_START_TM     0x1100
#define SET_ACQMODE_START_GM     0x1200
#define SET_ACQMODE_CS           0x1300

#define SET_DREADY_MODE_1EVENT        0x7000
#define SET_DREADY_MODE_ALMOST_FULL   0x7100
#define SET_DREADY_MODE_NOT_EMPTY     0x7200

#define ENABLE_AUTO_LOAD         0x1800
#define DISABLE_AUTO_LOAD        0x1900
#define SAVE_CONFIG              0x1600

#define EDGEDET_RISE_ALL         0x6000
#define EDGEDET_FALL_ALL         0x6100
#define EDGEDET_BOTH_ALL         0x6600
#define EDGEDET_READ             0x6700
#define ENABLE_OVL_TRG           0x3800
#define DISABLE_OVL_TRG          0x3900
#define WRITE_ENABLE_PATTERN     0x2500

/* Macros */
#define V767_EXEC_MODULE_RESET(ii) {v767p[ii]->SSReset = 1;}
#define V767_EXEC_EVENT_RESET(ii)  {v767p[ii]->clearCount = 1;}
#define V767_EXEC_CLEAR(ii)        {v767p[ii]->clear = 1;}
#define V767_EXEC_TRIGGER(ii)      {v767p[ii]->trigger = 1;}


/* Function Prototypes */
STATUS v767Init (UINT32 addr, UINT32 addr_inc, int nadc, UINT16 crateID);
void   v767Status( int id, int reg, int sflag);

STATUS v767IntConnect (VOIDFUNCPTR routine, int arg, UINT16 level, UINT16 vector);
STATUS v767IntEnable (int id, UINT16 evCnt);
STATUS v767IntDisable (int iflag);

STATUS v767WriteMicro(int id, UINT16 data);

int    v767ReadData(int id, UINT32 *tdata, int maxWords);
int    v767Dready(int id);

void   v767Trig(int id);
void   v767Clear(int id);
void   v767Reset(int id);

int    v767Overflow(int id);
STATUS v767SetAcqMode(int id, int mode);
STATUS v767SetDReadyMode(int id, int mode);
STATUS v767BusErrEnable(int id);
STATUS v767BusErrDisable(int id);
STATUS v767SetBLKEndMode(int id, int mode);
STATUS v767AutoloadEnable(int id);
STATUS v767AutoloadDisable(int id);
STATUS v767SaveConfig(int id);
STATUS v767SetEdgeDetectionMode(int id, int mode);
int    v767GetEdgeDetectionMode(int id);
STATUS v767OverlapTrigEnable(int id);
STATUS v767OverlapTrigDisable(int id);
STATUS v767ChannelMask(int id, UINT16 *tdata );
STATUS v767AddrRotary(int id);
STATUS v767Rates(int id);
STATUS v767RateLoop(int id, int* ntries, int maxtries, int* noverflows, int maxoverflows, int* nhits);








 


