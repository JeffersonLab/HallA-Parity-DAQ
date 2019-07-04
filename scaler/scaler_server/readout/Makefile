#  Build system for scaler readout 
#
#   scread     -- TCP/IP client on workstation to read scalers
#                 SunOS version for CODA 2.0 data insertion
#                 HP-UX version for xscaler.
#   scaser.o   -- TCP/IP server to run on VME crate; it is compiled
#                 only on SunOS 
#
#  Jan 20, 2000   Major change:  Use the ET system (etInsertEvent)
#                 Scrap the DD system.  
#  Mar 5, 2001    Port to linux.  Scrap the HP-UX version.
#                 
#  Apr 25, 2009   Use the vxWorks 5.5 headers.

export OSNAME       := $(shell uname)

SCALDIR = /adaqfs/halla/adaq/scaler

CC  = $(PURE) cc

#CODA = /adaqfs/coda/2.6.2
CODA_BIN = $(CODA)/$(OSNAME)/bin
CODA_LIB = $(CODA)/$(OSNAME)/lib
CODA_INC = $(CODA)/common/include

AC_FLAGS  = -D_REENTRANT -D_POSIX_PTHREAD_SEMANTICS 
CODAFLAGS = -I$(CODA)/$(OSNAME)/include -I.

ET = /adaqfs/halla/adaq/ET/src
INCFLAGS = -I$(ET) $(CODA_INC)
LIBET = $(CODA_LIB)/libet.so

# VXDIR=/adaqfs/halla/adaq/scaler/tornado
VXDIR=/site/vxworks/5.5/ppc/target/h
CCVXFLAGS =  -I$(VXDIR) -DCPU_FAMILY=PPC -DCPU=PPC604 -mlongcall 

ifeq ($(OSNAME),Linux)
    AC_FLAGS += -DMUTEX_NOSHARE
    CFLAGS =  -O2 -m32 -fPIC $(AC_FLAGS) $(CODAFLAGS) 
    LIBS =  -L$(CODA_LIB) $(LIBET)  -lieee -lm -lpthread -ldl -lresolv
    SWFLAGS = -DLINUX
endif

ifeq ($(OSNAME),SunOS)
   CFLAGS    = -mt -fast -xO5 -KPIC -I$(ET)/src $(AC_FLAGS) $(CODAFLAGS)
   LIBS    = -L$(CODA_LIB) $(LIBET) -lm -lposix4 \
      -lthread -lC -lm -lsocket -lnsl -lresolv -ldl 
   SWFLAGS = -DSUNVERS
endif



all : etInsertEvent.o scread scaser.o


scread: scacli.o  
	$(CC) -m32 -o $@ scacli.o etInsertEvent.o $(LIBS)

scacli.o: scacli.c  vxWscaler.h scaler1.h scaler2.h
	rm -f $@
	$(CC) -c $(CFLAGS) $(SWFLAGS) scacli.c 

etInsertEvent.o: etInsertEvent.c
	rm -f $@
	$(CC) -c $(CFLAGS) $(INCFLAGS) $(SWFLAGS) etInsertEvent.c

scaser.o: scaser.c  vxWscaler.h scaler1.h scaler2.h
	rm -f $@
	ccppc -c $(CCVXFLAGS) scaser.c

clean : 
	rm -f *.o scread buffread badread core






