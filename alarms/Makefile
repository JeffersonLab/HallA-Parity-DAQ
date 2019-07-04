# TCP/IP client/server

export OSNAME       := $(shell uname)

CC  = cc

AC_FLAGS  = -D_REENTRANT -D_POSIX_PTHREAD_SEMANTICS 

ifeq ($(OSNAME),Linux)
    AC_FLAGS += -DMUTEX_NOSHARE
    CFLAGS =  -O2 -fPIC $(AC_FLAGS) $(CODAFLAGS) 
    LIBS =  -lieee -lm -lpthread -ldl -lresolv
    SWFLAGS = -DLINUX
endif

ifeq ($(OSNAME),SunOS)
   CFLAGS    = -mt -fast -xO5 -KPIC -I$(ET)/src $(AC_FLAGS) 
   LIBS    = -lm -lposix4 \
      -lthread -lC -lm -lsocket -lnsl -lresolv -ldl 
   SWFLAGS = -DSUNVERS
endif

all: bclient bserver

bclient: bclient.o 
	rm -f $@
	$(CC) -o $@ bclient.o $(LIBS)

bserver: bserver.o 
	rm -f $@
	$(CC) -o $@ bserver.o $(LIBS)

clean:
	rm *.o bclient bserver

