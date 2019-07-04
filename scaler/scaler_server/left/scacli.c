/* Client code to read VME scalers and possibly print them and/or rates 
   R. Michaels, version Dec 2015   */

/* Note, the server runs on the VME cpu */

/* time interval for rate, if computing rate */
#define NMUSEC 2000000   /* usec using usleep */

/* Location in buffer of clock; 0 is first chan */
#define CLKCHAN 0

#include <sys/ioctl.h>
#include <sys/types.h>
#include "time.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include "vxWscaler.h"

/***********************************************************
*
*   Purpose:  Read the VME scalers.
*
*   vxWorks client running on a UNIX host
*
*
*/

main (int argc, char *argv[])
  {
#ifdef print_to_file
       int print_file;
       FILE *outfile;
#endif
       int   sFd;                            /*  socket file descriptor */
       int i,k,n,ilen,numchan;
       long stat;
       int   mlen;                           /*  msg length  */
       int isca,sca,shead,nRead,nRead1;
       static int clearall=0;           /* flag to clear scalers (danger!) */
       static int lprint=1;
       struct request myRequest;             /*  request to send to server */
       struct request bobreply;
       struct sockaddr_in serverSockAddr;    /*  socket addr of server */
       char  reply;
       char  replyBuf[REPLY_MSG_SIZE];
       static char  defMsg[] = "Bob wants some data";
       static int debug=0;
       int bytesread;
       int dainsert;
       int pick1,wchan,sdiff,clkdiff,showrate;
       double scalrate;
       int irate;
       double clockrate=200.;  /* kHz */
       int nskip,nchan,ntot;
       char cb;
       long tag;
       long ev_len;
       struct EVBUF {  // for possible insertion to datastream
           long header[4];
           long bobbuf[32*MAXBLOCKS];  /* assume 32 chan */
       } buf;
       long sdata1[32*MAXBLOCKS];   
       long sdata2[32*MAXBLOCKS];

#include "scaler1.h"
#include "scaler2.h"



	 /* Process arguments */

    dainsert=0;
    clearall=0;
    pick1=0;
    wchan=0;   /* pick out location in buffer of your chan */
    showrate=0;

    for (i=0; i<argc; i++) {
      if(strstr(argv[i],"-")==NULL) wchan=atoi(argv[i]);         
    }

    while (--argc > 0) {
        ++argv;
        if ((*argv)[0]==NULL) break;
        cb = (*argv)[1];
        switch (cb) {
           case 'c':                  /* clear all scalers */
              clearall=1;
              break;
           case 'x':                  /* Hexidecimal printout */
              lprint=2;
              break;
           case 'd':                  /* Decimal printout */
              lprint=3;
              break;
           case 'i':                  /* Data inserted into datastream */
              lprint=0;
              dainsert=1;
              break;
    	   case 'p' :                 /* Pick (p) a channel */
 	      pick1=1;             
              break;
    	   case 'r' :                 /* To show rate instead of counts */
 	      showrate=1;             
              break;
	   case 'h' :                 /* print help */
  	      goto usage;
           default:
  	      break;
usage:
              printf("USAGE: scread [-c -d -x -h -i -p N -r ]\n");
              printf("With no option, scread prints raw data to screen\n");
              printf("-c is to clear scalers\n");
              printf("During a run, the server should probably be\n");
              printf("set up to disallow scread to clear.\n");
              printf("Other options :\n");
              printf("-x is to get hexidecimal printout \n");
              printf("-d is to get integer printout \n");
              printf("-h is to get help (this printout)\n");
              printf("-i is to get data insertion via (et)or(da)Insert\n");
              printf("For monitoring a specific channel\n");
              printf("-p to print channel counts (default is chan #0)\n");
              printf("-p N to print counts on channel N = 0,1,2 ...\n");
              printf("-p N -r to print rate channel N = 0,1,2 ...\n");
              exit(1);
	 }
    }

    if (debug) printf("State: %d %d %d \n",pick1,wchan,showrate);

    if (wchan < 0 | wchan >= 32*MAXBLOCKS) {
      printf("ERROR: index outside of buffer range \n");
      exit(0);
    }


      myRequest.clearflag = htonl(clearall);   

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

    serverSockAddr.sin_family = PF_INET;
    serverSockAddr.sin_port = htons (SERVER_PORT_NUM);
    serverSockAddr.sin_addr.s_addr = inet_addr (SERVER_INET_ADDR);

/* connect to server */
    
/*    printf("About to connect \n"); */

    if(connect (sFd, (struct sockaddr *) &serverSockAddr, sizeof(serverSockAddr)) == ERROR)
       {
       printf("failed to connect \n");
       perror ("connect");
       close (sFd);
       return (ERROR);
       }
/*    printf("Finished trying to connect \n"); */

/* build request, prompting user for message */

    myRequest.reply = 1;

/* send request to server */

    if(write(sFd, (char *)&myRequest, sizeof(myRequest)) == ERROR)
      {
      perror ("write");
      close (sFd);
      return (ERROR);
      }
    
    nRead = 0;
    while (nRead < sizeof(bobreply)) {
      nRead1  =read (sFd, ((char *)&bobreply)+nRead, sizeof(bobreply)-nRead);
      if(nRead1 < 0) {
         printf("Error from reading from scaler server\n");
         exit(0);
      }
      nRead += nRead1;
    }
    close (sFd);

    if (debug) printf("read nRead %d   %d\n",nRead,sizeof(bobreply));

    if(clearall) {
          printf("Scalers cleared \n");
          exit(1);
    }

/*    printf ("MESSAGE FROM SERVER :  \n %s \n",bobreply.message); */


/* byte swapping */
       for (k=0 ; k < 32*MAXBLOCKS; k++) bobreply.ibuf[k] = ntohl(bobreply.ibuf[k]);


       if( pick1 ) {  /* picking one channel to print */

         if (showrate) {  /* compute rate, need a 2nd reading */

	   /* store 1st reading */

            for (k=0 ; k < 32*MAXBLOCKS; k++) sdata1[k] = bobreply.ibuf[k];

   /* Read 2nd time to get rate, after a sleep interval */

            usleep(NMUSEC);

            if ((sFd = socket (PF_INET, SOCK_STREAM, 0)) == ERROR ) {
                  perror ("socket");
                  return (ERROR);
            }

            if(connect (sFd, (struct sockaddr *) &serverSockAddr, sizeof(serverSockAddr)) == ERROR) {
               printf("failed to connect \n");
               perror ("connect");
               close (sFd);
               return (ERROR);
             }
             myRequest.reply = 1;
             if(write(sFd, (char *)&myRequest, sizeof(myRequest)) == ERROR){
                perror ("write");
                close (sFd);
                return (ERROR);
            }
            nRead = 0;
            while (nRead < sizeof(bobreply)) {
               nRead1  =read (sFd, ((char *)&bobreply)+nRead, sizeof(bobreply)-nRead);
               if(nRead1 < 0) {
                  printf("Error, 2nd reading from scaler server\n");
                  exit(0);
               }
               nRead += nRead1;
	    }
            close (sFd);
            /* byte swapping */
            for (k=0 ; k < 32*MAXBLOCKS; k++) 
              bobreply.ibuf[k] = ntohl(bobreply.ibuf[k]);
	    /* 2nd reading */
            for (k=0 ; k < 32*MAXBLOCKS; k++) {
              sdata2[k] = bobreply.ibuf[k];
	          if (debug && k < 16) printf("chk %d  %d   %d \n",k,sdata1[k],sdata2[k]);
	        }
               
            scalrate = 0;
	        sdiff = sdata2[wchan] - sdata1[wchan];
            clkdiff = sdata2[CLKCHAN] - sdata1[CLKCHAN];
            if (clkdiff != 0) {
	          scalrate = 1000. * sdiff * clockrate / clkdiff; // Hz
            } else { /* no clock, make a crude estimate */
	          scalrate = 1e6 * sdiff / NMUSEC; /* NMUSEC is in mircrosec */
	        }

            if (debug) printf("chan %d  data %d %d %d clock %d  rate %f \n",wchan,sdata2[wchan],sdata1[wchan],sdiff,clkdiff,scalrate);

            irate = (int)scalrate;

            printf("%d",irate);  /* integer -- throws out 0.xxx */


	      } else {  /*  print counts only */

  	        printf("%d", bobreply.ibuf[wchan]);
          
	      }

          exit(1);  /* job done */

        }  /* if (pick1) */


        if(lprint==1) {
          printf ("\n\n\n  ========    Scalers   ===============\n\n");
          for (isca=0; isca<NUMSCALER; isca++) {
	        printf("scaler %d   ----> \n",isca+1);
  	        k=4;  /* 32 chan per scaler */
            if (bobreply.message[isca]=='0') k=2; /* 16 chan */
            sca = k*isca;
            if (debug) printf("msg %c  k %d  %d \n",bobreply.message[isca],k,sca);
            for (i=0;i<k;i++) {
              printf ("%3d :  %8d %8d %8d %8d %8d %8d %8d %8d \n",
              8*(sca+i)+1,bobreply.ibuf[(sca+i)*8],
              bobreply.ibuf[(sca+i)*8+1],
              bobreply.ibuf[(sca+i)*8+2], 
              bobreply.ibuf[(sca+i)*8+3],
              bobreply.ibuf[(sca+i)*8+4],
              bobreply.ibuf[(sca+i)*8+5],
              bobreply.ibuf[(sca+i)*8+6],
              bobreply.ibuf[(sca+i)*8+7]);
	        }
	      }
        }

        if(lprint==2) {
          k=0;
          for (isca=0; isca<NUMSCALER; isca++) {
            nchan=16;
            if(scalertype[isca]==7200) nchan=32;  
            if(scalertype[isca]==3800) nchan=32;  
            if(scalertype[isca]==3801) nchan=32;  
            if(scalertype[isca]>=380000) nchan=32;  
            shead = vmeheader[isca] + nchan;
            printf("%x\n",shead);
            for (i=0;i<nchan;i++) {
              printf("%x\n",bobreply.ibuf[k++]);
	        }
	      }
        }

        if(lprint==3) {
          k=0;
	      for (isca=0; isca<NUMSCALER; isca++) {
            nchan=16;
            if(scalertype[isca]==7200) nchan=32;  
            if(scalertype[isca]==3800) nchan=32;  
            if(scalertype[isca]==3801) nchan=32;  
            if(scalertype[isca]>=380000) nchan=32;  
            shead = vmeheader[isca] + nchan;
            printf("%x\n",shead);
            for (i=0;i<nchan;i++) {
              printf("%d\n",bobreply.ibuf[k++]);
	        }
          }
        }



        ntot=0;
        k=0;
        for (isca=0; isca<NUMSCALER; isca++) {
          nchan=32;
          if (bobreply.message[isca]=='0') k=16;
          if (debug) printf("msg %c  nchan %d \n",bobreply.message[isca],nchan);
          buf.bobbuf[ntot++]=shead;
          for (i=0; i<nchan; i++) {
            buf.bobbuf[ntot++] = bobreply.ibuf[k++];
          }
        }


// On Compton setup we dont use ET for inserting data.
// Hence this block of code is "ifdef"d out.
#ifdef HAVE_ET

      tag = 140;
      ev_len = 4 + ntot;
      buf.header[0] = ev_len-1;         /* event length */
      buf.header[1] = (tag<<16) + 0x10cc;
      buf.header[2] = ev_len-3;
      buf.header[3] = 1<<8;    /* data type of integer.  no tag for now */


/* Insert data into Data stream ? */

      if (dainsert) {
        if(debug) printf("\nAttempt to insert into data stream\n");

/* Using ET system (onla) */
        stat = etInsertEvent(buf.header,"/tmp/et_sys_onla");

        if(stat==0) {
         if(debug) printf("scaler_insert: etInsertEvent ok \n");
        } else {
         printf("scaler_insert error due to etInsertEvent status=%d\n",stat);
        }

      }

#endif

}
