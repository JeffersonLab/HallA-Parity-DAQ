/*----------------------------------------------------------------------------*
 *  Copyright (c) 1991, 1992  Southeastern Universities Research Association, * 
 *                            Continuous Electron Beam Accelerator Facility   *
 *                                                                            *
 *    This software was developed under a United States Government license    *
 *    described in the NOTICE file included as part of this distribution.     *
 *                                                                            *
 * CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606 *
 *      heyes@cebaf.gov   Tel: (804) 249-7030    Fax: (804) 249-7363          *
 *----------------------------------------------------------------------------*
 * Discription: follows this header.
 *
 * Author:
 *	Graham Heyes
 *	CEBAF Data Acquisition Group
 *
 * Revision History:
 *      $Log:	obj.h,v $
*	  Revision 1.1  94/05/04  13:23:47  13:23:47  heyes (Graham Heyes)
*	  Initial revision
*	  
*	  Revision 1.1  94/03/15  11:54:02  11:54:02  heyes (Graham Heyes)
*	  Initial revision
*	  
*	  Revision 1.1  93/10/27  08:48:41  08:48:41  heyes (Graham Heyes)
*	  Initial revision
*	  
 *	  Revision 1.11  1993/05/04  12:36:49  watson
 *	  array of client handles
 *
 *	  Revision 1.10  1993/03/16  14:04:39  heyes
 *	  reinsert for coda 1.2
 *
 *	  Revision 1.9  1992/11/06  19:40:00  quarrie
 *	  Add DAREADMESSAGES etc.
 *
 *	  Revision 1.8  1992/11/05  18:03:38  heyes
 *	  ing states added correctly
 *
 *	  Revision 1.7  1992/09/14  15:03:13  quarrie
 *	  Correct stupid typos
 *
 *	  Revision 1.6  1992/09/14  15:00:14  quarrie
 *	  More aliases defined
 *
 *	  Revision 1.5  1992/09/09  13:32:10  quarrie
 *	  Added more Actions
 *
 *	  Revision 1.4  1992/07/21  17:52:17  heyes
 *	  run time debug on vxworks
 *
 *	  Revision 1.3  1992/06/29  15:48:31  heyes
 *	  made consistent with rc.h
 *
 *	  Revision 1.2  1992/06/25  19:01:26  heyes
 *	  insert into RCS for first time
 *
 *	  Revision 1.1  1992/06/25  18:30:26  heyes
 *	  Initial revision
 *
 *
 *----------------------------------------------------------------------------*/

#ifndef _RC_INCLUDED_
#include "rc.h"
#endif

#define DA_MAGIC         0x7030
/* define states possible... */

#define DA_BOOTING        1
#define DA_BOOTED         2
#define DA_CONFIGURING    3
#define DA_CONFIGURED     4
#define DA_DOWNLOADING    5
#define DA_DOWNLOADED     6
#define DA_PRESTARTING    7
#define DA_PAUSED         8
#define DA_PAUSING        9
#define DA_ACTIVATING     10
#define DA_ACTIVE         11
#define DA_ENDING         12
#define DA_VERIFYING      13
#define DA_VERIFIED       14
#define DA_TERMINATING    15
#define DA_PRESTARTED     16
#define DA_RESUMING       17
#define DA_STATES         18


/* define possible state actions for historical reasons */

#define DA_CONFIGURE     DACONFIGURE
#define DA_DOWNLOAD      DADOWNLOAD
#define DA_PRESTART      DAPRESTART
#define DA_END           DAEND
#define DA_PAUSE         DAPAUSE
#define DA_GO            DAGO
#define DA_TERMINATE     DATERMINATE
#define DA_RESET         DARESET
#define DA_REPORT        DAREPORT
#define DA_BECOMEMASTER  DABECOMEMASTER
#define DA_CANCELMASTER  DACANCELMASTER
#define DA_ISMASTER      DAISMASTER
#define DA_READMESSAGES  DAREADMESSAGES

#define DA_CLASSES      12
#define STREAM_SOCKETS   1
#define STREAM_INTERNAL  2
#define STREAM_INPUT     0
#define STREAM_OUTPUT    4

#define TS_CLASS         2
#define ROC_CLASS        3
#define EB_CLASS         4
#define ANA_CLASS        5
#define RCS_CLASS        6

typedef struct classStruct *objClass;
typedef struct classStruct SERVICE;

typedef struct classStruct {
  short magic;
  int class;
  char *client[2];
  char *logClient;
  char *name;
  int *instance;
  configS config;
  int protocolNumber;
  short state;
  int action_count;
  int (**actions)();
  long *transitions;

  long **intParams;
  char **intParamN;
  int  nbIntParams;

  char ***charParams;
  char **charParamN;
  int  nbCharParams;

  float **realParams;
  char **realParamN;
  int  nbRealParams;
  char *log;
  char *target;
  long targetPN;
  int targetOK;
} objClassStore;
