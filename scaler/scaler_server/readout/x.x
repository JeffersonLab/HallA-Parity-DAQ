1,2c1,2
< /* Client code to read VME scalers 
<    R. Michaels, Sept 2009   */
---
> /* Client code to read VME scalers */
> /* Server runs on the VME cpu */
4c4,22
< /* Note, the server runs on the VME cpu */
---
> /* Modified March 2000 by D. Meekins */
> /* if print_to_file is true then the routine will compile and        */
> /* allow an additional option -p to print to a data file             */
> /* the data file will contain the raw triggers for types 1 through 5 */
> /* and the downstream BCMX3 signal                                   */
> 
> /* Mod, Aug 2002 R. Michaels
>    Option '-r' gets the ring buffer, for purpose of debugging
>    scaler readout in G0 mode.  Data goes to "ring.out".
>    This is dangerous because the server clears the ring buffer.  
>    Therefore it is not an advertised feature, but only for debugging 
>    But this option is also TURNED OFF when TEST_RING is undefined.
>    Reason: To be compatible with xscaler (SunOS) which unfortuantely
>    I cannot compile at the moment !
> */
> 
>    
> 
> #define print_to_file 1
6,9c24,33
< /* time interval for rate, if computing rate */
< #define NMUSEC 2000000   /* usec using usleep */
< /* Location in buffer of clock */
< #define CLKCHAN 3
---
> #ifdef print_to_file
> #define OUTPUT_FILE "/adaqfs/halla/adaq/scaler/hadron/scaler_subset.dat"
> #define BCMX3 86
> #define T1 80
> #define T2 81
> #define T3 82
> #define T4 83
> #define T5 84
> #define TIME 87
> #endif
10a35,39
> #ifdef HPVERS
> #include "da.h"
> #include "obj.h"
> #include "rc.h"
> #endif
20d48
< #include "scaler1.h"
36a65
>        FILE *ringout;
42a72,73
>        int checkend;
>        static int getring=0;
43a75
>        static int fprint=0;
53,56d84
<        int pick1,wchan,sdiff,clkdiff,showrate;
<        double scalrate;
<        int irate;
<        double clockrate=5500.;  /* kHz */
61c89
<        struct EVBUF {  // for possible insertion to datastream
---
>        struct EVBUF {
63,67c91,92
<            long bobbuf[32*MAXBLOCKS];  /* assume 32 chan */
<        } buf;
<        long sdata1[32*MAXBLOCKS];   
<        long sdata2[32*MAXBLOCKS];
< 
---
>            long bobbuf[32*MAXSCALER];  /* should assume STR7200 (32 chan) */
> 	 } buf;
68a94,95
> #include "scaler1.h"
> #include "scaler2.h"
74,80c101,102
<     pick1=0;
<     wchan=0;   /* pick out location in buffer of your chan */
<     showrate=0;
< 
<     for (i=0; i<argc; i++) {
<       if(strstr(argv[i],"-")==NULL) wchan=atoi(argv[i]);         
<     }
---
>     checkend=0;
>     getring=0;
84a107
>         if ((*argv)[0]!= '-') goto usage;
89a113,119
>    	   case 'e':                  /* check for end-run (for last_scaler)*/
>               checkend=1;
>               break;
> /* Getting the ring buffer is dangerous, see top of code */
>    	   case 'r':                  /* get the ring buffer */
>               getring=1;
>               break;
100,106c130,135
<     	   case 'p' :                 /* Pick (p) a channel */
<  	      pick1=1;             
<               break;
<     	   case 'r' :                 /* To show rate instead of counts */
<  	      showrate=1;             
<               break;
< 	   case 'h' :                 /* print help */
---
> #ifdef print_to_file
>   	   case 'p':                  /* Print to data file only selected scalers */
> 	      fprint=1;	  
> 	      break;
> #endif
> 	   case 'h':                  /* help */
109c138
<   	      break;
---
>               printf("ERROR: Illegal option %c \n\n",cb);
111,116c140,151
<               printf("USAGE: scread [-c -d -x -h -i -p N -r ]\n");
<               printf("With no option, scread prints raw data to screen\n");
<               printf("-c is to clear scalers\n");
<               printf("During a run, the server should probably be\n");
<               printf("set up to disallow scread to clear.\n");
<               printf("Other options :\n");
---
>               printf("USAGE: scread [-c -e -d -x -h -i ");
> #ifdef print_to_file
> 	      printf("-p ");
> #endif
> 	      printf("]\n");
>               printf("-c is to clear scalers (if I let you)\n");
>               printf("During experiment, the server probably be set up to\n");
>               printf("disallow scread to clear -> instead by CODA RunStart\n");
>               printf("-e is to check for end of run flag\n");
>               printf("if 1 is printed, its ended, if not its a failure\n");
>               printf("With no option, scread prints oldstyle to screen\n");
>               printf("These printout options (normally for CODA 2.0):\n");
118a154,156
> #ifdef print_to_file
> 	      printf("-p print to data file with option -d or -x\n");
> #endif
121,124d158
<               printf("For monitoring a specific channel\n");
<               printf("-p to print channel counts (default is chan #0)\n");
<               printf("-p N to print counts on channel N = 0,1,2 ...\n");
<               printf("-p N -r to print rate channel N = 0,1,2 ...\n");
129d162
<     if (debug) printf("State: %d %d %d \n",pick1,wchan,showrate);
131,133c164,175
<     if (wchan < 0 | wchan >= 32*MAXBLOCKS) {
<       printf("ERROR: index outside of buffer range \n");
<       exit(0);
---
> /* Check consistency between NUMBLOCKS and NUMSCALER */
> 
>    numchan=0;
>    for (isca=0; isca<NUMSCALER; isca++) {
>      /* 3801 scalers have 2 helicities and 32 channels */
>      if (scalertype[isca]==3801) {
>          numchan += 32;
>      } else if( (scalertype[isca]==7200) || (scalertype[isca]==3800)) {
>         numchan += 32;
>      } else {
>         numchan += 16;
>      }
134a177,187
>     if(numchan!=16*NUMBLOCKS) {
>        printf("\n\n Error -- cannot run -- \n");
>        printf(" The following is not satisfied: \n");
>        printf("NUMSCALER = number of scaler units \n");
>        printf("NUMBLOCKS = number of 16-channel blocks =\n"); 
>        printf("num_1151_scalers + num_v560_scalers \n");
>        printf("      + 2*num_str7200_scalers \n");
>        printf("      + 2*num_sis3800_scalers \n");
>        printf("      + 2*num_sis3801_scalers of each helicity \n");
>        exit(1);
>      }
135a189
> /*    bzero (&bobreply, sizeof(bobreply));*/
139c193,196
<       for (k=0; k<=sizeof(defMsg); k++) {
---
>       myRequest.checkend = htonl(checkend);
> 
> 
>        for (k=0; k<=sizeof(defMsg); k++) {
141c198
<       }
---
>        }
156a214
> /*    bzero (&serverSockAddr, sizeof(serverSockAddr) );*/
164a223,225
> #ifdef HPVERS
>     if(connect (sFd, &serverSockAddr, sizeof(serverSockAddr)) == ERROR)
> #else
165a227
> #endif
175a238,241
> /*    printf("Message to send :  \n");
> *    mlen = read (0, myRequest.message, sizeof(myRequest.message));
> *    myRequest.message[mlen-1] = '\0';
> */
177a244,249
> #ifdef TEST_RING
>     myRequest.getring = htonl(getring);
> #else
>     if (getring) printf("Warning, cannot test ring buffer (this vers)\n");
> #endif
> 
186,196d257
<     
<     nRead = 0;
<     while (nRead < sizeof(bobreply)) {
<       nRead1  =read (sFd, ((char *)&bobreply)+nRead, sizeof(bobreply)-nRead);
<       if(nRead1 < 0) {
<          printf("Error from reading from scaler server\n");
<          exit(0);
<       }
<       nRead += nRead1;
<     }
<     close (sFd);
197a259,263
>     nRead  =read (sFd, (char *)&bobreply, sizeof(bobreply));
>     if(nRead < 0) {
>        printf("Error from reading from scaler server\n");
>        exit(0);
>     }
199a266,276
>     while (nRead < sizeof(bobreply)) {
>        nRead1 = read (sFd, ((char *) &bobreply)+nRead,
>                     sizeof(bobreply)-nRead);
>        if (debug) printf("reading some more \n");
>        if(nRead1 < 0) {
>           printf("Error from reading from scaler server\n");
>           exit(0);
>        }
>        nRead += nRead1;
>     }
> 
203a281,284
>     if(checkend) {
>           printf("%d",ntohl(bobreply.checkend));
>           exit(1);
>     }
207d287
< 
209,285c289,293
<        for (k=0 ; k < 32*MAXBLOCKS; k++) bobreply.ibuf[k] = ntohl(bobreply.ibuf[k]);
< 
< 
<        if( pick1 ) {  /* picking one channel to print */
< 
<          if (showrate) {  /* compute rate, need a 2nd reading */
< 
< 	   /* store 1st reading */
< 
<             for (k=0 ; k < 32*MAXBLOCKS; k++) sdata1[k] = bobreply.ibuf[k];
< 
<    /* Read 2nd time to get rate, after a sleep interval */
< 
<             usleep(NMUSEC);
< 
<             if ((sFd = socket (PF_INET, SOCK_STREAM, 0)) == ERROR ) {
<                   perror ("socket");
<                   return (ERROR);
<             }
< 
<             if(connect (sFd, (struct sockaddr *) &serverSockAddr, sizeof(serverSockAddr)) == ERROR) {
<                printf("failed to connect \n");
<                perror ("connect");
<                close (sFd);
<                return (ERROR);
<              }
<              myRequest.reply = 1;
<              if(write(sFd, (char *)&myRequest, sizeof(myRequest)) == ERROR){
<                 perror ("write");
<                 close (sFd);
<                 return (ERROR);
<             }
<             nRead = 0;
<             while (nRead < sizeof(bobreply)) {
<                nRead1  =read (sFd, ((char *)&bobreply)+nRead, sizeof(bobreply)-nRead);
<                if(nRead1 < 0) {
<                   printf("Error, 2nd reading from scaler server\n");
<                   exit(0);
<                }
<                nRead += nRead1;
< 	    }
<             close (sFd);
<             /* byte swapping */
<             for (k=0 ; k < 32*MAXBLOCKS; k++) 
<               bobreply.ibuf[k] = ntohl(bobreply.ibuf[k]);
< 	    /* 2nd reading */
<             for (k=0 ; k < 32*MAXBLOCKS; k++) {
<               sdata2[k] = bobreply.ibuf[k];
< 	      if (debug && k < 16) printf("chk %d  %d   %d \n",k,sdata1[k],sdata2[k]);
< 	    }
<                
<             scalrate = 0;
< 	    sdiff = sdata2[wchan] - sdata1[wchan];
<             clkdiff = sdata2[CLKCHAN] - sdata1[CLKCHAN];
<             if (clkdiff != 0) {
< 	      scalrate = 1000. * sdiff * clockrate / clkdiff; // Hz
<             } else { /* no clock, make a crude estimate */
< 	      scalrate = 1e6 * sdiff / NMUSEC; /* NMUSEC is in mircrosec */
< 	    }
< 
<             if (debug) printf("chan %d  data %d %d %d clock %d  rate %f \n",wchan,sdata2[wchan],sdata1[wchan],sdiff,clkdiff,scalrate);
< 
<             irate = (int)scalrate;
< 
<             printf("%d",irate);  /* integer -- throws out 0.xxx */
< 
< 
< 	 } else {  /*  print counts only */
< 
<   	    printf("%d", bobreply.ibuf[wchan]);
<           
< 	 }
< 
<          exit(1);  /* job done */
< 
<        }  /* if (pick1) */
< 
---
>        for (k=0 ; k < 16*MAXBLOCKS; k++) bobreply.ibuf[k] = ntohl(bobreply.ibuf[k]);
> #ifdef TEST_RING
>        for (k=0 ; k < NUM_IN_RING*MAXRING; k++) 
>            bobreply.ring[k] = ntohl(bobreply.ring[k]);
> #endif
289,295c297,300
<          for (isca=0; isca<NUMSCALER; isca++) {
< 	   printf("scaler %d   ----> \n",isca+1);
<   	    k=4;  /* 32 chan per scaler */
<             if (bobreply.message[isca]=='0') k=2; /* 16 chan */
<             sca = k*isca;
<             if (debug) printf("msg %c  k %d  %d \n",bobreply.message[isca],k,sca);
<             for (i=0;i<k;i++) {
---
>          sca=0;
>          for (k=0; k<NUMBLOCKS; k++) {
>             sca=2*k;
>             for (i=0;i<2;i++) {
340a346,365
> #ifdef TEST_RING
>        if (getring) {
>          if((ringout=fopen("ring.out","a"))==NULL) {
>            printf("could not open file ring.out\n");
>          } else {
>            n = bobreply.ring[0];
> 	   if (n > MAXRING-1) {  
>   	      printf("Error :  replied ring buffer exceeds NUM_IN_RING*(MAXRING-1)\n");
>               n = MAXRING-1;
> 	   }
>            fprintf(ringout,"%d\n",n);
> 	   if (debug) printf("-->  n =   %d\n",n); 
>            for (i = 0; i < n*NUM_IN_RING; i++) { 
>                 fprintf(ringout,"%d\n",bobreply.ring[i+1]);
> 	        if (debug) printf("%d  %d\n",i,bobreply.ring[i+1]); 
>             }
>             fclose(ringout);
>          }   
>        }
> #endif
341a367,392
> #ifdef print_to_file
>        if(fprint==1) {
> 	 if((outfile=fopen(OUTPUT_FILE,"w+"))==NULL) {
> 	   if(debug) printf("could not open file %s\n",OUTPUT_FILE);
> 	   goto Skip_fprint;
> 	 }
> 	 k=0;
> 	 for (isca=0; isca<NUMSCALER; isca++) {
> 	    nchan=16;
>             if(scalertype[isca]==7200) nchan=32;  
>             if(scalertype[isca]==3800) nchan=32;  
>             if(scalertype[isca]==3801) nchan=32;  
>             if(scalertype[isca]>=380000) nchan=32;  
>             shead = vmeheader[isca] + nchan;
> 	 }
> 	 fprintf(outfile,"Trigger 1 :%d\n",bobreply.ibuf[T1]);
> 	 fprintf(outfile,"Trigger 2 :%d\n",bobreply.ibuf[T2]);
> 	 fprintf(outfile,"Trigger 3 :%d\n",bobreply.ibuf[T3]);
> 	 fprintf(outfile,"Trigger 4 :%d\n",bobreply.ibuf[T4]);
> 	 fprintf(outfile,"Trigger 5 :%d\n",bobreply.ibuf[T5]);
> 	 fprintf(outfile,"BCM (x3)  :%d\n",bobreply.ibuf[BCMX3]);
> 	 fprintf(outfile,"Time      :%d\n",bobreply.ibuf[TIME]);
> 	 fclose(outfile);
>        }
> #endif
> Skip_fprint:
346,348c397,402
<            nchan=32;
<            if (bobreply.message[isca]=='0') k=16;
<            if (debug) printf("msg %c  nchan %d \n",bobreply.message[isca],nchan);
---
>            nchan=16;
>            if(scalertype[isca]==7200) nchan=32;
>            if(scalertype[isca]==3800) nchan=32;  
>            if(scalertype[isca]==3801) nchan=32;  
>            if(scalertype[isca]>=380000) nchan=32;  
>            shead = vmeheader[isca] + nchan;
355,359d408
< 
< // On Compton setup we dont use ET for inserting data.
< // Hence this block of code is "ifdef"d out.
< #ifdef HAVE_ET
< 
372a422,425
>         char *sess;
>         sess = getenv("SESSION");
> 	/*        printf("sess = %s \n",sess); */
> 
374c427,431
<         stat = etInsertEvent(buf.header,"/tmp/et_sys_onla");
---
>         char cfile[100];
>         sprintf(cfile,"/tmp/et_sys_%s",sess);
> 	/*        printf("cfile = %s \n",cfile); */
> 
>         stat = etInsertEvent(buf.header,cfile);
383,385c440
< 
< #endif
< 
---
>       close (sFd);
