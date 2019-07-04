/* Client code to read VME scalers */
/* Server runs on the VME cpu */

#ifdef HPVERS
#include <da.h>
#include <obj.h>
#include <rc.h>
#endif

#include <sys/ioctl.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include "/adaqfs/halla/adaq/scaler/SIS3801_left/SIS.h"
#include "buffser.h"

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
       FILE *fp;
       FILE *runfile;
	 
	 unsigned long temp_buf[35*BUFF_SIZE];
	 unsigned long temp_long;
	 short temp_short;
	 
       int sFd;                            /*  socket file descriptor */
       int i,k;
       long stat;
       static int lprint=2;
       struct request myRequest;             /*  request to send to server */
       struct request buffreply;
       struct sockaddr_in serverSockAddr;    /*  socket addr of server */
       static char  defMsg[] = "buff wants some data";
       static int debug=0;
       char cb;
	 char outfilename[100];
	 char runfilename[100];
	 char directory[100];
	 char indirectory[100];
       int  runnumber;
	 int  window_size_set = 128 *1024;
       int window_size_get_len;
       int window_size_get ;
       int sizeget;
	 int num_read;
	 /* Process arguments */

    while (--argc > 0) {
        ++argv;
        if ((*argv)[0]==NULL) break;
        if ((*argv)[0]!= '-') goto usage;
        cb = (*argv)[1];
        switch (cb) {
           case 'b':                  /* Print to binary file */
              ++argv;
              lprint=1;
              break;
           case 's':                  /* Print to screen */
              ++argv;
              lprint=2;
              break;
           case 't':                  /* print to text file */
              ++argv;
              lprint=3;
              break;
	     case 'h':                  /* help */
  	        goto usage;
           default:
              printf("ERROR: Illegal option %c \n\n",cb);
usage:
              printf("USAGE: scread [-b -s -d -h]\n");
              printf("\t -b is to print to a binary file \n");
              printf("\t -d is to print to a text file \n");
              printf("\t -s is to print to the screen\n");
              printf("\t -h is to get help (this printout)\n");
              exit(1);
	 }
    }



      

/*    bzero (&buffreply, sizeof(buffreply));*/

/*
 *        for (k=0; k<=sizeof(defMsg); k++) {
 *            myRequest.message[k] = defMsg[k];
 *        }
 */


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

/* Sets the TCP window size to 128 kbytes */
    

/*
 *     if(setsockopt(sFd, SOL_SOCKET, SO_SNDBUF,
 *           &window_size_set, sizeof(window_size_set))==ERROR)
 * 	   perror("setsockopt");
 * 
 *     if(setsockopt(sFd, SOL_SOCKET, SO_RCVBUF,
 *           &window_size_set, sizeof(window_size_set))==ERROR)
 * 	   perror("setsockopt");
 * 
 *      getsockopt(sFd, SOL_SOCKET, SO_SNDBUF,
 *           (char*)&window_size_get, &window_size_get_len);
 * 
 *      printf("window_size_get = %d\n", window_size_get);
 */

/* connect to server */
    
    if(debug) printf("About to connect \n"); 

#ifdef HPVERS
    if(connect (sFd, &serverSockAddr, sizeof(serverSockAddr)) == ERROR)
#else
    if(connect (sFd, (struct sockaddr *) &serverSockAddr, sizeof(serverSockAddr)) == ERROR)
#endif
       {
       printf("failed to connect \n");
       perror ("connect");
       close (sFd);
       return (ERROR);
       }
    if(debug) printf("Finished trying to connect \n"); 

/* build request, prompting user for message */

    myRequest.reply = 1;

/* send request to server */

    if(write(sFd, (char *)&myRequest, sizeof(myRequest)) == ERROR){
       perror ("write");
       close (sFd);
       return (ERROR);
    }
    
    
    read (sFd, (char *)&temp_short, sizeof(temp_short));
	  buffreply.reply = temp_short;
    
    read (sFd, (char *)&temp_long, sizeof(temp_long));
	  buffreply.status = temp_long;
    
    printf("status = %d %d\n",buffreply.reply,temp_long);
    
    
    for(i=0;i<NET_BUFF_SIZE;i++){
	  for(k=0;k<35;k++){
		if ((num_read=read (sFd, (char *)&temp_long, sizeof(temp_long))) < 0){
		   perror ("read");
      	   close (sFd);
      	   return (ERROR);
		}
		buffreply.ibuf[35*i+k] = temp_long;
	  }
    }    
   
