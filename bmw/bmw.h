/* bmw.h - include file for beam modulation works */

/*
  $Header: /group/halla/parity/cvs/devices/bmw/bmw.h,v 1.2 2010/03/10 17:07:47 paschke Exp $
 */

/***** includes *****/
#include <vxWorks.h>
#include <stdio.h>
#include "../caFFB/caFFB.h"
#include "/adaqfs/home/apar/devices/flexio/flexioLib.h"

/***** defines *****/

/* Size of names for bmwClient */
#define MAX_SEQS         8             /* maximum number of sequences */
#define MAX_OBJS         8               /* maximum number of objects */
#define Max_Amplitude  200   /* absolute highest amplitude allowed */

/***** typedefs *****/
struct bmwSequence {
  int active;
  int periods;
  int nobj;
  int coil_1;
  int coil_2;
  int amp_1;
  int amp_2;
  int freq;
};

/***** locals *****/
LOCAL int                 bmw_IsInit = 0;
LOCAL int                 bmw_trig = 2;
LOCAL int                 bmw_flag_1 = 3;
LOCAL int                 bmw_flag_2 = 4;
LOCAL int                 bmw_flag_3 = 5;

// Definitions of up to 8 cycles
LOCAL struct bmwSequence bmw_seq[MAX_SEQS];

LOCAL int bmw_seq_coil_1[MAX_SEQS] = {   1,   2,   3,   4,   5,   6,   7,   8};
//LOCAL int bmw_seq_coil_1[MAX_SEQS] = {   1,   1,   1,   1,   1,   1,   1,   1};
LOCAL int bmw_seq_coil_2[MAX_SEQS] = {   0,   0,   0,   0,   0,   0,   0,   0};
//LOCAL int bmw_seq_amp_1[MAX_SEQS]  = {  1,  2,  3,  4,  5,  6,  7,  8};
//LOCAL int bmw_seq_amp_1[MAX_SEQS]  = {  100,  100,  100,  100,  100,  100,  100,  100};
LOCAL int bmw_seq_amp_1[MAX_SEQS]  = {  70,  100,  70,  100,  70,  70,  100, 70};
//LOCAL int bmw_seq_amp_1[MAX_SEQS]  = {  10,  10,  10,  10,  10,  10,  10,  10};
LOCAL int bmw_seq_amp_2[MAX_SEQS]  = {   0,   0,   0,   0,   0,   0,   0,   0};
LOCAL int bmw_seq_periods[MAX_SEQS]= {  50,  50,  50,  50,  50,  50, 50,  50};
//LOCAL int bmw_seq_periods[MAX_SEQS]= {  20,  20,  20,  20,  20,  20, 20,  20};
LOCAL int bmw_seq_activated[MAX_SEQS]= { 1,   1,   1,   1,   1,   1,   1,   1};

LOCAL int bmw_energy_channel = 8;  // channel number that maps to energy in caFFB

/***** globals *****/

// Global frequency
int bmw_dither_frequency = 15;

// manage RF lock issues,
int bmw_RFlock_in_HallC = 1;
int bmw_RFlock_in_HallA = 0;

//  bmw diagnostic message level
BOOL bmw_c_verbose = 1;                /* 1 to get lots of diagnostics */
BOOL bmw_c_terse = 0;                  /* 1 to get a few diagnostics */
BOOL bmw_test=0;

/* These globals are used to communicate between the client and the ROC task. */
int   bmw_arm_trig=0;
int   bmw_seq_ind = -1;             /* current Sequence index */
BOOL  bmw_clean = FALSE;           /* TRUE if this sequence should have been triggered */

int bmw_object=-1;
int bmw_freq=0;
int bmw_phase=0;
int bmw_period=0;
int bmw_phase_num=0;
int bmw_phase_cnt=-1;

int   bmw_cycle_count = 0;            // A running count of dithering cycles
int   bmw_cycle_number = -1;               // Holds cycle number, or zero if cycle is over

/* This global flag is used to communicate the desired function */
int bmw_flight_plan = 0;

/* This global flag is used to communicate a death-wish from users to
   the otherwise infinite bmw loop */
BOOL  bmw_die_die_die = FALSE; 

/* Global flag used to keep the total number of Clients to 1, and to
   provide status for external programs */
int  bmw_alive = 0; 

// dummy variable, so that vxworks crates does not crash
int  bmw_status = 0; 

// globals used only to communicate with test
int bmw_test_object=1;
int bmw_test_value =0;

// I'm pretty sure that the clock ticks at 72 Hz
int bmw_ticks_per_step   =   72;  /* pause counts in steps to allow check of die word */
int bmw_steps_per_pause  =   480; /* Number of steps to rest after last object, 480 sec = 8 min*/
int bmw_FFBpause_wait    =   1*72; /* number of steps before cycle for FFB pause = 1 sec */
int bmw_FFBresume_wait   =   3*72; /* number of ticks after cycle for FFB resume  = 3 sec*/
int bmw_config_wait      =   1*72; /* number of ticks to allow function generator configuration  1 sec */
int bmw_arm_wait         =   24;  /* number of ticks to allow function generator to arm  1/3 sec */

/***** forward declarations *****/
STATUS bmwClient();
STATUS bmwClient_script();
STATUS modulateObjectBMW( int object, int server );
STATUS setNotice(int flag, int val);

STATUS initBMW();
STATUS defineSeqBMW(int iseq, int coil, int amp, int nperiods);
STATUS defineSeq2CoilBMW(int iseq, int coil, int amp, int coil2, int amp2, int ncycles);
STATUS activateSeqBMW(int iseq, int active);
STATUS getConfigBMW();
STATUS getConfigSeqBMW(int iseq);
STATUS trigBMW(int trig);
STATUS printSeqBMW(int iseq);
