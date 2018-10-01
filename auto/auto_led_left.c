#include <vxWorks.h>
#include <stdio.h>

//#include "../module_map.h"(stderr," \n *** Debug 1  \n");
#include "../module_map.h"

#include "auto_led.h"

void print_auto_led() {
  fprintf(stderr,"\n Printing AUTO_LED Params\n");
  fprintf(stderr,"   auto_led_base_min = %d \n",auto_led_base_min);
  fprintf(stderr,"   auto_led_base_max = %d \n",auto_led_base_max);
  fprintf(stderr,"   auto_led_base_steps = %d \n",auto_led_base_steps);
  fprintf(stderr,"   auto_led_diff = %d \n",auto_led_diff);
  fprintf(stderr,"\n   auto_led_diff_enable_flag = %d \n",auto_led_diff_enable_flag);
  fprintf(stderr,"   auto_led_base_enable_flag = %d \n\n",auto_led_base_enable_flag);
  fprintf(stderr,"   ...oh, and \n auto_led_die = %d \n\n",auto_led_die);
  return;
}

STATUS init0AutoLED() {
  unlockAutoLED();
  enableDiffAutoLED(0);
  enableBaseAutoLED(0);
  setBaseAutoLED(auto_led_base_def);
  setDiffAutoLED(auto_led_diff_def);
  auto_led_in_progress =0;
  lockAutoLED();
  setDataSCAN(1,0);
  setDataSCAN(2,0);
  setCleanSCAN(0);
  return(OK);
}

STATUS initAutoLED() {
  unlockAutoLED();
  enableDiffAutoLED(0);
  enableBaseAutoLED(0);
  auto_led_in_progress =0;
  lockAutoLED();
  setDataSCAN(1,0);
  setDataSCAN(2,0);
  setCleanSCAN(0);
  return(OK);
}

STATUS checkLockAutoLED() {
  if (auto_led_unlock) {
    return(OK);
  } 
  // no dude, yer locked out
  // shut everything down
//   unset_tir_oport(auto_led_diff_flag);
//   unset_tir_oport(auto_led_base_flag);
  return(ERROR);
}

int enableDiffAutoLED(int state) {
  if (checkLockAutoLED()==ERROR ) return(-1);
  //write state to enable diff signal
  auto_led_diff_state_left = state;
  setDataSCAN(2,state);
  return(state);
}

int enableBaseAutoLED(int state) {
  if (checkLockAutoLED()==ERROR ) return(-1);
  //write state to enable diff signal
  auto_led_base_state_left = state;
//   if(state==1)      set_tir_oport(auto_led_base_flag);
//   else if(state==0) unset_tir_oport(auto_led_base_flag);  
  return(state);
}

int toggleDiffAutoLED() {
  int newout;
  int retval;

  // toggle diff auto led
  if (getStateDiffAutoLED() == 0 ) {
    newout =1;
  } else {
    newout = 0;
  }
  retval = enableDiffAutoLED(newout);
  return(retval);
}

int requestToggleDiffAutoLED() {
  int newout;

  // request crl to toggle diff
  if (getStateDiffAutoLED() == 0 ) {
    newout =1;
  } else {
    newout = 0;
  }
  auto_led_next_diff_state = newout;
  return(newout);
}


int setBaseAutoLED(int value) {
  int errcd;
  if (checkLockAutoLED()==ERROR ) return(-1);
  if (value >= auto_led_base_min && value <= auto_led_base_max) {
    setCleanSCAN(0);
    errcd = setDACHAPTB(2,value);
    setDataSCAN(1,errcd);
    // printf("*** debug setBaseAutoLED*** \n");
    //    printf("*** Setting AUTO_LED Baseline to %d *** \n",value);
  } else {
    enableBaseAutoLED(0);
    return(-1);
  }
  return(errcd);
}

int setDiffAutoLED(int value) {
  int errcd;
  if (checkLockAutoLED()==ERROR ) return(-1);
  if (value >= auto_led_diff_min && value <= auto_led_diff_max) {
    errcd = setDACHAPTB(1,value);
  } else {
    enableDiffAutoLED(0);
    return(-1);
  }
  return(errcd);
}

int setStateBeamAutoLED(int value) {
  if (checkLockAutoLED()==ERROR ) return(-1);
  auto_led_with_beam = value;
}

int getDiffAutoLED() {
  int value;
  value = getDACHAPTB(1);
  return(value);
}

int getBaseAutoLED() {
  int value;
  value = getDACHAPTB(2);
  return(value);
}

int getStateDiffAutoLED() {
  return auto_led_diff_state_left;
}

int getStateBaseAutoLED() {
  return auto_led_base_state_left;
}

int getStateBeamAutoLED() {
  return auto_led_with_beam;
}

