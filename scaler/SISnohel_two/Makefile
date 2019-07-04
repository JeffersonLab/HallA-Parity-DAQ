#  Build system for scaler readout of 3801 with no helicity sorting
#


export OSNAME       := $(shell uname)

VXDIR=/site/vxworks/5.5/ppc/target/h
# VXDIR=/adaqfs/halla/adaq/scaler/tornado
CCVXFLAGS =  -I$(VXDIR) -DCPU_FAMILY=PPC -DCPU=PPC604 -mlongcall


all : SIS3801.o 


SIS3801.o: SIS3801.c SIS3801.h SIS.h
	rm -f $@
	ccppc -c $(CCVXFLAGS) SIS3801.c


clean : 
	rm -f *.o






