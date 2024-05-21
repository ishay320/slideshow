INCUDE_FOLDER=-I./include/
RAYLIB_INC=-I./raylib/build/raylib/include/
RAYLIB_LIB=-L./raylib/build/raylib/ -lraylib

HEIF=`pkg-config libheif --cflags --libs`

main: src/main.cpp
	${CXX} -g -o $@ $^ ${INCUDE_FOLDER} ${RAYLIB_INC} ${RAYLIB_LIB} -Wl,-rpath=./raylib/build/raylib/ ${HEIF}
