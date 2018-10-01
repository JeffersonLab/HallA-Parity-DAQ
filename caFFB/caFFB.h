/************************************************************************
 * caFFB.h
 *    Header file for EPICS Channel Access 
 *
 * June, 2003 B. Moffit
 *
 ***********************************************************************/

// Function Prototypes
int cagetFFB(int);
int caputFFB(int,double);
int cagetFFBverbose(int);
int caputFFBverbose(int,double);
double caget(int);
int caput(int,double);

int cagetFFB_modState();
int cagetFFB_waveState(int);
int cagetFFB_freq(int);
int cagetFFB_amp(int);
int cagetFFB_period(int);
int cagetFFB_load(int);
int cagetFFB_enterTrig(int);
int cagetFFB_leaveTrig(int);
int cagetFFB_trig(int);
int cagetFFB_relay(int);
//int cagetFFB_gen(char *ch);

int caputFFB_modState(int);
int caputFFB_freq(int,int);
int caputFFB_amp(int,int);
int caputFFB_period(int,int);
int caputFFB_load(int,int);
int caputFFB_enterTrig(int,int);
int caputFFB_leaveTrig(int,int);
int caputFFB_trig(int,int);
int caputFFB_relay(int,int);
//int caputFFB_gen(char *ch,int);

char *caFFB_coils(int,char *ch);
char *caFFB_coilsRelay(int);

#define MY_SEVCHK(status)    \
{                            \
  if(status != ECA_NORMAL )  \
    {                        \
      SEVCHK(status,NULL);   \
      return(status);        \
    }                        \
}
