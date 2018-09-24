
/* launch with:

ld < 'bmwClient.o'

sp bmwClient

 */

/*
  $Header: /group/halla/parity/cvs/devices/bmw/bmwClient.c,v 1.8 2010/03/10 17:07:48 paschke Exp $
 */

/***** includes *****/
#include "bmw.h"
#include "/adaqfs/home/apar/devices/module_map.h"

STATUS defineSeqBMW(int iseq, int coil, int amp, int nperiods) {

  if(iseq>=0 && iseq<MAX_SEQS) {
    bmw_seq[iseq].active=0;
    bmw_seq[iseq].nobj=1;
    bmw_seq[iseq].coil_1 = 0;
    bmw_seq[iseq].amp_1 = 0;
    bmw_seq[iseq].coil_2 = 0;
    bmw_seq[iseq].amp_2 = 0;
    bmw_seq[iseq].freq = bmw_dither_frequency;
    if (coil>0&&coil<=MAX_OBJS) {
      bmw_seq[iseq].active=1;
      bmw_seq[iseq].coil_1 = coil;
      if (amp<=Max_Amplitude) {
	bmw_seq[iseq].amp_1 = amp; 
      }      
    }
    if (nperiods>0 && nperiods<512) {
      bmw_seq[iseq].periods = nperiods;
    } else {
      bmw_seq[iseq].periods= 1;
    }
  }  
  return( getConfigSeqBMW(iseq) );
}



STATUS defineSeq2CoilBMW(int iseq, int coil1, int amp, int coil2, int amp2, int nperiods) {

  if(iseq>=0 && iseq<MAX_SEQS) {

    bmw_seq[iseq].active=0;
    bmw_seq[iseq].nobj=1;

    bmw_seq[iseq].coil_1 = 0;
    bmw_seq[iseq].amp_1 = 0;

    bmw_seq[iseq].coil_2 = 0;
    bmw_seq[iseq].amp_2 = 0;

    bmw_seq[iseq].freq = bmw_dither_frequency;

/*     if(bmw_c_verbose){  */
/*       fprintf(stdout, "\nDefault Configuration:: \n"); */
/*       printSeqBMW(iseq); */
/*     } */

    if (coil1>0&&coil1<=MAX_OBJS) {
      bmw_seq[iseq].active=1;
      bmw_seq[iseq].coil_1 = coil1;
      if (amp<=Max_Amplitude) {
	bmw_seq[iseq].amp_1 = amp; 
      }      

      if (coil2>0&&coil2<=MAX_OBJS && coil2!=coil1) {
	bmw_seq[iseq].nobj=2;
	bmw_seq[iseq].coil_2 = coil2;
	if (amp<=Max_Amplitude) {
	  bmw_seq[iseq].amp_2 = amp2; 
	}
      }
    }
    if (nperiods>0 && nperiods<512) {
      bmw_seq[iseq].periods = nperiods;
    } else {
      bmw_seq[iseq].periods= 1;
    }
  }  

/*   if(bmw_c_verbose) fprintf(stdout, "Configured:: \n"); */
  return( getConfigSeqBMW(iseq) );
  //  return( printSeqBMW(iseq) );
}

/******************************************************************************

getConfigSeqBMW - print out coils, amplitude, frequency and period for a specific sequence

*/

STATUS getConfigSeqBMW (int iseq)
{
  int nob;
  struct bmwSequence seq;

  fprintf(stdout,"Sequence %d: \n",iseq);
  if (iseq<0 || iseq>=MAX_SEQS) return(ERROR);
  seq = bmw_seq[iseq];
  if (seq.active > 0) {
    nob = seq.nobj;
    if (nob==2) fprintf(stdout,"  2-coil modulation \n");
    fprintf(stdout,"  coil = %d,  amplitude = %d \n",seq.coil_1,seq.amp_1);
    if (nob==2) fprintf(stdout,"  coil #2 = %d,  amplitude = %d \n",seq.coil_2,seq.amp_2);
    fprintf(stdout,"  # of periods = %d, frequency = %d \n",seq.periods,seq.freq);
  } else {
    fprintf(stdout,"  sequence not active \n");
  }    
  return (ERROR);
}

