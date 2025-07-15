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


// Triangle.h: interface for the Triangle class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRIANGLE_H__B076B031_36ED_11D3_BE80_000000000000__INCLUDED_)
#define AFX_TRIANGLE_H__B076B031_36ED_11D3_BE80_000000000000__INCLUDED_

#include <common/Line.h>

class Triangle
{
public:
	Triangle();
	virtual ~Triangle();

	Vector &getNormal() { return faceN_; }

	void setPointComponents(const float ptA1, const float ptA2, const float ptA3, 
		Vector &normalA,
		const float ptB1, const float ptB2, const float ptB3, 
		Vector &normalB,
		const float ptC1, const float ptC2, const float ptC3,
		Vector &normalC);

	bool pointInBoundingBox(const Vector &pt);
	bool pointInTriangle(const Vector &pt);

	virtual bool rayIntersect(
		const Line &ray, 
		Vector &intersectPt, 
		Vector &intersectN,
		float &intersectDist,
		const bool checkPtOnLine = true);

	virtual bool sphereIntersect(
		Vector &sphereCentre, 
		float &sphereRadius,
		Vector &intersectPt,
		Vector &intersectN,
		float &intersectDist);

protected:	
	enum largestNormalPart 
	{ 
		scalarX, 
		scalarY, 
		scalarZ 
	} largest_;

	Vector ptA_, ptB_, ptC_;
	Vector faceN_;

	void calcNormal();
	void calcLargest();
};

#endif // !defined(AFX_TRIANGLE_H__B076B031_36ED_11D3_BE80_000000000000__INCLUDED_)
