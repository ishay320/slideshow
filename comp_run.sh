#!/usr/bin/env bash
set -x

INCLUDE="-I./include/third_party/ -I./include/"

SRC="./src/glad.c \
    ./src/image_renderer.cpp \
    ./src/image.cpp \
    ./src/stb_image.cpp"
    
rm ./main
g++ main.cpp -o main -ggdb -g ${INCLUDE} ${SRC} -lglfw && ./main
