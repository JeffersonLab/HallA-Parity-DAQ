#include <vxWorks.h>
#include <stdio.h>
#include "../caFFB/caFFB.h"

#include "../module_map.h"
#define autoRHWPnang 19     // number of RHWP angles  

void auto_rhwp() {

  int i = 0;
  int iloop = 0;
  int readback;

  // RHWP setpoints
  int rhwpset[autoRHWPnang] = { 50, 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000, 5500, 6000, 6500, 7000, 7500, 8000, 8500, 9000 }; 

  // RHWP angles to be used in scandata1
  int rhwpval[autoRHWPnang] = {  1,  10,   20,   30,   40,   50,   60,   70,   80,   90,  100,  110,  120,  130,  140,  150,  160,  170,  180 };


  for(i=0;i<autoRHWPnang;i++){
     setCleanSCAN(0);
       taskDelay(12);
       caput(0,rhwpset[i]);
       fprintf(stderr,"Setting rwhpsetpoint here to %d \n",rhwpval[i]);
       setDataSCAN(1,rhwpval[i]);
     //    wait until rhwp readback == rhwpset[i];
     iloop=0;
     while(1){
	 fprintf(stderr,"Checking rwhpsetpoint here\n");
	 readback = (int) caget(1);
	 if (rhwpset[i]==readback)
	   break;
	 taskDelay(72);
	 if(iloop>240){
	   fprintf(stderr,"Timeout waiting for RHWP setpoint\n");
	   return;
	 }
	 iloop++;
     }
     setCleanSCAN(1);
     taskDelay(3600);    //    wait 100 sec;
  }

  setCleanSCAN(0);
  fprintf(stderr,"RHWP autoscan complete\n");
  fprintf(stderr,"Thank you for using RHWP autoscan.  Please come again\n");
  // end the run
}
