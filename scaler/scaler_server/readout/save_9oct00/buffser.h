/* vxWscaler.h  -- used by server and client */

/* The following defines the IP address of the server */
/* halladaq4.cebaf.gov */
#define SERVER_INET_ADDR "129.57.164.11"   
/* hallavme3.cebaf.gov */
/*#define SERVER_INET_ADDR "129.57.192.29"  */
/* The following defines the port number of the server */
#define SERVER_PORT_NUM (5010)

#define MSG_SIZE (30)
#define NET_BUFF_SIZE (150)

/* structure for requests from client */
struct request
{ 
short reply;
unsigned long ibuf[NET_BUFF_SIZE*35]; 
long status; /* 0 = in a run
			1 = new run started
			2 = run stopped 
			3 = between run (gates ON but CODA do not run) */

};


#define SERVER_WORK_PRIORITY  (100)
#define SERVER_STACK_SIZE (10000)
#define MAX_QUEUED_CONNECTIONS (5)

#define SOCK_ADDR_SIZE  sizeof(struct sockaddr_in))
#define REPLY_MSG_SIZE (1000)

#define ERROR (-1)

