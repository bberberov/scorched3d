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

#if !defined(AFX_Vector4_H__AD959187_7A1C_11D2_957C_00A0C9A4CA3E__INCLUDED_)
#define AFX_Vector4_H__AD959187_7A1C_11D2_957C_00A0C9A4CA3E__INCLUDED_

#include <common/Vector.h>

class Vector4  
{
public:
	Vector4()
	{
		V[0] = V[1] = V[2] = V[3] = 0.0f;
	}

	Vector4(const Vector &v, float a = 1.0f)
	{
		V[0] = ((Vector &) v)[0];
		V[1] = ((Vector &) v)[1];
		V[2] = ((Vector &) v)[2];
		V[3] = a;
	}

	Vector4(const Vector4 &c1, const Vector4 &c2, float scal) 
	{
		V[0] = (c1[0]*(1-scal) + c2[0]*scal);
		V[1] = (c1[1]*(1-scal) + c2[1]*scal);
		V[2] = (c1[2]*(1-scal) + c2[2]*scal);
		V[3] = (c1[3]*(1-scal) + c2[3]*scal);
	}

	Vector4(const Vector4 &v)
	{
		V[0] = ((Vector4 &) v)[0];
		V[1] = ((Vector4 &) v)[1];
		V[2] = ((Vector4 &) v)[2];
		V[3] = ((Vector4 &) v)[3];
	}

	Vector4(const float Pt[4])
	{
		V[0] = Pt[0];
		V[1] = Pt[1];
		V[2] = Pt[2];
		V[3] = Pt[3];
	}

	Vector4(const float ptA, const float ptB, const float ptC, float ptD=0.0f)
	{
		V[0] = ptA;
		V[1] = ptB;
		V[2] = ptC;
		V[3] = ptD;
	}

	Vector4(const int ptA, const int ptB, const int ptC, const int ptD=0)
	{
		V[0] = (float) ptA;
		V[1] = (float) ptB;
		V[2] = (float) ptC;
		V[3] = (float) ptD;
	}

	void zero()
	{
		V[0] = V[1] = V[2] = V[3] = 0.0f;
	}

	bool operator==(const Vector4 &Vin1)
	{
		return (Vin1.V[0]==V[0] && Vin1.V[1]==V[1] && Vin1.V[2]==V[2] && Vin1.V[3]==V[3]);
	}

	bool operator!=(const Vector4 &Vin1)
	{
		return !((*this) == Vin1);
	}

	Vector4 &operator+=(const Vector4 &qc)
	{
		V[0] += qc.V[0];
		V[1] += qc.V[1];
		V[2] += qc.V[2];
		V[3] += qc.V[3];

		return *this;
	}

	Vector4 operator*(const Vector4 &qc)
	{
		Vector4 qa;
		Vector4 &qb = *this;
		
		// dQMultiply0 from ODE
		qa[0] = qb[0]*qc[0] - qb[1]*qc[1] - qb[2]*qc[2] - qb[3]*qc[3];
		qa[1] = qb[0]*qc[1] + qb[1]*qc[0] + qb[2]*qc[3] - qb[3]*qc[2];
		qa[2] = qb[0]*qc[2] + qb[2]*qc[0] + qb[3]*qc[1] - qb[1]*qc[3];
		qa[3] = qb[0]*qc[3] + qb[3]*qc[0] + qb[1]*qc[2] - qb[2]*qc[1];
	
		return qa;
	}

	///> component wise linear interpolation
	Vector4 lerp(Vector4 &c1, Vector4 &c2) 
	{
		return Vector4(
			c1[0] * (1.0f - V[0]) + c2[0] * V[0],
			c1[1] * (1.0f - V[1]) + c2[1] * V[1],
			c1[2] * (1.0f - V[2]) + c2[2] * V[2],
			c1[3] * (1.0f - V[3]) + c2[3] * V[3]);
	}

	void Normalize();

	// Quaternion maths
	void setQuatFromAxisAndAngle(Vector &axis, float angle);
	void getRotationMatrix(float *R); // R = float[4*3];
	void getOpenGLRotationMatrix(float *R); // R = float[16];
	void getRelativeVector(Vector &result, Vector &position);
	static void dDQfromW(Vector4 &dq, Vector &w, Vector4 &q);

	float &operator[](const int m) { DIALOG_ASSERT(m<=3); return V[m]; }
	float const &operator[](const int m) const { DIALOG_ASSERT(m<=3); return V[m]; }

	operator float*() { return V; }
	static Vector4 &getNullVector();

protected:
	float V[4];

};

#endif // !defined(AFX_Vector4_H__AD959187_7A1C_11D2_957C_00A0C9A4CA3E__INCLUDED_)