STATUS printSeqBMW (int iseq)
{
  int nob;
  struct bmwSequence seq;

  fprintf(stdout,"Sequence %d: \n",iseq);
  if (iseq<0 || iseq>MAX_SEQS) return(ERROR);
  seq = bmw_seq[iseq];
  nob = seq.nobj;
  fprintf(stdout,"  active = %d,  nobj = %d \n",seq.active,nob);
  fprintf(stdout,"  coil #1 = %d,  amplitude = %d \n",seq.coil_1,seq.amp_1);
  fprintf(stdout,"  coil #2 = %d,  amplitude = %d \n",seq.coil_2,seq.amp_2);
  fprintf(stdout,"  # of periods = %d, frequency = %d \n",seq.periods,seq.freq);

  return (ERROR);
}

/******************************************************************************
getConfigBMW - print coils, amplitude, frequency and period for all sequences
*/

STATUS getConfigBMW ()
{
  int i, nc, amp;

  fprintf ( stdout, 
	    "\ngetConfigBMW():: Listing Coil parameters for each cycle...\n");
  for ( i = 0; i < MAX_SEQS; i++ ) {
    getConfigSeqBMW(i);
  }

  fprintf (stdout,"  Timing information, in steps with %d clock ticks per step \n",bmw_ticks_per_step);
  fprintf (stdout,"     pause between cycles = %d steps \n",bmw_steps_per_pause);
  fprintf (stdout,"     wait to pause (resume) FFB = %d (%d) ticks \n",bmw_FFBpause_wait,bmw_FFBresume_wait);
  fprintf (stdout,"     wait for FG configure = %d ticks \n",bmw_config_wait);
    
  return (ERROR);
}
 
STATUS activateSeqBMW(int iseq, int active) {
  // set iseq to active =1 or =0
  if(iseq>=0 && iseq<MAX_SEQS) {
    bmw_seq[iseq].active = active;
    return(OK);
  } else {
    return(ERROR);
  }
}

STATUS trigBMW(int trig){
  // for trig, set it to 0
  // bmw_trig = 2, means 0x02
  flexioWriteChan(trig, bmw_trig);
  //  if(bmw_c_verbose){
  //    if(trig==0) fprintf(stdout, "trigBMW():: Trigger Disabled.\n");
  //    else if(trig==1) fprintf(stdout, "trigBMW():: Trigger Enabled.\n");
  //    else return (ERROR);
  //  }
  return(OK);
}

STATUS initBMW()
{
  int i;

  if(bmw_c_verbose){
    fprintf(stdout, "initBMW():: Initializing BMW\n");
    fprintf(stdout, "initBMW():: MAX_SEQS:: %d\n", MAX_SEQS);
    fprintf(stdout, "initBMW():: MAX_OBJS:: %d\n", MAX_OBJS);
  }

  // initial modulation definitions
  for (i = 0; i<MAX_SEQS; i++) {
    defineSeq2CoilBMW(i,bmw_seq_coil_1[i],bmw_seq_amp_1[i],bmw_seq_coil_2[i],bmw_seq_amp_2[i],bmw_seq_periods[i]);
  }

  bmw_IsInit=1;
  getConfigBMW();

  // disable the trigger
  trigBMW(0);  

  return(OK);
}


STATUS bmwClient() 
{
  //  bmw_flight_plan = 0;
  while (TRUE) {
    if (!bmw_die_die_die) {
      if (bmw_flight_plan==1){
	bmw_test=0;
	bmwClient_script();
      }
      if (bmw_flight_plan==2){
	bmw_test=1;
	bmwClient_script(); 
      }
    }
    taskDelay (72*10);  // (seconds*72) wait to check for start
  }
  return (ERROR);
}

