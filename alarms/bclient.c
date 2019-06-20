/* Client/server code  -- example for other apps but used here to beep a computer
   remotely.

   This is the client.

   author: Bob Michaels, Oct 2004 

   Update to process alarms.  July 2005

*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <netdb.h>

int process_alarms();

#define PORT  5077

int s;                          /* connected socket descriptor */
struct hostent *hp;             /* pointer to host info for remote host */
struct servent *sp;             /* pointer to service information */
long timevar;                   /* contains time returned by time() */
char *ctime();                  /* declare time formatting routine */

int optval = 1;
int verbose = 0;
struct sockaddr_in myaddr_in;   /* for local socket address */
struct sockaddr_in peeraddr_in; /* for peer socket address */


main(int argc, char *argv[])
{
  int addrlen, i, j, k, n;
  char alarmfile[100];
  int debug=0;
  int test1=0;

  struct charint {
    char cbuf[10];
    int ibuf[12];
  } ;
  struct charint sendbuf;
  struct charint recvbuf;

  if (argc < 3) {
    fprintf(stderr, "Usage:  %s <remote host> <alarm file>\n", argv[0]);
    exit(1);
  }

  strcpy(alarmfile, argv[2]);

  if(debug) printf("Handling alarm file %s\n",alarmfile);

  int ialarm;
  ialarm = process_alarms(alarmfile);

  if (ialarm <= 0) exit(1);
  sendbuf.ibuf[0] = ialarm;

  if(debug) printf("Alarm result %d \n",sendbuf.ibuf[0]);

  if (test1 == 1) exit(1);             

  /* clear out address structures */
  memset ((char *)&myaddr_in, 0, sizeof(struct sockaddr_in));
  memset ((char *)&peeraddr_in, 0, sizeof(struct sockaddr_in));

  /* Set up the peer address to which we will connect. */
  peeraddr_in.sin_family = AF_INET;
  /* Get the host information for the hostname that the
   * user passed in.  */
  hp = gethostbyname (argv[1]);

  if (hp == NULL) {
    fprintf(stderr, "%s: %s not found in /etc/hosts\n",
        argv[0], argv[1]);
    exit(1);
  }
  peeraddr_in.sin_addr.s_addr =  ((struct in_addr *)
      (hp->h_addr))->s_addr  ; 


  /* Use an available port and allow re-use of socket*/
  peeraddr_in.sin_port = htons(PORT);

  /* Create the socket. */
  s = socket (AF_INET, SOCK_STREAM, 0);
  if (s == -1) {
    perror(argv[0]);
    fprintf(stderr, "%s: unable to create socket\n", argv[0]);
    exit(1);
  }
  if (setsockopt(s, SOL_SOCKET, 
        SO_REUSEADDR,&optval,sizeof(optval) ) == -1) {
    printf("Conn aborted (reuseaddr)\n");
    exit(1);
  }


  /* Try to connect to the remote server at the address
   * which was just built into peeraddr.
   */
  if (connect(s, &peeraddr_in, sizeof(struct sockaddr_in)) == -1) {
    perror(argv[0]);

    fprintf(stderr, "%s: unable to connect to remote %s \n", argv[0],argv[1]) ;
    exit(1);
  }

  addrlen = sizeof(struct sockaddr_in);
  if (getsockname(s, &myaddr_in, &addrlen) == -1) {
    perror(argv[0]);
    fprintf(stderr, "%s: unable to read socket address\n", argv[0]);
    exit(1);
  }

  time(&timevar);
  if (verbose) printf("Connected to %s on port %u at %s",
      argv[1], ntohs(myaddr_in.sin_port), ctime(&timevar));

  if (write(s,(char *)&sendbuf, sizeof(sendbuf)) == -1) {
    fprintf(stderr, "%s: Connection aborted on error ",
        argv[0]);
    fprintf(stderr, "on send number %d\n", i);
    exit(1);
  }

  if (shutdown(s, 1) == -1) {
    perror(argv[0]);
    fprintf(stderr, "%s: unable to shutdown socket\n", argv[0]);
    exit(1);
  }


  while (i = read(s, (char *)&recvbuf, sizeof(recvbuf))) {
    if (i == -1) {
errout:                 perror(argv[0]);
                        fprintf(stderr, "%s: error reading result\n", argv[0]);
                        exit(1);
    }

  }


  time(&timevar);

}


int process_alarms(char *file) {

  /* This routine decides which alarm to cause, based on the
     syntax of the input "file".  The alarm is an index pointing
     to which alarm (1,2,3...).  This variable is "result". */

  int ldebug = 0;
  int result = 0;
  char strin[100],strin2[100],dummy[50];
  char epicsvar[50], command[50];
  int which_alarm;
  float value,epicsdata;
  FILE *fd, *fd2;
  fd = fopen(file,"r");
  if (fd == NULL) {
    printf("Cannot open file %s\n",file);
    return 0;
  }

  while(fgets(strin,100,fd)!=NULL) {

    if (strstr(strin,"#")!=NULL || strlen(strin) <= 5) {
      if (ldebug) printf("Comment \n");
    } else {
      if (ldebug) printf("Not comment = %s \n",strin);
      sscanf(strin,"%s %s %f %d",&epicsvar,&command,&value,&which_alarm);
      if (ldebug) 
        printf("epicsvar = %s \n  command %s \n   value = %f   alarm = %d\n",
            epicsvar,command,value,which_alarm);

      /* get the EPICS data */
      char syscommand[100];
      sprintf(syscommand,"/adaqfs/home/apar/devices/alarms/getvar %s",epicsvar);
      if (ldebug) printf("syscommand = %s \n",syscommand);
      system(syscommand);
      fd2 = fopen("temp.dat","r");
      if (fd2 == NULL) {
        printf("Error 1 obtaining EPICS data \n");
        return 0;
      }
      if (fgets(strin2,100,fd2)==NULL) return 0;
      sscanf(strin2,"%s %f",&dummy,&epicsdata);

      if (ldebug) printf("epicsdata  = %s =  %f\n",dummy,epicsdata);

      if(strstr(dummy, epicsvar) == NULL) {
        printf("Error 2 obtaining EPICS data \n");
        return 0;
      }

      int icommand = 0;
      if ( strstr(command,"greater")!=NULL ||
          strstr(command,"GREATER")!=NULL ||
          strstr(command,"ge")!=NULL ||
          strstr(command,"GE")!=NULL ||
          strstr(command,"gt")!=NULL ||
          strstr(command,"GT")!=NULL ) {
        icommand = 1;
      }
      if ( strstr(command,"less")!=NULL ||
          strstr(command,"LESSER")!=NULL ||
          strstr(command,"le")!=NULL ||
          strstr(command,"LE")!=NULL ||
          strstr(command,"lt")!=NULL ||
          strstr(command,"LT")!=NULL ) {
        icommand = 2;
      }

      int rlogic = 0;
      switch (icommand) {
        case 1:
          if (epicsdata > value) rlogic = 1;
          break;
        case 2:
          if (epicsdata < value) rlogic = 1;
          break;
        default:
          break;

      }
      if (rlogic) result = which_alarm;

      if (ldebug) printf("command = %d  rlogic %d  result %d \n",
          icommand,rlogic,result);

      return result;

    }

  }
  return 0;   

}
