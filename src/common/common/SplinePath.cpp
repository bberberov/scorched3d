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

#include <common/SplinePath.h>
#include <common/SplineCurve.h>
#include <common/Defines.h>
#include <math.h>

SplinePath::SplinePath() : 
	pathTime_(0), pointsPerSecond_(0)
{
}

SplinePath::~SplinePath()
{
}

void SplinePath::generate(
	std::vector<FixedVector> &inPoints, 
	int resolution,
	int polynomials,
	fixed pointsPerSecond)
{
	pointsPerSecond_ = pointsPerSecond;
	controlPoints_ = inPoints;
	SplineCurve::generate(controlPoints_, pathPoints_, 
		resolution, polynomials);

	DIALOG_ASSERT(pointsPerSecond_ > 0);
	DIALOG_ASSERT(!controlPoints_.empty());
	DIALOG_ASSERT(!pathPoints_.empty());
}

void SplinePath::simulate(fixed frameTime)
{
	pathTime_ += frameTime;
}

void SplinePath::getPathAttrs(FixedVector &position, FixedVector &direction)
{
	fixed currentPointTime = pathTime_ * pointsPerSecond_;

	unsigned int noPoints = (unsigned int) pathPoints_.size();
	unsigned int currentPointId = (unsigned int) (currentPointTime).asInt();
	fixed currentPointDiff = currentPointTime - fixed(currentPointId);
	currentPointId = currentPointId % noPoints;
	unsigned int nextPointId = currentPointId + 1;
	nextPointId = nextPointId % noPoints;
	unsigned int nextNextPointId = currentPointId + 2;
	nextNextPointId = nextNextPointId % noPoints;

	FixedVector &currentPoint = pathPoints_[currentPointId];
	FixedVector &nextPoint = pathPoints_[nextPointId];
	FixedVector &nextNextPoint = pathPoints_[nextNextPointId];

	FixedVector diff = nextPoint - currentPoint;
	FixedVector nextDiff = nextNextPoint - nextPoint;
	FixedVector diffDiff = nextDiff - diff;

	diffDiff *= currentPointDiff;
	direction = diff;
	direction += diffDiff;
	direction.StoreNormalize();

	diff *= currentPointDiff;
	position = currentPoint;
	position += diff;
}

#ifndef S3D_SERVER
#include <GLEXT/GLState.h>
#endif

void SplinePath::draw()
{
#ifndef S3D_SERVER
	std::vector<FixedVector>::iterator itor;
	GLState state(GLState::TEXTURE_OFF | GLState::BLEND_OFF);
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_LINE_LOOP);
		for (itor = pathPoints_.begin();
			itor != pathPoints_.end();
			++itor)
		{
			Vector &pt = (*itor).asVector();
			glVertex3fv(pt);
		}
	glEnd();

	glColor3f(1.0f, 1.0f, 0.0f);
	glPointSize(3.0f);
	glBegin(GL_POINTS);
		for (itor = controlPoints_.begin();
			itor != controlPoints_.end();
			++itor)
		{
			Vector &pt = (*itor).asVector();
			glVertex3fv(pt);
		}
		glColor3f(0.5f, 0.5f, 1.0f);
		FixedVector position, direction;
		getPathAttrs(position, direction);
		glVertex3fv(position.asVector());
	glEnd();

	glPointSize(1.0f);
#endif // #ifndef S3D_SERVER
}
