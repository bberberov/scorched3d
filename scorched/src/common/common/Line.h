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


// Line.h: interface for the Line class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Line_H__54F37DA5_36EB_11D3_BE80_000000000000__INCLUDED_)
#define AFX_Line_H__54F37DA5_36EB_11D3_BE80_000000000000__INCLUDED_

#include <common/Vector.h>

class Line  
{
public:
	Line();
	Line(Vector &start, Vector &end);
	virtual ~Line();

	float dotP(const Line &line);
	bool intersect(const Line &line, Vector &interPt, const bool checkPtOnLine = true);
	Vector operator*(const Line &line);

	const Vector &getDirection();
	const Vector &getStart();
	const Vector &getEnd();
	void setStart(const Vector &start);
	void setEnd(const Vector &end);
	void setPoints(const Vector &start, const Vector &end);

	Vector get2DPerp();

protected:
	Vector start_, end_;
	Vector dir_;
};

#endif // !defined(AFX_Line_H__54F37DA5_36EB_11D3_BE80_000000000000__INCLUDED_)
