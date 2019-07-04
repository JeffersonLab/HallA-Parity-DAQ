#  Build system for scaler readout 
#
#   scread     -- TCP/IP client on workstation to read scalers
#                 SunOS version for CODA 2.0 data insertion
#                 HP-UX version for xscaler.
#   scaser.o   -- TCP/IP server to run on VME crate
#


export OSNAME       := $(shell uname)

CC  = $(PURE) cc

CODA = /adaqfs/coda/2.5
CODA_BIN = $(CODA)/$(OSNAME)/bin
CODA_LIB = $(CODA)/$(OSNAME)/lib
CODA_INC = $(CODA)/common/include

AC_FLAGS  = -D_REENTRANT -D_POSIX_PTHREAD_SEMANTICS 
CODAFLAGS = -I$(CODA)/$(OSNAME)/include -I.

# If you had the ET system .... (but not used for Compton)
ET = /adaqfs/halla/a-onl/ET/src
INCFLAGS = -I$(ET) $(CODA_INC)
LIBET = $(CODA_LIB)/libet.so

VXWORKS_GCC=/home/compton/vxworks/5.5/gnu_vxworks/bin/ccppc
WIND_BASE=/home/compton/vxworks/5.5
VXDIR=$(WIND_BASE)/includes
CCVXFLAGS =  -I$(VXDIR) -DCPU_FAMILY=PPC -DCPU=PPC604 -mlongcall


ifeq ($(OSNAME),Linux)
    AC_FLAGS += -DMUTEX_NOSHARE
    CFLAGS =  -O2 -fPIC $(AC_FLAGS) $(CODAFLAGS) 
    LIBS =  -L$(CODA_LIB) -lieee -lm -lpthread -ldl -lresolv
    SWFLAGS = -DLINUX
endif

ifeq ($(OSNAME),SunOS)
   CFLAGS    = -mt -fast -xO5 -KPIC -I$(ET)/src $(AC_FLAGS) $(CODAFLAGS)
   LIBS    = -L$(CODA_LIB) $(LIBET) -lm -lposix4 \
      -lthread -lC -lm -lsocket -lnsl -lresolv -ldl 
   SWFLAGS = -DSUNVERS
endif



all : scread scaser.o

scread: scacli.o
	$(CC) -o $@ scacli.o $(LIBS)

scacli.o: scacli.c  vxWscaler.h
	rm -f $@
	$(CC) -c $(CFLAGS) $(SWFLAGS) scacli.c

scaser.o: scaser.c  vxWscaler.h
	rm -f $@
	$(VXWORKS_GCC) -c $(CCVXFLAGS) scaser.c

clean :
	rm -f *.o scread






