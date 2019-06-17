
/* cfSockCli - send requests to server over a TCP socket 
               for configuration utility */
/*
  DESCRIPTION
  This file contains the client-side of the VxWorks TCP example code.
*/

/* includes */

#include "netinet/in.h"
#include <netinet/tcp.h>
#include <string.h>  //added by Caryn
#include <stdio.h> //added by Caryn

#include "cfSockCli.h"
/****************************************************************************
 *
 * cfSockCli - send requests to server over a TCP socket for configuration utility
 *
 * This routine connects over a TCP socket to a server, and sends a
 * user-provided message to the server. Optionally, this routine
 * waits for the server's reply message.
 *
 * This routine may be invoked as follows:
 *       -> cfSockCli "remoteSystem"
 *       Message to send:
 *       Hello out there
 *       Would you like a reply (Y or N):
 *       y
 *       value = 0 = 0x0
 *       -> MESSAGE FROM SERVER:
 *       Server received your message
 *
 * RETURNS: OK, or ERROR if the message could not be sent to the server.
*/


int cfSockCli ( 
	       int crate_number,      /* which crate to address */
	       struct request *myRequest,    /* request to send to server */
	       struct request *serverReply   /* reply from server */
 )
{
  struct sockaddr_in  serverAddr;    /* server's socket address */
  char                replyBuf[REPLY_MSG_SIZE]; /* buffer for reply */
  int                 sockAddrSize;  /* size of socket address structure */
  int                 sFd;           /* socket file descriptor */
  int  nWrite;
  int i, j;
  char optval;
  char * servername;
  int verbose = 0;

  /* create client's socket */
  switch (crate_number) {
  case Crate_CountingHouse:
    servername = ServerName_CountingHouse;
    break;
  case Crate_LeftSpect:
    servername = ServerName_LeftSpect;
    break;
  case Crate_RightSpect:
    servername = ServerName_RightSpect;
    break;
  case Crate_Injector:
    servername = ServerName_Injector;
    break;
  case Crate_TestCrate:
    servername = ServerName_TestCrate;
    break;
  default:
    return (SOCK_ERROR);
  }  

  if ((sFd = socket (PF_INET, SOCK_STREAM, 0)) == SOCK_ERROR)
    {
      perror ("socket");
      return (SOCK_ERROR);
    }
  
  optval = 1;
  setsockopt ( sFd, IPPROTO_TCP, TCP_NODELAY, 
	       (char *) &optval,
	       4 ); /* black magic from Chowdhary's code */

  setsockopt(sFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

  /* bind not required - port number is dynamic */
    /* build server socket address */
  
  sockAddrSize = sizeof (struct sockaddr_in);
  bzero ((char *) &serverAddr, sockAddrSize);
  serverAddr.sin_family = PF_INET;
  serverAddr.sin_port = htons (SERVER_PORT_NUM);
  

  if (verbose) printf("Connect to crate num %d   server = %s   port = %d \n",crate_number, servername, SERVER_PORT_NUM); 

  if ((serverAddr.sin_addr.s_addr = inet_addr (servername)) == SOCK_ERROR)
    {
      perror ("unknown server name");
      close (sFd);
      return (SOCK_ERROR);
    }
  
  /* connect to server */  
  if (connect (sFd, (struct sockaddr *) &serverAddr, sockAddrSize) == SOCK_ERROR)
    {
      perror ("connect");
      close (sFd);
      return (SOCK_ERROR);
    }
  
  
  /*    buildRequestInteractive(myRequest); */
   if (verbose) { 
     printf("\n"); 
     printf("your command type will be: %d \n",ntohl(myRequest->command_type)); 
     printf("your command will be: %d \n",ntohl(myRequest->command)); 
     printf("your magic_cookie will be: %d \n",ntohl(myRequest->magic_cookie)); 
     printf("your request param_1 will be: %d \n",ntohl(myRequest->par1)); 
     printf("your request param_2 will be: %d \n",ntohl(myRequest->par2)); 
     printf("your request param_3 will be: %d \n",ntohl(myRequest->par3)); 
   } 

  /* send request to server */
  nWrite = send (sFd, (char *) myRequest, sizeof (*myRequest), 0);
  if ( nWrite != sizeof(*myRequest) ) 
    {
      printf ( "cfSockCli WARN: nWrite = %d, sizeof (*myRequest) = %d\n",
		     nWrite, sizeof (*myRequest) );
      perror ("Connection aborted on error");
      close(sFd);
      return (SOCK_ERROR);
    }
  
  if (myRequest->reply)        /* if expecting reply, read and display it */
    {
      i = recv ( sFd, (char *) serverReply, sizeof(*serverReply), 0 );

      if ( i == SOCK_ERROR ) 
	{
	  perror ("Error reading result\n");
	  return (SOCK_ERROR);
	}
      /* The reason this while loop exists is that there
       * is a remote possibility of the above recv returning
       * less than sizeof(*serverReply) bytes.  This is because a recv returns
       * as soon as there is some data, and will not wait for
       * all of the requested data to arrive.  Since sizeof(*serverReply) bytes
       * is relatively small compared to the allowed TCP
       * packet sizes, a partial receive is unlikely.  If
       * the reply size were 2048 bytes instead,
       * a partial receive would be far more likely.
       * This loop will keep receiving until all sizeof(*serverReply) bytes
       * have been received, thus guaranteeing that the
       * next recv at the top of the loop will start at
       * the beginning of the next reply.
       */
  while ( i < sizeof(*serverReply) ) {
	j = recv( sFd, ((char *) serverReply)+i, sizeof(*serverReply)-i, 0 );
	if( j == SOCK_ERROR ){
	  perror ("Error reading result\n");
	  close(sFd);
	  return (SOCK_ERROR);
	}
	i += j;
  }
  //handleReplyInteractive(serverReply);
}
  
  close (sFd);
  return (SOCK_OK);
}

/*

  main (int argc, char *argv[])
  {
    long command = 1, par1 = 1, par2 = 1;
    char *reply="Y";
    char *msg = "one";
    cfSockCommand(Crate_CountingHouse,command,par1,par2,reply,msg);
        printf ("MESSAGE FROM SERVER:  <%s> \n", msg);
        printf("Server reply command: %d \n",command);
        printf("Server reply param_1: %d \n",par1);
        printf("Server reply param_2: %d \n\n",par2);
  }
  main (int argc, char *argv[])
  {
  struct request myRequest;     // request to send to server 
  struct request serverReply;   // reply from server 
  
  
  //    cfSockCli("129.57.164.13");
  buildRequestInteractive(&myRequest);
  printf("\n");
  printf("your command will be: %d \n",ntohl(myRequest.command));
  printf("your magic_cookie will be: %d \n",ntohl(myRequest.magic_cookie));
  printf("your request param_1 will be: %d \n",ntohl(myRequest.par1));
  printf("your request param_2 will be: %d \n",ntohl(myRequest.par2));
  
  cfSockCli(Crate_CountingHouse, &myRequest,&serverReply);
    
  if (myRequest.reply)        // if expecting reply, read and display it
  handleReplyInteractive(&serverReply);
  
  }
*/

int cfSockCommand(int crate_number,
		  long command_type, long command,  
		  long req_param,    long req_param_2, long req_param_3, 
		  char *reply,       char *msg  )
{
  struct request myRequest;     // request to send to server 
  struct request serverReply;   // reply from server 
  int mlen;
  int errFlag;
  //  char *reply = "Y";

  myRequest.command_type = htonl(command_type);
  myRequest.command = htonl(command);
  myRequest.magic_cookie = htonl(MAGIC_COOKIE);
  myRequest.par1 = htonl(req_param);
  myRequest.par2 = htonl(req_param_2);
  myRequest.par3 = htonl(req_param_3);
  switch (*reply) 
   {
    case 'y':
    case 'Y': myRequest.reply = TRUE;
      break;
    default: myRequest.reply = FALSE;
      break;
    }
  mlen = strlen(msg);
  strncpy(myRequest.message,msg,REQUEST_MSG_SIZE);
  myRequest.msgLen = htonl(mlen);
  myRequest.message[mlen] = '\0';

  //  printf("\n");
  //  printf ("Message to send:   <%s> \n",myRequest.message);
  //  printf("your command type will be: %d \n",ntohl(myRequest.command_type));
  //  printf("your command will be: %d \n",ntohl(myRequest.command));
  //  printf("your request param_1 will be: %d \n",ntohl(myRequest.par1));
  //  printf("your request param_2 will be: %d \n",ntohl(myRequest.par2));
  //  printf("Requested reply?  : %s \n",reply);
  //  printf("\n");
  
  errFlag = cfSockCli(crate_number,&myRequest,&serverReply);
  if (errFlag != SOCK_OK) return errFlag;
    
  // if expecting reply, read and display it
  if (myRequest.reply) {
	msg = strcpy(serverReply.message,serverReply.message);
        command_type = htonl(serverReply.command_type);
        command = htonl(serverReply.command);
        req_param = htonl(serverReply.par1);
        req_param_2 = htonl(serverReply.par2);
        req_param_3 = htonl(serverReply.par3);
	//	        printf ("MESSAGE FROM SERVER:\n%s\n", msg);
	//	        printf("Server reply command: %d \n",command);
	//	        printf("Server reply param_1: %d \n",req_param);
	//	        printf("Server reply param_2: %d \n\n",req_param_2);
	//        handleReplyInteractive(&serverReply);
  }
  return errFlag;
}
/**
  DEPRECATED: 2019-05-13, replaced with GMSockCommand which
  takes no structs as arguments, making it callable from
  Python 3.
**/
int GreenSockCommand(int crate_number, struct greenRequest *gRequest)
{
  struct request myRequest;     // request to send to server 
  struct request serverReply;   // reply from server 
  long command_type;
  long command;
  long req_param;
  long req_param_2;
  //  char *reply;
  int mlen;
  int errFlag;
  
  //  char *reply = "Y";

  myRequest.command_type = htonl(gRequest->command_type);
  myRequest.command = htonl(gRequest->command);
  myRequest.magic_cookie = htonl(MAGIC_COOKIE);
  myRequest.par1 = htonl(gRequest->par1);
  myRequest.par2 = htonl(gRequest->par2);
  myRequest.par3 = htonl(gRequest->par3);

  switch (*(gRequest->reply))
    {
    case 'y':
    case 'Y': myRequest.reply = TRUE;
      break;
    default: myRequest.reply = FALSE;
      break;
    }
  mlen = strlen(gRequest->message);
  strncpy(myRequest.message,gRequest->message,REQUEST_MSG_SIZE);
  myRequest.msgLen = htonl(mlen);
  myRequest.message[mlen] = '\0';

  
  // printf("\n");
  // printf ("Message to send:   <%s> \n",myRequest.message);
  // printf("your command will be: %d \n",ntohl(myRequest.command));
  //  printf("your request param_1 will be: %d \n",ntohl(myRequest.par1));
  // printf("your request param_2 will be: %d \n",ntohl(myRequest.par2));
  // printf("Requested reply?  : %s \n",gRequest->reply);
  // printf("\n");
   
  errFlag = cfSockCli(crate_number, &myRequest,&serverReply);

  if (errFlag != SOCK_OK) return errFlag;
    
  // if expecting reply, read and display it
  if (myRequest.reply) {
    //	gRequest->message = strcpy(serverReply.message,serverReply.message);
    strcpy(gRequest->message,serverReply.message);
    gRequest->command_type = htonl(serverReply.command_type);
    gRequest->command = htonl(serverReply.command);
    gRequest->par1 = htonl(serverReply.par1);
    gRequest->par2 = htonl(serverReply.par2);
    gRequest->par3 = htonl(serverReply.par3);
    
     printf ("cfSockCli: MESSAGE FROM SERVER:   %s\n", gRequest->message);
     printf("Server reply command: %d \n",gRequest->command);
     //     printf("Server reply param_1: %d \n",gRequest->req_param);
     //     printf("Server reply param_2: %d \n\n",gRequest->req_param_2);
     //         handleReplyInteractive(&serverReply);
  }
  return errFlag;
}

int GMSockCommand(int crate_number,
                  long command_type, 
                  long command,
                  long par1,
                  long par2, 
                  long par3){
  struct request myRequest;     // request to send to server 
  struct request serverReply;   // reply from server 
  int mlen; int errFlag;
  
  myRequest.command_type = htonl(command_type);
  myRequest.command = htonl(command);
  myRequest.magic_cookie = htonl(MAGIC_COOKIE);
  myRequest.par1 = htonl(par1);
  myRequest.par2 = htonl(par2);
  myRequest.par3 = htonl(par3);

  FILE *fp; char str[100];
  char* in_fname = "transfer.txt";
  
  fp = fopen(in_fname, "r"); int nlines = 0;
  char message[100]; char reply[100];
  if(fp == NULL){printf("Could not open file %s", in_fname);}
  while(fgets(str, 100, fp) != NULL){
    if(nlines < 1){strcpy(message, str);}
    else{strcpy(reply, str);}
    nlines++;
  }
  fclose(fp);

  system("rm -f transfer.txt");
  switch (*(reply))
  {
    case 'y':
    case 'Y': myRequest.reply = TRUE;
      break;
    default: myRequest.reply = FALSE;
      break;
  }
  mlen = strlen(message);
  strncpy(myRequest.message,message,REQUEST_MSG_SIZE);
  myRequest.msgLen = htonl(mlen);
  myRequest.message[mlen] = '\0';
  
  errFlag = cfSockCli(crate_number, &myRequest,&serverReply);

  if(errFlag == SOCK_OK){
    FILE *fp_out; char str_out[100];
    char* out_fname = "reply.txt";
    
    fp_out = fopen(out_fname, "w");
    fprintf(fp_out, "%i\n", ntohl(serverReply.command_type));
    fprintf(fp_out, "%i\n", ntohl(serverReply.command));
    fprintf(fp_out, "%i\n", ntohl(serverReply.par1));
    fprintf(fp_out, "%i\n", ntohl(serverReply.par2));
    fprintf(fp_out, "%i\n", ntohl(serverReply.par3));
    fprintf(fp_out, "%s", serverReply.message);
    fprintf(fp_out, "%i\n", (int)serverReply.reply);
    fclose(fp_out);

    printf("(cfSockCli) MESSAGE FROM SERVER: %s\n", serverReply.message);
    printf("(cfSockCli) Server reply command: %d\n", ntohl(serverReply.command));
  }

  return errFlag;
}


void buildRequestInteractive(struct request *myRequest) {
  /* build request, prompting user for message */
  int                 mlen;          /* length of message */  
  char                reply;         /* if TRUE, expect reply back */
  
  printf ("Message to send: \n");
  mlen = read (STD_IN, myRequest->message, REQUEST_MSG_SIZE);
  myRequest->msgLen = htonl(mlen);
  myRequest->message[mlen - 1] = '\0';
  
  myRequest->command_type = htonl(1);
  printf("your command type will be: %d \n",ntohl(myRequest->command_type));
  myRequest->command = htonl(10);
  printf("your command will be: %d \n",ntohl(myRequest->command));
  myRequest->magic_cookie = htonl(MAGIC_COOKIE);
  printf("your magic_cookie will be: %d \n",ntohl(myRequest->magic_cookie));
  myRequest->par1 = htonl(1);
  printf("your request param_1 will be: %d \n",ntohl(myRequest->par1));
  myRequest->par2 = htonl(2);
  printf("your request param_2 will be: %d \n",ntohl(myRequest->par2));
  myRequest->par3 = htonl(3);
  printf("your request param_3 will be: %d \n",ntohl(myRequest->par3));


  printf ("Would you like a reply (Y or N): \n");
  read (STD_IN, &reply, 1);
  switch (reply)
    {
    case 'y':
    case 'Y': myRequest->reply = TRUE;
      break;
    default: myRequest->reply = FALSE;
      break;
    }
}
