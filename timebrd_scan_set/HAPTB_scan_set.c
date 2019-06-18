/*****************************************
*  HAPTB.c                               *
*  routines for performing timing scans  * 
*  with a timing board for the VQWK ADCs *
*                                        *
*  C. Clarke                   May 2019  *
*  K.D. Paschke                Sep 2002  *
*  R. Michaels, A. Vacheret    oct 2000  * 
******************************************/

#include <vxWorks.h>
#include <stdio.h>
#include <stdlib.h>
#include "usrstrutils.c"

// Need methods to 
// Set initial and final ramp delay, int time, number of steps, step size, step time, loop N times and then do the execution
// Methods to set the values independently or all at once but without looping (so user can start it at their own time)
// Methods to ask current settings for individual and all at once, giving TB units and time us units
LOCAL int TB_SCAN_RAMPDELAYINITIAL = 0;   
LOCAL int TB_SCAN_RAMPDELAYFINAL   = 40;   
LOCAL int TB_SCAN_INTEGTIME = 40;
LOCAL int TB_SCAN_STEPLIMIT = 0;       // 0 step default, meaning no step limit
LOCAL int TB_SCAN_STEPN     = 0;       // initialize at 0
LOCAL int TB_SCAN_STEPSIZE  = 1;       // 1 HAPTB unit step size defualt
LOCAL double TB_SCAN_STEPTIME  = 1.0;       // 1 second sleep default
LOCAL int TB_SCAN_LOOPN     = 0;       // initialize at 0
LOCAL int TB_SCAN_LOOPLIMIT = 1;       // 1 loop default, meaning 1 loop limit
LOCAL int TB_SCAN_DONE = 0;            // Routine complete flag
LOCAL int LAST_SCAN_RAMPDELAY_TB;
LOCAL int LAST_SCAN_INTEGTIME_TB;

LOCAL int ORIG_RAMPDELAY_TB = 23;
LOCAL int ORIG_INTEGTIME_TB = 3000;
char* FLAGS_FILE_TB = "/adaqfs/home/apar/devices/crl/vqwkTiming.flags";
char  flagCrateName[255];
char  flagDelayName[255];
char  flagIntTimeName[255];
/*char *tb_flags_file = "/adaqfs/home/apar/devices/crl/vqwkTiming.flags";*/

#define HAPTB_DELAY_PATTERN(x) "HAPTB_delay_"x

#define HAPTB_RAMP_DELAY_CH       "HAPTB_delay_CH"
#define HAPTB_RAMP_DELAY_INJ      "HAPTB_delay_INJ"
#define HAPTB_RAMP_DELAY_RHRS     "HAPTB_delay_RHRS"
#define HAPTB_RAMP_DELAY_LHRS     "HAPTB_delay_LHRS"
#define HAPTB_INTEGRATE_TIME_CH   "HAPTB_int_time_CH"
#define HAPTB_INTEGRATE_TIME_INJ  "HAPTB_int_time_INJ"
#define HAPTB_INTEGRATE_TIME_RHRS "HAPTB_int_time_RHRS"
#define HAPTB_INTEGRATE_TIME_LHRS "HAPTB_int_time_LHRS"

/*********************************************************
 * Green Monster based control, with defaults in flags
 * file called on reboot
 *
 * Assume that the user will set the all the timing boards
 * they care about at the right times with correct passed 
 * values, set those to the TB using the given method
 *********************************************************/

int withDefaults_setTimeHAPTB_for_VQWK_crate(char *userCrate){
  strcpy(flagDelayName,"HAPTB_delay_");
  strncat(flagDelayName,userCrate,200);
  strcpy(flagIntTimeName,"HAPTB_int_time_");
  strncat(flagIntTimeName,userCrate,200);

  load_strings_from_file(FLAGS_FILE_TB);
  if (getflag(flagDelayName)==2 && getflag(flagIntTimeName)==2){
    ORIG_RAMPDELAY_TB = getint(flagDelayName);
    ORIG_INTEGTIME_TB = getint(flagIntTimeName);
    printf("Setting HAPPEX Timing Board in Crate: %s, delay: %d, int time: %d\n",FLAGS_FILE_TB,ORIG_RAMPDELAY_TB,ORIG_INTEGTIME_TB);
    return setTimeHAPTB(ORIG_RAMPDELAY_TB,ORIG_INTEGTIME_TB);
  }
  else {
    printf("The crate values requested were not found in the flags file: %s\n",FLAGS_FILE_TB);
    return 0;
  }
}

