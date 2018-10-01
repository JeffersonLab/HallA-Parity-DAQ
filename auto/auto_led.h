/*******************************************
*  auto_led.h                              *
*  header for auto_led, defines            *
*       variables and parameters           *
*                                          *
*    KDP   Aug 2009                        *
********************************************/

#include <vxWorks.h>

// Define global

BOOL auto_led_die = FALSE;

// Define Locals

LOCAL int auto_led_unlock = 0;

// right arm, flags for flexio board 
LOCAL int auto_led_base_enable_flag = 0x01;
LOCAL int auto_led_diff_enable_flag = 0x02;
//LOCAL int auto_led_base_enable_flag = 0x0;  // for test
//LOCAL int auto_led_diff_enable_flag = 0x0;  // for test

// left arm, for the tir board
LOCAL int auto_led_base_flag = 0x08;
LOCAL int auto_led_diff_flag = 0x10;  
LOCAL int auto_led_base_state_left;
LOCAL int auto_led_diff_state_left;
 
// flags for the timing board, DAC12/16
LOCAL int auto_led_base_min = 26500;
LOCAL int auto_led_base_max = 51500;
LOCAL int auto_led_base_def = 26500;
LOCAL int auto_led_base_steps = 20;
LOCAL int auto_led_diff = 380;
LOCAL int auto_led_diff_min = 275;
LOCAL int auto_led_diff_max = 500;
LOCAL int auto_led_diff_def = 380;
LOCAL int auto_led_diff_ntoggle = 15;
LOCAL int auto_led_toggle_time = 1;
LOCAL int auto_cycle_repeat = 100;

LOCAL int auto_led_with_beam=0;
LOCAL int auto_led_in_progress;
LOCAL int auto_led_next_base;
LOCAL int auto_led_next_diff_state;

void print_auto_led();

STATUS checkLockAutoLED();
STATUS unlockAutoLED();
STATUS lockAutoLED();
STATUS initAutoLED();
STATUS init0AutoLED();
int enableDiffAutoLED(int state);
int enableBaseAutoLED(int state);
int toggleDiffAutoLED();
int setBaseAutoLED(int value);
int setDiffAutoLED(int value);
int getBaseAutoLED();
int getDiffAutoLED();
int getStateBaseAutoLED();
int getStateDiffAutoLED();
int requestToggleDiffAutoLED();
STATUS stepAutoLED();
int inProgressAutoLED();

int setStateBeamAutoLED();
int getStateBeamAutoLED();

STATUS cycleAutoLED();
