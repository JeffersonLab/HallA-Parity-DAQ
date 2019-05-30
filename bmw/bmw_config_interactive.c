/*****************************************
*  bmw_config.c                          *
*  routines used to interface the        * 
*  bmw Client with the configuration GUI *
*                                        *
*  K. Paschke           May, 2002        *
******************************************/

#include <vxWorks.h>
#include <types.h>
#include <vme.h>
#include <stdioLib.h>
#include <logLib.h>
#include <math.h>
#include <string.h>
#include <ioLib.h>

#include "bmw_config.h"
#include "../cfSock/cfSock.h"

extern STATUS bmwClient();

void task_BMW_interactive(long command)
{
  int junk;
  char                workName[16];  /* name of work task */
  switch (command)
    {
    case BMW_KILL:
      // set kill flag for bmw
      printf("send BMW kill\n");
      bmw_die_die_die = TRUE;
      bmw_flight_plan = 0;
      return;
    case BMW_UNKILL:
      printf("send BMW unkill\n");
      // set kill flag for bmw
      bmw_die_die_die = FALSE;
      return;
    case BMW_GET_STATUS:
      // return flag for status of bmw
      printf("bmw_status=0x%x\n",bmw_status);
      printf("bmw_die_die_die=0x%x\n",bmw_die_die_die);
      return;
    case BMW_CHECK_ALIVE:
      // return flag for status of bmw
      printf("bmw_status=0x%x\n",bmw_status);
      printf("bmw_status=0x%x\n",bmw_alive);
      bmw_alive = 0;
      return;
    case BMW_START:
      printf("starting bmwClient\n");
      bmw_die_die_die = FALSE;
      bmw_flight_plan = 1;
      taskSpawn("bmwClient", SERVER_WORK_PRIORITY, 0, SERVER_STACK_SIZE,
       		bmwClient,1,0,0,0, 0, 0, 0, 0, 0, 0);      
      return;
    case BMW_TEST_START:
      printf("starting bmwClient_test\n");
      bmw_die_die_die = FALSE;
      bmw_flight_plan = 2;
      taskSpawn("bmw_test", SERVER_WORK_PRIORITY, 0, SERVER_STACK_SIZE,
       		bmwClient,3,0,0,0, 0, 0, 0, 0, 0, 0);      
      return;
    }

}










