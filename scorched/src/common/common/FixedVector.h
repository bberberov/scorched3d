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

#if !defined(AFX_FixedVector_H__AD959187_7A1C_11D2_957C_00A0C9A4CA3E__INCLUDED_)
#define AFX_FixedVector_H__AD959187_7A1C_11D2_957C_00A0C9A4CA3E__INCLUDED_

#include <common/DefinesAssert.h>
#include <common/Vector.h>
#include <common/fixed.h>

class FixedVector  
{
public:
	FixedVector()
	{
		V[0] = V[1] = V[2] = fixed(0);
	}

	FixedVector(fixed a, fixed b, fixed c)
	{
		V[0] = a;
		V[1] = b;
		V[2] = c;
	}

	FixedVector(const fixed Pt[3])
	{
		V[0] = Pt[0];
		V[1] = Pt[1];
		V[2] = Pt[2];
	}

	FixedVector(const FixedVector &v)
	{
		V[0] = ((FixedVector &) v)[0];
		V[1] = ((FixedVector &) v)[1];
		V[2] = ((FixedVector &) v)[2];
	}

	void zero()
	{
		V[0] = V[1] = V[2] = fixed(0);
	}

	FixedVector Normalize()
	{
		fixed mag = Magnitude();
		FixedVector v;
		if (mag == fixed(0)) mag = fixed(true, 1);
		v = (*this) / mag;
		return v;
	}

	fixed Magnitude();

	FixedVector get2DPerp()
	{
		FixedVector v(V[1], -V[0], 0);
		return v;
	}

	fixed dotP(const FixedVector &Vin)
	{
		FixedVector &V1 = (*this);
		FixedVector &V2 = (FixedVector &) Vin;
		return (V1.V[0] * V2.V[0]) + (V1.V[1] * V2.V[1]) + (V1.V[2] * V2.V[2]);
	}

	fixed dotP2D(const FixedVector &Vin)
	{
		FixedVector &V1 = (*this);
		FixedVector &V2 = (FixedVector &) Vin;
		return (V1.V[0] * V2.V[0]) + (V1.V[1] * V2.V[1]);
	}

	void StoreInvert()
	{
		V[0] = -V[0];
		V[1] = -V[1];
		V[2] = -V[2];
	}

	fixed Max()
	{
		return MAX(V[0], MAX(V[1], V[2]));
	}

	void StoreNormalize()
	{
		fixed mag = Magnitude();
		if (mag == fixed(0)) mag = fixed(true, 1);
		(*this) /= mag;
	}

	FixedVector operator+(const fixed m)
	{
		FixedVector v(V[0]+m, V[1]+m, V[2]+m);
		return v;
	}

	FixedVector operator+(const FixedVector &Vin)
	{
		FixedVector v(
			((FixedVector &) Vin).V[0] + V[0], 
			((FixedVector &) Vin).V[1] + V[1], 
			((FixedVector &) Vin).V[2] + V[2]);
		return v;
	}

	FixedVector operator-(const fixed m)
	{
		FixedVector v(V[0]-m, V[1]-m, V[2]-m);
		return v;
	}

	FixedVector operator-(const FixedVector &Vin)
	{
		FixedVector v(
			V[0] - ((FixedVector &) Vin).V[0], 
			V[1] - ((FixedVector &) Vin).V[1], 
			V[2] - ((FixedVector &) Vin).V[2]);
		return v;
	}

	void operator*=(const FixedVector &Vin)
	{
		fixed a = V[1] * ((FixedVector &)Vin)[2] - V[2] * ((FixedVector &)Vin)[1];
		fixed b = V[2] * ((FixedVector &)Vin)[0] - V[0] * ((FixedVector &)Vin)[2];
		fixed c = V[0] * ((FixedVector &)Vin)[1] - V[1] * ((FixedVector &)Vin)[0];
		V[0] = a;
		V[1] = b;
		V[2] = c;
	}

