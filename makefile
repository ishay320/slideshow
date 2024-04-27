RAYLIB_INC=-I./raylib/build/raylib/include/
RAYLIB_LIB=-L./raylib/build/raylib/ -lraylib

main: main.cpp
	${CXX} -o $@ $^ ${RAYLIB_INC} ${RAYLIB_LIB} -Wl,-rpath=./raylib/build/raylib/ 
