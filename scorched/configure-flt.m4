dnl Floating point math
AC_MSG_CHECKING(for floating point math support)
have_fp=yes
AC_TRY_COMPILE([
	#include <math.h>
	],[
	float a = sinf(0.1f);
	],[
	have_fp=yes
	],[
	have_fp=no
])
AC_MSG_RESULT($have_fp)

if test "x$have_fp" != 'xyes'
then
	CFLAGS="$CPPFLAGS -DNO_FLOAT_MATH"
	CPPFLAGS="$CPPFLAGS -DNO_FLOAT_MATH"
	CXXFLAGS="$CXXFLAGS -DNO_FLOAT_MATH"
fi
