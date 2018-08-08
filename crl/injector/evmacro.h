/* Macro's to help build events */
#ifndef _EVMACRO_INCLUDED
#define _EVMACRO_INCLUDED

#define GENERAL_SEGMENT_HDR(t,ty,l) (unsigned long)\
((((t)&0xff)<<24)|((l)&0xffff)|(((ty) & 0xff)<<16));
#define GENERAL_BANK_HDR(t,ty,e) (unsigned long)\
((((t)&0xffff)<<16)|((e)&0xff) | (((ty) & 0xff)<<8));

#endif
