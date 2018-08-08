#ifndef _MY_EVTYPES_INCLUDED_
#define _MY_EVTYPES_INCLUDED_

#define GO_INFO_EVENT  0x85
#define END_INFO_EVENT 0x82

/*  ROC configuration events will have an event type
 *  like "BASE_ROCCONFIG_EVENT+ROC_Number", so reserve
 *  event types 0x90 through 0xaf for these events
 *  (roc # ranges from 0 to 31).
 */
#define BASE_ROCCONFIG_EVENT 0x90
#define ROCCONFIG_EVENT(roc) (unsigned int)(BASE_ROCCONFIG_EVENT+(roc))





#endif
