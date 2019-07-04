///////////////////////////////////////////////
//  HAPTB_config.c                           //
//  interface between configuration socket   //
//  server and the HAPPEX timing board       //
//  kdp, sep 2002                            //
///////////////////////////////////////////////

#include "HAPTB_cf_commands.h"

void taskHAPTB_CF(long* command, long *par1, long *par2, long *par3)
{
  int itb = ITB;
  int junk;
  switch (*command)
    {
    case HAPTB_GET_DATA:
      switch (*par1)
	{
        case HAPTB_RD:
	  *par2 = getRampDelayHAPTB(itb);
	  break;
        case HAPTB_IT:
	  *par2 = getIntTimeHAPTB(itb);
	  break;
	case HAPTB_OS:
	  *par2 = getOverSampleHAPTB(itb);
	  break;
	}
      break;
    case HAPTB_SET_DATA:
      switch (*par1)
	{
        case HAPTB_RD:
	  //  int setTimeHAPTB(int delay, int time)
	  junk = getIntTimeHAPTB(itb);
	  *command = setTimeHAPTB(*par2,junk,itb);
	  break;
        case HAPTB_IT:
	  //  int setTimeHAPTB(int delay, int time)
	  junk = getRampDelayHAPTB(itb);
	  *command = setTimeHAPTB(junk,*par2,itb);
	  break;
	case HAPTB_OS:
	  *command = setOverSampleHAPTB(*par2,itb);
	  break;
	}
      break;
    }
}


