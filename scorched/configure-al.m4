dnl Checking for OpenAL
AC_ARG_WITH(
	[openal-static],
	[AS_HELP_STRING(
		[--with-openal-static],
		[enable static linking for openal (default no)]
	)],
	[use_static_openal=${withval}],
	[]
)

AC_ARG_ENABLE(
	[openaltest],
	[AS_HELP_STRING(
		[--disable-openaltest],
		[Do not try to compile and run a test OpenAL program]
	)],
	[],
	[enable_openaltest=yes]
)
AC_MSG_CHECKING(for OpenAL support)
AC_PATH_PROG(OPENAL_CONFIG, openal-config, no)
if test "x$OPENAL_CONFIG" = 'xno'
then
	echo "The openal-config script installed by OpenAL could not be found"
	echo "Make sure openal-config is in your path, or set the OPENAL_CONFIG"
	echo "environment variable to the full path to openal-config."
	echo "Trying pkg-config instead."

	AC_PATH_PROG(PKG_CONFIG, pkg-config, no)
	if test "x$PKG_CONFIG" = 'xno'
	then
		echo "The pkg-config script could not be found"
		echo "Make sure pkg-config is in your path, or set the PKG_CONFIG"
		echo "environment variable to the full path to pkg-config."

		AC_MSG_ERROR([*** Can't find the openal library. Try: http://www.openal.org/])
	else
		OPENAL_CONFIG="$PKG_CONFIG openal";
	fi
fi

if test "x$use_static_openal" = 'xyes'
then
	AL_LIBS="/usr/local/lib/libopenal.a"
else
	AL_LIBS="`$OPENAL_CONFIG --libs`"
fi

AL_CFLAGS="`$OPENAL_CONFIG --cflags`"

AC_MSG_RESULT(yes)

AC_MSG_CHECKING(for Freealut support)
AC_PATH_PROG(FREEALUT_CONFIG, freealut-config, no)
if test "x$FREEALUT_CONFIG" = 'xno'
then
	echo "*** Warning: The freealut-config script installed by OpenAL could not be found."
	echo "*** The alut library is required, however some older OpenAL distribitions may include it."
	echo "*** If Scorched3D fails to link check alut is in the link line."
	echo "*** Alternatively, make sure freealut-config is in your path, or set the FREEALUT_CONFIG"
	echo "*** environment variable to the full path to openal-config."
else
	AL_LIBS="$AL_LIBS `$FREEALUT_CONFIG --libs`"
	AL_CFLAGS="$AL_CFLAGS `$FREEALUT_CONFIG --cflags`"

	AC_MSG_RESULT(yes)
fi

AC_MSG_CHECKING(for OpenAL compilation)
if test "x$enable_openaltest" = 'xyes'
then
	ac_save_CFLAGS="$CFLAGS"
	ac_save_LIBS="$LIBS"
	CFLAGS="$CFLAGS $AL_CFLAGS"
	LIBS="$AL_LIBS $LIBS"

	AC_COMPILE_IFELSE(
		[AC_LANG_PROGRAM(
			[[
#ifdef __APPLE__
	#include <OpenAL/al.h>
	#include <OpenAL/alc.h>
#else
	#include <AL/al.h>
	#include <AL/alut.h>
	#include <AL/alc.h>
#endif
			]],[[
return 0;
			]]
		)],
		[have_openal=yes],
		[
			echo "*** Failed to compile using the OpenAL library."
			echo "*** CFLAGS = $AL_CFLAGS";
			AC_MSG_ERROR([*** Check the OpenAL library is correctly installed.])
		]
	)

	AC_LINK_IFELSE(
		[AC_LANG_PROGRAM(
			[[
#ifdef __APPLE__
	#include <OpenAL/al.h>
	#include <OpenAL/alc.h>
#else
	#include <AL/al.h>
	#include <AL/alut.h>
	#include <AL/alc.h>
#endif
			]],[[
alutInit(argc, argv);
return 0;
			]]
		)],
		[have_openal=yes],
		[
			echo "*** Compiled but failed to link using the OpenAL library."
			echo "*** LIBS = $AL_LIBS";
			echo "*** Check the OpenAL library is on the LD_LIBRARY_PATH";
			AC_MSG_ERROR([*** Check the OpenAL library is correctly installed.])
		]
	)

	CFLAGS="$ac_save_CFLAGS"
	LIBS="$ac_save_LIBS"

	AC_MSG_RESULT(yes)
fi

AC_SUBST(AL_CFLAGS)
AC_SUBST(AL_LIBS)
