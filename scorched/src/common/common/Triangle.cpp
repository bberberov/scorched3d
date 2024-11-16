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


// Triangle.cpp: implementation of the Triangle class.
//
//////////////////////////////////////////////////////////////////////

#include <math.h>
#include <common/Triangle.h>
#include <common/Defines.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Triangle::Triangle()
{

}

Triangle::~Triangle()
{

}

void Triangle::setPointComponents(const float ptA1, const float ptA2, const float ptA3, 
	Vector &normalA,
	const float ptB1, const float ptB2, const float ptB3, 
	Vector &normalB,
	const float ptC1, const float ptC2, const float ptC3,
	Vector &normalC)
{
	ptA_.initialise(ptA1, ptA2, ptA3);
	ptB_.initialise(ptB1, ptB2, ptB3);
	ptC_.initialise(ptC1, ptC2, ptC3);

	faceN_ += normalA;
	faceN_ += normalB;
	faceN_ += normalC;
	faceN_ /= 3.0f;
	calcLargest();
}

void Triangle::calcLargest()
{
	largest_ = scalarZ;
	float f0 = fabsf(faceN_[0]);
	float f1 = fabsf(faceN_[1]);
	float f2 = fabsf(faceN_[2]);
	if (f0 >= f1)
	{
		if (f0 >= f2) largest_ = scalarX;
	}
	else 
	{
		if (f1 >= f2) largest_ = scalarY;
	}
}

bool Triangle::pointInBoundingBox(const Vector &pt)
{
	float max0 = MAX(ptA_[0], MAX(ptB_[0], ptC_[0]));
	float max1 = MAX(ptA_[1], MAX(ptB_[1], ptC_[1]));
	float max2 = MAX(ptA_[2], MAX(ptB_[2], ptC_[2]));
	float min0 = MIN(ptA_[0], MIN(ptB_[0], ptC_[0]));
	float min1 = MIN(ptA_[1], MIN(ptB_[1], ptC_[1]));
	float min2 = MIN(ptA_[2], MIN(ptB_[2], ptC_[2]));

	Vector &Pt = (Vector &) pt;
	if (Pt[0] < min0-0.1f || Pt[0] > max0+0.1f) return false;
	if (Pt[1] < min1-0.1f || Pt[1] > max1+0.1f) return false;
	if (Pt[2] < min2-0.1f || Pt[2] > max2+0.1f) return false;

	return true;
}

bool Triangle::pointInTriangle(const Vector &pt)
{
	Vector u, v;
	switch (largest_)
	{
	case scalarX:
		u[0] = pt[1] - ptA_[1];
		v[0] = pt[2] - ptA_[2];
		u[1] = ptB_[1] - ptA_[1];
		v[1] = ptB_[2] - ptA_[2];
		u[2] = ptC_[1] - ptA_[1];
		v[2] = ptC_[2] - ptA_[2];
		break;
	case scalarY:
		u[0] = pt[0] - ptA_[0];
		v[0] = pt[2] - ptA_[2];
		u[1] = ptB_[0] - ptA_[0];
		v[1] = ptB_[2] - ptA_[2];
		u[2] = ptC_[0] - ptA_[0];
		v[2] = ptC_[2] - ptA_[2];
		break;
	case scalarZ:
		u[0] = pt[0] - ptA_[0];
		v[0] = pt[1] - ptA_[1];
		u[1] = ptB_[0] - ptA_[0];
		v[1] = ptB_[1] - ptA_[1];
		u[2] = ptC_[0] - ptA_[0];
		v[2] = ptC_[1] - ptA_[1];
		break;
	default:
		return false;
		break;
	}

	float alpha, beta;
	if (u[1] == 0.0) 
	{
		if (u[2] != 0.0) beta = u[0] / u[2];
		else return false;
		if (beta>=0.0 && beta<=1.0) 
		{
			alpha = (v[0] - beta * v[2]) / v[1];
			if (alpha<0.0 || alpha>1.0) return false;
		}
		else return false;
	}
	else 
	{
		beta=(v[0] * u[1] - u[0] * v[1]) / (v[2] * u[1] - u[2] * v[1]);
		if (beta>=0.0 && beta<=1.0) 
		{
			alpha = (u[0] - beta * u[2]) / u[1];
			if (alpha<0.0 || alpha>1.0) return false;
		}
		else return false;
	}

	if (alpha+beta > 1.0) return false;
	return true;
}

bool Triangle::rayIntersect(const Line &ray, 
							Vector &intersectPt, 
							Vector &intersectN,
							float &intersectDist,
							const bool checkPtOnLine)
{
	// Remove const
	Line &Ray = (Line &) ray;

	// Only do intersections for triangles facing the ray
	float sd = faceN_.dotP((Vector &) Ray.getDirection());
	if (sd == 0.0f) return false;

	// Get the intersection point of the ray and the triangles
	// plane
	// Expand all vector maths so we create as few tmp vector
	// objects as possible, reuse intersectPt as temporary
	intersectPt = ptA_;
	intersectPt -= Ray.getStart();
	float si = -intersectPt.dotP(faceN_) / sd;

	intersectPt = (Vector &) Ray.getDirection();
	intersectPt *= - si;
	intersectPt += Ray.getStart();
	intersectDist = -si;
	intersectN = faceN_;

	// Check the intersection point is in the ray
	if (checkPtOnLine)
	{
		if (si > 0.0f || si < -1.0) return false;
	}

	// Check the intersection point is in the triangle
	if (!pointInBoundingBox(intersectPt)) return false;
	if (!pointInTriangle(intersectPt)) return false;
	return true;
}

bool Triangle::sphereIntersect(
		Vector &sphereCentre, 
		float &sphereRadius,
		Vector &intersectPt,
		Vector &intersectN,
		float &intersectDist)
{
	Vector actualPosition = sphereCentre + (faceN_ * sphereRadius);
	Vector destPosition = sphereCentre - (faceN_ * sphereRadius);
	Line direction(actualPosition, destPosition);

	if (rayIntersect(direction, intersectPt, intersectN, intersectDist, true))
	{
		float diameter = sphereRadius + sphereRadius;

		intersectDist = (diameter) - (intersectDist * diameter);
		return true;
	}
	return false;
}
