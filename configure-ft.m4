dnl Check for FreeType2

dnl pkg-config support was added in 2.1.5

dnl AC_CHECK_FT2([MINIMUM-VERSION [, ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl MINIMUM-VERSION is what libtool reports; the default is '7.0.1' (this is FreeType 2.0.4).

PKG_CHECK_MODULES(
	[FT2],
	[freetype2 >= 2.1.5],
	[],
	[AC_CHECK_FT2(
		[7.0.1],
		[],
		[AC_MSG_ERROR([FreeType2 must be installed on your system (https://freetype.org/)])]
	)]
)
