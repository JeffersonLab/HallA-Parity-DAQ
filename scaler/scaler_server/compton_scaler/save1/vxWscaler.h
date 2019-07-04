/* vxWscaler.h  -- used by server and client */

/* The following defines the IP address of the server */
#define SERVER_INET_ADDR "129.57.192.50"   
/* The following defines the port number of the server */
#define SERVER_PORT_NUM (5090)

#define MAXBLOCKS 20
#define MSG_SIZE  50

/* structure for requests from client */
/* this needs to be the SAME as for other scaler client/servers, so don't 
   change it, please */

struct request
{ 
int reply;
long ibuf[32*MAXBLOCKS]; 
char message[MSG_SIZE];
int clearflag;
int flag1;
};

#define SERVER_WORK_PRIORITY  (100)
#define SERVER_STACK_SIZE (10000)
#define MAX_QUEUED_CONNECTIONS (5)

#define SOCK_ADDR_SIZE  sizeof(struct sockaddr_in))
#define REPLY_MSG_SIZE (1000)

#define ERROR (-1)

