/* use vxWorks header files */
#include "vxWorks.h"
#include "sys/socket.h"
#include "in.h"
#include "inetLib.h"
#include "vxWscaler.h"

/* Flags used for interaction with 3800/3801 scalers */
extern short end_run_flag;
extern short coda_end_run;

/***********************************************************
*
*  vxWorks server for TCP/IP readout of scalers 
*
*  Modifications
*
*    27 Feb 98   Throw out the taskSpawn of Work task.
*                Put the scaler read directly into bobServer.
*     9 Mar 99   For the new SIS3801 scalers, get data from
*                a common shared memory which is filled by
*                another task.  Ability to clear scalers is
*                revived, at least for now.
*     3 Jun 99   Allow for both 3800 and 3801 scalers in code.
*     5 Mar 01   Port to linux, byte-swapping issues
*    12 Aug 02   Option to flush out the ring buffer at end
*                of buffer.
*
*    27 Aug 08   Won't be any 3801 in Fall 08, so throw out
*                references to runStartClrSIS and Read3801 
*                (put back later !)
*    15 Sep 09   Version for Compton.  Throw out 3800 too.
*
*/



VOID serverWorkTask (int sFd, struct sockaddr_in clntSockAddr);


STATUS bob2Server (void)

   {
   struct sockaddr_in myAddr ;       /* server's socket address  */
   struct sockaddr_in clientAddr;    /* client's socket address  */
   int   sFd;                        /* socket file descriptor   */
   int   newFd;                      /* socket descriptor from accept */
   int   sockAddrSize = sizeof (struct sockaddr_in);
   unsigned long *adrvme;
  struct request clientRequest;     /* request msg from client */
   struct request bobreply;
   int i,j,k,n,jdum,isca,nchan,status;
   long idata;
   int clr3800flg,clr3801flg;
   int    nRead;
   char   inetAddr[INET_ADDR_LEN];   /* client's inet addr */
   static char  replyMsg[] = "Server got your message";
   unsigned short temp;
   static int debug = 0;  /* to debug (1) or not (0) */
   
   static int never_clear = 0;  /* if 1, force scalers to NEVER clear */
                                /* This is safer for running conditions */


#include "scaler1.h"
#include "scaler2.h"

if(addr_assign==0) {
  addr_assign=1;
  for (ix=0; ix<NUMSCALER; ix++) {
    res = sysBusToLocalAdrs(0x39, vmeoff[ix], &laddr);
    if (res != 0) {
      printf("scaser: ERROR:  sysBusToLocalAdrs address %x\n",laddr);
    } else {
      if(scalertype[ix]==1151) 
	slecroy1151[ix] = (struct lecroy_1151_scaler *)laddr;
      if(scalertype[ix]==7200) 
	sstr7200[ix] = (struct str_7200_scaler *)laddr;
      if(scalertype[ix]==560) 
	scaen560[ix] = (struct caen_v560_scaler *)laddr;
    }
  }
}


/* set up local address */

   bzero (&myAddr, sockAddrSize);
   myAddr.sin_family = PF_INET;
   myAddr.sin_port = htons (SERVER_PORT_NUM);
   myAddr.sin_addr.s_addr = htonl (INADDR_ANY);

/* create socket */

   if ((sFd = socket (PF_INET, SOCK_STREAM, 0)) == ERROR)
     {
      perror ("socket");
      return (ERROR);
     }

/* bind socket */

   if (bind (sFd, (struct sockaddr *) &myAddr, sockAddrSize) == ERROR)
     {
      perror ("bind");
      close (sFd);
      return (ERROR);
     }

/* create queue for client connection requests */

   if (listen (sFd, MAX_QUEUED_CONNECTIONS) == ERROR)
     {
      perror ("listen");
      close (sFd);
      return (ERROR);
     }

/* accept new connect requests and spawn tasks to deal with them */

FOREVER 
  {
     if ((newFd = accept (sFd, &clientAddr, &sockAddrSize)) == ERROR)
       {
        perror ("accept");
        close (sFd);
        return (ERROR);
       }


/*  read client message, display message, and if requested, reply */


    for (k=0; k<=sizeof(replyMsg); k++) {
      bobreply.message[k] = 'e';
    }

    if(debug) printf("about to read\n");

    while ((nRead = fioRead (newFd, (char *) &clientRequest,
                    sizeof(clientRequest))) > 0)
      {
      /* convert internet address to dot notation */
      inet_ntoa_b (clientAddr.sin_addr, inetAddr);

     clientRequest.flag1 = ntohl(clientRequest.flag1);
     clientRequest.clearflag = ntohl(clientRequest.clearflag);
     clientRequest.reply = ntohl(clientRequest.reply);
#ifdef TEST_RING
     clientRequest.getring = ntohl(clientRequest.getring);
#endif

     if(debug)
        printf ("MESSAGE FROM: Internet Address %s, port %d \n %s \n",
            inetAddr, ntohs (clientAddr.sin_port), 
            clientRequest.message);    
      
      

/* Flag to clear data :
*      clearflag  = 1   --> clear scalers
*      clearflag != 1   --> read scalers      */

/* Note: never_clear, a compiled flag, can override this */
/* It is not safe to let users clear the scalers during an experiment. */
/* Only CODA start-run can do that ! */


    if((clientRequest.clearflag==1)&&(never_clear==0)) {

       if(debug) printf("\n Clearing scalers... \n");

         clr3800flg = 0;
         clr3801flg = 0;

         for (isca=0;isca<NUMSCALER; isca++) {
            if(scalertype[isca]==1151) {
               slecroy1151[isca]->reset = 0;
  	     }
            if(scalertype[isca]==7200) { 
               sstr7200[isca]->clrR = 0;
               sstr7200[isca]->gEnCntR = 0;
  	     }
            if(scalertype[isca]==560) {
               scaen560[isca]->ClrR = 0;
               temp=scaen560[isca]->clrVETOR;
            }
#ifdef HAVE_3800
            if(scalertype[isca]==3800) {
              clr3800flg = 1;  /* there is at least one 3800 */
	    }
            if(scalertype[isca]>=380101 && scalertype[isca]<=380104) {
              clr3801flg = 1;  /* there is at least one 3801 */
	    }
#endif
	 }            

#ifdef HAVE_3800
         if(clr3800flg==1) ClrSIS3800();
	 /*	 if(clr3801flg==1) runStartClrSIS(); */
#endif


      } else {    /* Read out scalers */

        k = 0;

        for (isca=0; isca<NUMSCALER; isca++) {

           if (debug) {
              printf("read scaler %d \n",isca);
              printf("scaler type %d \n",scalertype[isca]);
              printf("the vme address %x \n",vmeoff[isca]);
           }
	   if (isca < MSG_SIZE) bobreply.message[isca]='0';
            if(scalertype[isca]==1151) {
               for (i=0;i<16;i++) {
                 bobreply.ibuf[k++] = htonl(slecroy1151[isca]->scaler[i]);
                 if(debug) printf("LeCroy 1151 sca[%d] = %x\n",
                     k,slecroy1151[isca]->scaler[i]);
               }                 
	    }
            if(scalertype[isca]==7200) { 
  	       if (isca < MSG_SIZE) bobreply.message[isca]='1';
               for (i=0;i<32;i++) {
                 bobreply.ibuf[k++] = htonl(sstr7200[isca]->readCntR[i]);
                 if(debug) printf("Struck 7200 sca[%d] = %x\n",
                     k,sstr7200[isca]->readCntR[i]);
               }                 
	    }
            if(scalertype[isca]==560) {
               for (i=0;i<16;i++) {
                 bobreply.ibuf[k++] = htonl(scaen560[isca]->CntR[i]);
                 if(debug) printf("Caen V560 sca[%d] = %x\n",
                     k,scaen560[isca]->CntR[i]);
               }                 
	    }


#ifdef HAVE_3800
/* No support for 3801 scalers at the moment (Aug 2009) */
            if(scalertype[isca]==380101||scalertype[isca]==380102||
               scalertype[isca]==380103||scalertype[isca]==380104) {
  	       if (isca < MSG_SIZE) bobreply.message[isca]='1';
   	       for (i=0;i<32;i++) {
		 if(scalertype[isca]==380101) {
                     bobreply.ibuf[k++] = htonl(Read3801(0,0,i));
                     if(debug) printf("+ Hel SIS 3801 sca[%d] = %x\n",
				      i,Read3801(0,0,i));
		 }
                 if(scalertype[isca]==380102) {
                     bobreply.ibuf[k++] = htonl(Read3801(0,1,i));
                     if(debug) printf("- Hel SIS 3801 sca[%d] = %x\n",
				      i,Read3801(0,1,i));
		 } 
                 if(scalertype[isca]==380103) {
                     bobreply.ibuf[k++] = htonl(Read3801(1,0,i));
                     if(debug) printf("2nd + Hel SIS 3801 sca[%d] = %x\n",
				      i,Read3801(1,0,i));
		 }
                 if(scalertype[isca]==380104) {
                     bobreply.ibuf[k++] = htonl(Read3801(1,1,i));
                     if(debug) printf("2nd - Hel SIS 3801 sca[%d] = %x\n",
				      i,Read3801(1,1,i));
		 }
	       }
	    }

            if(scalertype[isca]==3800) {
  	       if (isca < MSG_SIZE) bobreply.message[isca]='1';
   	       for (i=0;i<32;i++) {
                   bobreply.ibuf[k++] = htonl(Read3800(vmeoff[isca],i));
                   if(debug) printf("SIS 3800 sca[%d] = %x\n",
			 i,Read3800(vmeoff[isca],i));
               }
	    }
#endif

	}
#ifdef TEST_RING
        if (clientRequest.getring == 1) {  /* Get ring buffer */
          n = NumRing();
          k = 1;
          if (n > MAXRING-1) n = MAXRING-1;
          bobreply.ring[0] = htonl(n);
          for (i = 0; i < n; i++) {
  	    for (j = 0; j < NUM_IN_RING; j++) {
               idata = ReadRing(i,j);
	       bobreply.ring[k++] = htonl( idata );
               if (debug) printf("ReadRing(%d, %d) = %d\n",i,j,idata);
	    }
	  }
          ResetRing(n);
        }
#endif
      
      }

      if (clientRequest.reply) {
          if (write (newFd, (char *) &bobreply, sizeof (bobreply)) == ERROR)
            perror ("write");
      }
  
      if (nRead == ERROR)
      perror ("read"); 
 
      close (newFd);

      }
  }
}









