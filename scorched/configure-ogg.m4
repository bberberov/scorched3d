dnl Check for ogg and vorbis

OGG_CFLAGS=
OGG_LIBS=

have_OGG=yes
XIPH_PATH_OGG(
	[XIPH_PATH_VORBIS(
		[
			OGG_CFLAGS="$OGG_CFLAGS $VORBIS_CFLAGS"
			OGG_LIBS="$OGG_LIBS $VORBIS_LIBS $VORBISFILE_LIBS"

			have_OGG=yes
			AC_DEFINE(HAVE_OGG)
		],
		[have_OGG=no]
	)],
	[have_OGG=no]
)

if test "x$have_OGG" != 'xyes'
then
	AC_MSG_WARN([OGG not found no ogg support compiled into Scorched3D])
fi

AC_SUBST(OGG_CFLAGS)
AC_SUBST(OGG_LIBS)
