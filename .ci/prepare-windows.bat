rem  Install necessary packages
python -m pip install meson
vcpkg install pkgconf
vcpkg owns pkgconf
vcpkg owns pkgconf.exe

dir C:\vcpkg
dir C:\vcpkg\installed
dir C:\vcpkg\installed\x64-windows
dir C:\vcpkg\packages
dir C:\vcpkg\packages\vcpkg-tool-meson_x64-windows
dir C:\vcpkg\packages\pkgconf_x64-windows
dir C:\vcpkg\scripts\buildsystems

rem  vcpkg install expat
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