/*********************************************************
 *  Read the flags file and find the CH entry: set that  
 *********************************************************/
int withDefaults_setTimeHAPTB_for_VQWK_CH(){ 
  /* Read defaults from file located at FLAGS_FILE_TB */
  load_strings_from_file(FLAGS_FILE_TB);
  if (getflag(HAPTB_RAMP_DELAY_CH)==2 && getflag(HAPTB_INTEGRATE_TIME_CH)==2){
    ORIG_RAMPDELAY_TB = getint(HAPTB_RAMP_DELAY_CH);
    ORIG_INTEGTIME_TB = getint(HAPTB_INTEGRATE_TIME_CH);
    return setTimeHAPTB(ORIG_RAMPDELAY_TB,ORIG_INTEGTIME_TB);
  }
  else {
    return 0;
  }
}

/*********************************************************
 *  Read the flags file and find the INJ entry: set that 
 *********************************************************/
int withDefaults_setTimeHAPTB_for_VQWK_INJ(){
  /* Read defaults from file located at FLAGS_FILE_TB */
  load_strings_from_file(FLAGS_FILE_TB);
  if (getflag(HAPTB_RAMP_DELAY_INJ)==2 && getflag(HAPTB_INTEGRATE_TIME_INJ)==2){
    ORIG_RAMPDELAY_TB = getint(HAPTB_RAMP_DELAY_INJ);
    ORIG_INTEGTIME_TB = getint(HAPTB_INTEGRATE_TIME_INJ);
    return setTimeHAPTB(ORIG_RAMPDELAY_TB,ORIG_INTEGTIME_TB);
  }
  else {
    return 0;
  }
}

/*********************************************************
 *  Set up the scan settings 
 *  input arguments:   initialDelay = starting ramp delay
 *                     finalDelay = final ramp delay
 *                     units = TB units, 2.5us each
 *********************************************************/
int setScanRampDelayHAPTB(int initialDelay, int finalDelay){
  TB_SCAN_RAMPDELAYINITIAL = initialDelay;
  TB_SCAN_RAMPDELAYFINAL   = finalDelay;
  return 1;
}

/*********************************************************
 *  Set up the scan settings 
 *  input arguments:   intTime = length of gate during scan
 *                     units = TB units, 2.5us each
 *********************************************************/
int setScanIntTimeHAPTB(int intTime){
  TB_SCAN_INTEGTIME = intTime;
  return 1;
}

/*********************************************************
 *  Set up the scan settings 
 *  input arguments:   nSteps = number of steps to take before terminating
 *                     This can truncate the loop or loops of scans early
 *                     units = steps of this HAPTB scan routine
 *********************************************************/
int setScanStepLimitHAPTB(int nSteps){
  TB_SCAN_STEPLIMIT = nSteps;
  return 1;
}

/*********************************************************
 *  Set up the scan settings 
 *  input arguments:   stepSize = number of TB units to step
 *                     units = TB units, 2.5us each
 *********************************************************/
int setScanStepSizeHAPTB(int stepSize){
  TB_SCAN_STEPSIZE = stepSize;
  return 1;
}

/*********************************************************
 *  Set up the scan settings 
 *  input arguments:   stepTime = length of each step in s
 *                     units = sleep units, 1 = 1s each
 *********************************************************/
int setScanStepTimeHAPTB(int stepTime){
  TB_SCAN_STEPTIME = stepTime;
  return 1;
}

/*********************************************************
 *  Set up the scan settings 
 *  input arguments:   loopLimit = number of times to scan
 *                     defualt value = 1 so just one loop
 *                     units = loops of scan
 *********************************************************/
int setScanLoopLimitHAPTB(int loopLimit){
  TB_SCAN_LOOPLIMIT = loopLimit;
  return 1;
}

/*********************************************************
 *  Set up all the scan settings, minus loop or step limits
 *  input arguments:   Same as those above
 *                     initialDelay = starting ramp delay
 *                     finalDelay = final ramp delay
 *                     intTime = length of gate during scan
 *                     stepSize = number of TB units to step
 *                     units = TB units, 2.5us each
 *                     stepTime = length of each step in s
 *                     units = sleep units, 1 = 1s each
 *********************************************************/
int setScanSettingsHAPTB(int initialRamp, int finalRamp, int intTime, int stepSize, int stepTime){
  setScanRampDelayHAPTB(initialRamp,finalRamp);
  setScanIntTimeHAPTB(intTime);
  setScanStepSizeHAPTB(stepSize);
  setScanStepTimeHAPTB(stepTime);
  return 1;
}

