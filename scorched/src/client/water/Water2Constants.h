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

#if !defined(__INCLUDE_Water2Constantsh_INCLUDE__)
#define __INCLUDE_Water2Constantsh_INCLUDE__

#include <common/Vector.h>

static inline float myfmod(float a, float b) { return a-floorf(a/b)*b; }//fmod is different for negative a/b

static const float wave_tidecycle_time = 10.24f;
static const unsigned int wave_phases = 256;
static const unsigned int wave_patch_width = 64;
static const unsigned int wave_resolution = 128;

static const float grid_size = 512.0f / (256.0f / float(wave_patch_width));
static const float half_grid_size = grid_size / 2.0f;

static const float wavetile_length = 256.0f;
static const float wave_waterwidth = wavetile_length;
static const float wavetile_length_rcp = 1.0f / wavetile_length;

static const float VIRTUAL_PLANE_HEIGHT = 25.0f;

#define REFRAC_COLOR_RES 32
#define FRESNEL_FCT_RES 256

class Water2Points
{
public:
	Vector &getPoint(int x, int y)
	{
		DIALOG_ASSERT(x>=0 && y>=0 && x<wave_resolution && y<wave_resolution);
		return points[x][y];
	};

private:
	Vector points[wave_resolution][wave_resolution];
};

static inline float exact_fresnel(float x)
{
	// the real formula (recheck it!)
/*
	float g = 1.333f + x*x - 1;
	float z1 = g-x;
	float z2 = g+x;
	return (z1*z1)*(1+((x*z2-1)*(x*z2-1))/((x*z1+1)*(x*z1+1)))/(2*z2*z2);
*/

/*
	// a very crude guess
	float tmp = 1-4*x;
	if (tmp < 0.0f) tmp = 0.0f;
	return tmp;
*/
	// a good approximation (1/(x+1)^8)
	float x1 = x + 1.0f;
	float x2 = x1*x1;
	float x4 = x2*x2;
	return 1.0f/(x4*x4);
}


#endif // __INCLUDE_Water2Constantsh_INCLUDE__
