CC=gcc
AR=ar

#ifdef WIN32
	INCLUDES=-I..\zlib\
	IO=iowin32.c
	IO_OUT=iowin32.o
#else
	INCLUDES=-I../zlib/
	IO=ioapi.c
	IO_OUT=ioapi.o
#endif

all:
	gcc $(INCLUDES) -c $(IO)
	gcc $(INCLUDES) -c zip.c
	ar rcs libzip.a $(IO_OUT) zip.o

clean:
	rm *.o *.a
