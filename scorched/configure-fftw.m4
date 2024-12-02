dnl Check for FFTW3
dnl FIXME Add static libraries support

if test `uname` = 'Darwin'
then
	PKG_CHECK_MODULES(
		[FFTW],
		[fftw3],
		[],
		[AC_MSG_ERROR([FFTW3 must be installed on your system (https://www.fftw.org/)])]
	)
else
	PKG_CHECK_MODULES(
		[FFTW],
		[fftw3f],
		[],
		[AC_MSG_ERROR([FFTW3 must be installed on your system (https://www.fftw.org/)])]
	)
fi
