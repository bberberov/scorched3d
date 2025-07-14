#! /bin/bash

# Install necessary packages
sudo apt-get update
sudo apt-get --assume-yes install --no-install-recommends \
	meson \
	\
	libexpat1-dev \
	libfftw3-dev \
	libalut-dev \
	libfreetype-dev \
	libgl-dev \
	libglew-dev \
	libglu1-mesa-dev \
	libjpeg-dev \
	liblua5.1-0-dev \
	libogg-dev \
	libopenal-dev \
	libpng-dev \
	libsdl1.2-dev \
	libsdl-net1.2-dev \
	libvorbis-dev \
	libwxgtk3.2-dev \
	zlib1g-dev
