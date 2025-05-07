python -m pip install meson

cd scorched

meson wrap install expat
rem  fftw3
rem  freealut
meson wrap install freetype2
rem  OpenGL
meson wrap install glew
rem  glu
meson wrap install libjpeg-turbo
meson wrap install lua
meson wrap install ogg
meson wrap install openal-soft
meson wrap install libpng
rem  SDL
rem  SDL_net
meson wrap install vorbis
rem  wx
meson wrap install zlib
