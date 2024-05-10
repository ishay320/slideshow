#!/bin/bash
# ----------------------------------------------------------------
# Script that compile dependencies for the project
# ----------------------------------------------------------------
set -e

raylib() {
	git clone https://github.com/raysan5/raylib.git --depth=1
	sed -i 's|.*\(#define SUPPORT_FILEFORMAT_JPG\s*\)|\1|' raylib/src/config.h
	cd raylib/
	mkdir build
	cd build/
	cmake -DBUILD_SHARED_LIBS=ON ..
	make -j4
}

raylib

# TODO: add libheif-dev
