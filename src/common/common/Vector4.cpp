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

#include <common/Vector4.h>
#include <common/Defines.h>

// Some maths funnies taken from ODE
#define _R(i,j) R[(i)*4+(j)]
#define dDOTpq(a,b,p,q) ((a)[0]*(b)[0] + (a)[p]*(b)[q] + (a)[2*(p)]*(b)[2*(q)])
#define dDOT41(a,b) dDOTpq(a,b,4,1)
#define dDOT(a,b)   dDOTpq(a,b,1,1)
#define dMULTIPLYOP1_331(A,op,B,C) \
do { \
  (A)[0] op dDOT41((B),(C)); \
  (A)[1] op dDOT41((B+1),(C)); \
  (A)[2] op dDOT41((B+2),(C)); \
} while(0)
#define dMULTIPLY1_331(A,B,C) dMULTIPLYOP1_331(A,=,B,C)

static Vector4 nullVector;

Vector4 &Vector4::getNullVector()
{
	nullVector.zero();
	return nullVector;
}

void Vector4::setQuatFromAxisAndAngle(Vector &axis, float angle)
{
	float l = axis[0]*axis[0] + axis[1]*axis[1] + axis[2]*axis[2];
	if (l > 0.0f) 
	{
		angle *= 0.5f;
		V[0] = cosf(angle);
		l = sinf(angle) * ((1.0f/sqrtf(l)));
		V[1] = axis[0] * l;
		V[2] = axis[1] * l;
		V[3] = axis[2] * l;
	}
	else 
	{
		V[0] = 1.0f;
		V[1] = 0.0f;
		V[2] = 0.0f;
		V[3] = 0.0f;
	}
}

// derived from equations in "An Introduction
// to Physically Based Modeling: Rigid Body Simulation - 1: Unconstrained
// Rigid Body Dynamics" by David Baraff, Robotics Institute, Carnegie Mellon
// University, 1997.
void Vector4::getRotationMatrix(float *R)
{
	// q = (s,vx,vy,vz)
	float qq1 = 2*V[1]*V[1];
	float qq2 = 2*V[2]*V[2];
	float qq3 = 2*V[3]*V[3];
	_R(0,0) = 1 - qq2 - qq3;
	_R(0,1) = 2*(V[1]*V[2] - V[0]*V[3]);
	_R(0,2) = 2*(V[1]*V[3] + V[0]*V[2]);
	_R(0,3) = 0.0f;
	_R(1,0) = 2*(V[1]*V[2] + V[0]*V[3]);
	_R(1,1) = 1 - qq1 - qq3;
	_R(1,2) = 2*(V[2]*V[3] - V[0]*V[1]);
	_R(1,3) = 0.0f;
	_R(2,0) = 2*(V[1]*V[3] - V[0]*V[2]);
	_R(2,1) = 2*(V[2]*V[3] + V[0]*V[1]);
	_R(2,2) = 1 - qq1 - qq2;
	_R(2,3) = 0.0f;
}

void Vector4::getOpenGLRotationMatrix(float *rotMatrix)
{
	static float matrix[12];
	getRotationMatrix(matrix);

	rotMatrix[0] = matrix[0];
	rotMatrix[1] = matrix[4];
	rotMatrix[2] = matrix[8];
	rotMatrix[4] = matrix[1];
	rotMatrix[5] = matrix[5];
	rotMatrix[6] = matrix[9];
	rotMatrix[8] = matrix[2];
	rotMatrix[9] = matrix[6];
	rotMatrix[10] = matrix[10];
	rotMatrix[3] = rotMatrix[7] = rotMatrix[11] = 0.0;
	rotMatrix[15] = 1.0;
	rotMatrix[12] = rotMatrix[13] = rotMatrix[14] = 0.0;	
}

void Vector4::Normalize()
{
	float l = dDOT(V,V) + V[3] * V[3];
	if (l > 0) 
	{
		l = 1.0f / sqrtf(l);
		V[0] *= l;
		V[1] *= l;
		V[2] *= l;
		V[3] *= l;
	}
	else 
	{
		V[0] = 1;
		V[1] = 0;
		V[2] = 0;
		V[3] = 0;
	}
}

void Vector4::getRelativeVector(Vector &r, Vector &p)
{
	static float matrix[12];
	getRotationMatrix(matrix);

	float *result = r;
	float *position = p;
	dMULTIPLY1_331(result, matrix, position);
}

void Vector4::dDQfromW(Vector4 &dq, Vector &w, Vector4 &q)
{
	dq[0] = 0.5f *(- w[0]*q[1] - w[1]*q[2] - w[2]*q[3]);
	dq[1] = 0.5f *(  w[0]*q[0] + w[1]*q[3] - w[2]*q[2]);
	dq[2] = 0.5f *(- w[0]*q[3] + w[1]*q[0] + w[2]*q[1]);
	dq[3] = 0.5f *(  w[0]*q[2] - w[1]*q[1] + w[2]*q[0]);
}
