dnl Checking for SDL
AC_MSG_CHECKING(for SDL)
SDL_VERSION=1.2.5
AM_PATH_SDL($SDL_VERSION,
            :,
            AC_MSG_ERROR([*** SDL version $SDL_VERSION not found. Try http://www.libsdl.org/])
)

dnl checking for SDL_net
AC_MSG_CHECKING(for SDL_net lib)
have_SDLnet=yes
if test "x$enable_sdltest" = 'xyes'
then

	ac_save_CFLAGS="$CFLAGS"
	ac_save_CXXFLAGS="$CXXFLAGS"
	ac_save_LIBS="$LIBS"
	CFLAGS="$CFLAGS $SDL_CFLAGS"
	CXXFLAGS="$CXXFLAGS $SDL_CFLAGS"
	LIBS="$SDL_LIBS $LIBS"

	AC_TRY_COMPILE([
		#include <SDL/SDL.h>
#ifdef __DARWIN__
		#include <SDL_net/SDL_net.h>
#else
		#include <SDL/SDL_net.h>
#endif

		int main(int argc, char *argv[])
		{ return 0; }
#undef  main
#define main K_and_R_C_main
		],[
		],[
		have_SDLnet=yes
		],[
		have_SDLnet=no
		])

	CFLAGS="$ac_save_CFLAGS"
	CXXFLAGS="$ac_save_CXXFLAGS"
	LIBS="$ac_save_LIBS"
fi

AC_MSG_RESULT($have_SDLnet)
if test "x$have_SDLnet" != "xyes"
then
	AC_MSG_ERROR([*** Can't find the SDL_net library Try: http://www.libsdl.org/projects/SDL_net])
fi

if test `uname` = 'Darwin'
then
	SDL_LIBS="$SDL_LIBS -framework SDL_net"
else
	SDL_LIBS="$SDL_LIBS -lSDL_net"
fi
