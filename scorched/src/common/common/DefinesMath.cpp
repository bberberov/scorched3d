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

#include <common/DefinesMath.h>
#include <common/Defines.h>

#include <math.h>
static float fastSin[628];
static float fastCos[628];
static bool calculatedFast = false;

static void calculateFast()
{
	calculatedFast = true;
	for (int i=0; i<628; i++)
	{
		float a = float(i) / 100.0f;
		fastSin[i] = (float) sin(a);
		fastCos[i] = (float) cos(a);
	}
}

float getFastSin(float angle)
{
	if (!calculatedFast) calculateFast();
	if (angle < 0.0f)
	{
		return -fastSin[(int(angle * -100)) % 628];
	}
	return fastSin[(int(angle * 100)) % 628];
}

float getFastCos(float angle)
{
	if (!calculatedFast) calculateFast();
	if (angle < 0.0f)
	{
		return fastCos[(int(angle * -100)) % 628];
	}
	return fastCos[(int(angle * 100)) % 628];
}
