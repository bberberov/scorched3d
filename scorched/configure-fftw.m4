dnl Check for FFTW3

FFTW_CFLAGS=
FFTW_LIBS=

AC_ARG_WITH(
	[fftw],
	[AS_HELP_STRING(
		[--with-fftw=PFX],
		[Prefix where fftw is installed (optional)]
	)],
	[fftw_prefix=$withval],
	[fftw_prefix='']
)
AC_ARG_WITH(
	[fftw-libraries],
	[AS_HELP_STRING(
		[--with-fftw-libraries=DIR],
		[Directory where fftw library is installed (optional)]
	)],
	[fftw_libraries=$withval],
	[fftw_libraries='']
)
AC_ARG_WITH(
	[fftw-includes],
	[AS_HELP_STRING(
		[--with-fftw-includes=DIR],
		[Directory where fftw header files are installed (optional)]
	)],
	[fftw_includes=$withval],
	[fftw_includes='']
)
AC_ARG_ENABLE(
	[fftwtest],
	[AS_HELP_STRING(
		[--disable-fftwtest],
		[Do not try to compile and run a test fftw program]
	)],
	[],
	[enable_fftwtest=yes]
)

if test "x$fftw_libraries" != 'x'
then
	FFTW_LIBS="-L$fftw_libraries"
elif test "x$fftw_prefix" != 'x'
then
	FFTW_LIBS="-L$fftw_prefix/lib"
elif test "x$prefix" != 'xNONE'
then
	FFTW_LIBS="-L$prefix/lib"
fi

if test `uname` = 'Darwin'
then
	FFTW_LIBS="$FFTW_LIBS -lfftw3"
else
	FFTW_LIBS="$FFTW_LIBS -lfftw3f"
fi

if test "x$fftw_includes" != 'x'
then
	FFTW_CFLAGS="-I$fftw_includes"
elif test "x$fftw_prefix" != 'x'
then
	FFTW_CFLAGS="-I$fftw_prefix/include"
elif test "x$prefix" != 'xNONE'
then
	FFTW_CFLAGS="-I$prefix/include -I/usr/include"
fi

AC_MSG_CHECKING(for FFTW)
no_fftw=""

if test "x$enable_fftwtest" = 'xyes'
then
	ac_save_CFLAGS="$CFLAGS"
	ac_save_LIBS="$LIBS"
	CFLAGS="$CFLAGS $FFTW_CFLAGS"
	LIBS="$LIBS $FFTW_LIBS"
dnl
dnl Now check if the installed fftw is sufficiently new.
dnl
	rm -f conf.fftwtest
	AC_RUN_IFELSE(
		[AC_LANG_PROGRAM(
			[[
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fftw3.h>

#ifdef FFTW_USE_DOUBLE
#define FFT_COMPLEX_TYPE fftw_complex
#define FFT_REAL_TYPE double
#define FFT_PLAN_TYPE fftw_plan
#define FFT_CREATE_PLAN fftw_plan_dft_c2r_2d
#define FFT_DELETE_PLAN fftw_destroy_plan
#define FFT_EXECUTE_PLAN fftw_execute
#else
#define FFT_COMPLEX_TYPE fftwf_complex
#define FFT_REAL_TYPE float
#define FFT_PLAN_TYPE fftwf_plan
#define FFT_CREATE_PLAN fftwf_plan_dft_c2r_2d
#define FFT_DELETE_PLAN fftwf_destroy_plan
#define FFT_EXECUTE_PLAN fftwf_execute
#endif

#define N 10
			]],[[
	FFT_COMPLEX_TYPE fft_in[N*(N/2+1)];
	FFT_REAL_TYPE fft_out[N*N];
	FFT_PLAN_TYPE plan;

	plan = FFT_CREATE_PLAN(N, N, fft_in, fft_out, 0);


	system("touch conf.fftwtest");
	return 0;
			]]
		)],
		[],
		[no_fftw=yes],
		[echo $ac_n "cross compiling; assumed OK... $ac_c"]
	)
	CFLAGS="$ac_save_CFLAGS"
	LIBS="$ac_save_LIBS"
fi

if test "x$no_fftw" = 'x'
then
	AC_MSG_RESULT(yes)
else
	AC_MSG_RESULT(no)
	if test -f conf.fftwtest
	then
		:
	else
		echo "*** Could not run FFTW test program, checking why..."
		CFLAGS="$CFLAGS $FFTW_CFLAGS"
		LIBS="$LIBS $FFTW_LIBS"
		AC_LINK_IFELSE(
			[AC_LANG_PROGRAM(
				[[
#include <stdio.h>
#include <fftw3.h>
				]],[[
return 0;
				]]
			)],
		[ echo "*** The test program compiled, but did not run. This usually means"
		echo "*** that the run-time linker is not finding FFTW or finding the wrong"
		echo "*** version of FFTW. If it is not finding FFTW, you'll need to set your"
		echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
		echo "*** to the installed location  Also, make sure you have run ldconfig if that"
		echo "*** is required on your system"
		echo "***"
		echo "*** If you have an old version installed, it is best to remove it, although"
		echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"],
		[ echo "*** The test program failed to compile or link. See the file config.log for the"
		echo "*** exact error that occured. This usually means FFTW was incorrectly installed"
		echo "*** or that you have moved FFTW since it was installed." ])
		CFLAGS="$ac_save_CFLAGS"
		LIBS="$ac_save_LIBS"
	fi
	FFTW_CFLAGS=""
	FFTW_LIBS=""

	AC_MSG_ERROR([
                FFTW must be installed on your system but couldn't be found.
                Please check that FFTW is installed.
                FFTW version 3 is required.
	])


fi
rm -f conf.fftwtest

AC_SUBST(FFTW_CFLAGS)
AC_SUBST(FFTW_LIBS)
