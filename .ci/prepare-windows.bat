rem  Install necessary packages
python -m pip install meson
vcpkg install pkgconf
vcpkg owns pkgconf.exe

rem  dir C:\vcpkg
rem  dir C:\vcpkg\installed
rem  dir C:\vcpkg\installed\x64-windows
rem  dir C:\vcpkg\installed\x64-windows\include\pkgconf\libpkgconf
rem  dir C:\vcpkg\installed\x64-windows\lib\pkgconfig
rem  dir C:\vcpkg\installed\x64-windows\share\pkgconf
rem  dir C:\vcpkg\installed\x64-windows\share\vcpkg-tool-meson
rem  dir C:\vcpkg\installed\x64-windows\tools\pkgconf
rem  dir C:\vcpkg\packages
rem  dir C:\vcpkg\packages\vcpkg-tool-meson_x64-windows\share\vcpkg-tool-meson
rem  dir C:\vcpkg\packages\pkgconf_x64-windows\lib\pkgconfig
rem  dir C:\vcpkg\packages\pkgconf_x64-windows\share\pkgconf
rem  dir C:\vcpkg\packages\pkgconf_x64-windows\tools\pkgconf
rem  type C:\vcpkg\scripts\buildsystems\meson\meson.template.in

vcpkg install ^
	expat ^
	fftw3 ^
	freealut ^
	freetype ^
	opengl ^
	glew ^
	^
	libjpeg-turbo ^
	lua ^
	libogg ^
	openal-soft ^
	libpng ^
	sdl1 ^
	sdl1-net ^
	libvorbis ^
	wxwidgets ^
	zlib

vcpkg owns expat.pc
