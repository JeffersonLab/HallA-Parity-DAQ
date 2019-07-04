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
 * Revision History:<
 *      $Log: da.h,v $
*	  Revision 1.1  1994/05/04  13:23:40  heyes
*	  Initial revision
*
*	  Revision 1.1  94/03/16  07:55:32  07:55:32  heyes (Graham Heyes)
*	  Initial revision
*	  
*	  Revision 1.1  94/03/15  11:52:53  11:52:53  heyes (Graham Heyes)
*	  Initial revision
*	  
*	  Revision 1.1  93/10/27  08:46:27  08:46:27  heyes (Graham Heyes)
*	  Initial revision
*	  
 *	  Revision 1.9  93/04/29  14:32:13  14:32:13  heyes (Graham Heyes)
 *	  fix alpha test problems
 *	  
 *	  Revision 1.8  1993/03/29  14:43:44  heyes
 *	  remove referneces to VxWorks.h
 *
 *	  Revision 1.7  1993/03/24  15:31:55  heyes
 *	  remove roc id from event headers
 *
 *	  Revision 1.6  1993/03/16  14:05:32  heyes
 *	  reinsert for coda 1.2
 *
 *	  Revision 1.5  1992/08/14  15:30:54  heyes
 *	  1.1.2 release- support for VME
 *
 *	  Revision 1.4  1992/07/21  17:52:00  heyes
 *	  run time debug on vxworks
 *
 *	  Revision 1.3  1992/06/26  12:44:20  heyes
 *	  removed uio.h from Vxworks, no longer needed
 *
 *	  Revision 1.2  1992/06/25  19:01:07  heyes
 *	  insert into RCS for first time
 *
 *	  Revision 1.1  1992/06/25  18:29:25  heyes
 *	  Initial revision
 *
 *
 *----------------------------------------------------------------------------*/

#define _DADEFINED
#ifdef VXWORKS

#include "vxWorks.h"
#include "types.h"
#include "logLib.h"
#include "memLib.h"
#include "string.h"
#include "semLib.h"
#include "taskLib.h"
#include "socket.h"
#include "sockLib.h"
#include "in.h"
#include "intLib.h"
#include "iosLib.h"
#include "inetLib.h"
#include "hostLib.h"
#include "ioLib.h"
#include "stdioLib.h"
#include "loadLib.h"
#include "errnoLib.h"
#include "sigLib.h"
#include "selectLib.h"
#include "msgQLib.h"
#include "sysSymTbl.h"
#include "symLib.h"
#include "msgLib.h"
#include <netdb.h>
#include <rpc.h>
#include <rpcGbl.h>
#else
#include "sys/types.h"
#include "errno.h"
#include "sys/uio.h"
#include "netinet/in.h"
#include "sys/time.h"

#include <stdio.h>
#include <strings.h>
#include <sys/socket.h>
#include <netdb.h>
#include <rpc/rpc.h>
#include <sys/time.h>

#include "msgLib.h"

typedef void (*VOIDFUNCPTR) ();

#endif /* ifdef VXWORKS */

#include <setjmp.h>

#define MAX_FRAG 4096                            /* lwords */

#define TCP_ON 1
#define LOCK_STEP 2

#define DA_TRIGGER_PROC     5
#define DA_DONE_PROC        6
#define DA_EVENT_WRITE_PROC 7
#define EV_SYNC     16
#define EV_PRESTART 17
#define EV_GO       18
#define EV_PAUSE    19
#define EV_END      20

#define EV_BANK_HDR  0x00000100
#define EV_BAD       0x10000000

#define PHYS_BANK_HDR(t,e) (unsigned long)((((t)&0xf)<<16) | \
					     ((e)&0xff) | \
					     EV_BANK_HDR)

#define CTL_BANK_HDR(t) (unsigned long)((((t)&0xffff)<<16) | \
					  0x000001CC)

#define IS_BANK(b) (((unsigned long) (b) && EV_BANK_HDR)==EV_BANK_HDR)

#define DECODE_BANK_HEADER(b,t,e) { t = (b[1]>>16)&0xffff;\
				      e = b[1]&0xff;}


#define EV_BANK_ID 0xc0010100
#define EV_HDR_LEN 4

typedef struct configStruct *configS;

typedef struct configStruct
  {
    char *readOutFn;
    char *runName;
    int runType;
    int runNumber;
    int hardwareMask;
    int triggerMask;
    int configurationMask;
    int maxEvents;
    int priority;
    int lockMode;
    unsigned long magic;
    unsigned long text;
    unsigned long data;
    unsigned long bss;
    char *dacode[3];
    VOIDFUNCPTR *daprocs;
    long *dabufp;
    long    *dabuf;		/* current transmit buffer */
    long    *outBuf;		/* current transmit buffer */
    long     *dabufs[2];	/* pointers to data buffers */
    char whichbuf;			/* current buffer index */

    unsigned long recNb;
    int ctableS;
    char ctableN[80];
    unsigned long *ctable;
    int eventTask;
    int ourID;			/* ID of this roc */
#ifdef VXWORKS
    SEM_ID eventMsgQ;		/* isr synchronization semaphore */
#endif
    char *usrString;
    
  } configStructS;

extern jmp_buf global_env[];
extern long global_env_depth;
extern char *global_routine[100];

#define recoverContext(name) (setjmp(global_env[++global_env_depth])+((global_routine[global_env_depth] = name)==0))


