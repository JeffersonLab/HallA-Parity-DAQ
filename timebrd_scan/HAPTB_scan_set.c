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

LOCAL int ORIG_RAMPDELAY_TB = getRampDelayHAPTB();
LOCAL int ORIG_INTEGTIME_TB = getIntTimeHAPTB();
LOCAL char* FLAGS_FILE_TB = "/adaqfs/home/apar/devices/crl/vqwkTiming.flags"

/*********************************************************
 * Green Monster based control, with defaults in flags
 * file called on reboot
 *
 * Assume that the user will set the all the timing boards
 * they care about at the right times with correct passed 
 * values, set those to the TB using the given method
 *********************************************************/

/*********************************************************
 *  Read the flags file and find the CH entry: set that  
 *********************************************************/
int withDefaults_setTimeHAPTB_for_VQWK_CH(){ 
  // Read defaults from file located at FLAGS_FILE_TB 
  load_strings_from_file(FLAGS_FILE_TB);
  if (getflag(VQWK_DEFAULT_HAPTB_RAMP_DELAY_CH)==2){
    ORIG_RAMPDELAY_TB = getint(VQWK_DEFAULT_HAPTB_RAMP_DELAY_CH);
  }
  if (getflag(VQWK_DEFAULT_HAPTB_INTEGRATE_TIME_CH)==2){
    ORIG_INTEGTIME_TB = getint(VQWK_DEFAULT_HAPTB_INTEGRATE_TIME_CH);
  }
  return setTimeHAPTB(ORIG_RAMPDELAY_TB,ORIG_INTEGTIME_TB);
}

/*********************************************************
 *  Read the flags file and find the INJ entry: set that 
 *********************************************************/
int withDefaults_setTimeHAPTB_for_VQWK_INJ(){
  load_strings_from_file(FLAGS_FILE_TB);
  if (getflag(VQWK_DEFAULT_HAPTB_RAMP_DELAY_INJ)==2){
    ORIG_RAMPDELAY_TB = getint(VQWK_DEFAULT_HAPTB_RAMP_DELAY_INJ);
  }
  if (getflag(VQWK_DEFAULT_HAPTB_INTEGRATE_TIME_INJ)==2){
    ORIG_INTEGTIME_TB = getint(VQWK_DEFAULT_HAPTB_INTEGRATE_TIME_INJ);
  }
  return setTimeHAPTB(ORIG_RAMPDELAY_TB,ORIG_INTEGTIME_TB);
}

/*********************************************************
 *  Given a parameter to search for, make it into an auto
 *                   generated line in the given file   
 *      input arguments:  filename = name of file to edit
 *                        editname = name of value to place
 *                        editval  = value to change to
 *********************************************************/
