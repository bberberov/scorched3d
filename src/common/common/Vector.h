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

#if !defined(AFX_VECTOR_H__AD959187_7A1C_11D2_957C_00A0C9A4CA3E__INCLUDED_)
#define AFX_VECTOR_H__AD959187_7A1C_11D2_957C_00A0C9A4CA3E__INCLUDED_

#include <common/Defines.h>
#include <math.h>

class Vector  
{
public:
	Vector()
	{
		V[0] = V[1] = V[2] = 0.0f;
	}

	Vector(const Vector &v)
	{
		V[0] = ((Vector &) v)[0];
		V[1] = ((Vector &) v)[1];
		V[2] = ((Vector &) v)[2];
	}

	Vector(const float Pt[3])
	{
		V[0] = Pt[0];
		V[1] = Pt[1];
		V[2] = Pt[2];
	}

	Vector(const float ang, const double length)
	{
		V[0] = (float) sin(ang / 180.0f * 3.14f) * float(length);
		V[1] = (float) cos(ang / 180.0f * 3.14f) * float(length);
		V[2] = 0.0f;
	}

	Vector(const float ptA, const float ptB, const float ptC=0.0f)
	{
		V[0] = ptA;
		V[1] = ptB;
		V[2] = ptC;
	}

	Vector(const int ptA, const int ptB, const int ptC=0)
	{
		V[0] = (float) ptA;
		V[1] = (float) ptB;
		V[2] = (float) ptC;
	}

	void initialise(const float a, const float b, const float c)
	{
		V[0] = a;
		V[1] = b;
		V[2] = c;
	}

	Vector Normalize()
	{
		float mag = Magnitude();
		if (mag == 0.0f) mag = 0.00001f;
		Vector v(V[0] / mag, V[1] / mag, V[2] / mag);
		return v;
	}

	Vector Normalize2D()
	{
		float mag = Magnitude2d();
		if (mag == 0.0f) mag = 0.00001f;
		Vector v(V[0] / mag, V[1] / mag, V[2] / mag);
		return v;
	}

	float Magnitude()
	{
		return float(sqrt(MagnitudeSquared()));
	}

	float Magnitude2d()
	{
		return float(sqrt(Magnitude2dSquared()));
	}

	float Magnitude2dSquared()
	{
		return V[0]*V[0] + V[1]*V[1];
	}

	float MagnitudeSquared()
	{
		return V[0]*V[0] + V[1]*V[1] + V[2]*V[2];
	}

	float dotP(const Vector &Vin)
	{
		Vector &V2 = (Vector &) Vin;
		return (V[0] * V2.V[0]) + (V[1] * V2.V[1]) + (V[2] * V2.V[2]);
	}

	float Max()
	{
		return MAX(V[0], MAX(V[1], V[2]));
	}

	Vector get2DPerp()
	{
		Vector v(V[1], -V[0], 0.0f);
		return v;
	}

	void StoreInvert()
	{
		V[0] = -V[0];
		V[1] = -V[1];
		V[2] = -V[2];
	}

	void StoreNormalize()
	{
		float mag = Magnitude();
		if (mag == 0.0f) mag = 0.00001f;
		V[0] /= mag;
		V[1] /= mag;
		V[2] /= mag;
	}

	void zero()
	{
		V[0] = V[1] = V[2] = 0.0f;
	}

	Vector operator+(const float m)
	{
		Vector v(V[0]+m, V[1]+m, V[2]+m);
		return v;
	}

	Vector operator+(const Vector &Vin)
	{
		Vector v(Vin.V[0] + V[0], Vin.V[1] + V[1], Vin.V[2] + V[2]);
		return v;
	}

	Vector operator-(const float m)
	{
		Vector v(V[0]-m, V[1]-m, V[2]-m);
		return v;
	}

	Vector operator-(const Vector &Vin)
	{
		Vector v(V[0] - Vin.V[0], V[1] - Vin.V[1], V[2] - Vin.V[2]);
		return v;
	}

