#include <stdlib.h>
#include <stdio.h>

#define USLEEPTIME 10000000

void main() {

  
  char strin[100];
  int dat1, dat2; 
  FILE *fd1;
  
  system("/adaqfs/home/apar/scaler/readout/scread -p 14 > x.dat");
  usleep(USLEEPTIME);
  system("echo ' ' >> x.dat; /adaqfs/home/apar/scaler/readout/scread -p 14  >> x.dat");

  fd1=fopen("/adaqfs/home/apar/scaler/readout/x.dat","r");
  if (fd1==NULL) { 
      printf("cannot open file \n");
      exit(0); 
  }

int i=0;
  while (fgets(strin,100,fd1) != NULL)  {
      printf("data = %s \n",strin);
      if(i==0) sscanf(strin,"%d ",&dat1);
      if(i==1) sscanf(strin,"%d ",&dat2); 
      i++;
  }
  printf("data  %d  %d \n",dat1,dat2);

 printf("rate ! %f \n",1e6*(dat2-dat1)/USLEEPTIME);


}
