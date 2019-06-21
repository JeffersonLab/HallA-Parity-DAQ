# Makefile for 2-ET system
# R. Michaels, rom@jlab.org, Sept 18, 2018
#
# Test Executibles:
# etclient --  test of ET connection for online data.
#
# All the root stuff could be discarded (with a little surgery
# on the sources), but I leave it in the Makefile, which was 
# taken from another project.
#
# Note regarding software from DAQ group
# evio.h, evio.C, swap_util.C, et.h are taken from DAQ group intact
# from 2001  (only a few tiny mod's by me).  
# Hmmm .... I suppose we should use a more modern EVIO.
# For this reason I did not rename those modules with prefix "THa". 
#
# You need an environment variable to define ROOT. E.g.
#   ROOTSYS =/apps/root/5.34.36/root
# User must have LD_LIBRARY_PATH = $ROOTSYS/lib:$LD_LIBRARY_PATH
#
# Use this if compiling online code (ET system)
# User must have LD_LIBRARY_PATH = $CODA/$OSNAME/lib:$LD_LIBRARY_PATH

ROOTCFLAGS   := $(shell root-config --cflags)
ROOTLIBS     := -pthread -lm -ldl -rdynamic #$(shell root-config --libs)
ROOTGLIBS    := -pthread -lm -ldl -rdynamic #$(shell root-config --glibs)
ROOTBIN      := $(shell root-config --bindir)

export OSNAME := $(shell uname)

ifeq ($(OSNAME),SunOS)
# Note, this does not actually work on SunOS anymore, but I leave
# this here as a template.  Maybe someone can get it to compile
# with scons ?

   ROOTCFLAGS    = $(shell root-config --cflags)
   ROOTLIBS      = $(shell root-config --libs)
   ROOTGLIBS     = $(shell root-config --glibs)
   CXX           = CC
#   CXXFLAGS      = -O  -DSUNVERS -I$(ROOTINC)
   CFLAGS        = -O  -DSUNVERS 
   CXXFLAGS      = -O  -DSUNVERS 
   LD            = CC
   LDFLAGS       = -g -D
   SOFLAGS       = -G
   CFLAGS       += $(ROOTCFLAGS)
   CXXFLAGS     += $(ROOTCFLAGS)
   EVIO_LIB=libevio.a
   GLIB =  -lm -lc -lgen -lw -lsocket -lnsl -ldl
   SLIB = -L/opt/SUNWspro/SC4.2/lib -lF77 -lM77 -lsunmath
# all this needed for ET...  up to comment below
   ET_AC_FLAGS = -D_REENTRANT -D_POSIX_THREAD_SEMANTICS
   ET_CFLAGS = -mt -fast -xO5  $(ET_AC_FLAGS) -DSUNVERS
   CODA = /adaqfs/halla/apar/coda/2.2.1
   LIBET = $(CODA)/SunOS/lib/libet.so
   ONLIBS = $(LIBET) -lposix4 -lnsl -lsocket -lresolv
   LIBS = $(EVIO_LIB) $(GLIB)
   ifdef ONLINE
     ALL_LIBS = $(LIBS) $(ONLIBS) 
# ... finished, what ET needs.
   else
     ALL_LIBS = $(LIBS) 
   endif
   ALL_LIBS += $(ROOTLIBS) $(ROOTGLIBS)

endif


ifeq ($(OSNAME),Linux)
#
#   ifdef OLDROOT
#     ROOTLIBS      = -L$(ROOTLIBDIR) -lNew -lBase -lCint -lClib -lCont -lFunc \
#                    -lGraf -lGraf3d -lHist -lHtml -lMatrix -lMeta -lMinuit -lNet \
#                    -lPhysics -lPostscript -lProof -lRint -lTree -lUnix -lZip
#     ROOTGLIBS     = -lGpad -lGui -lGX11 -lX3d
#   else
#     ROOTLIBS      = $(shell root-config --libs)
#     ROOTGLIBS     = $(shell root-config --glibs)
#   endif

   CXX           = g++
   CFLAGS        = -Wall -fno-exceptions -fPIC \
                   -DLINUXVERS -O $(ROOTCFLAGS)
   CXXFLAGS      = -Wall -fno-rtti -fno-exceptions -fPIC \
                   -DLINUXVERS -O $(ROOTCFLAGS)

# Linux with egcs
   CXX           = g++

# attempt to use -m32
#   ROOTCFLAGS=-pthread -m32 -I/adaqfs/apps/ROOT/pro/include

   CFLAGS        = -O -Wall -fno-exceptions -fPIC  $(ROOTCFLAGS)
   CXXFLAGS      = -O -Wall -fno-rtti -fno-exceptions -fPIC  $(ROOTCFLAGS)
   LD            = g++
   LDFLAGS       = 
   SOFLAGS       = -shared

   LIBS          = $(ROOTLIBS)
   GLIBS         = $(ROOTGLIBS) -L/usr/X11R6/lib -lXpm -lX11

   EVIO_LIB=libevio.a
   ALL_LIBS = $(EVIO_LIB) $(GLIBS) $(ROOTLIBS) 

  ET_AC_FLAGS = -D_REENTRANT -D_POSIX_PTHREAD_SEMANTICS
  ET_CFLAGS = -02 -fPIC $(ET_AC_FLAGS) -DLINUXVERS

