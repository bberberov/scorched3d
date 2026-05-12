rem  Install necessary packages
python -m pip install meson
vcpkg install pkgconf
vcpkg owns pkgconf
vcpkg owns pkgconf.exe

dir C:\vcpkg
dir C:\vcpkg\*
dir C:\vcpkg\x64-windows
dir C:\vcpkg\x64-windows\*

vcpkg install expat
rem  vcpkg install fftw3
rem  vcpkg install freealut
rem  vcpkg install freetype
rem  vcpkg install opengl
rem  vcpkg install glew

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
