#
# Description:  Makefile for v775Lib.o
#   This driver is specific to VxWorks BSPs and must be compiled
#   with access to vxWorks headers.
#


ARCH = VXWORKSPPC

INCDIR=/adaqfs/halla/a-onl/vxworks/headers/5.5/h
CC = ccppc
LD = ldppc
DEFS = -mcpu=604 -DCPU=PPC604 -DVXWORKS -D_GNU_TOOL -DVXWORKSPPC
INCS = -Wall -fno-for-scope -fno-builtin -fvolatile -fstrength-reduce -mlongcall -I. -I$(INCDIR)

CFLAGS = -O $(DEFS)


all: v767Lib.o

v767Lib.o: caen767Lib.c v767Lib.h
	$(CC) -c $(CFLAGS) $(INCS) -o $@ caen767Lib.c

clean:
	rm -f *.o

