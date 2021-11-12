

main: main.cpp
	g++ $^ -o $@ -lglut -lGLU -lGL -lSOIL 
	./main

clean:
	-rm main
.PHONY: clean