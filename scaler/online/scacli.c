/* Client code to read VME scalers */
/* Server runs on the VME cpu */
/* Option '-r' gets the ring buffer.                                 */

/***********************************************************
*
*   Purpose:  Read the VME scalers.
*
*   vxWorks client running on a UNIX host
*
*
*/

#include <sys/ioctl.h>
#include <sys/types.h>
#include "time.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include "vxWscaler.h"

main (int argc, char *argv[])
  {
       int print_file=0;                     /* to print(1) data or not(0) */
       FILE *outfile;                        /* data file for output */
       char outfile_name[200];
       int   sFd;                            /* socket file descriptor */
       int i,j,k,n,ilen,numchan,mlen;
       long stat;
       int isca,sca,shead,nRead,nRead1;
       static int clearall=0;                /* flag to clear scalers */
       static int getring=0;                 /* to get ring(1) or not(0) */
       static int lprint=1;
       struct request myRequest;             /*  request to send to server */
       struct scalerdata bobReply;           /*  data sent from server    */
       struct sockaddr_in serverSockAddr;    /*  socket addr of server */
       char  reply;
       char  replyBuf[REPLY_MSG_SIZE];
       static char  defMsg[] = "Bob wants data";  /* test message for VME */
       static int debug=0;
       int runnum = 0;
       int bytesread;
       int nskip,nchan,ntot,nring,nhel;
       char cb;

#include "scaler1.h"
#include "scaler2.h"

   /* defaults */

    clearall=0;
    getring=0;
    print_file=0;
    sprintf(outfile_name,"scaler.dat");

    /* Process arguments */

    while (--argc > 0) {
        ++argv;
        if ((*argv)[0]==NULL) break;
        if ((*argv)[0]!= '-') goto usage;
        cb = (*argv)[1];
        switch (cb) {
           case 'c':                  /* clear all scalers */
              clearall=1;
              break;
  	   case 's':                  /* silent mode */
 	      lprint = 0;
              break;
   	   case 'r':                  /* specify a run number */
              --argc;
              ++argv;
	      runnum = atoi(*argv);
              break;
  	   case 'b':                  /* get the ring buffer */
              getring=1;
              break;
           case 'x':                  /* Hexidecimal debug printout */
              lprint=2;
              break;
           case 'd':                  /* Decimal debug printout */
              lprint=3;
              break;
  	   case 'p':                  /* Print to data file */
	      print_file=1;	  
	      break;
   	   case 'f':                  /* Adjust default name of output file */
	      print_file=1;	  
              --argc;
              ++argv;
              if (strlen(*argv) < 200) {
		strcpy(outfile_name,*argv);
	      } else {
                printf("ERROR: filename %s is spuriously long\n",*argv);
	      }
	      break;
	   case 'h':                  /* help */
  	      goto usage;
           default:
              printf("ERROR: Illegal option %c \n\n",cb);
usage:
              printf("USAGE: scread [-c -s -r -x -d -p -r run -f file -h]\n");
              printf("-c is to clear scalers.\n");
              printf("With no further options, scread prints cumulative\n");
              printf("counts to terminal with compact format.\n");
              printf("-r run is to specify a run number.\n");
              printf("-s run is to run silently\n");
              printf("-b is to obtain ring buffer additionally\n");
              printf("-x is to get hexidecimal printout \n");
              printf("-d is to get integer printout \n");
	      printf("-p write to [default] data file with option -d or -x\n");
	      printf("-f filename writes data to output file filename (-p implicit here)\n");
              printf("-h is to get help (this printout)\n");
              exit(1);
	 }
    }

    if (lprint && print_file) printf("Will print output to %s\n",outfile_name);

    myRequest.clearflag = htonl(clearall);   
    myRequest.getring   = htonl(getring);

    for (k=0; k<=sizeof(defMsg); k++) {
         myRequest.message[k] = defMsg[k];
    }

/* create socket */

    if ((sFd = socket (PF_INET, SOCK_STREAM, 0)) == ERROR )
        {
        perror ("socket");
        return (ERROR);
        }


/* bind not required -- port number will be dynamic */

/* build server socket address */

/*    bzero (&serverSockAddr, sizeof(serverSockAddr) );*/
    serverSockAddr.sin_family = PF_INET;
    serverSockAddr.sin_port = htons (SERVER_PORT_NUM);
    serverSockAddr.sin_addr.s_addr = inet_addr (SERVER_INET_ADDR);

/* connect to server */
    

    if(connect (sFd, (struct sockaddr *) &serverSockAddr, sizeof(serverSockAddr)) == ERROR)
       {
       printf("failed to connect \n");
       perror ("connect");
       close (sFd);
       return (ERROR);
       }


/* send request to server */

    if(write(sFd, (char *)&myRequest, sizeof(myRequest)) == ERROR)
      {
      perror ("write");
      close (sFd);
      return (ERROR);
      }

    nRead  =read (sFd, (char *)&bobReply, sizeof(bobReply));
    if(nRead < 0) {
       printf("Error from reading from scaler server\n");
       exit(0);
    }
    if (debug) printf("read nRead %d   %d\n",nRead,sizeof(bobReply));

    while (nRead < sizeof(bobReply)) {
       nRead1 = read (sFd, ((char *) &bobReply)+nRead,
                    sizeof(bobReply)-nRead);
       if (debug) printf("reading some more \n");
       if(nRead1 < 0) {
          printf("Error from reading from scaler server\n");
          exit(0);
       }
       nRead += nRead1;
    }

    if(clearall) {
          printf("Scalers cleared \n");
          exit(1);
    }

    if (debug) printf ("MESSAGE FROM SERVER :  \n %s \n",bobReply.message); 

/* byte swapping */
       for (k=0 ; k < 32*MAXSCALER; k++) bobReply.scalersums[k] = ntohl(bobReply.scalersums[k]);
       for (k=0 ; k < NUM_RING*32*MAXSCALER; k++) bobReply.ringbuff[k] = ntohl(bobReply.ringbuff[k]);

       if(lprint==1) {
         printf ("\n\n\n  ========    Scalers   ===============\n\n");
         sca=0;
         for (k=0; k < NUMSCALER; k++) {
            sca=4*k;
            for (i=0;i<4;i++) {
                printf ("%3d :  %8d %8d %8d %8d %8d %8d %8d %8d \n",
                8*(sca+i)+1,bobReply.scalersums[(sca+i)*8],
                bobReply.scalersums[(sca+i)*8+1],
                bobReply.scalersums[(sca+i)*8+2], 
                bobReply.scalersums[(sca+i)*8+3],
                bobReply.scalersums[(sca+i)*8+4],
                bobReply.scalersums[(sca+i)*8+5],
                bobReply.scalersums[(sca+i)*8+6],
                bobReply.scalersums[(sca+i)*8+7]);
	      }
	 }
	 if (getring) {
           nring = bobReply.ringbuff[0];
	   printf("\nRing buffer.  Num in ring %d \n",nring);
           for (j = 0; j < nring; j++) {
             printf("clock %d    qrt+helicity %d\n",bobReply.ringbuff[j*34+1],bobReply.ringbuff[j*34+2]);
  	     for (k = 0; k < 4; k++) {
	       printf("ring#%d buf[%d] = %d %d %d %d %d %d %d %d\n",j,8*k,bobReply.ringbuff[j*34+8*k+3],bobReply.ringbuff[j*34+8*k+4],bobReply.ringbuff[j*34+8*k+5],bobReply.ringbuff[j*34+8*k+6],bobReply.ringbuff[j*34+8*k+7],bobReply.ringbuff[j*34+8*k+8],bobReply.ringbuff[j*34+8*k+9],bobReply.ringbuff[j*34+8*k+10]);
	     }
	   }
	 }
       }

       if(lprint>1) {
         k=0;
         for (isca=0; isca<NUMSCALER; isca++) {
            nchan=32;
            for (i=0;i<nchan;i++) {
               if (lprint==2) printf("%x\n",bobReply.scalersums[k++]);
               if (lprint==3) printf("%d\n",bobReply.scalersums[k++]);
	    }
	 }
       }

       if(print_file) {
	 if((outfile=fopen(outfile_name,"a+"))==NULL) {
	   if(debug) printf("could not open file %s\n",outfile_name);
	   goto skip1;
	 }
         k=0;
         for (isca=0; isca<NUMSCALER; isca++) {
            fprintf(outfile,"run=%d  scaler=%x \n",runnum,isca);
            nchan=32;
            for (i=0;i<nchan;i++) {
  	       fprintf(outfile,"%d \n",bobReply.scalersums[k++]);
	    }
	 }
  	 if (getring) {   /* print ring buffer data */
             nring = bobReply.ringbuff[0];
             fprintf(outfile,"nring=%d\n",nring);
             for (j = 0; j < nring; j++) {
               fprintf(outfile,"clock=%d    qrt+helicity=%d\n",bobReply.ringbuff[j*34+1],bobReply.ringbuff[j*34+2]);
  	     for (k = 0; k < 4; k++) {
	       fprintf(outfile,"%d %d %d %d %d %d %d %d\n",bobReply.ringbuff[j*34+8*k+3],bobReply.ringbuff[j*34+8*k+4],bobReply.ringbuff[j*34+8*k+5],bobReply.ringbuff[j*34+8*k+6],bobReply.ringbuff[j*34+8*k+7],bobReply.ringbuff[j*34+8*k+8],bobReply.ringbuff[j*34+8*k+9],bobReply.ringbuff[j*34+8*k+10]);
	     }
	    }
         } else {
             fprintf(outfile,"nring=0\n");
	 }
	 fclose(outfile);
       }

skip1:

      close (sFd);
}
