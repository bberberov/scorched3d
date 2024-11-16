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

#if !defined(AFX_Matrix16_H__AD959187_7A1C_11D2_957C_00A0C9A4CA3E__INCLUDED_)
#define AFX_Matrix16_H__AD959187_7A1C_11D2_957C_00A0C9A4CA3E__INCLUDED_

#include <common/Defines.h>
#include <math.h>

class Matrix16  
{
public:
	Matrix16(float *init = 0);
	~Matrix16();

	void multiply(float *b);
	void scale(float x, float y, float z);
	void translate(float x, float y, float z);
	void identity();

	float &operator[](const int m) { DIALOG_ASSERT(m<=15); return M[m]; }
	float const &operator[](const int m) const { DIALOG_ASSERT(m<=15); return M[m]; }

	operator float*() { return M; }

protected:
	float M[16];

};

#endif // !defined(AFX_Matrix16_H__AD959187_7A1C_11D2_957C_00A0C9A4CA3E__INCLUDED_)

