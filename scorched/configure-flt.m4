dnl Floating point math
dnl NOTE: Function list taken from DefinesMath.h

AC_MSG_CHECKING(for floating point math support)

AC_COMPILE_IFELSE(
	[AC_LANG_PROGRAM(
		[#include <math.h>],
		[
float a = sinf(   0.1f );
float b = cosf(   0.1f );
#ifndef __DARWIN__
float c = fabsf(  0.1f );
#endif
float d = sqrtf(  0.1f );
float e = acosf(  0.1f );
float f = atan2f( 0.1f, 0.1f );
float g = powf(   0.1f, 0.1f );
float h = floorf( 0.1f );
		]
	)],
	[AC_MSG_RESULT([yes])],
	[
		AC_MSG_RESULT([no])
		AC_MSG_WARN([Not all floating point functions found. Setting NO_FLOAT_MATH.])
		AC_DEFINE([NO_FLOAT_MATH])
	]
)
