#! /bin/bash

# Install necessary packages
brew install \
	meson \
	\
	expat \
	fftw \
	freealut \
	freetype2 \
	\
	glew \
	mesa-glu \
	jpeg-turbo \
	\
	libogg \
	openal-soft \
	libpng \
	sdl12-compat \
	sdl2_net \
	libvorbis \
	wxwidgets \
	zlib

meson wrap install lua
