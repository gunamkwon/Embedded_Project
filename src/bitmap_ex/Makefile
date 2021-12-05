all: bitmap.out

bitmap.out: bitmap.c libbitmap.o
	arm-linux-gnueabi-gcc bitmap.c libbitmap.o -o bitmap.out -lpthread

libbitmap.o: libbitmap.c libbitmap.h
	arm-linux-gnueabi-gcc libbitmap.c -c -o libbitmap.o

clear:
	rm *.o *.out
