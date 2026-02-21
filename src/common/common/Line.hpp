////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011, 2026
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

#ifndef __INCLUDE_Line_hpp_INCLUDE__
#define __INCLUDE_Line_hpp_INCLUDE__

// Line.hpp: interface for the Line class.
//
//////////////////////////////////////////////////////////////////////

#include <common/Vector.hpp>

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

#endif // __INCLUDE_Line_hpp_INCLUDE__
