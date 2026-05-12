REM Install necessary packages
REM python -m pip install meson
vcpkg install vcpkg-tool-meson
vcpkg owns meson.exe

vcpkg env --tools vcpkg-tool-meson

vcpkg install pkgconf
vcpkg owns pkgconf.exe
vcpkg owns pkg-config.exe

vcpkg env --tools pkgconf

vcpkg install expat
vcpkg install fftw3
vcpkg install freealut
vcpkg install freetype
vcpkg install opengl
vcpkg install glew

vcpkg install libjpeg-turbo
vcpkg install lua
vcpkg install libogg
vcpkg install openal-soft
vcpkg install libpng
vcpkg install sdl1
vcpkg install sdl1-net
vcpkg install libvorbis
vcpkg install wxwidgets
vcpkg install zlib
