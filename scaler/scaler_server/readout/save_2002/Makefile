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
#                 

export OSNAME       := $(shell uname)

SCALDIR = /home/adev/scaler

ifeq ($(OSNAME),HP-UX)
    CFLAGS = -I$(CODA)/HP_UX/include -I.
    LIBROOT = $(CODA)/HP_UX/lib
    LIBS =   $(LIBROOT)/libcopy.a $(LIBROOT)/libmsg.a
    INCFLAGS =
    SWFLAGS = -DHPVERS 
    CC = cc -Aa -D_HPUX_SOURCE
endif

ifeq ($(OSNAME),SunOS)

   CC  = $(PURE) cc

   RANLIB = ranlib
   CODA = /adaqfs/coda/2.2
   CODA_BIN = $(CODA)/SunOS/bin
   CODA_LIB = $(CODA)/SunOS/lib
   CODA_INC = $(CODA)/common/include

   AC_FLAGS  = -D_REENTRANT -D_POSIX_PTHREAD_SEMANTICS 
   CODAFLAGS = -I$(CODA)/SunOS/include -I.

   ET = /home/adev/ET
   CFLAGS    = -mt -fast -xO5 -KPIC -I$(ET)/src $(AC_FLAGS) $(CODAFLAGS)

   LIBNAM      = libet
   LIB_DYNAMIC = $(CODA_LIB)/$(LIBNAM).so
   LIBS    = -L$(CODA_LIB) $(LIB_DYNAMIC) -lm -lposix4 \
    -lthread -lC -lm -lsocket -lnsl -lresolv -ldl 

   SWFLAGS = -DSUNVERS
   VXDIR=/home/adev/scaler/tornado
   CCVXFLAGS =  -I$(VXDIR) -DCPU_FAMILY=PPC -DCPU=PPC604

endif

ifeq ($(OSNAME),Linux)
  CC = gcc
  SWFLAGS = -DLINUX
  LIBS = -lieee -ldl -lresolv
  VXDIR=/adaqfs/halla/apar/tornado
  CCVXFLAGS =  -I$(VXDIR) -DCPU_FAMILY=PPC -DCPU=PPC604
endif

ifeq ($(OSNAME),HP-UX)
    all : scread 
endif
ifeq ($(OSNAME),SunOS)
    all : etInsertEvent.o scread scaser.o 
endif
ifeq ($(OSNAME),Linux)
    all : scread scaser.o 
endif


ifeq ($(OSNAME),HP-UX)
scread: scacli.o  
	$(CC) -o $@ scacli.o  $(LIBS)

endif

ifeq ($(OSNAME),SunOS) 
scread: scacli.o  
	$(CC) -o $@ scacli.o etInsertEvent.o $(LIBS)
endif

ifeq ($(OSNAME),Linux) 
scread: scacli.o  
	$(CC) -o $@ scacli.o $(LIBS)
endif

scacli.o: scacli.c  vxWscaler.h scaler1.h scaler2.h
	rm -f $@
	$(CC) -c $(CFLAGS) $(SWFLAGS) scacli.c 

ifeq ($(OSNAME),SunOS)
etInsertEvent.o: etInsertEvent.c
	rm -f $@
	$(CC) -c $(CFLAGS) $(SWFLAGS) etInsertEvent.c
endif


scaser.o: scaser.c  vxWscaler.h scaler1.h scaler2.h
	rm -f $@
	ccppc -c $(CCVXFLAGS) scaser.c

clean : 
	rm -f *.o scread core






