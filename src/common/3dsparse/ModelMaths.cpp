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

#include <3dsparse/ModelMaths.h>
#include <common/Defines.h>
#include <math.h>

void ModelMaths::quaternionSlerp(fixed p[4], fixed q[4], fixed t, fixed qt[4])
{
	int i;
	fixed omega, cosom, sinom, sclp, sclq;

	// decide if one of the quaternions is backwards
	fixed a = 0;
	fixed b = 0;
	for (i = 0; i < 4; i++) {
		a += (p[i]-q[i])*(p[i]-q[i]);
		b += (p[i]+q[i])*(p[i]+q[i]);
	}
	if (a > b) {
		for (i = 0; i < 4; i++) {
			q[i] = -q[i];
		}
	}

	cosom = p[0]*q[0] + p[1]*q[1] + p[2]*q[2] + p[3]*q[3];

	if ((fixed(1) + cosom) > fixed(true, 1)) {
		if ((fixed(1) - cosom) > fixed(true, 1)) {
			omega = acosx( cosom );
			sinom = sinx( omega );
			sclp = sinx( (fixed(1) - t)*omega) / sinom;
			sclq = sinx( t*omega ) / sinom;
		}
		else {
			sclp = fixed(1) - t;
			sclq = t;
		}
		for (i = 0; i < 4; i++) {
			qt[i] = sclp * p[i] + sclq * q[i];
		}
	}
	else {
		qt[0] = -p[1];
		qt[1] = p[0];
		qt[2] = -p[3];
		qt[3] = p[2];
		
		sclp = sinx( (fixed(1) - t) * fixed::XPIO2);
		sclq = sinx( t * fixed::XPIO2);
		for (i = 0; i < 3; i++) {
			qt[i] = sclp * p[i] + sclq * qt[i];
		}
	}
}

void ModelMaths::quaternionMatrix(fixed quaternion[4], fixed matrix[3][4])
{
	matrix[0][0] = fixed(1) - fixed(2) * quaternion[1] * quaternion[1] - fixed(2) * quaternion[2] * quaternion[2];
	matrix[1][0] = fixed(2) * quaternion[0] * quaternion[1] + fixed(2) * quaternion[3] * quaternion[2];
	matrix[2][0] = fixed(2) * quaternion[0] * quaternion[2] - fixed(2) * quaternion[3] * quaternion[1];

	matrix[0][1] = fixed(2) * quaternion[0] * quaternion[1] - fixed(2) * quaternion[3] * quaternion[2];
	matrix[1][1] = fixed(1) - fixed(2) * quaternion[0] * quaternion[0] - fixed(2) * quaternion[2] * quaternion[2];
	matrix[2][1] = fixed(2) * quaternion[1] * quaternion[2] + fixed(2) * quaternion[3] * quaternion[0];

	matrix[0][2] = fixed(2) * quaternion[0] * quaternion[2] + fixed(2) * quaternion[3] * quaternion[1];
	matrix[1][2] = fixed(2) * quaternion[1] * quaternion[2] - fixed(2) * quaternion[3] * quaternion[0];
	matrix[2][2] = fixed(1) - fixed(2) * quaternion[0] * quaternion[0] - fixed(2) * quaternion[1] * quaternion[1];
}

void ModelMaths::angleQuaternion(FixedVector &angles, fixed quaternion[4])
{
	fixed		angle;
	fixed		sr, sp, sy, cr, cp, cy;

	// FIXME: rescale the inputs to 1/2 angle
	angle = angles[2] * fixed(true, 5000);
	sy = angle.sin();
	cy = angle.cos();
	angle = angles[1] * fixed(true, 5000);
	sp = angle.sin();
	cp = angle.cos();
	angle = angles[0] * fixed(true, 5000);
	sr = angle.sin();
	cr = angle.cos();

	quaternion[0] = sr*cp*cy-cr*sp*sy; // X
	quaternion[1] = cr*sp*cy+sr*cp*sy; // Y
	quaternion[2] = cr*cp*sy-sr*sp*cy; // Z
	quaternion[3] = cr*cp*cy+sr*sp*sy; // W
}

void ModelMaths::angleMatrix(FixedVector &angles, fixed matrix[3][4])
{
	fixed		angle;
	fixed		sr, sp, sy, cr, cp, cy;
	
	angle = angles[2] * (fixed::X2PI / 360);
	sy = angle.sin();
	cy = angle.cos();
	angle = angles[1] * (fixed::X2PI / 360);
	sp = angle.sin();
	cp = angle.cos();
	angle = angles[0] * (fixed::X2PI / 360);
	sr = angle.sin();
	cr = angle.cos();

	// matrix = (Z * Y) * X
	matrix[0][0] = cp*cy;
	matrix[1][0] = cp*sy;
	matrix[2][0] = -sp;
	matrix[0][1] = sr*sp*cy+cr*-sy;
	matrix[1][1] = sr*sp*sy+cr*cy;
	matrix[2][1] = sr*cp;
	matrix[0][2] = (cr*sp*cy+-sr*-sy);
	matrix[1][2] = (cr*sp*sy+-sr*cy);
	matrix[2][2] = cr*cp;
	matrix[0][3] = 0;
	matrix[1][3] = 0;
	matrix[2][3] = 0;
}

void ModelMaths::concatTransforms(fixed in1[3][4], fixed in2[3][4], fixed out[3][4])
{
	out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] +
				in1[0][2] * in2[2][0];
	out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] +
				in1[0][2] * in2[2][1];
	out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] +
				in1[0][2] * in2[2][2];
	out[0][3] = in1[0][0] * in2[0][3] + in1[0][1] * in2[1][3] +
				in1[0][2] * in2[2][3] + in1[0][3];
	out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] +
				in1[1][2] * in2[2][0];
	out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] +
				in1[1][2] * in2[2][1];
	out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] +
				in1[1][2] * in2[2][2];
	out[1][3] = in1[1][0] * in2[0][3] + in1[1][1] * in2[1][3] +
				in1[1][2] * in2[2][3] + in1[1][3];
	out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] +
				in1[2][2] * in2[2][0];
	out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] +
				in1[2][2] * in2[2][1];
	out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] +
				in1[2][2] * in2[2][2];
	out[2][3] = in1[2][0] * in2[0][3] + in1[2][1] * in2[1][3] +
				in1[2][2] * in2[2][3] + in1[2][3];
}

void ModelMaths::vectorIRotate(FixedVector &in1, fixed in2[3][4], FixedVector &out)
{
	out[0] = in1[0]*in2[0][0] + in1[1]*in2[1][0] + in1[2]*in2[2][0];
	out[1] = in1[0]*in2[0][1] + in1[1]*in2[1][1] + in1[2]*in2[2][1];
	out[2] = in1[0]*in2[0][2] + in1[1]*in2[1][2] + in1[2]*in2[2][2];
}
