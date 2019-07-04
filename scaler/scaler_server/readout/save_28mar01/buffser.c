/* use vxWorks header files */
#include <vxWorks.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <in.h>
#include <inetLib.h>
#include "/home/adev/scaler/SIS3801_right/SIS.h"
#include "/home/adev/scaler/SIS3801_right/SIS3801.h"
#include "buffser.h"

/* The following data are filled by SIS3801.c */
extern struct SISbuffer SIS1buffer[BUFF_SIZE];

extern unsigned  long num_read[NUMSCALERS];
extern short start_flag;
extern short new_run_flag;
extern short end_run_flag;
extern short between_run_flag;
extern unsigned long client_read;


/***********************************************************
*
*  vxWorks server for TCP/IP readout of scalers 
*
*  Modifications
*
*    27 Feb 98   Throw out the taskSpawn of Work task.
*                Put the scaler read directly into buffServer.
*     9 Mar 99   For the new SIS3801 scalers, get data from
*                a common shared memory which is filled by
*                another task.  Ability to clear scalers is
*                revived, at least for now.
*     3 Jun 99   Allow for both 3800 and 3801 scalers in code.
*
*/



VOID serverWorkTask (int sFd, struct sockaddr_in clntSockAddr);


STATUS BufferServer (void)

   {
   struct sockaddr_in myAddr ;       /* server's socket address  */
   struct sockaddr_in clientAddr;    /* client's socket address  */
   int   sFd;                        /* socket file descriptor   */
   int   newFd;                      /* socket descriptor from accept */
   int   sockAddrSize = sizeof (struct sockaddr_in);
   struct request clientRequest;     /* request msg from client */
   struct request buffreply;
   int i,j,k;
   int    nRead;
   char   inetAddr[INET_ADDR_LEN];   /* client's inet addr */
   static char  replyMsg[] = "Server got your message";
   static int debug = 0;  /* to debug (1) or not (0) */
   int skip_one, this_read;
   unsigned long pointer_read;
   unsigned long pointer_write, loss_read;
   int window_size_set = 128 *1024 ;
   int window_size_get ;
   int bytesend;
   int window_size_get_len;
   unsigned long temp_long;
   unsigned long buff_write;
   
   
   
/* set up local address */

   bzero (&myAddr, sockAddrSize);
   myAddr.sin_family = PF_INET;
   myAddr.sin_port = htons (SERVER_PORT_NUM);
   myAddr.sin_addr.s_addr = htonl (INADDR_ANY);

/* create socket */

   if ((sFd = socket(PF_INET, SOCK_STREAM, 0)) == ERROR)
     {
      perror ("socket");
      return (ERROR);
     }

/* Sets the TCP window size to 128 kbytes */ 
/*
 *    setsockopt(sFd, SOL_SOCKET, SO_SNDBUF,
 *           &window_size_set, sizeof(window_size_set));
 * 
 *    setsockopt(sFd, SOL_SOCKET, SO_RCVBUF,
 *           &window_size_set, sizeof(window_size_set));
 */

/*
 *    getsockopt(sFd, SOL_SOCKET, SO_SNDBUF,
 *           (char*)&window_size_get, &window_size_get_len);
 *    
 *    printf("window_size_get = %d\n", window_size_get);
 */

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



    pointer_read = 0;
    client_read = 0;

/* accept new connect requests and spawn tasks to deal with them */

FOREVER 
  {
    if(debug) printf("about to accept\n");
        
    if ((newFd = accept (sFd, &clientAddr, &sockAddrSize)) == ERROR)
       {
        perror ("accept");
        close (sFd);
        return (ERROR);
       }



/*  read client message, display message, and if requested, reply */


/*
 *     for (k=0; k<=sizeof(replyMsg); k++) {
 *     buffreply.message[k] = replyMsg[k];
 *     }
 */

    if(debug) printf("about to read\n");

    while ((nRead = fioRead (newFd, (char *) &clientRequest,
                    sizeof(clientRequest))) > 0)
      {
      /* convert internet address to dot notation */
      inet_ntoa_b (clientAddr.sin_addr, inetAddr);

     if(debug)
        printf ("Request FROM: Internet Address %s, port %d \n",
            inetAddr, ntohs (clientAddr.sin_port));    
/*
 *         printf ("MESSAGE FROM: Internet Address %s, port %d \n %s \n",
 *             inetAddr, ntohs (clientAddr.sin_port), 
 *             clientRequest.message);    
 */
    bzero((char *)&buffreply,    sizeof(buffreply));
   
    


      
	buff_write = num_read[0];
	pointer_write = (buff_write-1) % BUFF_SIZE;
      if(client_read>0){
	   pointer_read  = (client_read-1) % BUFF_SIZE;
      }else{
	   pointer_read  = 0;
	}   
	this_read = 0;
	
	if((buff_write-client_read)<=BUFF_SIZE){ /* The buffer did not rollover */
	   
	   if((buff_write-client_read)<=NET_BUFF_SIZE){ /* Less data than the net buffer size */

		    if(pointer_write>pointer_read){


            	  k=0;
			  for(i=0;i<(pointer_write-pointer_read);i++){

			     buffreply.ibuf[k++]=SIS1buffer[i+pointer_read+1].nbread;
			     buffreply.ibuf[k++]=SIS1buffer[i+pointer_read+1].h_bit1;
			     buffreply.ibuf[k++]=SIS1buffer[i+pointer_read+1].error_cond;

			     for(j=0;j<32;j++){
		      	         buffreply.ibuf[k++]=
			            	 SIS1buffer[i+pointer_read+1].data[j]&DATA_MASK;
	      	             }
			     client_read++;
			     this_read++;
			  }

		    
			  if(debug) printf("\nclient_read = %d, buff_write = %d \n",client_read,buff_write);
	      	  if(debug) printf("pointer_read = %d, pointer_write = %d \n",pointer_read,pointer_write);
		        if(debug) printf("less than net_buff_size and pointer_write > pointer _read \n");
		     
		     }else{


            	  k=0;
			  for(i=0;i<(BUFF_SIZE-1-pointer_read);i++){

			     buffreply.ibuf[k++]=SIS1buffer[i+pointer_read+1].nbread;
			     buffreply.ibuf[k++]=SIS1buffer[i+pointer_read+1].h_bit1;
			     buffreply.ibuf[k++]=SIS1buffer[i+pointer_read+1].error_cond;

			     for(j=0;j<32;j++){
		      	             buffreply.ibuf[k++]=
			            	 SIS1buffer[i+pointer_read+1].data[j]&0xffffff;
	      	              }
			     client_read++;
			     this_read++;
			  }
	      	         
			  for(i=0;i<(pointer_write+1);i++){

			     buffreply.ibuf[k++]=SIS1buffer[i].nbread;
			     buffreply.ibuf[k++]=SIS1buffer[i].h_bit1;
			     buffreply.ibuf[k++]=SIS1buffer[i].error_cond;

			     for(j=0;j<32;j++){
			          buffreply.ibuf[k++]=
			            	 SIS1buffer[i].data[j]&0xffffff;
			     }
			     client_read++;
			     this_read++;
			  }
			 
			 
			  if(debug) printf("\nclient_read = %d, buff_write = %d \n",client_read,buff_write);
	      	  if(debug) printf("pointer_read = %d, pointer_write = %d \n",pointer_read,pointer_write);
		        if(debug) printf("less than net_buff_size and pointer_write < pointer _read \n");
		     
		     
		     }
	   
	    }else {  /* more data than the net buffer size */
		  
		    if((BUFF_SIZE-1-pointer_read)>=NET_BUFF_SIZE){


			  k=0;
			  for(i=0;i<NET_BUFF_SIZE;i++){

			     buffreply.ibuf[k++]=SIS1buffer[i+pointer_read+1].nbread;
			     buffreply.ibuf[k++]=SIS1buffer[i+pointer_read+1].h_bit1;
			     buffreply.ibuf[k++]=SIS1buffer[i+pointer_read+1].error_cond;

			     for(j=0;j<32;j++){
			           buffreply.ibuf[k++]=
			            	 SIS1buffer[i+pointer_read+1].data[j]&DATA_MASK;
			     }
			     client_read++;
			     this_read++;
			  }

			  if(debug) printf("\nclient_read = %d, buff_write = %d \n",client_read,buff_write);
	      	  if(debug) printf("pointer_read = %d, pointer_write = %d \n",pointer_read,pointer_write);
		        if(debug) printf("more than net_buff_size and BUFF_SIZE-1-pointer _read>NET_BUFF_SIZE \n");


		    }else{


			  k=0;
			  for(i=0;i<(BUFF_SIZE-1-pointer_read);i++){

			     buffreply.ibuf[k++]=SIS1buffer[i+pointer_read+1].nbread;
			     buffreply.ibuf[k++]=SIS1buffer[i+pointer_read+1].h_bit1;
			     buffreply.ibuf[k++]=SIS1buffer[i+pointer_read+1].error_cond;

			     for(j=0;j<32;j++){
			           buffreply.ibuf[k++]=
			            	 SIS1buffer[i+pointer_read+1].data[j]&0xffffff;
			     }
			     client_read++;
			     this_read++;
			  }
			  
			  for(i=0;i<(NET_BUFF_SIZE-(BUFF_SIZE-1-pointer_read));i++){

			     buffreply.ibuf[k++]=SIS1buffer[i].nbread;
			     buffreply.ibuf[k++]=SIS1buffer[i].h_bit1;
			     buffreply.ibuf[k++]=SIS1buffer[i].error_cond;

			     for(j=0;j<32;j++){
			          buffreply.ibuf[k++] = SIS1buffer[i].data[j]&0xffffff;
			     }
			     client_read++;
			     this_read++;
			  }
		     

			  if(debug) printf("\nclient_read = %d, buff_write = %d \n",client_read,buff_write);
	      	  if(debug) printf("pointer_read = %d, pointer_write = %d \n",pointer_read,pointer_write);
		        if(debug) printf("more than net_buff_size and BUFF_SIZE-1-pointer _read<NET_BUFF_SIZE \n");
		     
		     
		     }
	    }  
		  	      
	 

	} else {   /* THE BUFFER ROLL OVER */
	    k=0;
	    if(debug) printf("\n***WARNING*** The buffer has rolled over between to client read !!!\n");
	    
	    loss_read = buff_write-client_read-BUFF_SIZE-1;

    /* the last read was the first window in the pair */
    
/*
 * 	    if((client_read-1)%2==start_flag){ 
 * 	       client_read--;
 * 	       pointer_read  = (client_read-1) % BUFF_SIZE;
 *              if(debug) printf("\t client_read -- = %d , pointer_read =%d\n",
 * 		    client_read,pointer_read);
 * 	       loss_read++;
 * 	    }
 */
	    
	    skip_one = 0;
	    if((pointer_write+1)%2!=start_flag) {
               if(debug) printf("I will skip one event\n");
	         skip_one = 1;
		   loss_read++;
	    }
	    
          if(debug) printf("we loss %d events\n",loss_read);
	    
	    client_read += loss_read;	   
	    
	       
	      if((BUFF_SIZE-1-(pointer_write+skip_one))>=NET_BUFF_SIZE) { 
		  
		  k=0;
	          
		  for(i=0;i<NET_BUFF_SIZE;i++){
		      buffreply.ibuf[k++]=SIS1buffer[i+pointer_write+1+skip_one].nbread;
		      buffreply.ibuf[k++]=SIS1buffer[i+pointer_write+1+skip_one].h_bit1;
		      buffreply.ibuf[k++]=SIS1buffer[i+pointer_write+1+skip_one].error_cond;

		      for(j=0;j<32;j++){
			 buffreply.ibuf[k++]=
			      	SIS1buffer[i+pointer_write+1+skip_one].data[j]&0xffffff;
	              }
		      client_read++;
		      this_read++;
		  }
		  skip_one = 0;
	      
	      } else {
		 
		  k=0;
		  for(i=0;i<(BUFF_SIZE-1-(pointer_write+skip_one));i++){
		      buffreply.ibuf[k++]=SIS1buffer[i+pointer_write+1+skip_one].nbread;
		      buffreply.ibuf[k++]=SIS1buffer[i+pointer_write+1+skip_one].h_bit1;
		      buffreply.ibuf[k++]=SIS1buffer[i+pointer_write+1+skip_one].error_cond;

		      for(j=0;j<32;j++){
			 buffreply.ibuf[k++]=
			      	SIS1buffer[i+pointer_write+1+skip_one].data[j]&0xffffff;
	              }
		      client_read++;
		      this_read++;
		  }
		  
		  for(i=0;i<(NET_BUFF_SIZE-(BUFF_SIZE-1-(pointer_write+skip_one)));i++){
		      buffreply.ibuf[k++]=SIS1buffer[i].nbread;
		      buffreply.ibuf[k++]=SIS1buffer[i].h_bit1;
		      buffreply.ibuf[k++]=SIS1buffer[i].error_cond;

		      for(j=0;j<32;j++){
			 buffreply.ibuf[k++]=
			      	SIS1buffer[i].data[j]&0xffffff;
	              }
		      client_read++;
		      this_read++;
		  }
		  
		  skip_one = 0;
		  		
	      }
	
	     if(debug) printf("\nclient_read = %d, buff_write = %d \n",client_read,buff_write);
	     if(debug) printf("pointer_read = %d, pointer_write = %d \n",pointer_read,pointer_write);
	}  /* END IF ROLL OVER */
	   

	               
	buffreply.reply = this_read;
	if(debug) printf("I have been read %d times \n",buffreply.reply);
	
	if (client_read%1000==0) 
	   printf("client_read = %d, num_read = %d \n",client_read,buff_write);
           
    buffreply.status = 0; /* reset the status flag 
                             0 = in a run
				     1 = new run started
				     2 = run stopped 
				     3 = between run (gates ON but CODA do not run) */
    if(new_run_flag){
        client_read=0;
	  buffreply.status = 1;
	  new_run_flag=0;
        printf("See new run flag \n");
    }
       
    if(end_run_flag){
	  buffreply.status = 2;
        printf("See end run flag \n");
    }
    
    if(between_run_flag){
	  buffreply.status = 3;
        printf("See between run flag \n");
    }
	
      if (clientRequest.reply){
          if ((bytesend=write (newFd, (char *) &buffreply, sizeof (buffreply))) == ERROR)
               perror ("write");
         if(debug)  printf("I send %d bytes \n",bytesend);
	}
   }  /* End While read */

  /*  if (nRead == ERROR) perror ("read");  */
 
   close (newFd);

  }  /* end Forever */
}

