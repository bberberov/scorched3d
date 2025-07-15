dnl Checking for OpenAL and Freealut
dnl FIXME Add static libraries support

PKG_CHECK_MODULES(
	[OPENAL],
	[openal],
	[],
	[AC_MSG_ERROR([OpenAL must be installed on your system (https://www.openal-soft.org/)])]
)

PKG_CHECK_MODULES(
	[FREEALUT],
	[freealut],
	[],
	[AC_MSG_ERROR([freealut must be installed on your system (https://github.com/vancegroup/freealut)])]
)
