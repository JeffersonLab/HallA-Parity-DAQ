/*----------------------------------------------------------------------------*
 *  Copyright (c) 1998        Southeastern Universities Research Association, *
 *                            Thomas Jefferson National Accelerator Facility  *
 *                                                                            *
 *    This software was developed under a United States Government license    *
 *    described in the NOTICE file included as part of this distribution.     *
 *                                                                            *
 * TJNAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606 *
 *      heyes@cebaf.gov   Tel: (757) 269-7030    Fax: (757) 269-5800          *
 *----------------------------------------------------------------------------*
 * Description:
 *      ET system sample event client
 *
 * Author:
 *	Carl Timmer
 *	TJNAF Data Acquisition Group
 *
 * Revision History:
 *
 *----------------------------------------------------------------------------*/
 
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#ifdef sun
#include <thread.h>
#endif
#include "et.h"



/* prototype */
static void * signal_thread (void *arg);

int main(int argc,char **argv) {  
  int             i, j, status, swtch, numread, totalread=0;
  int		  con[ET_STATION_SELECT_INTS];
  pthread_t       tid;
  et_statconfig   sconfig;
  et_openconfig   openconfig;
  sigset_t        sigblock;
  struct timespec timeout;
  et_att_id       attach1;
  et_stat_id      my_stat;
  et_sys_id       id;
  int             selections[] = {1,-1,-1,-1}; /* 1 selects ev_type 1 only */
  int heartbeat, pid;
  char hostname[255];

  /*  Default input parameters.  */
  int  quietmode   = 0;
  char *et_file    = "/tmp/et_sys_par2";
  char *input_host = "129.57.192.102";




  for (i=1; i<argc; i++){
    if (argv[i][0] == '-' && (argv[i][1]=='h'||argv[i][1]=='u')){
      printf("Usage: et_status [-q] [-H <host_address>] [<et_filename>]\n");
      exit(0);
    } else if (argv[i][0] == '-' && argv[i][1]=='q'){
      /*      printf("Quiet mode!\n");  */
      quietmode = 1;
    } else if (argv[i][0] == '-' && argv[i][1]=='H'){
      /* Host address */
      if ( (i+1)<argc){
	i++;
	/*	printf("Input host address == %s\n", argv[i]); */
	input_host = &argv[i][0];
      }
    } else {
      /*      printf("Input ET filename == %s\n", argv[i]); */
      et_file = &argv[i][0];
    }
  }

  timeout.tv_sec  = 1;
  timeout.tv_nsec = 1;
  
  /*************************/
  /* setup signal handling */
  /*************************/
  /* block all signals */
  sigfillset(&sigblock);
  status = pthread_sigmask(SIG_BLOCK, &sigblock, NULL);
  if (status != 0) {
    printf("et_status: pthread_sigmask failure\n");
    exit(1);
  }

#ifdef sun
  /* prepare to run signal handling thread concurrently */
  thr_setconcurrency(thr_getconcurrency() + 1);
#endif

  /* spawn signal handling thread */
  pthread_create(&tid, NULL, signal_thread, (void *)NULL);
  

  /* open ET system */
  et_open_config_init(&openconfig);
  et_open_config_sethost(openconfig, input_host);
  et_open_config_setcast(openconfig, ET_DIRECT);

  if (et_open(&id, et_file, openconfig) != ET_OK) {
    printf("et_status: Can't open ET file, %s, on host, %s.\n",
	   et_file, input_host);
    exit(1);
  }
  et_open_config_destroy(openconfig);

  if (quietmode == 0){
    et_system_getpid(id, &pid);
    et_system_getheartbeat(id, &heartbeat);
    et_system_gethost(id, &hostname);
    printf("ET system host      \t: %s\n", hostname);
    printf("ET system filename  \t: %s\n", et_file);
    printf("ET system process ID\t: %d\nET system heartbeat\t: %d\n",
	   pid, heartbeat);
  }
  if (et_close(id) != ET_OK) {
    printf("et_status: error closing ET");
    exit(1);
  }


  
  exit(0);
}

/************************************************************/
/*              separate thread to handle signals           */
static void * signal_thread (void *arg)
{
  sigset_t   signal_set;
  int        sig_number;
 
  sigemptyset(&signal_set);
  sigaddset(&signal_set, SIGINT);
  
  /* Not necessary to clean up as ET system will do it */
  sigwait(&signal_set, &sig_number);
  printf("et_status: got a control-C, exiting\n");
  exit(1);
}
