    INCFLAGS =
    VXDIR=/adaqfs/home/adaq/vxworks/headers/5.5/h
    CCPPCFLAGS =  -I$(VXDIR) -DCPU_FAMILY=PPC -DCPU=PPC604


all: HelBoard.o

HelBoard.o: HelBoard.c  
	rm -f $@
	ccppc -c -mlongcall $(CCPPCFLAGS) HelBoard.c 

clean:
	rm -f *.o
