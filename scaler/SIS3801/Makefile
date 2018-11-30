#  Build system for scaler readout of SIS3800 and SIS3801 scalers
#  This version is for vxWorks 5.5 (necessary for 6100 boards).

export OSNAME       := $(shell uname)

VXDIR=/site/vxworks/5.5/ppc/target/h
CCVXFLAGS =  -I$(VXDIR) -DCPU_FAMILY=PPC -DCPU=PPC604 -mlongcall


all : SIS3801.o SIS3800.o  


SIS3800.o: SIS3800.c SIS3800.h
	rm -f $@
	ccppc -c $(CCVXFLAGS) SIS3800.c


SIS3801.o: SIS3801.c SIS3801.h SIS.h
	rm -f $@
	ccppc -c $(CCVXFLAGS) SIS3801.c

clean : 
	rm -f *.o






