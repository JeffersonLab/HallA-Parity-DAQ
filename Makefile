# version
VERS=1.0

# Uncomment if to profile (warning: this will slow down the code)
# export PROFILE = 1

# Use vxWorks 5.5 for 6100 boards
export USE55=1

ROOTLIBS   = $(shell root-config --libs --new)
ROOTGLIBS  = $(shell root-config --glibs --new)
INCLUDES   = -I$(ROOTSYS)/include
#
EPICS_INC  = -I/adaqfs/halla/apar/bryan/epics/base/include
CXX        = g++
CXXFLAGS   = -O -Wall -fno-exceptions -fPIC $(INCLUDES) 
LD         = g++
LDFLAGS    = 
SOFLAGS    = -shared
# LIBS       = $(ROOTLIBS)
GLIBS      = $(ROOTGLIBS) -L/usr/X11R6/lib -lXpm -lX11
#LIBDC      = libdc.a
LIBET = $(CODA)/Linux/lib/libet.so
ONLIBS = $(LIBET) -lieee -lpthread -ldl -lresolv
ALL_LIBS   = $(GLIBS) $(ROOTLIBS)

VQWKTARGET=powerpc-wrs-vxworks
VQWKCC=$(VQWKTARGET)-gcc

CC = gcc
SWFLAGS = -DLINUX
LIBS = -lieee -ldl -lresolv
ifdef USE55
  VXDIR = /adaqfs/halla/a-onl/vxworks/headers/5.5/h
else
  VXDIR = /adaqfs/halla/a-onl/vxworks/headers/5.4/h
endif
CCVXFLAGS =  -I$(VXDIR) -DCPU_FAMILY=PPC -DCPU=PPC604
ifdef USE55
  CCVXFLAGS += -c -mlongcall
endif

ifdef PROFILE
  CXXFLAGS += -pg
  LDFLAGS += -pg
endif

# ADC16 16-bit (while ADC18 18-bit) HAPPEX ADC
ADC16 = adc16/HAPADC_ch.o adc16/HAPADC_inj.o adc16/HAPADC_lspec.o \
	adc16/HAPADC_rspec.o  adc16/HAPADC_test.o  adc16/HAPADC_config.o
ADC18 = adc18/hapAdc18Test.o adc18/hapAdc18Count.o \
	adc18/hapAdc18Left.o adc18/hapAdc18Right.o \
	adc18/hapAdc18Inj.o  adc18/hapAdc18UVa.o
BMW = bmw/bmwClient.o bmw/bmw_config.o  bmw/bmwDummy.o
TB = timebrd/HAPTB_util.o timebrd/HAPTB_config.o
AUXTB = auxtimebrd/AUXTB_util.o
SOCK =  cfSock/cfSockSer.o cfSock/cfSockCli.o
SCAN = scan/SCAN_util.o scan/SCAN_config.o
CAFFB = caFFB/caFFB.o
AUTO = auto/auto_rhwp.o auto/auto_filter.o auto/auto_led.o auto/auto_led_left.o auto/auto_PITA.o
FLEXIO = flexio/flexio_lib_ch.o flexio/flexio_lib_rspec.o
FLEXIO = flexio/flexioLib.o
CAFFB = caFFB/caFFB.o
VQWK = qweak/vqwk.o qweak/vqwk_32.o qweak/vqwk_32_longcall.o qweak/vqwk_config.o
GMPROG = config/config
SIS3801 = sis3801IntLib/scalIntLib.o sis3801IntLib/scalMsgLib.o
STR7200 = STR7200/STR7200.o
TEMPEDMA = tempeDma/usrTempeDma.o

