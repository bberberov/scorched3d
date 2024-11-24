dnl Check for FreeType2
AC_CHECK_FT2(7.0.1, ftype=1)
if test "$ftype" != 1; then
	AC_MSG_ERROR([
                FreeType2 must be installed on your system
                but freetype-config couldn't be found.
                Please check that freetype-config is in the path, the directory
                where the freetype libraries are installed (returned by
                'freetype-config --libs' command) is in LD_LIBRARY_PATH or
                equivalent variable and freetype version is 7.0.1 or above
                Try http://www.freetype.org to download freetype.
	])
fi
