rem  Install necessary packages
python -m pip install meson
vcpkg install pkgconf
vcpkg owns pkgconf.exe

rem  dir C:\vcpkg
rem  dir C:\vcpkg\installed
dir C:\vcpkg\installed\x64-windows
dir C:\vcpkg\installed\x64-windows\bin
dir C:\vcpkg\installed\x64-windows\include
dir C:\vcpkg\installed\x64-windows\lib
dir C:\vcpkg\installed\x64-windows\share
dir C:\vcpkg\installed\x64-windows\tools
rem  dir C:\vcpkg\packages
dir C:\vcpkg\packages\vcpkg-tool-meson_x64-windows\share
dir C:\vcpkg\packages\pkgconf_x64-windows\lib
dir C:\vcpkg\packages\pkgconf_x64-windows\share
dir C:\vcpkg\packages\pkgconf_x64-windows\tools
dir C:\vcpkg\scripts\buildsystems\meson

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
