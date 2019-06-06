/* cfSock.h  - header used by both client and server of cfSock */
/* defines */

#define SERVER_PORT_NUM         5001  /* server's port number for bind() */
#define SERVER_WORK_PRIORITY    125    /* priority of server's work task */
#define SERVER_STACK_SIZE       10000  /* stack size of server's work task */
#define SERVER_MAX_CONNECTIONS  5      /* max clients connected at a time */

#define REQUEST_MSG_SIZE        1024   /* max size of request message */
#define REPLY_MSG_SIZE          1024   /* max size of reply message */

#ifndef SOCK_ERROR
#define SOCK_ERROR (-1)
#endif
#ifndef SOCK_OK
#define SOCK_OK    (0)
#endif
#define MAGIC_COOKIE -7654321
#define SOCK_ADDR_SIZE sizeof(struct sockaddr_in))

int IgnoreRuns;
int Running;

/* structure for requests from clients to server */

struct request
{ 
   int  magic_cookie;
   int  command_type;
   int  command;
   int  par1;
   int  par2;
   int  par3;
   int  reply;                        /* TRUE = request reply from server */
   char  message[REQUEST_MSG_SIZE];    /* message buffer */
   int  msgLen;                       // length of message text
};

void setIgnoreRun(int flag);

int getIgnoreRun();

void setRunFlag(int flag);

int getRunFlag();
