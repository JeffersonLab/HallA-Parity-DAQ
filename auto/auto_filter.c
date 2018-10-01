#include <vxWorks.h>
#include <stdio.h>
#include "../caFFB/caFFB.h"

#include "../module_map.h"

// Define constants

// Define Globals

const int NUM_FILS = 6;
const int NUM_TIMES = 13;
const int NUM_INT = 4;
const int NUM_CONV = 16;
const int NUM_HV_STEPS = 10;
const int HV_INIT = 0;

int sec_per_win = 56;
int sec_per_step = 0;


int current_filter = 0;
int oversample = 3;
int cycles = 1;
int click_patt = 0;
int curr_hv;



void print_setup() {
   fprintf(stderr,"\n Printing AUTO_FILTER Params\n");
   fprintf(stderr,"   steps = %d \n",NUM_FILS);
   fprintf(stderr,"   sec_per_win = %d \n",sec_per_win);
   fprintf(stderr,"   Oversample = %d \n",oversample);
   fprintf(stderr,"   sec_per_win = %d \n",sec_per_win);
   return;
 }

int getSecPerWin() {
  fprintf(stderr, "\nThere are %d per filter", sec_per_win);
}

int getSecPerStep() {
  fprintf(stderr, "\n %d per filter", sec_per_step);
}

void setSecPerWin(int new) {
  sec_per_win = new;
  sec_per_step = sec_per_win/oversample;
  fprintf(stderr, "\nSecPerWin has been changed to %d", 
	  sec_per_win);
}

void setOversample(int new) {
  oversample = new;
  fprintf(stderr, "\nOversample has been changed to %d", 
	  oversample);
}

void setCycles(int new) {
  cycles = new;
  fprintf(stderr, "\nCycles has been changed to %d", 
	  cycles);
}

void setHV(int hv) {

  float dac = 32767 - 15*hv;

  curr_hv = hv;

  setCleanSCAN(0);

  setDACHAPTB(2,dac);
  fprintf(stderr, "\nHigh Voltage has been changed to %d",hv); 

  taskDelay(256);

  setDataSCAN(2,hv);
  setCleanSCAN(1);


}

int getHV() {

  float dac = 32767 - 15*curr_hv;

  fprintf(stderr, "\nHigh Voltage is currently set to %d",curr_hv); 

}


void clickWheel() {

  setDACHAPTB(1,1000);  
  taskDelay(12);
  setDACHAPTB(1,4095);
  taskDelay(12);
  setDACHAPTB(1,0);  

  current_filter++;
  if(current_filter==6) current_filter = 0;

  setCleanSCAN(0);

  setDataSCAN(1,current_filter);

  fprintf(stderr,"\n Stepping filter \n");
  fprintf(stderr,"\n new filter setting is %d \n",current_filter);

  setCleanSCAN(1);  
  
}

void auto_filter() {
  

  int i = 0, j = 0, k = 0, m = 0, n = 0, p = 0,q = 0,r = 0,s = 0;
  
  int clicks[] = {1,1,1,1,1,1};
  
  //int clicks0[] = {3,4,1,4};
  int clicks0[] = {2,3,4,4,3,2};
  int clicks1[] = {3,2,2,3,4,4};
  int clicks2[] = {4,3,2,2,3,4};
  
  setOversample(3);
  setSecPerWin(56);
  setCycles(1);


  if (click_patt==0) {
    for (k = 0; k <NUM_FILS; k++) {
      clicks[k] = clicks0[k];
    }
  }
  else if (click_patt==1) {
    for (k = 0; k <NUM_FILS; k++) {
      clicks[k] = clicks1[k];
    }
  }
  else if (click_patt==2) {
    for (k = 0; k <NUM_FILS; k++) {
      clicks[k] = clicks2[k];
    }
  }


  for(n=0; n<cycles; n++) {
    for(m=0; m<oversample; m++) {
      for(i=0; i<NUM_FILS; i++){
	
	
	for (j=0; j<clicks[i]; j++) {
	  
	  setCleanSCAN(0);
	  
	  setDACHAPTB(1,1000);  
	  taskDelay(12);
	  setDACHAPTB(1,4095);
	  taskDelay(12);
	  setDACHAPTB(1,0);  
	  
	  current_filter++;
	  if(current_filter==6) current_filter = 0;
	  
	  fprintf(stderr,"\n Stepping filter: ");
	  fprintf(stderr," new filter setting is %d\n",current_filter);
	  
	  taskDelay(12);
	  setDataSCAN(1,current_filter);
	  
	}
	
	fprintf(stderr,"\nThe final filter this round is %d \n",current_filter);
	
	setCleanSCAN(1);
	taskDelay(sec_per_step * 72);

      }
    }
  }

  setCleanSCAN(0);  // set NOT CLEAN to finish
  
  fprintf(stderr,"\n ****  FILTER autoscan complete ****  \n");
  fprintf(stderr,"\n Thank you for using FILTER autoscan. Please come again\n\n");
  // end the run

}


void scan_IT() {
  
  int ramp = 300, gate = 0, r = 0;
  
  setOversample(1);
  setSecPerWin(14);
  setCycles(1);


  for (r=0; r<NUM_TIMES; r++) {
    
    gate=(r+1)*1000+300;
    
    setTimeHAPTB(ramp,gate);
    dumpRegHAPTB();
    
    
    fprintf(stderr,"\nThe final integration time this round is %d \n",gate);
    
    taskDelay(sec_per_step * 72);
    
  }

  setCleanSCAN(0);  // set NOT CLEAN to finish
  
  fprintf(stderr,"\n ****  FILTER autoscan complete ****  \n");
  fprintf(stderr,"\n Thank you for using FILTER autoscan. Please come again\n\n");
  // end the run

}



void scan_gains() {

  int p = 0, q = 0;
  
  setOversample(1);
  setSecPerWin(7);
  setCycles(1);

  for (p=0; p<NUM_CONV; p++) {
    for (q=0; q<NUM_INT; q++) {
      
      //adc18_intgain(0,q);    /* intgain: 0-3, where 3 is minimum */
      //adc18_setconv(0,p);    /* conv (gain): 0-15 where 0 is minimum */
      //adc18_print_setup(0);
      adc18_setup(0,0,q,p);
      
      fprintf(stderr,"\nThe gain settings for this round are %d and %d\n",q,p);
      taskDelay(sec_per_step * 72);
      
      
    }
  }

  setCleanSCAN(0);  // set NOT CLEAN to finish
  
  fprintf(stderr,"\n ****  FILTER autoscan complete ****  \n");
  fprintf(stderr,"\n Thank you for using FILTER autoscan. Please come again\n\n");
  // end the run

  
}


void scan_hv() {

  int s = 0;

  setOversample(1);
  setSecPerWin(14);
  setCycles(1);

  
  for (s=0; s<NUM_HV_STEPS; s++) {
    
    curr_hv = HV_INIT+100*s;
    
    setHV(curr_hv);
    
    taskDelay(sec_per_step * 72);
    
  }
  
  
  setCleanSCAN(0);  // set NOT CLEAN to finish
  
  fprintf(stderr,"\n ****  FILTER autoscan complete ****  \n");
  fprintf(stderr,"\n Thank you for using FILTER autoscan. Please come again\n\n");
  // end the run
  
}
