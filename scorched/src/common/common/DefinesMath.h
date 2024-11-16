////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011
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

#ifndef __DEFINESMATH__
#define __DEFINESMATH__

#define PI 3.14159f
#define TWOPI 6.28318f
#define HALFPI 1.570795f
#define PIO180 0.017453278f

#define RAND ((float) rand() / (float) RAND_MAX)
#define MAX(x,y) ((x)>(y)?(x):(y))
#define MIN(x,y) ((x)<(y)?(x):(y))

extern float getFastSin(float angle);
extern float getFastCos(float angle);

#ifdef NO_FLOAT_MATH

#define sinf(x) ((float)sin(x))
#define cosf(x) ((float)cos(x))
#ifndef __DARWIN__
#define fabsf(x) ((float)fabs(x))
#endif
#define sqrtf(x) ((float)sqrt(x))
#define acosf(x) ((float)acos(x))
#define atan2f(x,y) ((float)atan2(x,y))
#define powf(x,y) ((float)pow(x,y))
#define floorf(x) ((float)floor(x))

#endif // NO_FLOAT_MATH

#endif // __DEFINESMATH__
