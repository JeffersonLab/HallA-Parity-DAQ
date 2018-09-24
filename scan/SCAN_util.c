/*****************************************
*  SCAN.c                                *
*  routines used to interface the        * 
*  SCAN utilities                        *
*                                        *
*  K.D. Paschke                Mar 2004  *
******************************************/

#include "SCAN_util.h"


int getDataSCAN(int which)
{
  if (which==1) {
    return SCAN_DATA1;
  } else if (which==2) {
    return SCAN_DATA2;
  }
  return 0;
}

int getCleanSCAN()
{
  if (SCAN_CLN!=0) return 1;
  return 0;
}

int setDataSCAN(int which, int value)
{
  if (which==1) {
    SCAN_DATA1 = value;
    return which;
  } else if (which==2) {
    SCAN_DATA2 = value;
    return which;
  }
  return 0;
}
 
int setCleanSCAN(int value)
{
  if (value!=0) {
    SCAN_CLN = 1;
  } else {
    SCAN_CLN = 0;
  }
  return 1;
}
 


