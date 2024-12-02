dnl Check for ogg and vorbis
XIPH_PATH_OGG(
	[XIPH_PATH_VORBIS(
		[AC_DEFINE(HAVE_OGG)],
		[AC_MSG_WARN([OGG not found no ogg support compiled into Scorched3D])]
	)],
	[AC_MSG_WARN([OGG not found no ogg support compiled into Scorched3D])]
)