	FixedVector operator*(const FixedVector &Vin)
	{
		FixedVector v(
			V[1] * ((FixedVector &)Vin)[2] - V[2] * ((FixedVector &)Vin)[1], 
			V[2] * ((FixedVector &)Vin)[0] - V[0] * ((FixedVector &)Vin)[2],
			V[0] * ((FixedVector &)Vin)[1] - V[1] * ((FixedVector &)Vin)[0]);

		return v;
	}

	FixedVector operator*(const fixed a)
	{
		FixedVector v(V[0]*a, V[1]*a, V[2]*a);
		return v;
	}

	FixedVector operator/(const fixed a)
	{
		const fixed b = (fixed(a)==fixed(0)?fixed(true, 1):a);
		FixedVector v(V[0]/b, V[1]/b, V[2]/b);
		return v;
	}

	FixedVector operator-()
	{
		fixed a = -V[0];
		FixedVector v(-V[0], -V[1], -V[2]);
		return v;
	}

	Vector &asVector()
	{
		static Vector a[10];
		static unsigned int count = 0;
		
		Vector &b = a[++count % 10];
		b[0] = V[0].asFloat();
		b[1] = V[1].asFloat();
		b[2] = V[2].asFloat();
		return b;
	}

	void asVector(Vector &dest)
	{
		dest[0] = V[0].asFloat();
		dest[1] = V[1].asFloat();
		dest[2] = V[2].asFloat();
	}

	static FixedVector &fromVector(Vector &vec)
	{
		static FixedVector result;
		result[0] = fixed::fromFloat(vec[0]);
		result[1] = fixed::fromFloat(vec[1]);
		result[2] = fixed::fromFloat(vec[2]);
		return result;
	}

	void operator*=(const fixed a)
	{
		V[0] *= a;
		V[1] *= a;
		V[2] *= a;
	}

	void operator/=(const fixed a)
	{
		const fixed b = (fixed(a)==fixed(0)?fixed(true, 1):a);
		V[0] /= b;
		V[1] /= b;
		V[2] /= b;
	}

	void operator/=(const FixedVector &Vin)
	{
		fixed a = ((FixedVector &)Vin)[0];
		fixed b = ((FixedVector &)Vin)[1];
		fixed c = ((FixedVector &)Vin)[2];

		const fixed a2 = (a==0?fixed(true, 1):a);
		const fixed b2 = (b==0?fixed(true, 1):b);
		const fixed c2 = (c==0?fixed(true, 1):c);

		V[0] /= a2;
		V[1] /= b2;
		V[2] /= c2;
	}

	void operator+=(const fixed a)
	{
		V[0] += a;
		V[1] += a;
		V[2] += a;
	}

	void operator+=(const FixedVector &Vin)
	{
		V[0] += Vin.V[0];
		V[1] += Vin.V[1];
		V[2] += Vin.V[2];
	}

	void operator-=(const fixed a)
	{
		(*this) += -fixed(a);
	}

	void operator-=(const FixedVector &Vin)
	{
		V[0] -= Vin.V[0];
		V[1] -= Vin.V[1];
		V[2] -= Vin.V[2];
	}

	bool operator==(const FixedVector &Vin)
	{
		return (
			((FixedVector &) Vin).V[0]==V[0] && 
			((FixedVector &) Vin).V[1]==V[1] && 
			((FixedVector &) Vin).V[2]==V[2]);
	}

	bool operator!=(const FixedVector &Vin1)
	{
		return !((*this) == Vin1);
	}

	fixed &operator[](const int m) { DIALOG_ASSERT(m<=2); return V[m]; }
	fixed const &operator[](const int m) const { DIALOG_ASSERT(m<=2); return V[m]; }

	const char *asQuickString();

	operator fixed*() { return V; }
	static FixedVector &getNullVector();
	static FixedVector &getMaxVector();
	static FixedVector &getMinVector();

protected:
	fixed V[3];

};

#endif // !defined(AFX_FixedVector_H__AD959187_7A1C_11D2_957C_00A0C9A4CA3E__INCLUDED_)

