# Modifed from CODA 2.2.1 filetoevent
#
CC  = $(PURE) cc
LD  = ld
CODAET_LIB = /adaqfs/home/apar/et-12.0/src/libsrc/.Linux-x86_64/
CODAET_INC = /adaqfs/home/apar/et-12.0/include

AC_FLAGS  = -D_REENTRANT -D_POSIX_PTHREAD_SEMANTICS -DMUTEX_NOSHARE
CFLAGS    = -O2 -fPIC -I$(CODAET_INC) $(AC_FLAGS) -DLINUXVERS

# Use the following flag when using cmlog's messages with dalogmsg
#   AC_FLAGS = ... -DWITH_DALOGMSG

LIBS    = -L$(CODAET_LIB) -lieee -lm -lpthread -lnsl -ldl 
# Use the following terms when using cmlog's messages with dalogmsg
#   LIBS = ... -L$(CMLOGROOT)/lib/solaris -lcmlog

PROGS = 

all: 

clean: 
	rm -f core *~ *.o *.so *.a $(PROGS)

.c:
	$(CC) -o $* $(CFLAGS) $< -L$(CODAET_LIB) -let $(LIBS)


