/* use vxWorks header files */
#include "vxWorks.h"
#include "sys/socket.h"
#include "in.h"
#include "inetLib.h"
#include "vxWscaler.h"
#include "SIS.h"

/***********************************************************
*
*  vxWorks server for TCP/IP readout of scalers 
*
*/



VOID serverWorkTask (int sFd, struct sockaddr_in clntSockAddr);


STATUS bobServer (void)

   {
   struct sockaddr_in myAddr ;       /* server's socket address  */
   struct sockaddr_in clientAddr;    /* client's socket address  */
   int   sFd;                        /* socket file descriptor   */
   int   newFd;                      /* socket descriptor from accept */
   int   sockAddrSize = sizeof (struct sockaddr_in);
   unsigned long *adrvme;
   struct request clientRequest;     /* request msg from client */
   struct scalerdata bobReply;
   int i,j,k,n,jdum,isca,nchan,status;
   long idata;
   int clr3800flg,clr3801flg;
   int    nRead, nRead1;
   char   inetAddr[INET_ADDR_LEN];   /* client's inet addr */
   static char  replyMsg[] = "Server got your message";
   unsigned short temp;
   static int debug = 0;  /* to debug (1) or not (0) */
   
   static int never_clear = 0;  /* if 1, force scalers to NEVER clear */
                                /* This is safer for running conditions */


#include "scaler1.h"
#include "scaler2.h"

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
      bobReply.message[k] = replyMsg[k];
    }

    if(debug) printf("about to read\n");

    nRead  =read (newFd, (char *)&clientRequest, sizeof(clientRequest));
    if(nRead < 0) {
       printf("Error from reading request from client\n");
       exit(0);
    }
    if (debug) printf("read nRead %d   %d\n",nRead,sizeof(clientRequest));

    while (nRead < sizeof(clientRequest)) {
       nRead1 = read (sFd, ((char *) &clientRequest)+nRead,
                    sizeof(clientRequest)-nRead);
       if (debug) printf("reading some more \n");
       if(nRead1 < 0) {
          printf("Error from reading from scaler server\n");
          exit(0);
       }
       nRead += nRead1;
    }

    /* convert internet address to dot notation */
    inet_ntoa_b (clientAddr.sin_addr, inetAddr);

    clientRequest.clearflag = ntohl(clientRequest.clearflag);
    clientRequest.getring = ntohl(clientRequest.getring);

    if(debug)
        printf ("MESSAGE FROM: Internet Address %s, port %d \n %s \n",
            inetAddr, ntohs (clientAddr.sin_port), 
            clientRequest.message);    
    if (debug) printf("flags   clear=%d    getring=%d \n",clientRequest.clearflag, clientRequest.getring);      

/* Flag to clear data :
*      clearflag  = 1   --> clear scalers
*      clearflag != 1   --> read scalers      */

/* Note: never_clear, a compiled flag, can override this */
/* It is not safe to let users clear the scalers during an experiment. */
/* Only CODA start-run can do that ! */


    if((clientRequest.clearflag==1)&&(never_clear==0)) {

       if(debug) printf("\n Clearing scalers... \n");

         ClrSIS3800();
	 runStartClrSIS(); 

      } else {    /* Read out scalers */

        k = 0;
        for (isca=0; isca<NUMSCALER; isca++) {
	  /* printf("isca = %d  type %d \n",isca,scalertype[isca]); */
   	   for (i=0; i<32; i++) {
	      if(scalertype[isca]==380101) {
                 bobReply.scalersums[k++] = htonl(Read3801(0,0,i));
                 if(debug) printf("+ Hel SIS 3801 sca[%d] = 0x%x\n",
				    i,Read3801(0,0,i));
	      }
              if(scalertype[isca]==380102) {
                 bobReply.scalersums[k++] = htonl(Read3801(0,1,i));
                 if(debug) printf("- Hel SIS 3801 sca[%d] = 0x%x\n",
				      i,Read3801(0,1,i));
	      } 
              if(scalertype[isca]==3800) {
                 bobReply.scalersums[k++] = htonl(Read3800(vmeoff[isca],i));
                 if(debug) printf("SIS 3800 sca[%d] = 0x%x\n",
			 i,Read3800(vmeoff[isca],i));
	      }
	   }
	}

 /* Ring buffer assumed for only one scaler, at the moment */
        if (clientRequest.getring == 1) {  /* Get ring buffer */
           n = NumRing();
           k = 1;
           if (debug) printf("Ring depth %d %d \n",n,RING_DEPTH);
           if (n > RING_DEPTH-1) n = RING_DEPTH-1;
 /* By convention, 1st element = num in ring. */
           bobReply.ringbuff[0] = htonl(n); 
 /* Next two elements : clock, and qrt+helicity.  Then 32 channels of data */
           for (i = 0; i < n; i++) {
 	     idata = ReadRing(i,0);  /* clock */
	     bobReply.ringbuff[k++] = htonl( idata );
 	     idata = ReadRing(i,1);  /* qrt & helicity bits */
	     bobReply.ringbuff[k++] = htonl( idata );
    	     for (j = 0; j < 32; j++) {
               idata = ReadRing(i,j+2);
	       bobReply.ringbuff[k++] = htonl( idata );
               if (debug) printf("ReadRing(%d, %d) = %d\n",i,j,idata);
	    }
	  }
          ResetRing(n);
       }
      }
      
      if (write (newFd, (char *) &bobReply, sizeof (bobReply)) == ERROR)
            perror ("write");
  
      close (newFd);

   }

 }