STATUS stepAutoLED() {
   // intended as a trigger level call from the crl, 
  // to implemen whatever changes the auto_led script wants to see
  //   printf("*** debug stepAutoLED ***\n");
  if (checkLockAutoLED()==ERROR ) return(-1);
  
  if ( auto_led_next_base != getBaseAutoLED() ) {
    setBaseAutoLED(auto_led_next_base);
  }else {
    setCleanSCAN(1);
  }
  if (auto_led_next_diff_state != getStateDiffAutoLED() ) {
    enableDiffAutoLED(auto_led_next_diff_state);
  }
  return(OK);
}

STATUS unlockAutoLED() {
  printf("** You are unlocking AutoLED.** \n");
  printf("**  DO NOT BLOW UP THE PMT!  ** \n");
  auto_led_unlock=1;
  unlockDACHAPTB();
  return(OK);
}

STATUS lockAutoLED() {
  printf("** You are locking AutoLED.** \n");
  printf("**  about damn time!  ** \n");
  auto_led_unlock=0;
  lockDACHAPTB();
  return(OK);
}

int inProgressAutoLED() {
  return(auto_led_in_progress);
}

STATUS cycleAutoLED() {
  
  // printf("*** debug cycleAutoLED ***\n");

  int bincrement;
  int ibase;

  int i = 0;
  int j = 0;
  int l = 0,p=0;

  // int k[]= {46500,50500,48500,50500,47500,36500,49500,48000,49500,46000,37500,47000,49000,49500,50500,34500};
  // int k[]= {40500,36500,34500,26500,28500,39500,32500,29500,38500,37500,27500,30500,33500,31500,35500};
  //  int withbeam[]={42500,40500,41500,26500,41500,40500,36500,34500,35500,28500,42500,35500,38500,40500,30500}; //50 uA
  int withbeam[]={42500,40500,41500,26500,42500,36500,40500,28500,38500,40500,30500,41500,39500,34500,38500,36500,42500,38500,40500,32500};  
  // no beam
  //int withoutbeam[]={52500,41500,48500,26500,50500,46500,52500,28500,38500,40500,46500,32500,44500,34500,50500,30500,42500,46500,39500,36500};  
  // for 15 uA beam
  int withoutbeam[]={50500,41500,48500,26500,49500,46500,50500,28500,38500,40500,46500,32500,44500,34500,47500,30500,42500,46500,39500,36500};  

  int k[30];

  if(auto_led_with_beam)   for(p=0;p<20;p++) k[p]=withbeam[p];
  else   for(p=0;p<20;p++) k[p]=withoutbeam[p];

  // also set the number of steps
  //  if(auto_led_with_beam) auto_led_base_steps = 15;
  // else  auto_led_base_steps = 20;

  // unlock LED activity
  unlockAutoLED();
  auto_led_in_progress = 1;

  auto_led_base_max = 54500;
  // auto_led_diff_def = 360;

  // now crl should be toggling DiffEnable
  // assume diff DAC is always constant, otherwise set it somehow here.

  // loop over baseline settings
  // queue baseline value (next trigger finish by setting new baseline value)
  // 

  // bincrement = (auto_led_base_max - auto_led_base_min)/(auto_led_base_steps -1);

  enableBaseAutoLED(1);
  for(l=0; l<auto_cycle_repeat; l++){
    for(i=0; i<auto_led_base_steps; i++){
      
      if (auto_led_die) {
	fprintf (stderr," \n *** AUTO_LED aborting on request *** \n\n");
	return(ERROR);
      }
      
      // ibase = auto_led_base_min + bincrement*i;
      ibase = k[i];
      if (ibase < auto_led_base_min || ibase > auto_led_base_max) {
	fprintf (stderr,
		 "\n *** AUTO_LED ERROR: requested invalid BASE level %d \n\n",
		 ibase);
	fprintf (stderr," *** AUTO_LED ABORTING *** \n\n");
	return;
      }    auto_led_next_base = ibase;  // this will be set by crl at next dwell
      // fprintf(stderr," \n *** Setting AUTO_LED Baseline to %d  \n",auto_led_next_base);
      
      // loop over diff states
      
      for(j=0; j<auto_led_diff_ntoggle; j++) {
	if (auto_led_die) {
	  fprintf (stderr," \n *** AUTO_LED aborting on request *** \n\n");
	  return(ERROR);
	}
	
	//fprintf(stderr," \n %d \t *** toggling AUTO_LED Diff setting ***\n", j);
	requestToggleDiffAutoLED();
	taskDelay(auto_led_toggle_time*2);
	//      // ****test****
	// taskDelay(auto_led_toggle_time*72) for 1 sec.
	//      stepAutoLED(); // only because CODA can't do it yet
	//      //
      }
      auto_led_next_diff_state = 0; // turn diff off
    }
  }
  // complete loop, disable base and lock up.

  enableDiffAutoLED(0);
  enableBaseAutoLED(0);
  taskDelay(2);
  setCleanSCAN(0);
  auto_led_in_progress=0;
  lockAutoLED();
  setDataSCAN(1,0);
  setDataSCAN(2,0);
 
  fprintf(stderr,"\n ****  AutoLED cycle complete ****  \n");
  fprintf(stderr,"\n Thank you for using AutoLED. Please come again\n\n");
  // end the run
  return(OK);

}
