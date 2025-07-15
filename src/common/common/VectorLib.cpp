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


#include <common/VectorLib.h>
#include <common/Defines.h>
#include <math.h>

Vector VectorLib::reflection(Vector &ray, Vector &normal)
{
	Vector s = ray.Normalize();
	Vector n = normal.Normalize();

	return -((n * (2 * s.dotP(n))) - s);
}

bool VectorLib::sphereIntersection(Vector &start, Vector &end, 
									Vector &sphereCentre, float radius, float &distance)
{
	if (start == end) return false;

	Vector EO = sphereCentre - start;
	Vector V = (end - start).Normalize();
	float v = EO.dotP(V);
	float disc = (radius * radius) - (EO.dotP(EO) - (v * v));

	if (disc >= 0.0f)
	{
		float d = sqrtf(disc);
		if (v - d > 0)
		{
			distance = v - d;
			return (distance >= 0.0f && distance <= 1.0f);
		}
		else if (v + d > 0)
		{
			distance = v + d;
			return (distance >= 0.0f && distance <= 1.0f);
		}
	}

	return false;
}

Vector VectorLib::rotationAroundAxis(Vector &p, float angle, Vector &axis)
{
	Vector q;
	axis.StoreNormalize();
	float costheta = cos(angle); 
	float sintheta = sin(angle); 
	q[0] += (costheta + (1 - costheta) * axis[0] * axis[0]) * p[0]; 
	q[0] += ((1 - costheta) * axis[0] * axis[1] - axis[2] * sintheta) * p[1]; 
	q[0] += ((1 - costheta) * axis[0] * axis[2] + axis[1] * sintheta) * p[2]; 
	q[1] += ((1 - costheta) * axis[0] * axis[1] + axis[2] * sintheta) * p[0]; 
	q[1] += (costheta + (1 - costheta) * axis[1] * axis[1]) * p[1]; 
	q[1] += ((1 - costheta) * axis[1] * axis[2] - axis[0] * sintheta) * p[2]; 
	q[2] += ((1 - costheta) * axis[0] * axis[2] - axis[1] * sintheta) * p[0]; 
	q[2] += ((1 - costheta) * axis[1] * axis[2] + axis[0] * sintheta) * p[1]; 
	q[2] += (costheta + (1 - costheta) * axis[2] * axis[2]) * p[2]; 
	
	return q; 
}