	Vector operator*(const float a)
	{
		Vector v(V[0]*a, V[1]*a, V[2]*a);
		return v;
	}

	Vector operator*(const Vector &Vin)
	{
		Vector v(V[1] * ((Vector &)Vin)[2] - V[2] * ((Vector &)Vin)[1], 
				V[2] * ((Vector &)Vin)[0] - V[0] * ((Vector &)Vin)[2],
				V[0] * ((Vector &)Vin)[1] - V[1] * ((Vector &)Vin)[0]);

		return v;
	}

	Vector operator/(const float a)
	{
		const float b = (a==0.0f?0.00001f:a);
		Vector v(V[0]/b, V[1]/b, V[2]/b);
		return v;
	}

	Vector operator/(const Vector &Vin)
	{
		float a = ((Vector &)Vin)[0];
		float b = ((Vector &)Vin)[1];
		float c = ((Vector &)Vin)[2];

		const float a2 = (a==0.0f?0.00001f:a);
		const float b2 = (b==0.0f?0.00001f:b);
		const float c2 = (c==0.0f?0.00001f:c);

		Vector v(V[0]/ a2, V[1]/ b2, V[2]/ c2);
		return v;
	}

	Vector operator-()
	{
		Vector v(-V[0], -V[1], -V[2]);
		return v;
	}

	void operator*=(const float a)
	{
		V[0] *= a;
		V[1] *= a;
		V[2] *= a;
	}

	void operator*=(const Vector &Vin)
	{
		float a = V[1] * ((Vector &)Vin)[2] - V[2] * ((Vector &)Vin)[1];
		float b = V[2] * ((Vector &)Vin)[0] - V[0] * ((Vector &)Vin)[2];
		float c = V[0] * ((Vector &)Vin)[1] - V[1] * ((Vector &)Vin)[0];
		V[0] = a;
		V[1] = b;
		V[2] = c;
	}

	void operator/=(const float a)
	{
		const float b = (a==0.0f?0.00001f:a);
		V[0] /= b;
		V[1] /= b;
		V[2] /= b;
	}

	void operator/=(const Vector &Vin)
	{
		float a = ((Vector &)Vin)[0];
		float b = ((Vector &)Vin)[1];
		float c = ((Vector &)Vin)[2];

		const float a2 = (a==0.0f?0.00001f:a);
		const float b2 = (b==0.0f?0.00001f:b);
		const float c2 = (c==0.0f?0.00001f:c);

		V[0] /= a2;
		V[1] /= b2;
		V[2] /= c2;
	}

	void operator+=(const float a)
	{
		V[0] += a;
		V[1] += a;
		V[2] += a;
	}

	void operator+=(const Vector &Vin)
	{
		V[0] += Vin.V[0];
		V[1] += Vin.V[1];
		V[2] += Vin.V[2];
	}

	void operator-=(const float a)
	{
		V[0] -= a;
		V[1] -= a;
		V[2] -= a;
	}

	void operator-=(const Vector &Vin)
	{
		V[0] -= Vin.V[0];
		V[1] -= Vin.V[1];
		V[2] -= Vin.V[2];
	}

	bool operator==(const Vector &Vin1)
	{
		return (Vin1.V[0]==V[0] && Vin1.V[1]==V[1] && Vin1.V[2]==V[2]);
	}

	bool operator!=(const Vector &Vin1)
	{
		return (Vin1.V[0]!=V[0] || Vin1.V[1]!=V[1] || Vin1.V[2]!=V[2]);
	}

	float &operator[](const int m) { DIALOG_ASSERT(m<=2); return V[m]; }
	float const &operator[](const int m) const { DIALOG_ASSERT(m<=2); return V[m]; }

	operator float*() { return V; }
	static Vector &getNullVector();

protected:
	float V[3];

};

#endif // !defined(AFX_VECTOR_H__AD959187_7A1C_11D2_957C_00A0C9A4CA3E__INCLUDED_)

