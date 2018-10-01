#include <vxWorks.h>
#include <stdio.h>
#include "../caFFB/caFFB.h"

#include "../module_map.h"

// Define constants

// Define Globals

int auto_rhwp_start = 0;
int auto_rhwp_end = 180;
int auto_rhwp_steps = 19;
int auto_rhwp_sec_per_step = 50;

BOOL auto_die = FALSE;

void print_auto_rhwp() {
  fprintf(stderr,"\n Printing AUTO_RHWP Params\n");
  fprintf(stderr,"   auto_rhwp_start = %d \n",auto_rhwp_start);
  fprintf(stderr,"   auto_rhwp_end = %d \n",auto_rhwp_end);
  fprintf(stderr,"   auto_rhwp_steps = %d \n",auto_rhwp_steps);
  fprintf(stderr,"   auto_rhwp_sec_per_step = %d \n",auto_rhwp_sec_per_step);
  fprintf(stderr,"   ...oh, and \n auto_die = %d \n\n",auto_die);
  return;
}

void auto_rhwp() {
  
  int i = 0;
  int iloop = 0;
  int readback;
  int iangle;
  int iset;
  int istat;
  int angle_increment;
    
  angle_increment = (auto_rhwp_end - auto_rhwp_start)/(auto_rhwp_steps -1);

  for(i=0; i<auto_rhwp_steps; i++){
 
    if (auto_die) {
      fprintf (stderr," \n *** AUTO_RHWP aborting on request *** \n\n");
      return;
    }

    if (iangle == 1) iangle =0;
    iangle = auto_rhwp_start + angle_increment*i;
    if (iangle<0 || iangle>180) {
      fprintf (stderr,
	       "\n *** AUTO_RHWP ERROR: requested invalid rhwp angle %d \n\n",
	       iangle);
      fprintf (stderr," *** AUTO_RHWP ABORTING *** \n\n");
      return;
    }
    if (iangle == 0) iangle =1;
    iset = iangle*50;
    
    setCleanSCAN(0);
    taskDelay(12);
    caput(0,iset);
    fprintf(stderr,"\n Setting rwhp angle to %d, setpoint is %d \n",
	    iangle,iset);
    setDataSCAN(1,iangle);

    //    wait until rhwp readback == iset;
    iloop=0;
    istat = 0;

    while(1){

      if (auto_die) {
	fprintf (stderr," \n *** AUTO_RHWP aborting on request *** \n\n");
	return;
      }

      readback = (int) caget(1);
      if (readback == iset)  break;

      istat++;
      if (istat == 5) { // every 5 seconds give an update
        fprintf(stderr,"Checking rhwp setpoint, should be moving to %d \n",
		iset);
	istat=0;
      }

      iloop++;
      if(iloop>120){   // timeout after about 2 minutes
	fprintf(stderr,"\n\n ***** AUTO_RHWP ERROR **** \n");
	fprintf(stderr,"   Timeout waiting for RHWP setpoint\n");
	fprintf(stderr,"   AUTO_RHWP script aborted  \n\n");
	return;
      }

      taskDelay(144);   // wait 2 second between checks

    }
    setCleanSCAN(1);
    taskDelay(auto_rhwp_sec_per_step * 72);
  }
  
  setCleanSCAN(0);  // set NOT CLEAN to finish

  fprintf(stderr,"\n ****  RHWP autoscan complete ****  \n");
  fprintf(stderr,"\n Thank you for using RHWP autoscan. Please come again\n\n");
  // end the run
}
