////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011, 2026
//
//    This file is part of Scorched3D.
//
//    Scorched3D is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    Scorched3D is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef __INCLUDE_DefinesMath_hpp_INCLUDE__
#define __INCLUDE_DefinesMath_hpp_INCLUDE__

// BEGIN Some constants from GNU math.h and/or MS, and others

// ### Doubles ###

#ifndef M_2xPI
#define M_2xPI      6.28318530717958647693  /* 2*pi */
#endif

#ifndef M_PI
#define M_PI        3.14159265358979323846  /* pi */
#endif

#ifndef M_PI_2
#define M_PI_2      1.57079632679489661923  /* pi/2 */
#endif

#ifndef M_PI_4
#define M_PI_4      0.78539816339744830962  /* pi/4 */
#endif

#ifndef M_PI_180
#define M_PI_180    0.01745329251994329577  /* pi/180 */
#endif

#ifndef M_180_PI
#define M_180_PI    57.2957795130823208768  /* 180/pi */
#endif

// ### Floats ###

#ifndef M_2xPIf
#define M_2xPIf     6.28318530717958647693f  /* 2*pi */
#endif

#ifndef M_PIf
#define M_PIf       3.14159265358979323846f  /* pi */
#endif

#ifndef M_PI_2f
#define M_PI_2f     1.57079632679489661923f  /* pi/2 */
#endif

#ifndef M_PI_4f
#define M_PI_4f     0.78539816339744830962f  /* pi/4 */
#endif

#ifndef M_PI_180f
#define M_PI_180f   0.01745329251994329577f  /* pi/180 */
#endif

#ifndef M_180_PIf
#define M_180_PIf   57.2957795130823208768f  /* 180/pi */
#endif

// END   Some constants from GNU math.h and/or MS, and others

#define RAND        ( (float)rand() / (float)RAND_MAX )
#define MAX( x, y ) ( ( x ) > ( y ) ? ( x ) : ( y ) )
#define MIN( x, y ) ( ( x ) < ( y ) ? ( x ) : ( y ) )

extern float getFastSin( float angle );
extern float getFastCos( float angle );

#ifdef NO_FLOAT_MATH

#define sinf( x ) ( (float)sin( x ) )
#define cosf( x ) ( (float)cos( x ) )
#ifndef __DARWIN__
#define fabsf( x ) ( (float)fabs( x ) )
#endif
#define sqrtf( x )     ( (float)sqrt( x ) )
#define acosf( x )     ( (float)acos( x ) )
#define atan2f( x, y ) ( (float)atan2( x, y ) )
#define powf( x, y )   ( (float)pow( x, y ) )
#define floorf( x )    ( (float)floor( x ) )

#endif  // NO_FLOAT_MATH

#endif  // __INCLUDE_DefinesMath_hpp_INCLUDE__
