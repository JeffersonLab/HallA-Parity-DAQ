#  Build system for HAPPEX 18 bit ADC
#  R. Michaels, Feb 2007
#Changed to 5.5 vxworks header and ccppc, P.Zhu.Sep 2011

export OSNAME       := $(shell uname)

#VXDIR=$(VXWORKS_INC)
#VXDIR=/site/vxworks/5.5/ppc/target/h
VXDIR=/adaqfs/halla/apar/vxworks/5.5/5.5
CCPPC = /usr/local/gnu-ppc/bin/ccppc
CCVXFLAGS =  -I$(VXDIR) -DCPU_FAMILY=PPC -DCPU=PPC604 -mlongcall


# Normally you should "make adc18/adc18" in ../  (i.e. in directory above)

all : hapAdc18Left.o hapAdc18Right.o hapAdc18Count.o

adc18 : hapAdc18Lib.o hapAdc18Test.o hapAdc18Count.o hapAdc18Left.o hapAdc18Right.o hapAdc18Inj.o

hapAdc18Lib.o: hapAdc18Lib.c hapAdc18Lib.h
	rm -f $@
	$(CCPPC) -o $@ -c $(CCVXFLAGS) -DTESTCRATE hapAdc18Lib.c

hapAdc18Test.o: hapAdc18Lib.c hapAdc18Lib.h
	rm -f $@
	$(CCPPC) -o $@ -c $(CCVXFLAGS) -DTESTCRATE hapAdc18Lib.c

hapAdc18Count.o: hapAdc18Lib.c hapAdc18Lib.h
	rm -f $@
	$(CCPPC) -o $@ -c $(CCVXFLAGS) -DCOUNTINGHOUSE hapAdc18Lib.c

hapAdc18Left.o: hapAdc18Lib.c hapAdc18Lib.h
	rm -f $@
	$(CCPPC) -o $@ -c $(CCVXFLAGS) -DLEFTSPECT hapAdc18Lib.c

hapAdc18Right.o: hapAdc18Lib.c hapAdc18Lib.h
	rm -f $@
	$(CCPPC) -o $@ -c $(CCVXFLAGS) -DRIGHTSPECT hapAdc18Lib.c

hapAdc18Inj.o: hapAdc18Lib.c hapAdc18Lib.h
	rm -f $@
	$(CCPPC) -o $@ -c $(CCVXFLAGS) -DINJECTOR hapAdc18Lib.c

clean : 
	rm -f *.o

deepclean:
	rm -f *.o *~ *#






