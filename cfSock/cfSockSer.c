/* cfSockSer.c - configuration utility VXWORKS server */

/*
  DESCRIPTION
  This file contains the server-side of the VxWorks TCP example code.
*/

/* includes */

#include <inetLib.h>
#include <sys/socket.h>
#include <stdio.h>

#include "cfSock.h"
#include "cfSock_types.h"

/*------------------------- Global Variables --------------------------------*/

int           cf_sFd;		/* socket file descriptor */
int           sockAddrSize = sizeof (struct sockaddr_in); 
                                /* size of socket address structure */
int           closeSocket = 0;  /* switch to close socket and terminate */

/* function declarations */

void cfSockWorkTask (int task_sFd, char * address, u_short port);

extern void taskSCAN_CF(long*,long*,long*,long*);
extern void taskHAPTB_CF(long*,long*,long*,long*);
extern void taskHAPADC_CF(long*,long*,long*,long*);
extern void taskADC18_CF(long*,long*,long*,long*);
//extern void task_BMW(long*,long*,long*,long*);
extern void taskVQWK_CF(long*,long*,long*,long*); 

/****************************************************************************
 *
 * cfSockSer - accept and process requests over a TCP socket
 *
 * This routine creates a TCP socket, and accepts connections over the socket
 * from clients. Each client connection is handled by spawning a separate
 * task to handle client requests.
 *
 * This routine may be invoked as follows:
 *       -> sp cfSockSer
 *       task spawned: id = 0x3a6f1c, name = t1
 *       value = 3829532 = 0x3a6f1c
 *       -> MESSAGE FROM CLIENT (Internet Address 150.12.0.10, port 1027):
 *       Hello out there
 *
 * RETURNS: Never, or when client requests that the server dies followed by
 *                    a subsequent request (kludgey, eh?)
 *                 or ERROR if resources could not be allocated.
 *            
 */

STATUS cfSockSer (void)
{
  struct sockaddr_in  serverAddr;    /* server's socket address */
  struct sockaddr_in  clientAddr;    /* client's socket address */
  int                 newFd;         /* socket descriptor from accept */
  int                 ix = 0;        /* counter for work task names */
  char                workName[16];  /* name of work task */
  int optval = 1;
  
  /* set up the local address */

  closeSocket = 0;  
  sockAddrSize = sizeof (struct sockaddr_in);
  bzero ((char *) &serverAddr, sockAddrSize);
  serverAddr.sin_family = PF_INET;
  serverAddr.sin_port = htons (SERVER_PORT_NUM);
  serverAddr.sin_addr.s_addr = htonl (INADDR_ANY);
  
  /* create a TCP-based socket */
  
  if ((cf_sFd = socket (PF_INET, SOCK_STREAM, 0)) == SOCK_ERROR)
    {
      close(cf_sFd);
      if ((cf_sFd = socket (PF_INET, SOCK_STREAM, 0)) == SOCK_ERROR)
	{
	  perror ("socket");
	  return (SOCK_ERROR);
	}
    }
  
  setsockopt(cf_sFd, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(optval));
  setsockopt(cf_sFd, SOL_SOCKET, SO_KEEPALIVE, (char *)&optval, sizeof(optval));


  /* bind socket to local address */
  
  if (bind (cf_sFd, (struct sockaddr *) &serverAddr, sockAddrSize) == SOCK_ERROR)
    {
      perror ("bind");
      close(cf_sFd);
      return (SOCK_ERROR);
    }
  
  /* create queue for client connection requests */
  
  if (listen (cf_sFd, SERVER_MAX_CONNECTIONS) == SOCK_ERROR)
    {
      perror ("listen");
      close(cf_sFd);
      return (SOCK_ERROR);
    }
  
  /* accept new connect requests and spawn tasks to process them */
  
  while(closeSocket==0)
    {
      //      printf("closeSocket flag is %d",closeSocket);
      if ((newFd = accept (cf_sFd, (struct sockaddr *) &clientAddr,
			   &sockAddrSize)) == SOCK_ERROR)
	{
	  perror ("accept");
	  return (SOCK_ERROR);
	}
      
      sprintf (workName, "tcfSockWork%d", ix++);
      if (taskSpawn(workName, SERVER_WORK_PRIORITY, 0, SERVER_STACK_SIZE,
		    (FUNCPTR) cfSockWorkTask, newFd,
		    (int) inet_ntoa (clientAddr.sin_addr), 
		    ntohs (clientAddr.sin_port),
		    0, 0, 0, 0, 0, 0, 0) == SOCK_ERROR)
	{
	  /* if taskSpawn fails, close fd and return to top of loop */	  
	  perror ("taskSpawn");
	  close (newFd);
	}
    }

  close (cf_sFd);
}

