/* Header file for g0vme1 */
/*
 *  pking, 2005oct11:  Corrected the path for the g0vme1.flags file.
 *  Oct 5, 2015    porting to ADAQ
 */

#define INTERNAL_FLAGS "ffile=/adaqfs/home/apar/devices/crl/injector/g0inj.flags,vqwk_verbose=1"

#define NUM_ADCS "nadc"
#define BASE_ADC "badc"
#define VQWK_SAMPLE_PERIOD  "vqwkperiod"
#define VQWK_NUM_BLOCKS     "vqwkblocks"
#define VQWK_SAMP_PER_BLOCK "vqwksamples"
#define VQWK_GATE_DELAY     "vqwkdelay"
#define VQWK_INT_GATE_FREQ  "vqwkgatefreq"
#define VQWK_INTERNAL_MODE  "vqwkinternal"

#define VQWK_VERBOSE_MODE   "vqwk_verbose"


#include "usrstrutils.c"
#include "go_info.c"

#include "myevtypes.h"

typedef unsigned int UINT32;

// scan utilities 
extern int getDataScan(int);
extern int getCleanScan();
extern int setDataScan(int,int);
extern int setCleanScan(int);


