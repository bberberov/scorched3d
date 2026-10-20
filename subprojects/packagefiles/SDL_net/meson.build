# Minimal Meson build of SDL_net 1.2 for platforms that no longer package it
# (e.g. Homebrew on macOS only provides SDL2_net)

project(
	'SDL_net',
	'c',
	version : '1.2.8',
	license : 'Zlib',
)

sdl_dep = dependency('sdl', version : ['>= 1.2.4', '< 1.3'])

deps = [sdl_dep]
if host_machine.system() == 'windows'
	deps += meson.get_compiler('c').find_library('ws2_32')
	deps += meson.get_compiler('c').find_library('iphlpapi')
endif

# Expose the header as both <SDL/SDL_net.h> and <SDL_net/SDL_net.h>
subdir('include/SDL')
subdir('include/SDL_net')

sdl_net_lib = static_library(
	'SDL_net',
	'SDLnet.c',
	'SDLnetselect.c',
	'SDLnetTCP.c',
	'SDLnetUDP.c',
	dependencies : deps,
)

sdl_net_dep = declare_dependency(
	include_directories : include_directories('.', 'include'),
	link_with : sdl_net_lib,
	dependencies : sdl_dep,
)
meson.override_dependency('SDL_net', sdl_net_dep)
