## Setup correct target type
#

TARGET=powerpc-wrs-vxworks
CC=$(TARGET)-gcc

all : vqwk.o vqwk_longcall.o vqwk_32.o vqwk_32_longcall.o


vqwk.o: vqwk.c vqwk.h
	$(CC) -c -o $@ vqwk.c

vqwk_longcall.o: vqwk.c vqwk.h
	$(CC) -c -mlongcall -o $@ vqwk.c

vqwk_32.o: vqwk_32.c vqwk_32.h
	$(CC) -c -o $@ vqwk_32.c

vqwk_32_longcall.o: vqwk_32.c vqwk_32.h
	$(CC) -c -mlongcall -o $@ vqwk_32.c

clean:
	rm -f *~ *.o
