#! /bin/bash

# Install necessary packages
# NOTE SDL_net 1.2 is not available from Homebrew, it is built as a Meson subproject
brew install \
	meson \
	\
	expat \
	fftw \
	freealut \
	freetype2 \
	\
	glew \
	jpeg-turbo \
	lua@5.4 \
	libogg \
	openal-soft \
	\
	libpng \
	sdl12-compat \
	libvorbis \
	wxwidgets \
	zlib
