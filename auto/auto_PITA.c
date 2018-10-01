#include <vxWorks.h>
#include <stdio.h>
#include "../caFFB/caFFB.h"

#include "../module_map.h"
#include "auto_PITA.h"
//#define autoRHWPnang 19     // number of RHWP angles  

int pitaStepSize = 1000;
const int SCANPTS=3;

void auto_PITA(){

  int iloop = 0;
  int readbackpos, readbackneg;
  int scanflag;
  int k,i;

  // get the PC pos/neg setpoints
  int gun2pos= caget(3);
  int gun2neg= caget(4);

  /* double gun2pos = 5.958; */
  /* double gun2neg = 5.725; */

  //fprintf(stderr,"PC Setpoints:: %d \t %d\n",gun2pos,gun2neg);
  //return;

  int gun2p[SCANPTS];
  int gun2n[SCANPTS];

  // set up the PC PITA setpoints
  for(k=0;k<SCANPTS;k++){
    gun2p[k]=gun2pos + (1-k)*pitaStepSize;
    gun2n[k]=gun2neg - (1-k)*pitaStepSize;
  }

  for(k=0;k<SCANPTS;k++){
    fprintf(stderr,"PC Setpoint nr %d:: %d \t %d\n",k+1,gun2p[k],gun2n[k]);
    //fprintf(stderr,"PC Setpoints:: %d \t %d\n",gun2pos,gun2neg);
  }

  setDataSCAN(1,0);
  fprintf(stderr,"Setting Scandata1 to 0\n");


  for(i=0;i<SCANPTS;i++){
    // set scanclean and scandata1 to 0
    setCleanSCAN(0);
    fprintf(stderr,"Setting Scanclean to 0\n");

    taskDelay(12);

    // change the PC setpoint
    caput(gun2p[i],3);
    caput(gun2n[i],4);

    fprintf(stderr,"Set PC Setpoint to %d \t %d\n",gun2p[i],gun2n[i]);

    // set scandata1 flag
    //scanflag = 60*(gun2p[i]-gun2n[i]);
    setDataSCAN(1,gun2p[i]);
    setDataSCAN(2,gun2n[i]);
    fprintf(stderr,"Set Scandata1 to %d\n",gun2p[i]);
    fprintf(stderr,"Set Scandata2 to %d\n",gun2n[i]);

     //    wait until rhwp readback == rhwpset[i];
     iloop=0;
     while(1){
	 fprintf(stderr,"Checking PC Setpoints here\n");
	 readbackpos = caget(3);
	 readbackneg = caget(4);

	 /* readbackpos = gun2p[i]; */
	 /* readbackneg = gun2n[i]; */

	 // make sure that the PC setpoits are set correctly
	 if (gun2p[i]==readbackpos && gun2n[i]==readbackneg)	   break;

	 taskDelay(72);
	 if(iloop>4){
	   fprintf(stderr,"Timeout:: Waiting for PC Setpoints\n");
	   return;
	 }
	 iloop++;
     }

     setCleanSCAN(1);
     fprintf(stderr,"Setting Scanclean to 1\n");     
     taskDelay(delay);    //  Default, wait 60 sec
     //     taskDelay(36);    //    wait 0.6 sec;
     setCleanSCAN(0);
     fprintf(stderr,"Setting Scanclean to 0\n");
  }

  setCleanSCAN(0);
  fprintf(stderr,"Setting Scanclean to 0\n");
  setDataSCAN(1,0);
  fprintf(stderr,"Setting Scandata1 to 0\n");
  setDataSCAN(2,0);
  fprintf(stderr,"Setting Scandata2 to 0\n");

  fprintf(stderr,"PITA autoscan complete\n");
  fprintf(stderr,"Thank you for using PITA autoscan. Please come again!!\n");
  // end the run
}

void auto_PITA_print(){
  fprintf(stderr,"PC Gun2Pos:: %f\n", caget(3));
  fprintf(stderr,"PC Gun2Neg:: %f\n", caget(4));
  fprintf(stderr,"RHWP:: %d\n", caget(1));
  fprintf(stderr,"Taskdelay:: %f sec\n", delay/60.);
}

void auto_PITA_setDelay(int del){
  int deltime = del/60.;
  fprintf(stderr,"Setting taskdelay to %d sec\n", deltime);
  delay = del;
}
