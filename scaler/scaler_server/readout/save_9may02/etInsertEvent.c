/* etInsertEvent.c
*
*  R. Michaels, Jan 2000
*  For inserting data into the ET system.
*  Inputs: 
*     evbuffer:  CODA data in standard format
*     et_filename:  The ET memory file, eg. /tmp/et_sys_$SESSION
*
*  Notes:
*     This code was based on the /examples/et_producer.c
*     code which I got from C. Timmer in Jan 2000.
*
*/
   
#include <stdio.h>
#include <stdlib.h>
#include "et.h"

#define OK 0
#define ERROR -1
#define DEBUG 0   /* if 1, print some debug statements */

int etInsertEvent(int evbuffer[], char* et_filename)
{  
  int        i, j, status, nevents_max;
  et_att_id  attach;
  et_sys_id  id;
  et_event   *pe;
  et_openconfig openconfig;
  int    len, event_size;   
  char   *pdata;
  int    try;

  len = evbuffer[0]+1;
  event_size = len*sizeof(long);

  if(DEBUG) printf("in etInsertEvent, event length %d\n",event_size);
  
  /* opening the ET system is the first thing we must do */
  try = 1;
  et_open_config_init(&openconfig);
  if(DEBUG) printf("open config init\n");
tryloop:
  status = et_open(&id, et_filename, openconfig);
  if (status != ET_OK) {
    printf("etInsertEvent: et_open status = %d attempt %d\n",status,try);
    if (try < 2) {
        sleep(2);
        try = try+1;
        goto tryloop;
    }
    printf("etInsertEvent: ERROR: et_open failed\n");
    return ERROR;
  }
  et_open_config_destroy(openconfig);

  if(DEBUG) printf("et_open ok\n");
  
  /* set the level of debug output that we want (everything or nothing */
  if(DEBUG) { 
      et_system_setdebug(id, ET_DEBUG_INFO);
  } else {
      et_system_setdebug(id, ET_DEBUG_NONE);
  }   

  /* attach to GRANDCENTRAL station since we are producing events */
  if (et_station_attach(id, ET_GRANDCENTRAL, &attach) < 0) {
    printf("etInsertEvent: error in station attach\n");
    return ERROR;
  }

  if( !et_alive(id)) {
    printf("ERROR: ET is not alive\n");
    return ERROR;
  }

  if(DEBUG) printf("et alive\n");

  /* get new/unused event */  
  status = et_event_new(id, attach, &pe, ET_SLEEP, NULL, event_size);
  if (status != ET_OK) {
     printf("etInsertEvent: error in et_event_new\n");
     return ERROR;
  }

  if(DEBUG) printf("et got event new\n");  
  
  /* put data into the event here */  
  
  et_event_getdata(pe, (void **)&pdata);
  et_event_setlength(pe, event_size);

  memcpy((void*)pdata, (const void *)evbuffer, event_size);

  if(DEBUG) printf("getdata, setlength, memcpy ok\n");

  /* put event back into the ET system */
  status = et_event_put(id, attach, pe);
  if (status != ET_OK) {
     printf("etInsertEvent: put error\n");
     return ERROR;
  }
  
  if (et_station_detach(id, attach) != ET_OK) {
    printf("etInsertEvent: ERROR: et_station_detach\n");
  }
  if (et_close(id) != ET_OK) {
    printf("etInsertEvent: ERROR: et_close\n");
  }

  if(DEBUG) printf("etInsertEvent finished successfully\n");

  return OK;

}




