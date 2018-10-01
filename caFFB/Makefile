# version
VERS=1.0

# Uncomment if to profile (warning: this will slow down the code)
# export PROFILE = 1


#VXDIR = /adaqfs/halla/apar/tornado
#VXDIR = /adaqfs/halla/apar/vxworks/5.4/h
VXDIR = $(VXWORKS_INC)
EPICSDIR = /adaqfs/halla/apar/bryan/epics
#CCVXFLAGS =  -I$(VXDIR) -DCPU_FAMILY=PPC -DCPU=PPC604
CCVXFLAGS =  -I$(VXDIR) -I$(EPICSDIR)/base/include

all:  myget.o myput.o

clean:
	rm -f core *.o *~

myget.o : myget.c
	ccppc -o $@ $(CCVXFLAGS) $<

myput.o : myput.c
	ccppc -o $@ $(CCVXFLAGS) $<

