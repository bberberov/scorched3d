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

#if !defined(__INCLUDE_ModelMathsh_INCLUDE__)
#define __INCLUDE_ModelMathsh_INCLUDE__

#include <common/FixedVector.h>

class ModelMaths
{
public:
	static void angleQuaternion(FixedVector &angles, 
		fixed quaternion[4]);
	static void quaternionSlerp(fixed p[4], 
		fixed q[4], fixed t, fixed qt[4]);

	static void angleMatrix(
		FixedVector &angles, fixed matrix[3][4]);
	static void quaternionMatrix(
		fixed quaternion[4], fixed matrix[3][4]);

	static void concatTransforms(fixed in1[3][4],
		fixed in2[3][4], fixed out[3][4]);
	static void vectorIRotate(FixedVector &in1, 
		fixed in2[3][4], FixedVector &out);
	static fixed dotProduct(FixedVector &v1, 
		const FixedVector &v2)
	{
		return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
	}
	static void vectorRotate(FixedVector &in1, 
		fixed in2[3][4], FixedVector &out)
	{
		out[0] = dotProduct(in1, in2[0]);
		out[1] = dotProduct(in1, in2[1]);
		out[2] = dotProduct(in1, in2[2]);
	}

private:
	ModelMaths();
};

#endif // __INCLUDE_ModelMathsh_INCLUDE__
