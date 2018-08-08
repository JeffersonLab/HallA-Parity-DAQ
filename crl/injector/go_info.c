#include "evmacro.h"

#ifndef _MY_EVTYPES_INCLUDED_
#define GO_INFO_EVENT 0x85
#endif

#define TAG_INTERNAL_CONFIG_STRING 0
#define TAG_CONFIG_STRING 1
#define TAG_FILE_CONFIG_STRING 2

#define TYPE_STRING 3

int goflag;

#if 1
void send_go_info_event()
{
  int len,slen;
  unsigned long *sptr;
  unsigned long *lenptr;

#if 0
  lenptr = (rol->dabufp)++;		/* Save where to stuff the length */
  *(rol->dabufp)++ = PHYS_MBANK_HDR (GO_INFO_EVENT, config.ourID); /* Bank of Banks*/
#endif
  UEOPEN(GO_INFO_EVENT,BT_BANK,rol->pid);

  /* First bank is the internal configuration string */
  sptr = (unsigned long *) internal_configusrstr;
  slen = strlen((char *) sptr);
  len = (slen+3)/4;
  *(rol->dabufp)++ = len+1;
  *(rol->dabufp)++ = GENERAL_BANK_HDR(TAG_INTERNAL_CONFIG_STRING, TYPE_STRING, slen);
  while(len--) *(rol->dabufp)++ = *sptr++;

  /* Second bank is the .config file command line */
  sptr = (unsigned long *) rol->usrString;
  slen = strlen((char *) sptr);
  len = (slen+3)/4;
  *(rol->dabufp)++ = len+1;
  *(rol->dabufp)++ = GENERAL_BANK_HDR(TAG_CONFIG_STRING, TYPE_STRING, slen);
  while(len--) *(rol->dabufp)++ = *sptr++;

  /* Third bank is the user file configusrstr */
  sptr = (unsigned long *) file_configusrstr;
  slen = strlen((char *) sptr);
  len = (slen+3)/4;
  *(rol->dabufp)++ = len+1;
  *(rol->dabufp)++ = GENERAL_BANK_HDR(TAG_FILE_CONFIG_STRING, TYPE_STRING, slen);
  while(len--) *(rol->dabufp)++ = *sptr++;

#ifdef _THRESHOLD_INCLUDED
  add_bank_thresholds();
#endif

#ifdef _8LM_INCLUDE
  add_bank_8lm();
#endif

#ifdef _SCALERS_INCLUDE
  add_bank_scalers();
#endif

  UECLOSE;
  /*  *lenptr = (rol->dabufp-lenptr) - 1; /* Fill in event length */
  /**readout = 1;*/
}
#endif

