#  Build system for scaler readout (independent of CODA)
#  Tested on a RedHat Linux 3WS client.
#
#   scread     -- TCP/IP client to read scalers.  Runs on Linux.
#
#   scaser.o   -- TCP/IP server to run on VME crate.  Needs
#                 cross compiler ccppc and vxWorks headers.


export OSNAME       := $(shell uname)

CC  = $(PURE) cc

AC_FLAGS  = -D_REENTRANT -D_POSIX_PTHREAD_SEMANTICS 

WORK_DIR=/adaqfs/halla/apar/bob/devices/scaler/
VXDIR=/site/vxworks/5.5/ppc/target/h
INCFLAGS = -I$(VXDIR) -I$(WORK_DIR)/SIS3801 

CCVXFLAGS =  $(INCFLAGS) -DCPU_FAMILY=PPC -DCPU=PPC604 -mlongcall

AC_FLAGS += -DMUTEX_NOSHARE
CFLAGS =  -O2 -fPIC $(AC_FLAGS) 
LIBS =  -lieee -lm -lpthread -ldl -lresolv
SWFLAGS = -DLINUX


all : scread scaser.o 

scread: scacli.o  
	$(CC) -o $@ scacli.o $(LIBS)

scacli.o: scacli.c  vxWscaler.h scaler1.h scaler2.h
	rm -f $@
	$(CC) -c $(CFLAGS) $(SWFLAGS) scacli.c 

# Compile server with vxWorks cross compiler (ccppc)
scaser.o: scaser.c  vxWscaler.h scaler1.h scaler2.h
	rm -f $@
	ccppc -c $(CCVXFLAGS) scaser.c

clean : 
	rm -f *.o scread core






