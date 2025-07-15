dnl Checking for SDL
SDL_VERSION=1.2.5
AM_PATH_SDL(
	[$SDL_VERSION],
	[],
	[AC_MSG_ERROR([*** SDL version $SDL_VERSION not found. Try http://www.libsdl.org/])]
)

dnl checking for SDL_net
m4_ifdef(
	[PKG_CHECK_MODULES],
	[PKG_CHECK_MODULES(
		[SDL_NET],
		[SDL_net],
		[],
		[AC_MSG_ERROR([SDL_net must be installed on your system (https://github.com/libsdl-org/SDL_net/tree/SDL-1.2)])]
	)],
	[
		AC_MSG_CHECKING(for SDL_net)
		ac_save_CFLAGS="$CFLAGS"
		ac_save_CXXFLAGS="$CXXFLAGS"
		ac_save_LIBS="$LIBS"
		CFLAGS="$CFLAGS $SDL_CFLAGS"
		CXXFLAGS="$CXXFLAGS $SDL_CFLAGS"
		LIBS="$SDL_LIBS $LIBS"

		AC_COMPILE_IFELSE(
			[AC_LANG_PROGRAM(
				[[
#include <SDL/SDL.h>
#ifdef __DARWIN__
	#include <SDL_net/SDL_net.h>
#else
	#include <SDL/SDL_net.h>
#endif
				]],[[
return 0;
				]]
			)],
			[
				if test `uname` = 'Darwin'
				then
					SDL_LIBS="$SDL_LIBS -framework SDL_net"
				else
					SDL_LIBS="$SDL_LIBS -lSDL_net"
				fi
				AC_MSG_RESULT([yes])
			],
				[
				AC_MSG_RESULT([no])
				AC_MSG_ERROR([*** Can't find the SDL_net library Try: http://www.libsdl.org/projects/SDL_net])
			]
		)

		CFLAGS="$ac_save_CFLAGS"
		CXXFLAGS="$ac_save_CXXFLAGS"
		LIBS="$ac_save_LIBS"
	]
)
