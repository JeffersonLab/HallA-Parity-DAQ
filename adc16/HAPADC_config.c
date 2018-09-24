///////////////////////////////////////////////
//  HAPADC_config.c                          //
//  interface between configuration socket   //
//  server and the HAPPEX ADC utilities      //
//  kdp, sep 2002                            //
///////////////////////////////////////////////

#include "HAPADC_cf_commands.h"

void taskHAPADC_CF(long* command, long *par1, long *par2)
{
  int junk1;
  int junk2;
  int gainbit;
  int dacbit;
  int ibrd;
  int flag;
  switch (*command)
    {
    case HAPADC_GET_NUMADC:
      *par2 = getNumHAPADC();
      break;
    case HAPADC_GET_LABEL:
      ibrd = *par1;
      *par2 = getLabelHAPADC(ibrd);
      break;
    case HAPADC_GET_CSR:
      ibrd = *par1;
      *par2 = getCSRHAPADC(ibrd);
      break;
    case HAPADC_SET_CSR:
      ibrd = *par1;
      gainbit = *par2 & 0x2;
      if (gainbit==2) flag = 1;  // hi gain
      else flag =0;              // lo gain
      junk1 = setGainBitHAPADC(ibrd,flag);

      dacbit = *par2 & 0x1;
      if (dacbit==1) flag = 1;   // dac on
      else flag =0;              // dac off
      junk2 = setDACBitHAPADC(ibrd,flag);

      if (junk1==-2 || junk2==-2) *command = -2;  // run in progress flag
      if (junk1==-1 || junk2==-1) *command = -1;  // bad adc or other error
      if (junk1== 1 && junk2== 1) *command =  1;  // happy flag
      break;
    default:
      break;
    }
}









