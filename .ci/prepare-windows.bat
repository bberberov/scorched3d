REM Install necessary packages
REM python -m pip install meson
vcpkg install vcpkg-tool-meson
vcpkg owns meson
vcpkg owns meson.py

vcpkg env --tools "vcpkg-tool-meson"

vcpkg install pkgconf
vcpkg owns pkgconf
vcpkg owns pkg-config
vcpkg owns pkgconf.exe
vcpkg owns pkg-config.exe

vcpkg env --tools "pkgconf"

rem  vcpkg install expat
rem  vcpkg install fftw3
rem  vcpkg install freealut
rem  vcpkg install freetype
rem  vcpkg install opengl
rem  vcpkg install glew
rem
rem  vcpkg install libjpeg-turbo
rem  vcpkg install lua
rem  vcpkg install libogg
rem  vcpkg install openal-soft
rem  vcpkg install libpng
rem  vcpkg install sdl1
rem  vcpkg install sdl1-net
rem  vcpkg install libvorbis
rem  vcpkg install wxwidgets
rem  vcpkg install zlib
