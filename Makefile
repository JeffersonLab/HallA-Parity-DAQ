# version
VERS=1.0

# Uncomment if to profile (warning: this will slow down the code)
# export PROFILE = 1

# Use vxWorks 5.5 for 6100 boards
export USE55=1

INCLUDES   = -I$(ROOTSYS)/include
#
CXXFLAGS   = -O -Wall -fno-exceptions -fPIC $(INCLUDES) 

VQWKTARGET=powerpc-wrs-vxworks
VQWKCC=$(VQWKTARGET)-gcc

CC = gcc
SWFLAGS = -DLINUX
ifdef USE55
  VXDIR = /adaqfs/home/adaq/vxworks/headers/5.5/h
else
  VXDIR = /adaqfs/halla/a-onl/vxworks/headers/5.4/h
endif
CCVXFLAGS =  -I$(VXDIR) -DCPU_FAMILY=PPC -DCPU=PPC604
ifdef USE55
  CCVXFLAGS += -c -mlongcall
endif

# We dont use or need 16-bit ADC but we'll need this for a while until we remove
# some symbol dependencies
# ADC16 16-bit (while ADC18 18-bit) HAPPEX ADC
SOCK =  cfSockSer.o cfSockCli.o

socket: $(SOCK)
vxall: $(SOCK)
all: $(SOCK)

version: clean
	mkdir $(VERS) 
	cp *.C ./$(VERS) 
	cp *.h ./$(VERS) 
	cp Makefile ./$(VERS) 
	cp README ./$(VERS)

clean:
	rm -f *.o *.d

realclean:  clean
	rm -f *.d

# "GM" means Green Monster
GMSRC = GreenMonster.C GreenMonsterDict.C GreenTB.C \
      GreenADC.C GreenADC18.C GreenVQWK.C
GMHEAD = $(GMSRC:.C=.h) GreenSock.h GMSock.h
GMDEPS = $(GMSRC:.C=.d)
GMOBJS = $(GMSRC:.C=.o) GM/cfSock/cfSockCli.o

GM/cfSock/cfSockCli.o : GM/cfSock/cfSockCli.c GM/cfSock/cfSock.h GM/cfSock/cfSock_types.h
	rm -f $@
	$(CC) -o $@ -c $(CFLAGS) $(SWFLAGS) $(CXXFLAGS) GM/cfSock/cfSockCli.c 

GM/cfSock/cfSockSer.o: GM/cfSock/cfSockSer.c GM/cfSock/cfSock.h  GM/cfSock/cfSock_types.h \
        GM/cfSock/cfSockCli.h GM/cfSock/GreenSock.h
	rm -f $@
	ccppc -o $@ -c $(CCVXFLAGS) $(CXXFLAGS) GM/cfSock/cfSockSer.c