/* Directory where the file which content the run number could be find */   

   strcpy ( indirectory, "/adaqs2/adaqfs/halla/adaq/datafile");
  

/* Directory where we output data file will be write */   
   
   strcpy ( directory, 
       "/adaqfs/halla/adaq/scaler/hadron/analysis/data");


/* File which contents the CODA's run number */   

   sprintf ( runfilename, "%s/rcRunNumber", indirectory); 
   
   if ((runfile = fopen(runfilename, "r")) != NULL) {
	if(debug) printf("Open file %s \n",runfilename);
	fscanf (runfile , "%d", &runnumber );
	fclose (runfile);
	if(debug) fprintf ( stdout, "SIConsumer: Found run number %d\n", runnumber );
      
   } else{
	fprintf(stderr, "Can't open the file %s\n",runfilename);
	exit(1);
   }
   
   
/* Choose the outfile name, which is scal_XXXX.out where XXXX is the run number 
   if CODA is currently running
   or will be scaler.out if CODA is not running (between two runs) */ 
      

   if(buffreply.status!=3){ /* CODA is running */
	
	sprintf ( outfilename, "%s/scal_%d.out", 
		directory, runnumber); 
	
	if ((fp = fopen(outfilename, "a+")) != NULL) {
	   if(debug) fprintf(stdout, "Open the output file %s\n",outfilename);
	}else {
	   fprintf(stderr, "Can't open the output file %s\n",outfilename);
	   exit(1);
	}
   
   } else {                /* CODA is NOT running */
	
	sprintf ( outfilename, "%s/scaler.out", 
		directory); 

	if ((fp = fopen(outfilename, "a+")) != NULL) {
	   if(debug) fprintf(stdout, "Open the output file %s\n",outfilename);
	}else {
	   fprintf(stderr, "Can't open the output file %s\n",outfilename);
	   exit(1);
	}
   }  


    if(debug) printf("buffreply.reply = %d \n",buffreply.reply);


/* Print to a binary file */

       if(lprint==1) {
           for (k=0; k<buffreply.reply; k++) {
  		  if(buffreply.ibuf[35*k]!=0){
		     for(i=0;i<35;i++){
	      	  temp_buf[i] = buffreply.ibuf[35*k+i];
		     }
		     fwrite(temp_buf,sizeof(long),35,fp);
	        }
	     }
	}
      


/* Print to the screen */
	
	if(lprint==2) {
           for (k=0; k<buffreply.reply; k++) {
		  printf(" %8d    %d    %d \n",
		     buffreply.ibuf[35*k],buffreply.ibuf[35*k+1],buffreply.ibuf[35*k+2]);
		  for(i=0;i<4;i++){
		     printf("%8d %8d %8d %8d %8d %8d %8d %8d \n",
		        buffreply.ibuf[35*k+8*i+3],
		        buffreply.ibuf[35*k+8*i+4],
		        buffreply.ibuf[35*k+8*i+5],
		        buffreply.ibuf[35*k+8*i+6],
		        buffreply.ibuf[35*k+8*i+7],
		        buffreply.ibuf[35*k+8*i+8],
		        buffreply.ibuf[35*k+8*i+9],
		        buffreply.ibuf[35*k+8*i+10]);
		   }	
	      }	     
       }


/* Print to a text file */
       
	 if(lprint==3) {
           for (k=0; k<buffreply.reply; k++) {
		  fprintf(fp," %8d    %d    %d \n",
		     buffreply.ibuf[35*k],buffreply.ibuf[35*k+1],buffreply.ibuf[35*k+2]);
		  for(i=0;i<4;i++){
		     fprintf(fp,"%8d %8d %8d %8d %8d %8d %8d %8d \n",
		        buffreply.ibuf[35*k+8*i+3],
		        buffreply.ibuf[35*k+8*i+4],
		        buffreply.ibuf[35*k+8*i+5],
		        buffreply.ibuf[35*k+8*i+6],
		        buffreply.ibuf[35*k+8*i+7],
		        buffreply.ibuf[35*k+8*i+8],
		        buffreply.ibuf[35*k+8*i+9],
		        buffreply.ibuf[35*k+8*i+10]);
		   }	
	      }	     
       }

if(fclose(fp)!=0) printf("ERROR IN CLOSING OUTPUT FILE !!!\n");

 }
