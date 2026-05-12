rem  Install necessary packages
python -m pip install meson
vcpkg install pkgconf
vcpkg owns pkgconf.exe

vcpkg install ^
	lua[cpp]

rem  dir C:\vcpkg\installed
dir C:\vcpkg\installed\x64-windows
dir C:\vcpkg\installed\x64-windows\include
dir C:\vcpkg\installed\x64-windows\lib
dir C:\vcpkg\installed\x64-windows\lib\pkgconfig
dir C:\vcpkg\installed\x64-windows\share
dir C:\vcpkg\installed\x64-windows\share\pkgconf
rem  dir C:\vcpkg\installed\x64-windows\tools\pkgconf
rem  dir C:\vcpkg\packages
rem  dir C:\vcpkg\packages\vcpkg-tool-meson_x64-windows\share\vcpkg-tool-meson
rem  dir C:\vcpkg\packages\pkgconf_x64-windows\lib\pkgconfig
rem  dir C:\vcpkg\packages\pkgconf_x64-windows\share\pkgconf
rem  dir C:\vcpkg\packages\pkgconf_x64-windows\tools\pkgconf
rem  type C:\vcpkg\scripts\buildsystems\meson\meson.template.in

rem  vcpkg install ^
rem  	expat ^
rem  	fftw3 ^
rem  	freealut ^
rem  	freetype[png,zlib] ^
rem  	opengl ^
rem  	glew ^
rem  	^
rem  	libjpeg-turbo ^
rem  	lua[cpp] ^
rem  	libogg ^
rem  	openal-soft ^
rem  	libpng ^
rem  	sdl1 ^
rem  	sdl1-net ^
rem  	libvorbis ^
rem  	wxwidgets ^
rem  	zlib

vcpkg owns expat.pc
