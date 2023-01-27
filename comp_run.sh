#!/usr/bin/env bash

g++ main.cpp -o main -ggdb -g -I./include/third_party/ -I./include/ ./src/glad.c ./src/image_renderer.cpp -lglfw && ./main
