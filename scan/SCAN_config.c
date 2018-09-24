///////////////////////////////////////////////
//  SCAN_config.c                            //
//  interface between configuration socket   //
//  server and the SCAN server               //
//  kdp, mar 2004                            //
///////////////////////////////////////////////

#include "SCAN_cf_commands.h"

void taskSCAN_CF(long* command, long *par1, long *par2, long *par3)
{
  int junk;
  switch (*command)
    {
    case SCAN_GET_DATA:
      *par2 = getDataSCAN(*par1);
      break;
    case SCAN_SET_DATA:
      *command = setDataSCAN(*par1, *par2);
      break;
    case SCAN_GET_STATUS:
      *par1 = getCleanSCAN();
      break;
    case SCAN_SET_STATUS:
      *command = setCleanSCAN(*par1);
      break;
    }
}









