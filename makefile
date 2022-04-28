
CFLAGS=-Wall -Werror -std=c11 -pedantic -ggdb `pkg-config --cflags sdl2 SDL2_image`
LIBS=`pkg-config --libs sdl2 SDL2_image` -lm 

slide: main.c
	cc $(CFLAGS) -o slide main.c $(LIBS) 
run:
	cc $(CFLAGS) -o slide main.c $(LIBS) 
	./slide .
clean:
	-rm -f slide JPGFiles
.PHONY: clean run 