/*********************************************************
 *  Print the settings to the screen
 *********************************************************/
int dumpScanSettingsHAPTB(){
  printf("\nDump of scan settings for HAPTB scan : \n");
  printf("Starting scan ramp delay: %d = 22.5us+2.5us*initialDelay = %f us after trigger, final ramp delay: %d = 22.5us+2.5us*finalDelay %f us after trigger\n",TB_SCAN_RAMPDELAYINITIAL, TB_SCAN_RAMPDELAYINITIAL*2.5+22.5, TB_SCAN_RAMPDELAYFINAL, TB_SCAN_RAMPDELAYFINAL*2.5+22.5);
  printf("Scan gate time length: %d = 5us+intTime = %f us after beginning\n", TB_SCAN_INTEGTIME, TB_SCAN_INTEGTIME*2.5+5);
  printf("Scan step size: %d = 2.5us*stepSize = %f us jumps\n",TB_SCAN_STEPSIZE, TB_SCAN_STEPSIZE*2.5);
  printf("Scan step time: %d = stepTime = %f seconds per step\n",TB_SCAN_STEPTIME, TB_SCAN_STEPTIME);
  printf("Number of times to loop over scan: %d times\n",TB_SCAN_LOOPLIMIT);
  printf("Number of steps to go before terminating scan (0 = no limit): %d\n",TB_SCAN_STEPLIMIT);
  return 1;
}

/*********************************************************
 *  Print the status of the scan variables to the screen
 *********************************************************/
int dumpScanProgressHAPTB(){
  printf("\nDump of current scan settings for HAPTB scan : \n");
  printf("Current step = %d = 22.5us+2.5us*currentDelay = %f us delay after trigger\n",LAST_SCAN_RAMPDELAY_TB,LAST_SCAN_RAMPDELAY_TB*2.5+22.5);
  printf("Current integration time = %d = 5us+2.5us*currentIntTime = %f us gate time after delay\n",LAST_SCAN_INTEGTIME_TB,LAST_SCAN_INTEGTIME_TB*2.5+5);
  printf("Current loop number = %d\n",TB_SCAN_LOOPN);
  return 1;
}

/*********************************************************
 *  Perform the scan using the settings defined above
 *********************************************************/
int scanDelayHAPTB(){
  int sleepTime;
  ORIG_RAMPDELAY_TB = getRampDelayHAPTB();
  ORIG_INTEGTIME_TB = getIntTimeHAPTB();
  TB_SCAN_DONE = 0;
  TB_SCAN_LOOPN = 0;
  dumpScanProgressHAPTB();
  while (TB_SCAN_LOOPN < TB_SCAN_LOOPLIMIT && TB_SCAN_DONE == 0){
    if (TB_SCAN_STEPN > TB_SCAN_STEPLIMIT && TB_SCAN_STEPLIMIT>0){
      TB_SCAN_LOOPN = TB_SCAN_LOOPLIMIT;
      printf("Scan reached step limit (%d steps), scan completed after %d loops\n",TB_SCAN_STEPLIMIT,TB_SCAN_LOOPN);
    }
    setTimeHAPTB(LAST_SCAN_RAMPDELAY_TB,TB_SCAN_INTEGTIME);
    TB_SCAN_STEPN++;
    LAST_SCAN_RAMPDELAY_TB+=TB_SCAN_STEPSIZE;
    if (LAST_SCAN_RAMPDELAY_TB == TB_SCAN_RAMPDELAYFINAL+TB_SCAN_STEPSIZE){
      TB_SCAN_LOOPN++;
      LAST_SCAN_RAMPDELAY_TB=TB_SCAN_RAMPDELAYINITIAL;
    }
    sleepTime = TB_SCAN_STEPTIME*sysClkRateGet();
    taskDelay(sleepTime);
  }
  LAST_SCAN_RAMPDELAY_TB=TB_SCAN_RAMPDELAYINITIAL;
  TB_SCAN_STEPN=0;
  setTimeHAPTB(ORIG_RAMPDELAY_TB,ORIG_INTEGTIME_TB); /* Reset values to pre-scan values */
  printf("Finished timing board ramp delay scan\n");
  return 1;
}

int stopScanHAPTB(){
  TB_SCAN_DONE = 1;
  printf("Terminated timing board scan\n");
  return 1;
}
