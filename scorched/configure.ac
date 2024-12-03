dnl Process this file with autoconf to produce a configure script.

AC_INIT(
	[scorched3d],
	[44]
)
AC_CONFIG_SRCDIR([src/launcher/scorched/main.cpp])
AC_CANONICAL_TARGET

dnl Setup for automake
dnl FIXME Scorched3D server fails to build with Automake `subdir-objects` option on OBS
AM_INIT_AUTOMAKE([foreign -Wportability])

dnl Check for tools
AC_PROG_MAKE_SET
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_RANLIB

dnl Run once before any other pkg-config or pkgconf PKG_* macros
PKG_PROG_PKG_CONFIG(
	[0.29],
	[AC_MSG_ERROR([must install pkg-config 0.29 or later])]
)

AC_MSG_CHECKING([beer in fridge])
AC_MSG_RESULT([Hmm?!])
AC_MSG_WARN([No beer found in fridge!
	We highly suggest that you rectify this situation immediately.])

AC_ARG_ENABLE(
	[serveronly],
	[AS_HELP_STRING(
		[--enable-serveronly],
		[Enable server only compilation]
	)],
	[],
	[enable_serveronly=no]
)

# Include the other configuration files
if test "x$enable_serveronly" = 'xno'
then
	m4_include([configure-browser.m4])
	m4_include([configure-gl.m4])
	m4_include([configure-al.m4])
	m4_include([configure-wx.m4])
	m4_include([configure-ogg.m4])
	m4_include([configure-ft.m4])
	m4_include([configure-fftw.m4])
	S3D_SUBDIRS='launcher client server'
else
	S3D_SUBDIRS='server'
fi
m4_include([configure-sdl.m4])
m4_include([configure-flt.m4])
m4_include([configure-sql.m4])

AC_SUBST(S3D_SUBDIRS)

dnl Checks for Profiling
AC_MSG_CHECKING(for profiling enabled)
AC_ARG_ENABLE(
	[profiling],
	[AS_HELP_STRING(
		[--enable-profiling],
		[Enable gprof code profiling]
	)],
	[],
	[enable_profiling=no],
)
if test "x$enable_profiling" = 'xyes'
then
	LIBS="$LIBS -pg"
	CFLAGS="$CFLAGS -pg"
	CPPFLAGS="$CPPFLAGS -pg"
	CXXFLAGS="$CXXFLAGS -pg"
fi
AC_MSG_RESULT($enable_profiling)

# Check for extra functions
AC_CHECK_FUNCS(vsnprintf snprintf vasprintf asprintf iconv)

# Check for extra headers
AC_CHECK_HEADERS(unistd.h iconv.h)

# FIXME
dnl Checking for Zlib Png Jpeg
if test `uname` = 'Darwin'
then
	AC_DEFINE([__DARWIN__])
	AC_DEFINE([__MACOSX__])
	prefix=./
	LIBS="$LIBS -lexpat -lz -framework UnixImageIO -lmx"
else
	AC_PREFIX_DEFAULT([/usr/local/games/scorched3d])
	LIBS="$LIBS -lexpat -lz -lpng -ljpeg"
fi

# Directory setup stuff
dnl --with-docdir
AC_ARG_WITH(
	[docdir],
	[AS_HELP_STRING(
		[--with-docdir=DIR],
		[Use DIR to store documentation files (default DATADIR)]
	)],
	[docdir=${with_docdir}],
	[docdir=${datadir}/documentation]
)
AC_SUBST(docdir)

AC_DEFINE_UNQUOTED([S3D_BINDIR], ["${bindir}"])
AC_DEFINE_UNQUOTED([S3D_DATADIR],["${datadir}"])
AC_DEFINE_UNQUOTED([S3D_DOCDIR], ["${docdir}"])

# put here at end because of sdl.m4 not defining
AC_LANG([C++])

# Finally create all the generated files
AC_CONFIG_FILES([
Makefile
src/Makefile
src/launcher/Makefile
src/launcher/scorched/Makefile
src/client/Makefile
src/client/scorchedc/Makefile
src/server/Makefile
src/server/scorcheds/Makefile
])
AC_OUTPUT