int editFlagsFile(char* filename, char* editname, char* editval){
  // Consider using such methods as: fgets, strchr, fopen, free, char* malloc, strcpy, strncpy, getline, strstr...


/*
#define COMMENT_CHAR ';'


  char *file_configusrstr=0;
  char *ffile_name;
  int fd;
  char s[256], *flag_line;

  int configLineNum = 0;

  ffile_name = (char *) malloc(strlen(filename)+1);
  strcpy(ffile_name,input_name);
*/
  /* check that filename exists */
/*  fd = fopen(ffile_name,"r");
  if(!fd) {
    printf("Failed to open usr flag file %s\n",ffile_name); 
    free(ffile_name);
    if(file_configusrstr) {
      free(file_configusrstr);*/ /* Remove old line */
/*    }
    file_configusrstr = (char *) malloc(1);
    file_configusrstr[0] = '\0';
  } 
  else {
*/    /* Read till an uncommented line is found */
/*    flag_line = 0;
    while(fgets(s,255,fd)){
      configLineNum = configLineNum + 1; *//* Count the number of lines to get to config */
/*      char *arg;
      arg = strchr(s,COMMENT_CHAR); *//* Look for ';'s, if found then line = \0 */
/*      if(arg) *arg = '\0'; *//* Blow away comments */
/*      arg = s;			*//* Skip whitespace */
/*      while(*arg && isspace(*arg)){ *//* move along args memory, skipping spaces */
/*        arg++;
      }
      if(*arg) {
          flag_line = arg; *//* We found a non-whitespace start of an uncommented line, store it */
/*        break;
      }
    }
      if(file_configusrstr) free(file_configusrstr); *//* Remove old line */
/*    if(flag_line) {		*//* We have a config usrstr */
/*      file_configusrstr = (char *) malloc(strlen(flag_line)+1);
      strcpy(file_configusrstr,flag_line);
    } else {
      file_configusrstr = (char *) malloc(1);
      file_configusrstr[0] = '\0';
    }
    fclose(fd);
    free(ffile_name);
  }

  file_configuserstr
*/

  int flagsFile;
  bool newFile = false;
  bool auto_gen = false;

  std::vector<std::vector<char*>> filearray = {};
  int index_i = 0;
  int index_j = 0;
  char delim = ',';
  char* oldvalue;

  flagsFile = fopen(filename,"r");
  if (flagsFile != NULL) { 
    newFile = true; 
  }

  for (unsigned int row = 0; row<flagsFile.end(); row++) {
    std::vector<char*> line = flagsFile.parse(row,delim);
    filearray.push_back(line);
  }
  for (unsigned int row = 0; row<filearray.size(); row++) {
    if (filearray[row][0] == ";") {
      continue; // Then we are in a commented out line and should move on
    }
    else {
      int index_i = row;
      for (unsigned int column = 0; column<filearray[row].size(); column++) {
        char* tmpCheckName = filearray[row].parse(column,'=',before);
        char* tmpCheckVal = filearray[row].parse(column,'=',after);
        if (tmpCheckName == editname && tmpCheckVal != NULL) { 
          // If there is content after '=' then my name is here, the one to the right is the one to clobber
          int index_j = column;
          char* old_value = tmpCheckName;
          if (filearray[row][0] == "auto_generated_content!!!") {
            auto_gen = true; // Then just inline edit the contents
            filearray[row][column] = Form("%s=%s",editname,editval);
          }
          continue; // Break out of loop, don't keep checking subsequent values
        }
        else if (tmpCheckName == editname && tmpCheckVal == NULL) { 
          index_j = 255; // some large number so that later check fails
          // Then something is wrong -> the name is here, but with no value, so just append
          if (filearray[row][0] == "auto_generated_content!!!") {
            auto_gen = true; // Then just inline edit the contents
            filearray[row].push_back(Form("%s=%s",editname,editval));
          }
          continue; // Break out of loop, don't keep checking subsequent values
        }
      }
    }
  }

  if (auto_gen == false) {
    // If the prior file wasn't auto-generated then we should edit the previously active line to be commented out now and make a new updated, auto-generated line
    if (!newFile){
      std::vector<char*> tmpLine = filearray[index_i];
      if (filearray[index_i][0] != "auto_generated_content!!!") { // If it wasn't auto-appended already, now it is
        tmpLine.insert(0,"auto_generated_content!!!");
      }
      filearray[index_i].insert(0,";"); // Put a comment symbol at the beginning
      if (tmpLine.size() > index_j) {
        tmpLine[index_j] = Form("%s=%s",editname,editval);
      }
      else {
        tmpline.push_back(form("%s=%s",editname,editval));
      }
      filearray.push_back(tmpline);
    }
    else { // We have a new file, so just fill it
      std::vector<char*> tmpLine = {};
      tmpLine.push_back("auto_generated_content!!!");
      tmpline.push_back(form("%s=%s",editname,editval));
    }
    filearray.push_back(tmpline);
  }
  fclose(flagsFile);

  filenameMV = filename + system.date();
  system.exec(Form("mv %s %s",filename,filenameMV)); // Make a backup

  flagsOutFile = fopen(filename,"w");
  // Now print the newly updated data into the file, preserved
  for (unsigned int row = 0; row<filearray.size(); row++) {
    for (unsigned int column = 0; column<filearray[row].size()-1; column++) {
      flagsOutFile<<filearray[row][column]<<delim;
    flagsOutFile<<filearray[row][filearray[row].size()-1];
    flagsOutFile<<'\n';

  fclose(flagsFile);
}

/*********************************************************
 *  Change the default values in the flags file for CH   
 *      input arguments:   delay = ramp delay
 *                         time = integrate time
 *      units of arguments: 2.5 microseconds
 *********************************************************/
int changeDefault_timeHAPTB_for_VQWK_CH(int delay, int time){
  // Open the flags file, find either the AUTOGENERATED_DATA indicator or if none then the last line in the file
  // If there are any uncommented lines then comment them and save their information as the line to be editted and autogenerate-saved
  // Clobber the autogenerated line/add a new autogenerated line (which is the previous uncommented one if there was one) with the settings chosen here editted (leave other settings the same)
  // Close file
  return withDefaults_setTimeHAPTB_for_VQWK_CH();
}

/*********************************************************
 *  Change the default values in the flags file for INJ   
 *      input arguments:   delay = ramp delay
 *                         time = integrate time
 *      units of arguments: 2.5 microseconds
 *********************************************************/
int changeDefault_timeHAPTB_for_VQWK_INJ(int delay, int time){
  withDefaults_setTimeHAPTB_for_VQWK_INJ();
  return 0;
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
  /* setTimeHAPTB(LAST_SCAN_RAMPDELAY_TB,TB_SCAN_INTEGTIME); */
  setTimeHAPTB(ORIG_RAMPDELAY_TB,ORIG_INTEGTIME_TB);
  printf("Finished timing board ramp delay scan\n");
  return 1;
}

int stopScanHAPTB(){
  TB_SCAN_DONE = 1;
  printf("Terminated timing board scan\n");
  return 1;
}
