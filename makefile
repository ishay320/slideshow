
CFLAGS=-Wall -Werror -std=c11 -pedantic -ggdb `pkg-config --cflags sdl2`
LIBS=`pkg-config --libs sdl2` -lm

slide: main.c
	cc $(CFLAGS) -o slide main.c $(LIBS) 
	./slide

clean:
	-rm slide
.PHONY: clean