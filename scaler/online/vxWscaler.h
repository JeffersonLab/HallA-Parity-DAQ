/* vxWscaler.h  -- used by server and client */

/* The following defines the IP address of the server */
/* happex1.jlab.org */
#define SERVER_INET_ADDR "129.57.164.100"   
/* The following defines the port number of the server */
#define SERVER_PORT_NUM (5022)

#define NUM_RING 500
#define MSG_SIZE (50)
#include "scaler1.h"

/* structure for request to server */
struct request
{ 
char message[MSG_SIZE];         /* (char) test message   */
int clearflag;                  /* flag to clear scalers */
int getring;                    /* flag to get ring buffer */
int flag1,flag2,flag3,flag4;    /* misc. control flags. */
};

/* structure for data from scaler server */
struct scalerdata
{ 
long scalersums[32*MAXSCALER];             /* processed scaler data */
long ringbuff[NUM_RING*34*MAXSCALER+1]; /* Ring buffer: 32 chan + other info */
char message[MSG_SIZE];         /* (char) test message   */
int info1,info2,info3,info4;    /* various informations */
};


#define SERVER_WORK_PRIORITY  (100)
#define SERVER_STACK_SIZE (10000)
#define MAX_QUEUED_CONNECTIONS (5)

#define SOCK_ADDR_SIZE  sizeof(struct sockaddr_in))
#define REPLY_MSG_SIZE (1000)

#define ERROR (-1)

