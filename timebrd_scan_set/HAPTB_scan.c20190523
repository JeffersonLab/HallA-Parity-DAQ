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

/*********************************************************
 *  Change the default values in the flags file for CH   
 *      input arguments:   delay = ramp delay
 *                         time = integrate time
 *      units of arguments: 2.5 microseconds
 *********************************************************/
int setTimeHAPTB_for_VQWK_CH(){
  return setTimeHAPTB(23,3000);
}

/*********************************************************
 *  Change the default values in the flags file for INJ   
 *      input arguments:   delay = ramp delay
 *                         time = integrate time
 *      units of arguments: 2.5 microseconds
 *********************************************************/
int setTimeHAPTB_for_VQWK_INJ(){
  return setTimeHAPTB(23,3000);
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
  TB_SCAN_DONE = 0;
  TB_SCAN_LOOPN = 0;
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
  setTimeHAPTB(LAST_SCAN_RAMPDELAY_TB,TB_SCAN_INTEGTIME);
  printf("Finished timing board ramp delay scan\n");
  return 1;
}

int stopScanHAPTB(){
  TB_SCAN_DONE = 1;
  printf("Terminated timing board scan\n");
  return 1;
}
