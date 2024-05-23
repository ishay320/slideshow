INCUDE_FOLDER=-I./include/
RAYLIB_INC=-I./raylib/build/raylib/include/
RAYLIB_LIB=-L./raylib/build/raylib/ -lraylib

FILES=./src/effect.cpp \
      ./src/getters.cpp \
      ./src/wrappers.cpp

OBJ=${FILES:.cpp=.o}

HEIF=`pkg-config libheif --cflags --libs`

main: src/main.cpp ${OBJ}
	${CXX} -g -o $@ $< ${INCUDE_FOLDER} ${RAYLIB_INC} ${RAYLIB_LIB} -Wl,-rpath=./raylib/build/raylib/ ${HEIF} ${OBJ}

%.o: %.cpp
	${CXX} -g -c -o $@ $^ ${INCUDE_FOLDER} ${RAYLIB_INC} ${RAYLIB_LIB} -Wl,-rpath=./raylib/build/raylib/ 
