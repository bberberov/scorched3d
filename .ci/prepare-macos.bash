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
	\
	libpng \
	sdl12-compat \
	sdl2_net \
	libvorbis \
	wxwidgets \
	\

cd scorched
meson wrap install lua
