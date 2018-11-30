#  Build system for scaler readout of SIS3800 and SIS3801 scalers
#

export OSNAME       := $(shell uname)

VXDIR=/adaqfs/halla/a-onl/vxworks/headers/5.4/h
CCVXFLAGS =  -I$(VXDIR) -DCPU_FAMILY=PPC -DCPU=PPC604


all : scaloop.o


scaloop.o: scaloop.c 
	rm -f $@
	ccppc -c $(CCVXFLAGS) scaloop.c -mlongcall


clean : 
	rm -f *.o