STATUS bmwClient_script ()
{
  int i;
  int iseq;    /* sequence index */
  float fwait;
  int waittime;
  int coil, nobs;
  int count=0, count2=0;
  int delaycounter=0;

  // turn beam modulation on if it is not
  if(!bmw_test)  if(cagetFFB_modState()!=1){
    fprintf(stdout, "Beam Modulation is OFF. Call MCC to turn it ON!! \n");
    return(ERROR);
    //    caputFFB_modState(1.0);
  }
  
  bmw_alive = 1; // reset bmw_alive test flag

  if (bmw_RFlock_in_HallA==0 && bmw_RFlock_in_HallC==0) bmw_die_die_die=TRUE;

  // check initialized
  if (!bmw_IsInit) initBMW();
  bmw_alive=1;

  //  return(OK);
  //
  // Start Loop
  //
/*   while (!bmw_die_die_die)  { */

  while(!bmw_die_die_die){
    count++;
    if(bmw_test) fprintf(stdout,"bmwClient:: ***Running is test mode*** \n");

    // Start supercycle
    bmw_cycle_number = ++bmw_cycle_count;
    if(bmw_test){
      fprintf(stdout,"bmwClient:: bmw_cycle_number: \t %d\n", bmw_cycle_number);
      fprintf(stdout,"bmwClient:: bmw_cycle_count: \t %d\n", bmw_cycle_count);
    }

    //
    // Notify World that supercycle is starting
    //
    // Notify HallA
    fprintf(stdout,"bmwClient:: Here I set Hall A line flag\n");
    setNotice(1,1);

    // if Hall A RF lock, Notify World that RF will be unlocked
    if(bmw_RFlock_in_HallA==1) setNotice(2,1);
    //
    //  pause Hall A FFB, and wait for period to assure pause
    //
    fprintf ( stdout, "bmwClient::: Here I Pause Hall A FFB\n");
    if(!bmw_test) caputFFB(0,1);  // pause Hall A FFB.

    fprintf ( stdout, "bmwClient::: Here I Also Pause Compton Orbit Lock\n");
    if(!bmw_test) caputFFB(2,1);  // pause the Compton Orbit Lock.

    taskDelay( bmw_FFBpause_wait);
    
    bmw_alive =1; // reset bmw_alive test flag

    // Loop over objects and modulate each

    //********* modify to loop over defined sequences ***** //
    for ( iseq = 0;  iseq<=MAX_SEQS && !bmw_die_die_die; ++iseq ){
      bmw_alive =1;
      bmw_object = -1; 

      // check if sequence is active, else skip
      if ( bmw_seq[iseq].active ==1 ) {
	
	// If modulating energy, pause Hall C energy lock (if running)
	
	//	fprintf ( stdout, "bmwClient:: \nSequence %d\n",iseq);
	fprintf ( stdout, "\nbmwClient:: Selecting coil(s) for Seq %d \n", iseq);
	coil = bmw_seq[iseq].coil_1;
	nobs = bmw_seq[iseq].nobj;

	if(bmw_c_verbose){
	  fprintf ( stdout, "bmwClient:: Coil # %d \n", coil);
	  fprintf ( stdout, "bmwClient:: # objs %d \n", nobs);
	}

	// if energy coil, could be either coil 1 or 2
	if(coil==bmw_energy_channel) { 
	  
	  if (bmw_RFlock_in_HallC==1) {
	    //
	    // Notify begining of energy pause
	    //
	    fprintf(stdout,"bmwClient:: Here I set Hall C line flag\n");
	    setNotice(2,1);
	    //
	    //  pause Hall C FFB, and wait for period to assure pause
	    //
	    fprintf ( stdout, "bmwClient::: Here I Pause Hall C FFB\n");
	    if(!bmw_test) caputFFB(1,1);  // pause Hall C FFB.
	    
	    taskDelay ( bmw_FFBpause_wait );
	  }
	  //
	  // notify world of impending energy modulation
	  //
	  setNotice(3,1);
	  //	  
	}
		
	// set the state to CONFIG_STATE
	if(!bmw_test){
	  while ((cagetFFB_waveState(coil)!=1) && count2<5){
	    count2++;
	    caputFFB_leaveTrig(coil,1); // this sets the trig state to 0, and
	    taskDelay(72); // 1/6 sec
	    if(count2==5) fprintf(stdout,"Exiting the loop. Count limit exceeded!!\n");
	  }
	}

	count2=0;

	// set the config parameters
	// coil:{1,8}
	if (coil>0 && coil<=MAX_OBJS) {
	  if(!bmw_test){
	    caputFFB_freq(   coil,bmw_seq[iseq].freq);
	    caputFFB_period( coil,bmw_seq[iseq].periods);
	    caputFFB_amp(    coil,bmw_seq[iseq].amp_1);
	  }
	  /* 	  if (nobs==2) { */
	  /* 	    coil2 = bmw_seq[iseq].coil_2; */
	  /* 	    if (coil2>=0 && coil2 < MAX_OBJS) { */
	  /* 	      caputFFB_bmw_freq(   coil2,bmw_seq[iseq].freq); */
	  /* 	      caputFFB_bmw_periods(coil2,bmw_seq[iseq].periods); */
	  /* 	      caputFFB_bmw_amp(    coil2,bmw_seq[iseq].amp_2); */
	  /* 	    } else { */
	  /* 	      nobs = 1; */
	  /* 	    } */
	  /* 	  } */
	}
	
	fprintf ( stdout, "bmwClient:: Finished configuring CONFIG_STATE\n");
	// wait after config
	taskDelay(bmw_config_wait);
	bmw_alive =1;
	
	// load the sine wave, has to be done after setting config parameters
	if(!bmw_test) caputFFB_load(coil,1);
	
	fprintf ( stdout, "bmwClient:: Finished loading the config\n");
	// wait after arming
	taskDelay(bmw_arm_wait);
	bmw_alive =1;

	if(bmw_c_verbose) getConfigSeqBMW(iseq);
	
	// enter  TRIGGER_STATE
	if(!bmw_test){
	  while ((cagetFFB_waveState(coil)!=2) && count2<5){
	    count2++;
	    caputFFB_enterTrig(coil,1); 
	    taskDelay(150); // 72 is 1 sec
	    if(count2==5) fprintf(stdout,"Exiting the loop. Count limit exceeded!!\n");
	  }
	}

	fprintf ( stdout, "bmwClient:: In trig state now\n");
	count2=0;

	// set the variable bmw_freq to the currnt coil freq
	bmw_freq = cagetFFB_freq(coil);

	// calculate wait time, in seconds, * 72, + 36 clock ticks for safety margin
	//	fwait = bmw_seq[iseq].periods / bmw_seq[iseq].freq * 72 + 4;
	fwait = bmw_seq[iseq].periods / bmw_seq[iseq].freq * 73 + 36;
	waittime = (int) fwait;
	//	waittime = (int) fwait/bmw_seq[iseq].freq;

	// initiate software enter trig 
	//	if(!bmw_test) caputFFB_trig(coil,1);
	
	bmw_phase=0;
	bmw_phase_num=240/bmw_seq[iseq].freq;

	fprintf (stdout, "bmwClient:: bmw_phase_num = %d\n", bmw_phase_num);

	// set the variable bmw_object to the current coil
	bmw_object = coil; 
	bmw_period=0;
	bmw_phase_cnt=0;

	// initiate hardware trigger (using crl)
	fprintf (stdout, "bmwClient:: Selecting bmw_arm_trig to 1\n");
	bmw_arm_trig=1;
	// set bmw_arm_trig to 1
	// in crl: x
	// >    if(bmw_arm_trig==1){
	// >       flexioWriteMask(1,flexio_trig_bit);
	// >       bmw_arm_trig=0;
	// >    }
	//
	
	// wait for end of modulation cycle
	//****************************//      
	//	for(delaycounter=0;delaycounter<bmw_seq[iseq].periods;delaycounter++){
	taskDelay( waittime );
	  //	  bmw_phase++;
	  //	}

	//	bmw_clean = FALSE; // need to modify this ***********
	bmw_phase_cnt=-1;
	bmw_phase=0;
	bmw_period=0;
	bmw_object = -1; 
	bmw_alive =1;

	fprintf ( stdout, "bmwClient:: Selecting bmw_arm_trig to 0\n");
	bmw_arm_trig=0; // is alraedy done in the crl, do it again, just in case..

	// now the modulation for this particular magnet is done
	// so come out of TRIGGER_STATE
	// this, by default, is the CONFIG_STATE
	caputFFB_leaveTrig(coil,1);
	taskDelay(72);
	//	if(!bmw_test){
	//	  while(cagetFFB_waveState(coil)!=1 && count2<5)	{
	//	    count2++;
	//	    caputFFB_leaveTrig(coil,1);
	//	    taskDelay(36);
	//	    if(count2==5) fprintf(stdout,"Exiting the loop. Count limit exceeded!!\n");
	//	  }
	//	}

	count2=0;

	/* 	if (nobs==2)  */
	/* 	  caputFFB_bmw_state(coil2,0); */
       
	//	if(!bmw_test) cagetFFB_waveState(coil);

	if(coil==bmw_energy_channel) { // if energy
	  
	  //
	  // notify world of end of energy modulation 
	  setNotice(3,0);
	  //
	  
	  if (bmw_RFlock_in_HallC==1) {
	    //
	    //  un-pause Energy lock, and wait predefined pause for re-lock.
	    //
	    fprintf ( stdout, "bmwClient:::release Hall C FFB \n");
	    if(!bmw_test) caputFFB(1,0);  // restart Hall C FFB
	    
	    taskDelay ( bmw_FFBresume_wait );
	    
	    //
	    // Notify world that Hall C FFB should be back to being locked
	    //
	    setNotice(2,0);
	    bmw_alive =1;
	  }
	}  // if energy
	
      }  // if seq is active
    } // loop over sequences
	
    // After last object (whether successful or not) 
    // un-pause FFB, and wait for period to assure resumption
    // before releasing cycle notification
    fprintf ( stdout, "bmwClient::: Here I Release FFB\n");
    if(!bmw_test) caputFFB(0,0);  // restart FFB
    
    fprintf ( stdout, "bmwClient::: Here I Release Compton Orbit Lock\n");
     if(!bmw_test) caputFFB(2,0);  // restart Compton Lock
    
    taskDelay( bmw_FFBresume_wait );

    //    
    // notify the world that this is over
    //
    setNotice(1,0);
    if (bmw_RFlock_in_HallA==1) setNotice(2,0);
    bmw_alive =1;

    //
    // After last object (whether successful or not), take a break
    //
    bmw_cycle_number = -1;
    if(bmw_test) fprintf(stdout,"bmwClient:: bmw_cycle_number: \t %d\n", bmw_cycle_number);
    //
    if ( bmw_c_verbose || bmw_c_terse )
      fprintf ( stdout, "bmwClient::: starting rest period\n");
    
    for ( i = 0; i < bmw_steps_per_pause && !bmw_die_die_die; ++i)
      {
	bmw_alive =1;
	taskDelay ( bmw_ticks_per_step );
      }
  }    // end of while(!bmw_die_die_die)

  //
  // Be sure that world isn't thinking that I'm still modulating energy
  //
  setNotice(3,0);
  // make sure that all FFB are unpaused and all notices cleared, in
  // case kill switch set during cycle:
  if(!bmw_test){ 
    caputFFB(0,0);  // make sure Hall A FFB isn't paused
    caputFFB(2,0);
  }  

  // make sure Compton Lock isn't paused
  if (bmw_RFlock_in_HallC==1) {
    if(!bmw_test) caputFFB(1,0);  // make sure Hall C FFB isn't paused
  }
  taskDelay(bmw_FFBresume_wait);
  //
  // Be sure that world isn't waiting for Hall C FFB notice
  //
  setNotice(2,0);
  //
  // Be sure that world isn't waiting for Hall A FFB notice
  //
  setNotice(1,0);

  //  bmw_status =0;
  return (OK);
}

/******************************************************************************

setNotice - set FlexIO or DAC outputs for bmw script

*/

STATUS setNotice(int flag, int val) {
  int chan;

  fprintf(stdout,"Setting notice: flag %d   Value %d \n",flag,val);

  if (flag == 1) {
    chan = bmw_flag_1;
  } else if (flag == 2) {
    chan = bmw_flag_2;
  } else if (flag == 3) {
    chan = bmw_flag_3;
  } else {
    return(ERROR);
  }

  fprintf(stdout,"  Writing values: Chan: %d  Value:  %d \n ",chan,val);
  flexioWriteChan(val, chan);

  return(OK);
}