# whatever LIB you use, make sure to adjust LD_LIBRARY_PATH
# also need the et.h

# These are sources used by CODA 2.6.2.

LIBETDIR = /adaqfs/home/apar/et-12.0/src/libsrc/.Linux-x86_64/

LIBET = $(LIBETDIR)/libet.so
ONLIBS = $(LIBET) -lieee -lpthread -ldl -lresolv

ALL_LIBS += $(ONLIBS)

endif

ifeq ($(OSNAME),Darwin)

   ifdef OLDROOT
     ROOTLIBS      = -L$(ROOTLIBDIR) -lNew -lBase -lCint -lClib -lCont -lFunc \
                    -lGraf -lGraf3d -lHist -lHtml -lMatrix -lMeta -lMinuit -lNet \
                    -lPhysics -lPostscript -lProof -lRint -lTree -lUnix -lZip
     ROOTGLIBS     = -lGpad -lGui -lGX11 -lX3d
   else
     ROOTLIBS      = $(shell root-config --libs)
     ROOTGLIBS     = $(shell root-config --glibs)
   endif

   CXX           = g++
   CFLAGS        = -Wall -fno-exceptions -fPIC \
                   -DLINUXVERS -O $(ROOTCFLAGS)
   CXXFLAGS      = -Wall -fno-rtti -fno-exceptions -fPIC \
                   -DLINUXVERS -O $(ROOTCFLAGS)

# Linux with egcs
   CXX           = g++
   CFLAGS        = -O -Wall -fno-exceptions -fPIC $(ROOTCFLAGS)
   CXXFLAGS      = -O -Wall -fno-rtti -fno-exceptions -fPIC $(ROOTCFLAGS)
   LD            = g++
   LDFLAGS       = 
   SOFLAGS       = -shared

   ifdef OLDROOT
      LIBS          = $(ROOTLIBS) -lm -ldl -rdynamic
      GLIBS         = $(ROOTLIBS) $(ROOTGLIBS) -L/usr/X11R6/lib \
                      -lXpm -lX11 -lm -ldl -rdynamic
      CFLAGS       += -DOLDROOT
      CXXFLAGS     += -DOLDROOT
   else
      LIBS          = $(ROOTLIBS)
      GLIBS         = $(ROOTGLIBS) -L/usr/X11R6/lib -lXpm -lX11
   endif

   EVIO_LIB=libevio.a
   ALL_LIBS = $(EVIO_LIB) $(GLIBS) $(ROOTLIBS) 

# ONLIBS is needed for ET
  ET_AC_FLAGS = -D_REENTRANT -D_POSIX_PTHREAD_SEMANTICS
  ET_CFLAGS = -02 -fPIC $(ET_AC_FLAGS) -DLINUXVERS
# CODA may be an environment variable.  Typical examples
#  CODA = /adaqfs/coda/2.2
#  CODA = /data7/user/coda/2.2
  LIBET = $(CODA)/Linux/lib/libet.so
  ONLIBS = $(LIBET) -lieee -lpthread -ldl -lresolv

  ifdef ONLINE
     ALL_LIBS += $(ONLIBS)
  endif

  ifdef PROFILE
     CFLAGS += -pg
     CXXFLAG+= -pg
  endif

endif

HEAD = $(SRC:.C=.h)
DEPS = $(SRC:.C=.d)
DECODE_OBJS = $(SRC:.C=.o)

#all: etclient etHeartbeat tstio tstcoda libevio.a libcoda.a
# all: libcoda.a
all: etbridge

# "make" will make the library,  "make etbridge" to make the bridge program.

etbridge: et32_2_et64.o
	$(CXX) $(CXXFLAGS) -o $@ et32_2_et64.o $(ALL_LIBS)

# Here we build a library with all this stuff
libcoda.a: $(DECODE_OBJS) clean_evio evio.o swap_util.o swapped_intcpy.o
	rm -f $@
	ar cr $@ $(DECODE_OBJS) evio.o swap_util.o swapped_intcpy.o

# Below is the evio library, which comes rather directly 
# from CODA group with minor tweaking by R. Michaels & O. Hansen.

libevio.a: clean_evio evio.o swap_util.o swapped_intcpy.o
	rm -f $@
	ar cr $@ evio.o swap_util.o swapped_intcpy.o

evio.o: evio.C
	$(CXX) $(CXXFLAGS) -c  $<

swap_util.o: swap_util.C
	$(CXX) $(CXXFLAGS) -c  $<

swapped_intcpy.o: swapped_intcpy.c
	$(CC) $(CFLAGS) -c  $<

clean:  clean_evio
	rm -f *.o *.a core *~ *.d *.out *.tar etclient tstcoda tstio

realclean:  clean
	rm -f *.d

clean_evio:
	rm -f evio.o swap_util.o swapped_intcpy.o


.SUFFIXES:
.SUFFIXES: .c .cc .cpp .C .o .d

%.o:	%.C 
	$(CXX) $(CXXFLAGS) -c $<

%.d:	%.C
	@echo Creating dependencies for $<
	@$(SHELL) -ec '$(CXX) -MM $(CXXFLAGS) -c $< \
		| sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
		[ -s $@ ] || rm -f $@'

-include $(DEPS)
