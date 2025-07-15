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

#if !defined(AFX_FixedVector4_H__AD959187_7A1C_11D2_957C_00A0C9A4CA3E__INCLUDED_)
#define AFX_FixedVector4_H__AD959187_7A1C_11D2_957C_00A0C9A4CA3E__INCLUDED_

#include <common/FixedVector.h>
#include <common/Vector4.h>

class FixedVector4  
{
public:
	FixedVector4()
	{
		V[0] = V[1] = V[2] = V[3] = 0;
	}

	FixedVector4(const FixedVector &v, fixed a = 1)
	{
		V[0] = ((FixedVector &) v)[0];
		V[1] = ((FixedVector &) v)[1];
		V[2] = ((FixedVector &) v)[2];
		V[3] = a;
	}

	FixedVector4(const FixedVector4 &cc1, const FixedVector4 &cc2, fixed scal) 
	{
		FixedVector4 &c1 = (FixedVector4 &) cc1;
		FixedVector4 &c2 = (FixedVector4 &) cc2;

		V[0] = (c1[0]*(fixed(1)-scal) + c2[0]*scal);
		V[1] = (c1[1]*(fixed(1)-scal) + c2[1]*scal);
		V[2] = (c1[2]*(fixed(1)-scal) + c2[2]*scal);
		V[3] = (c1[3]*(fixed(1)-scal) + c2[3]*scal);
	}

	FixedVector4(const FixedVector4 &v)
	{
		V[0] = ((FixedVector4 &) v)[0];
		V[1] = ((FixedVector4 &) v)[1];
		V[2] = ((FixedVector4 &) v)[2];
		V[3] = ((FixedVector4 &) v)[3];
	}

	FixedVector4(const fixed Pt[4])
	{
		V[0] = Pt[0];
		V[1] = Pt[1];
		V[2] = Pt[2];
		V[3] = Pt[3];
	}

	FixedVector4(const fixed ptA, const fixed ptB, const fixed ptC, fixed ptD=0)
	{
		V[0] = ptA;
		V[1] = ptB;
		V[2] = ptC;
		V[3] = ptD;
	}

	void zero()
	{
		V[0] = V[1] = V[2] = V[3] = 0;
	}

	bool operator==(const FixedVector4 &Vin)
	{
		FixedVector4 &Vin1 = (FixedVector4 &) Vin;
		return (Vin1.V[0]==V[0] && Vin1.V[1]==V[1] && 
			Vin1.V[2]==V[2] && Vin1.V[3]==V[3]);
	}

	bool operator!=(const FixedVector4 &Vin1)
	{
		return !((*this) == Vin1);
	}

	FixedVector4 &operator+=(const FixedVector4 &qc)
	{
		V[0] += qc.V[0];
		V[1] += qc.V[1];
		V[2] += qc.V[2];
		V[3] += qc.V[3];

		return *this;
	}

	FixedVector4 operator*(const FixedVector4 &qc)
	{
		FixedVector4 qa;
		FixedVector4 &qb = *this;
		
		// dQMultiply0 from ODE
		qa[0] = qb[0]*qc[0] - qb[1]*qc[1] - qb[2]*qc[2] - qb[3]*qc[3];
		qa[1] = qb[0]*qc[1] + qb[1]*qc[0] + qb[2]*qc[3] - qb[3]*qc[2];
		qa[2] = qb[0]*qc[2] + qb[2]*qc[0] + qb[3]*qc[1] - qb[1]*qc[3];
		qa[3] = qb[0]*qc[3] + qb[3]*qc[0] + qb[1]*qc[2] - qb[2]*qc[1];
	
		return qa;
	}

	///> component wise linear interpolation
	FixedVector4 lerp(FixedVector4 &c1, FixedVector4 &c2) 
	{
		return FixedVector4(
			c1[0] * (fixed(1) - V[0]) + c2[0] * V[0],
			c1[1] * (fixed(1) - V[1]) + c2[1] * V[1],
			c1[2] * (fixed(1) - V[2]) + c2[2] * V[2],
			c1[3] * (fixed(1) - V[3]) + c2[3] * V[3]);
	}

	void Normalize();

	Vector4 &asVector4()
	{
		static Vector4 a[10];
		static unsigned int count = 0;
		
		Vector4 &b = a[++count % 10];
		b[0] = V[0].asFloat();
		b[1] = V[1].asFloat();
		b[2] = V[2].asFloat();
		b[3] = V[3].asFloat();
		return b;
	}

	void asVector(Vector4 &dest)
	{
		dest[0] = V[0].asFloat();
		dest[1] = V[1].asFloat();
		dest[2] = V[2].asFloat();
		dest[3] = V[3].asFloat();
	}

	// Quaternion maths
	void setQuatFromAxisAndAngle(FixedVector &axis, fixed angle);
	void getRotationMatrix(fixed *R); // R = fixed[4*3];
	void getOpenGLRotationMatrix(float *R); // R = fixed[16];
	void getRelativeVector(FixedVector &result, FixedVector &position);
	static void dDQfromW(FixedVector4 &dq, FixedVector &w, FixedVector4 &q);

	fixed &operator[](const int m) { DIALOG_ASSERT(m<=3); return V[m]; }
	fixed const &operator[](const int m) const { DIALOG_ASSERT(m<=3); return V[m]; }

	static FixedVector4 &getNullVector();

protected:
	fixed V[4];

};

#endif // !defined(AFX_FixedVector4_H__AD959187_7A1C_11D2_957C_00A0C9A4CA3E__INCLUDED_)

