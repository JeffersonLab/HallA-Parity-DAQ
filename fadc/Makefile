#  Build system for SIS3320 FADC
#  June 5, 2007  Switch to vxWorks 5.5

export OSNAME       := $(shell uname)

VXDIR=/adaqfs/halla/apar/vxworks/5.5/5.5
CCVXFLAGS =  -I$(VXDIR) -DCPU_FAMILY=PPC -DCPU=PPC604 -mlongcall


all : sis3320Lib.o  


sis3320Lib.o: sis3320Lib.c sis3320.h
	rm -f $@
	ccppc -c $(CCVXFLAGS) sis3320Lib.c

clean : 
	rm -f sis3320Lib.o






