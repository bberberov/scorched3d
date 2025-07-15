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


// Line.cpp: implementation of the Line class.
//
//////////////////////////////////////////////////////////////////////

#include <common/Line.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Line::Line()
{

}

Line::Line(Vector &start, Vector &end)
{
	start_ = start;
	end_ = end;

	dir_ = end_ - start_;
}

Line::~Line()
{

}

void Line::setPoints(const Vector &start, const Vector &end)
{
	start_ = start;
	end_ = end;
	dir_ = end_ - start_;
}

void Line::setStart(const Vector &start)
{
	start_ = start;
	dir_ = end_ - start_;
}

void Line::setEnd(const Vector &end)
{
	end_ = end;
	dir_ = end_ - start_;
}

Vector Line::operator*(const Line &line)
{
	return dir_ * line.dir_;
}

float Line::dotP(const Line &line)
{
	return dir_.dotP(line.dir_);
}

Vector Line::get2DPerp()
{
	return dir_.get2DPerp();
}

bool Line::intersect(const Line &line, Vector &interPt, const bool checkPtOnLine)
{
	if (dotP(line) == 0.0f) return false;

	Vector &dir1 = (Vector &) dir_;
	Vector &dir2 = (Vector &) ((Line &)line).getDirection();

	Vector pDir1 = Vector(dir1[1], -dir1[0], 0.0f);
	Vector pDir2 = Vector(dir2[1], -dir2[0], 0.0f);

	float u1 = pDir2.dotP((Vector &)((Line &)line).getStart() - start_) / pDir2.dotP(dir1);
	float u2 = pDir1.dotP(start_ - ((Line &)line).getStart()) / pDir1.dotP(dir2);

	if (checkPtOnLine)
	{
		if (u1 < 0.0f || u1 > 1.0f) return false;
		if (u2 < 0.0f || u2 > 1.0f) return false;
	}

	interPt = start_ + dir1 * u1;

	return true;
}

const Vector &Line::getDirection()
{
	return dir_;
}

const Vector &Line::getStart()
{
	return start_;
}

const Vector &Line::getEnd()
{
	return end_;
}
