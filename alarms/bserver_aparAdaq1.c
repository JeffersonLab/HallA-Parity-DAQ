/* beep me, please */

/* Client/server code  -- example for other apps but used here to beep a computer
   remotely.

   This is the server.

   author: Bob Michaels, Oct 2004 
*/


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <netdb.h>

#define PORT  5077

int s;                          /* connected socket descriptor */
int ls;                         /* listen socket descriptor */

struct hostent *hp;             /* pointer to host info for remote host */
struct servent *sp;             /* pointer to service information */

long timevar;                   /* contains time returned by time() */
char *ctime();                  /* declare time formatting routine */

struct linger linger = {1,1};   /* allow a lingering, graceful close; */
/* used when setting SO_LINGER */

int verbose = 1;
int optval = 0;
struct sockaddr_in myaddr_in;   /* for local socket address */
struct sockaddr_in peeraddr_in; /* for peer socket address */

main(argc, argv)
  int argc;
  char *argv[];
{
  int addrlen;
  /* clear out address structures */
  memset ((char *)&myaddr_in, 0, sizeof(struct sockaddr_in));
  memset ((char *)&peeraddr_in, 0, sizeof(struct sockaddr_in));
  myaddr_in.sin_family = AF_INET;
  myaddr_in.sin_addr.s_addr = htons(INADDR_ANY);
  /* Use an available port and allow re-use of socket*/
  myaddr_in.sin_port = htons(PORT);
  /* Create the listen socket. */
  ls = socket (AF_INET, SOCK_STREAM, 0);
  if (setsockopt(ls, SOL_SOCKET, 
        SO_REUSEADDR,&optval,sizeof(optval) ) == -1) {
errout:         printf("Conn with %s aborted (reuseaddr)\n");
                exit(1);
  }


  if (ls == -1) {
    perror(argv[0]);
    fprintf(stderr, "%s: unable to create socket\n", argv[0]);
    exit(1);
  }

  /* Bind the listen address to the socket. */
  if (bind(ls, &myaddr_in, sizeof(struct sockaddr_in)) == -1)
  {
    perror(argv[0]);
    fprintf(stderr, "%s: unable to bind address\n", argv[0]);
    exit(1);
  }
  if (listen(ls, 5) == -1) {
    perror(argv[0]);
    fprintf(stderr, "%s: unable to listen on socket\n", argv[0]);
    exit(1);
  }

  setpgrp();

  switch (fork()) {
    case -1:                /* Unable to fork, for some reason. */
      perror(argv[0]);
      fprintf(stderr, "%s: unable to fork daemon\n", argv[0]);
      exit(1);

    case 0:                 /* The child process (daemon) comes here. */
      fclose(stdin);
      fclose(stderr);

      signal(SIGCLD, SIG_IGN);
      for(;;) {
        addrlen = sizeof(struct sockaddr_in);
        s = accept(ls, &peeraddr_in, &addrlen);
        if ( s == -1) exit(1);
        switch (fork()) {
          case -1:        /* Can't fork, just continue. */
            exit(1);
          case 0:         /* Child process comes here. */
            server();
            exit(0);
          default:        /* Daemon process comes here. */
            close(s);
        }

      }

    default:                /* Parent process comes here. */
      exit(0);
  }
}

/*
 *                              S E R V E R
 *
 */
server()
{
  int reqcnt = 0;         /* keeps count of number of requests */

  struct charint {
    char cbuf[10];
    int ibuf[12];
  } ;
  struct charint sendbuf;
  struct charint recvbuf;

  char *inet_ntoa();
  char *hostname;        
  int i,len, len1;

  /* Close the listen socket inherited from the daemon. */
  close(ls);

  hp = gethostbyaddr ((char *) &peeraddr_in.sin_addr,
      sizeof (struct in_addr),
      peeraddr_in.sin_family);

  if (hp == NULL) {
    hostname = inet_ntoa(peeraddr_in.sin_addr);
  } else {
    hostname = hp->h_name;  /* point to host's name */
  }

  time (&timevar);

  if (verbose) printf("Startup from %s port %u at %s",
      hostname, ntohs(peeraddr_in.sin_port), ctime(&timevar));

  if (setsockopt(s, SOL_SOCKET, SO_LINGER, &linger,
        sizeof(linger)) == -1) {
errout:         printf("Connection with %s aborted on error\n", hostname);
                exit(1);
  }

  while (len = read(s, (char *)&recvbuf, sizeof(recvbuf))) {
    if (len == -1) goto errout; /* error from read */
    reqcnt++;
  }


  if (verbose) {
    printf("Message from client %s \n",recvbuf.cbuf);
    int i;
    for (i=0; i<1; i++)  printf("ibuf[%d] = %d\n",i,recvbuf.ibuf[i]);
  }

  /* here's where you decide what to do locally. */
  /* skip 1 to reduce somewhat the chance of ... someone else doing something */
  if (recvbuf.ibuf[0] == 2) {
    system("play /home/hacuser/parity-alarms/alert.wav");
  }
  if (recvbuf.ibuf[0] == 3) {
    system("/home/adaq/beep/beep_script sweeper1.wav");
  }
  if (recvbuf.ibuf[0] == 4) {
    system("/home/adaq/beep/beep_script compton_rateG.wav");
  }
  if (recvbuf.ibuf[0] == 5) {
    system("/home/adaq/beep/beep_script compton_rateE.wav");
  }
  if (recvbuf.ibuf[0] == 6) {
    system("/home/adaq/beep/beep_script septumIL.wav");
  }
  if (recvbuf.ibuf[0] == 7) {
    system("/home/adaq/beep/beep_script septumIR.wav");
  }

  if (write(s, (char *)&sendbuf, sizeof(sendbuf)) != sizeof(sendbuf)) goto errout;
  close(s);


}