adc16: $(ADC16)
adc18: $(ADC18)
timebrd: $(TB)
auxtimebrd: $(AUXTB)
bmw: $(BMW)
socket: $(SOCK)
scan: $(SCAN)
caFFB: $(CAFFB)
auto: $(AUTO)
flexio: $(FLEXIO)
vqwk: $(VQWK)
vxall: $(ADC16) $(ADC18) $(BMW) $(SOCK) $(TB)  $(SCAN) $(CAFFB) $(AUXTB) $(AUTO) $(FLEXIO) $(VQWK) $(SIS3801) $(STR7200) $(TEMPEDMA)
all:  $(ADC16) $(ADC18) $(BMW) $(SOCK) $(TB) $(SCAN) $(CAFFB) $(GMPROG)  $(AUXTB) $(AUTO) $(FLEXIO) $(VQWK) $(SIS3801) $(STR7200) $(TEMPEDMA)

version: clean
	mkdir $(VERS) 
	cp *.C ./$(VERS) 
	cp *.h ./$(VERS) 
	cp Makefile ./$(VERS) 
	cp README ./$(VERS)

clean:
	rm -f *.o core *~ *.d *.out $(GMPROG) *Dict* *.so
	rm -f core *.o
	rm -f config/core config/*.o config/*Dict* config/*.d config/*.so
	rm -f adc16/core adc16/*.o adc16/*.d
	rm -f adc18/*.o adc18/*.d
	rm -f bmw/core bmw/*.o bmw/*.d
	rm -f cfSock/core cfSock/*.o cfSock/*.d
	rm -f timebrd/core timebrd/*.o timebrd/*.d
	rm -f auxtimebrd/core auxtimebrd/*.o auxtimebrd/*.d
	rm -f scan/core scan/*.o scan/*.d
	rm -f caFFB/core caFFB/*.o caFFB/*.d
	rm -f caFFB/core caFFB/*.o caFFB/*.d
	rm -f flexio/flexioLib.o
	rm -f qweak/*.o
	rm -f auto/auto_rhwp.o auto/auto_filter.o auto/auto_led.o auto/auto_led_left.o auto/auto_PITA.o
	rm -f sis3801IntLib/scalIntLib.o sis3801IntLib/scalMsgLib.o
	rm -f STR7200/STR7200.o
	rm -f tempeDma/usrTempeDma.o

realclean:  clean
	rm -f *.d

# "GM" means Green Monster
GMSRC = config/GreenMonster.C config/GreenMonsterDict.C config/GreenTB.C \
      config/GreenADC.C config/GreenADC18.C config/GreenVQWK.C
GMHEAD = $(GMSRC:.C=.h) cfSock/GreenSock.h config/GMSock.h
GMDEPS = $(GMSRC:.C=.d)
GMOBJS = $(GMSRC:.C=.o) cfSock/cfSockCli.o

config/libGreenMonster.so: $(GMOBJS) $(GMHEAD)
	$(CXX) -shared -O -o config/libGreenMonster.so $(GMOBJS) $(ALL_LIBS)

# This is the GreenMonster executible.  "greenmonster" is a link to /config/config.
config/config: config/config.o $(GMOBJS) $(GMSRC) $(GMHEAD)
	rm -f $@
	$(CXX) $(CXXFLAGS) -o $@ config/config.o $(GMOBJS) $(ALL_LIBS) 

adc16/HAPADC_ch.o : adc16/HAPADC.c adc16/HAPADC.h
	rm -f $@
	ccppc -o $@ $(CCVXFLAGS) -DCOUNTINGHOUSE adc16/HAPADC.c

adc16/HAPADC_inj.o : adc16/HAPADC.c adc16/HAPADC.h
	rm -f $@
	ccppc -o $@ $(CCVXFLAGS) -DINJECTOR adc16/HAPADC.c

adc16/HAPADC_lspec.o : adc16/HAPADC.c adc16/HAPADC.h
	rm -f $@
	ccppc -o $@ $(CCVXFLAGS) -DLEFTSPECT adc16/HAPADC.c

adc16/HAPADC_rspec.o : adc16/HAPADC.c adc16/HAPADC.h
	rm -f $@
	ccppc -o $@ $(CCVXFLAGS) -DRIGHTSPECT adc16/HAPADC.c

adc16/HAPADC_test.o: adc16/HAPADC.c adc16/HAPADC.h
	rm -f $@
	ccppc -o $@ $(CCVXFLAGS) -DTESTCRATE adc16/HAPADC.c

adc16/HAPADC_config.o : adc16/HAPADC_config.c adc16/HAPADC_cf_commands.h
	rm -f $@ 
	ccppc -o $@ $(CCVXFLAGS) adc16/HAPADC_config.c ; echo "Should `make all` .... all done"

adc18/hapAdc18Test.o: adc18/hapAdc18Lib.c adc18/hapAdc18Lib.h
	rm -f $@
	ccppc -o $@ $(CCVXFLAGS) -DTESTCRATE adc18/hapAdc18Lib.c

adc18/hapAdc18Count.o: adc18/hapAdc18Lib.c adc18/hapAdc18Lib.h
	rm -f $@
	ccppc -o $@ $(CCVXFLAGS) -DCOUNTINGHOUSE adc18/hapAdc18Lib.c

adc18/hapAdc18Left.o: adc18/hapAdc18Lib.c adc18/hapAdc18Lib.h
	rm -f $@
	ccppc -o $@ $(CCVXFLAGS) -DLEFTSPECT adc18/hapAdc18Lib.c

adc18/hapAdc18Right.o: adc18/hapAdc18Lib.c adc18/hapAdc18Lib.h
	rm -f $@
	ccppc -o $@ $(CCVXFLAGS) -DRIGHTSPECT adc18/hapAdc18Lib.c

adc18/hapAdc18UVa.o: adc18/hapAdc18Lib.c adc18/hapAdc18Lib.h
	rm -f $@
	ccppc -o $@ $(CCVXFLAGS) -DCPU_FAMILY=PPC -DCPU=PPC604 -DUVACRATE adc18/hapAdc18Lib.c

adc18/hapAdc18Inj.o: adc18/hapAdc18Lib.c adc18/hapAdc18Lib.h
	rm -f $@
	ccppc -o $@ $(CCVXFLAGS) -DINJECTOR adc18/hapAdc18Lib.c

bmw/bmwClient.o : bmw/bmwClient.c bmw/bmw.h flexio/flexioLib.h caFFB/caFFB.h
	rm -f $@
	ccppc -o $@ -c $(CCVXFLAGS) bmw/bmwClient.c

bmw/bmwDummy.o : bmw/bmwDummy.c bmw/bmw.h
	rm -f $@
	ccppc -o $@ -c $(CCVXFLAGS) bmw/bmwDummy.c

bmw/bmw_config.o : bmw/bmw_config.c bmw/bmw_config.h bmw/bmw_cf_commands.h
	rm -f $@
	ccppc -o $@ -c $(CCVXFLAGS) bmw/bmw_config.c

flexio/flexioLib.o : flexio/flexioLib.c flexio/flexioLib.h
	rm -f $@
	ccppc -o $@ $(CCVXFLAGS) flexio/flexioLib.c

cfSock/cfSockCli.o : cfSock/cfSockCli.c cfSock/cfSock.h cfSock/cfSock_types.h
	rm -f $@
	$(CC) -o $@ -c $(CFLAGS) $(SWFLAGS) cfSock/cfSockCli.c 

cfSock/cfSockSer.o: cfSock/cfSockSer.c cfSock/cfSock.h  cfSock/cfSock_types.h \
        cfSock/cfSockCli.h cfSock/GreenSock.h
	rm -f $@
	ccppc -o $@ -c $(CCVXFLAGS) cfSock/cfSockSer.c

timebrd/HAPTB_util.o: timebrd/HAPTB_util.c timebrd/HAPTB.h    \
              timebrd/HAPTB_util.h
	cd timebrd; rm -f $@; \
	ccppc  -c $(CCVXFLAGS) HAPTB_util.c

timebrd/HAPTB_config.o: timebrd/HAPTB_config.c timebrd/HAPTB_cf_commands.h
	cd timebrd; rm -f $@; \
	ccppc  -c $(CCVXFLAGS) HAPTB_config.c

scan/SCAN_util.o: scan/SCAN_util.c  scan/SCAN_util.h
	cd scan; rm -f $@; \
	ccppc  -c $(CCVXFLAGS) SCAN_util.c

scan/SCAN_config.o: scan/SCAN_config.c scan/SCAN_cf_commands.h
	cd scan; rm -f $@; \
	ccppc  -c $(CCVXFLAGS) SCAN_config.c

qweak/vqwk.o: qweak/vqwk.c qweak/vqwk.h
	rm -f $@ 
	ccppc  -o $@ $(CCVXFLAGS)  qweak/vqwk.c

qweak/vqwk_longcall.o: qweak/vqwk.c qweak/vqwk.h
	rm -f $@
	ccppc  -mlongcall -o $@ $(CCVXFLAGS) qweak/vqwk.c

qweak/vqwk_32.o: qweak/vqwk_32.c qweak/vqwk_32.h
	rm -f $@
	ccppc  -o $@ $(CCVXFLAGS) qweak/vqwk_32.c

qweak/vqwk_32_longcall.o: qweak/vqwk_32.c qweak/vqwk_32.h
	rm -f $@
	ccppc  -mlongcall -o $@ $(CCVXFLAGS) qweak/vqwk_32.c

qweak/vqwk_config.o: qweak/vqwk_config.c qweak/VQWK_cf_commands.h
	rm -f $@; \
	ccppc  -o $@ -c $(CCVXFLAGS) qweak/vqwk_config.c 

caFFB/caFFB.o: caFFB/caFFB.c caFFB/caFFB.h
	ccppc  -c $(CCVXFLAGS) $(EPICS_INC) $< -o $@

auxtimebrd/AUXTB_util.o: auxtimebrd/AUXTB_util.c auxtimebrd/AUXTB.h   
	cd auxtimebrd; rm -f $@; \
	ccppc  -c $(CCVXFLAGS) AUXTB_util.c

auto/auto_rhwp.o: auto/auto_rhwp.c
	cd auto; rm -f $@; \
	ccppc -c $(CCVXFLAGS) auto_rhwp.c

auto/auto_PITA.o: auto/auto_PITA.c
	cd auto; rm -f $@; \
	ccppc -c $(CCVXFLAGS) auto_PITA.c

auto/auto_led.o: auto/auto_led.c auto/auto_led.h
	cd auto; rm -f $@; \
	ccppc -c $(CCVXFLAGS) auto_led.c

auto/auto_led_left.o: auto/auto_led_left.c auto/auto_led.h
	cd auto; rm -f $@; \
	ccppc -c $(CCVXFLAGS) auto_led_left.c

auto/auto_filter.o: auto/auto_filter.c
	cd auto; rm -f $@; \
	ccppc -c $(CCVXFLAGS) auto_filter.c

sis3801IntLib/scalIntLib.o: sis3801IntLib/scalIntLib.c sis3801IntLib/scalIntLib.h
	rm -f $@
	ccppc -c $(CCVXFLAGS) $(INCS) -o $@ sis3801IntLib/scalIntLib.c

sis3801IntLib/scalMsgLib.o: sis3801IntLib/scalMsgLib.c
	rm -f $@
	ccppc -c $(CCVXFLAGS) $(INCS) -o $@ sis3801IntLib/scalMsgLib.c

STR7200/STR7200.o: STR7200/STR7200.c STR7200/STR7200.h
	rm -f $@
	ccppc -c $(CCVXFLAGS) $(INCS) -o $@ STR7200/STR7200.c

tempeDma/usrTempeDma.o: tempeDma/usrTempeDma.c tempeDma/tempe.h tempeDma/sysTempeDma.c
	rm -f $@
	ccppc -c $(CCVXFLAGS) $(INCS) -o $@ tempeDma/usrTempeDma.c


#.SUFFIXES:
#.SUFFIXES: .c .cc .cpp .C .o .d

config/GreenMonsterDict.C: config/GreenMonster.h config/GreenMonsterLinkDef.h
	@echo "Generating dictionary $@..."
	cd config; \
	rootcint -f GreenMonsterDict.C -c GreenMonster.h GreenMonsterLinkDef.h

config/%.o: %.C
	$(CXX) $(CXXFLAGS) -c config/$<