/****************************************************************************
 *
 * cfSockWorkTask - process client requests
 *
 * This routine reads from the server's socket, and processes client
 * requests. If the client requests a reply message, this routine
 * will send a reply to the client.
 *
 * RETURNS: N/A.
 */

void cfSockWorkTask
(
 int                 task_sFd,            /* server's socket fd */
 char *              address,        /* client's socket address */
 u_short             port            /* client's socket port */
 )
{
  struct request      clientRequest;  /* request/message from client */
  struct request      serverReply;    /* reply/message from server  */
  int                 nRead;          /* number of bytes read */
  static char         replyMsg[] = "Server received your message";
  int ireq = 0;
  char                workName[16];  /* process name of requested task */
  long ltmp;
  int verbose = 0;  
  
  /* read client request, display message */
  
  while ((nRead = fioRead (task_sFd, (char *) &clientRequest,
			   sizeof (clientRequest))) > 0)
    {

      ltmp = ntohl(clientRequest.magic_cookie); clientRequest.magic_cookie = ltmp;
      ltmp = ntohl(clientRequest.command_type); clientRequest.command_type = ltmp;
      ltmp = ntohl(clientRequest.command); clientRequest.command = ltmp;
      ltmp = ntohl(clientRequest.par1); clientRequest.par1 = ltmp;
      ltmp = ntohl(clientRequest.par2); clientRequest.par2 = ltmp;
      ltmp = ntohl(clientRequest.par3); clientRequest.par3 = ltmp;
      ltmp = ntohl(clientRequest.msgLen); clientRequest.msgLen = ltmp;

      if (verbose) {
            printf("magic_cookie is %ld\n",clientRequest.magic_cookie);
            printf("MAGIC_COOKIE is %ld\n",MAGIC_COOKIE);
            printf("Command Type is %ld\n",clientRequest.command_type);
            printf("Command is %ld\n",clientRequest.command);
            printf("Request param_1 is %ld\n",clientRequest.par1);
            printf("Request param_2 is %ld\n",clientRequest.par2);
            printf("Request param_3 is %ld\n",clientRequest.par3);
            printf("msgLen is %ld\n",clientRequest.msgLen);
            printf ("MESSAGE FROM CLIENT (Internet Address %s, port %d):\n%s\n",
      	      address, port, clientRequest.message);
      
      }

      free (address);                 /* free malloc from inet_ntoa() */

      if (strncmp(clientRequest.message,"Q",sizeof(clientRequest.message)) == 0) 
	{
	  printf ("Client request to kill server \n ");
	  closeSocket = 1;
	}
      
      if (clientRequest.command_type==COMMAND_HAPTB) {
	taskHAPTB_CF(&clientRequest.command,
                     &clientRequest.par1,
		     &clientRequest.par2,
		     &clientRequest.par3);
		sprintf (workName, "request%d", ireq++);
		if (taskSpawn(workName, SERVER_WORK_PRIORITY, 0, SERVER_STACK_SIZE,
			      taskHAPTB_CF, clientRequest.command,
			      clientRequest.par1,
			      clientRequest.par2,
			      clientRequest.par3,
			      0, 0, 0, 0, 0, 0, 0) == SOCK_ERROR)
		  {
		    /* if taskSpawn fails, close fd and return to top of loop */
		    perror ("taskSpawn");
		  }
	    if (verbose){
		printf("HAPTB config utility returns:\n");
		printf("  command: %d\n",clientRequest.command);
		printf("  par1:    %d\n",clientRequest.par1);
		printf("  par2:    %d\n",clientRequest.par2);
		printf("  par3:    %d\n",clientRequest.par3);
	        printf("HAPTB call ended\n");
		}
      }


      if (clientRequest.command_type==COMMAND_HAPADC) {
	taskHAPADC_CF(&clientRequest.command,
		      &clientRequest.par1,
		      &clientRequest.par2,
		      &clientRequest.par3);
	//	printf("HAPADC config utility returns:\n");
	//	printf("  command: %d\n",clientRequest.command);
	//	printf("  par1:    %d\n",clientRequest.par1);
	//	printf("  par2:    %d\n",clientRequest.par2);
      }

      if (clientRequest.command_type==COMMAND_BMW) {
	//	 task_BMW(&clientRequest.command, 
	//	 &clientRequest.par1, 
	//	 &clientRequest.par2, 
	//	 &clientRequest.par3); 
      }

      if (clientRequest.command_type==COMMAND_SCAN) {
        if (verbose) {
   	  printf("Calling scan util \n");
	  printf("  command: %d\n",clientRequest.command);
	  printf("  par1:    %d\n",clientRequest.par1);
	  printf("  par2:    %d\n",clientRequest.par2);
	  printf("  par3:    %d\n",clientRequest.par3);
          printf("Will call scan util    ++++++++++++++++++++++++++++\n");
	}
	taskSCAN_CF(&clientRequest.command,
		    &clientRequest.par1,
		    &clientRequest.par2,
		    &clientRequest.par3);
      }

      if (clientRequest.command_type==COMMAND_ADC18) {
        if (verbose) {
   	  printf("Calling 18 bit ADC \n");
	  printf("  command: %d\n",clientRequest.command);
	  printf("  par1:    %d\n",clientRequest.par1);
	  printf("  par2:    %d\n",clientRequest.par2);
	  printf("  par3:    %d\n",clientRequest.par3);
          printf("Will call ADC18_CF    ++++++++++++++++++++++++++++\n");
	}
	taskADC18_CF(&clientRequest.command,
		     &clientRequest.par1,
		     &clientRequest.par2,
		     &clientRequest.par3);
      }

       if (clientRequest.command_type==COMMAND_VQWK) {  
         if (verbose) { 
       	  printf("Calling VQWK ADC \n"); 
       	  printf("  command: %d\n",clientRequest.command); 
       	  printf("  par1:    %d\n",clientRequest.par1); 
       	  printf("  par2:    %d\n",clientRequest.par2); 
       	  printf("  par3:    %d\n",clientRequest.par3); 
           printf("Will call VQWK_CF    ++++++++++++++++++++++++++++\n"); 
       	} 
       	taskVQWK_CF(&clientRequest.command, 
       		     &clientRequest.par1, 
       		     &clientRequest.par2, 
       		     &clientRequest.par3); 
       } 

      if (clientRequest.reply) {
	char thisReply[sizeof(replyMsg)+sizeof(clientRequest.message) + 10];
	strcpy(thisReply,replyMsg);
	strcat(thisReply," : ");
	strcat(thisReply,clientRequest.message);
	//	if (write (task_sFd, replyMsg, sizeof (replyMsg)) == SOCK_ERROR)
	//	printf ("Reply to Client %s\n ",thisReply);
	strcpy(serverReply.message,thisReply);
	serverReply.command_type = htonl(clientRequest.command_type);
	serverReply.command = htonl(clientRequest.command);
	serverReply.magic_cookie = htonl(clientRequest.magic_cookie);
	serverReply.par1 = htonl(clientRequest.par1);
	serverReply.par2 = htonl(clientRequest.par2);
	serverReply.par3 = htonl(clientRequest.par3);
	serverReply.msgLen = htonl(strlen(serverReply.message));
	if (write (task_sFd, (char *) &serverReply, sizeof (serverReply)) == SOCK_ERROR)
	perror ("write");
      }


    }
  
  if (nRead == SOCK_ERROR)                 /* error from read() */
    perror ("read");
  
  close (task_sFd);                        /* close server socket connection */
}
















