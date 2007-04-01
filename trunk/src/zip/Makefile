CC=gcc
AR=ar


all:
	@echo "Ich hab keine Ahnung, wie man dumme Windows-Makefiles baut und kann's nicht testen."
	@echo "Das muss hier aber irgendwann noch rein, weil wir ne DLL brauchen, die die für zip nötigen Funktionen mitbringt."

	gcc -c ioapi.c
	gcc -c zip.c
	ar rcs libzip.a ioapi.o zip.o

clean:
	rm *.o *.a
