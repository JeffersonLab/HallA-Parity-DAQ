#  Build system for global memory for scalers.
#

export OSNAME       := $(shell uname)

VXDIR=/adaqfs/halla/a-onl/vxworks/headers/5.4/h
CCVXFLAGS =  -I$(VXDIR) -DCPU_FAMILY=PPC -DCPU=PPC604


all : GlobSca.o


GlobSca.o: GlobSca.c GlobSca.h SIS.h
	rm -f $@
	ccppc -c $(CCVXFLAGS) GlobSca.c


clean : 
	rm -f *.o






