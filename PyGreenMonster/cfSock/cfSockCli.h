/* cfSockCli.h - header used by cfSock client, replacing the
                 missing vxWorks.h  */

#if	!defined(FALSE) || (FALSE!=0)
#define FALSE		0
#endif

#if	!defined(TRUE) || (TRUE!=1)
#define TRUE		1
#endif

/* low-level I/O input, output, error fd's */
#define	STD_IN	0
#define	STD_OUT	1
#define	STD_ERR	2

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "GreenSock.h"
#include "cfSock.h"

/* Name or IP addresses of the various servers inet on ethernet*/
char * ServerName_CountingHouse="129.57.164.13"; 
char * ServerName_LeftSpect="129.57.192.14";  
char * ServerName_RightSpect="129.57.192.60"; 
/*char * ServerName_Injector="129.57.206.10"; */   
/*char * ServerName_Injector="129.57.206.93";*/
char * ServerName_Injector="129.57.164.61";
char * ServerName_TestCrate="129.57.164.100"; 

extern int GMSockCommand(int crate_number,
                         long command_type, 
                         long command,
                         long par1,
                         long par2, 
                         long par3);


/* [apar@adaq2 ~/doc]$ more vmeram.doc */


/*    VME RAM for HAPPEX DAQ */
/*    ====================== */


/* Left HRS  (hatsv4 port 8  ?? need to check ) */
/* (unknown date) */

/*    boot device          : geisc0  */
/*    processor number     : 0  */
/*    host name            : adaql2  */
/*    file name            : /adaqfs/halla/a-onl/rich/kern/5.5/vx6100_v4.1  */
/*    inet on ethernet (e) : 129.57.192.14:ffffff00  */
/*    inet on backplane (b):  */
/*    host inet (h)        : 129.57.164.59  */
/*    gateway inet (g)     : 129.57.192.1  */
/*    user (u)             : adev  */
/*    ftp password (pw) (blank = use rsh):  */
/*    flags (f)            : 0x20  */
/*    target name (tn)     : hallavme14  */
/*    startup script (s)   : /adaqfs/halla/apar/vxworks/happex8.boot  */
/*   other (o)            :  */


/* Right HRS */
/* (Oct 7, 2015) */


/*   boot device          : geisc0  */
/*   processor number     : 0  */
/*   host name            : adaq2  */
/*   file name            : /adaqfs/home/apar/vxworks/5.5/vx6100_v3.1  */
/*   inet on ethernet (e) : 129.57.192.60:ffffff00  */
/*   inet on backplane (b):  */
/*   host inet (h)        : 129.57.164.5  */
/*   gateway inet (g)     : 129.57.192.1  */
/*   user (u)             : adev  */
/*   ftp password (pw) (blank = use rsh):  */
/*   flags (f)            : 0x20  */
/*   target name (tn)     : happex7  */
/*   startup script (s)   : /adaqfs/halla/apar/vxworks/happex7.boot  */
/*   other (o)            :  */


/* Trigger Supervisor */
/* (Oct 7, 2015) */

/*   boot device          : fei                  */
/*   unit number          : 0                                                      */
/*   processor number     : 0                                            */
/*   host name            : adaq2                             */
/*   file name            : /adaqfs/halla/apar/vxworks/vx5100_feiFD_64MB     */
/*   inet on ethernet (e) : 129.57.164.103:ffffff00                    */
/*   host inet (h)        : 129.57.164.5                                */
/*   gateway inet (g)     : 129.57.164.1                                */
/*   user (u)             : adev                                   */
/*   flags (f)            : 0x20                                 */
/*   target name (tn)     : happex4                   */
/*   startup script (s)   : /adaqfs/halla/apar/vxworks/happex4.boot  */
                                                                  

/* Counting room ROC23 */
/* (Oct 7, 2015) */

/*    boot device          : geisc */
/*    unit number          : 0  */
/*    processor number     : 0  */
/*    host name            : adaq2 */
/*    file name            : /adaqfs/home/apar/vxworks/5.5/vx6100_v3.1 */
/*    inet on ethernet (e) : 129.57.164.13:ffffff00 */
/*    host inet (h)        : 129.57.164.5 */
/*    gateway inet (g)     : 129.57.164.1 */
/*    user (u)             : adev */
/*    flags (f)            : 0x20  */
/*    target name (tn)     : halladaq6 */
/*   startup script (s)   : /adaqfs/halla/apar/vxworks/halladaq6.boot */


/* Injector -- Qweak crate */
/* (Oct 7, 2015) */

/*   boot device          : geisc0  */
/*   processor number     : 0  */
/*   host name            : adaq2  */
/*   file name            : /adaqfs/home/apar/vxworks/vx6100_v4.1big  */
/*   inet on ethernet (e) : 129.57.206.93:fffffc00  */
/*   inet on backplane (b):  */
/*   host inet (h)        : 129.57.164.5  */
/*   gateway inet (g)     : 129.57.204.1  */
/*   user (u)             : adev  */
/*   ftp password (pw) (blank = use rsh):  */
/*   flags (f)            : 0x20  */
/*   target name (tn)     : g0inj  */
/*   startup script (s)   : /adaqfs/home/apar/vxworks/g0inj.boot  */
/*   other (o)            :  */
