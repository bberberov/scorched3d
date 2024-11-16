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

#include <math.h>
#include <stdlib.h>
#include <common/Defines.h>
#include <GLEXT/GLState.h>
#include <GLEXT/GLCamera.h>
#include <GLEXT/GLViewPort.h>

GLCamera *GLCamera::currentCamera_ = 0;

GLCamera::GLCamera(GLsizei windowWidth, GLsizei windowHeight) :
	rotationXY_(0.0f), rotationYZ_(PI / 4), zoom_(150.0f),
	useHeightFunc_(false), minHeightFunc_(0), maxHeightFunc_(0), 
	totalTime_(0.0f), 
	shake_(0.0f), minHeightData_(0), maxHeightData_(0)
{
	currentCamera_ = this;
	setWindowOffset(0, 0);
	setWindowSize(windowWidth, windowHeight);
	calculateWantedOffset();
	currentPosition_ = wantedOffset_ + lookAt_;
}

GLCamera::~GLCamera()
{

}

void GLCamera::setUseHeightFunc(bool toggle)
{
	useHeightFunc_ = toggle;
}

void GLCamera::setMinHeightFunc(MinHeightFunc func, void *data)
{
	minHeightFunc_ = func;
	minHeightData_ = data;
	if (func)
	{
		useHeightFunc_ = true;
	}
}

void GLCamera::setMaxHeightFunc(MaxHeightFunc func, void *data)
{
	maxHeightFunc_ = func;
	maxHeightData_ = data;
	if (func)
	{
		useHeightFunc_ = true;
	}
}

void GLCamera::setLookAt(Vector &lookAt, bool instant)
{
	wantedLookAt_ = lookAt;
	if (instant)
	{
		lookAt_ = lookAt;
	}
}

void GLCamera::setCurrentPos(Vector &pos)
{
	currentPosition_ = pos;
}

void GLCamera::setOffSet(Vector &offSet, bool instant)
{
	wantedOffset_ = offSet;
 	if (instant)
	{
		currentPosition_ = wantedOffset_ + lookAt_;
	}
}

void GLCamera::setWindowSize(GLsizei windowWidth, GLsizei windowHeight)
{
	windowW_ = windowWidth;
	windowH_ = windowHeight;
	windowAspect_ = float(windowWidth) / float(windowHeight?windowHeight:0.0001);
}

void GLCamera::setWindowOffset(GLsizei windowLeft, GLsizei windowTop)
{
	windowL_ = windowLeft;
	windowT_ = windowTop;
}

void GLCamera::calculateWantedOffset()
{
	wantedOffset_[0] =  zoom_ * float(sin(rotationXY_) * sin(rotationYZ_));
	wantedOffset_[1] =  zoom_ * float(cos(rotationXY_) * sin(rotationYZ_));
	wantedOffset_[2] =  zoom_ * float(cos(rotationYZ_));
}

void GLCamera::moveViewport(Vector &lookFrom, Vector &lookAt)
{
	GLfloat lz(lookFrom[2]);

	Vector direction = lookAt - lookFrom;
	direction[2] = 0.0f;
	direction.StoreNormalize();
	direction *= 3.0f;
	if (minHeightFunc_ && useHeightFunc_)
	{
		lz = MAX(lz, (*minHeightFunc_)(int(lookFrom[0]), 
			int(lookFrom[1]), minHeightData_));
		lz = MAX(lz, (*minHeightFunc_)(int(lookFrom[0] + direction[0]), 
			int(lookFrom[1] + direction[1]), minHeightData_));
	}
	if (maxHeightFunc_ && useHeightFunc_)
	{
		lz = MIN(lz, (*maxHeightFunc_)(int(lookFrom[0]),
			int(lookFrom[1]), maxHeightData_));
		lz = MIN(lz, (*maxHeightFunc_)(int(lookFrom[0] + direction[0]),
			int(lookFrom[1] + direction[1]), maxHeightData_));
	}

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	
	glViewport(windowL_, windowT_, windowW_, windowH_);
	gluPerspective(60.0f, windowAspect_, 1.0f, 2500.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(lookFrom[0], lookFrom[1], lz, lookAt[0], lookAt[1], lookAt[2] ,0.0f, 0.0f, 1.0f);
}

void GLCamera::simulate(float frameTime)
{
	const float SecondsToReachTarget = 0.15f;
	const float ShakeDecrease = 0.06f;

	// Make some constant changes, regardless of framerate
	totalTime_ += frameTime;
	while (totalTime_ > 0.03f)
	{
		totalTime_ -= 0.03f;

		// Calculate any camera shake
		shake_ -= ShakeDecrease;
		if (shake_ > 0.0f)
		{
			shakeV_[0] = RAND * shake_;
			shakeV_[1] = RAND * shake_;
			shakeV_[2] = RAND * shake_;
		}
		else 
		{
			shake_ = 0.0f;
			shakeV_.zero();
		}

		// Calculate the new look at value
		Vector directionLookAt = wantedLookAt_ - lookAt_;
		directionLookAt	*= SecondsToReachTarget;
		lookAt_ += directionLookAt;

		// Calculate the new look from value
		Vector wantedPosition = wantedOffset_ + lookAt_;
		Vector directionPosition = wantedPosition - currentPosition_;
		directionPosition *= SecondsToReachTarget;
		currentPosition_ += directionPosition;
		velocity_ = directionPosition;
	}
}

void GLCamera::draw()
{
	static Vector look;
	currentCamera_ = this;
	look = lookAt_;
	look += shakeV_;
	moveViewport(currentPosition_, look);
}

void GLCamera::addShake(float shake)
{
	shake_ += shake;
	if (shake_ > 5.0f) shake_ = 5.0f;
}

void GLCamera::movePosition(float XY, float YZ, float Z)
{
	rotationXY_ = XY;
	rotationYZ_ = YZ;
	zoom_ = Z;

	calculateWantedOffset();
}

void GLCamera::movePositionDelta(float XY, float YZ, float Z)
{
	XY += rotationXY_;
	YZ += rotationYZ_;
	Z += zoom_;

	if (YZ < 0.17f) YZ = 0.17f;
	if (YZ > 1.91f) YZ = 1.91f;
	if (Z < 5.0) Z = 5.0f;
	if (Z > 250.0f) Z = 250.0f;

	movePosition(XY, YZ, Z);
}

bool GLCamera::getDirectionFromPt(GLfloat winX, GLfloat winY, Line &direction)
{
	static GLdouble modelMatrix[16];
	static GLdouble projMatrix[16];
	static GLint viewport[4];

	glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
	glGetIntegerv(GL_VIEWPORT, viewport);

	float realX = winX / GLViewPort::getWidthMult();
	float realY = GLViewPort::getActualHeight() - 
		((GLViewPort::getHeight() - winY) / GLViewPort::getHeightMult());

	GLdouble x,y,z;
	gluUnProject(
		realX,
		realY,
		0.0f,
		modelMatrix,
		projMatrix,
		viewport,
		&x,
		&y,
		&z);
	Vector pos1((float) x, (float) y, (float) z);

	gluUnProject(
		realX,
		realY,
		100.0f,
		modelMatrix,
		projMatrix,
		viewport,
		&x,
		&y,
		&z);

	Vector pos2((float) x, (float) y, (float) z);

	Vector dir = pos2 - pos1;

	float dist = dir[2];
	if (dist == 0.0f)
	{
		dist = 0.000001f;
	}
	float number = (float) fabs(pos2[2] / dist);

	Vector groundPos = pos2 - dir * number;
	direction.setStart(groundPos);
	direction.setEnd(pos2);

	return true;
}

void GLCamera::scroll(ScrollDir direction, 
	float minWidth, float minHeight,
	float maxWidth, float maxHeight, 
	float amount)
{
	float x = 0.0f, y = 0.0f;
	switch (direction)
	{
	case eScrollLeft:
		x = -amount;
		break;
	case eScrollRight:
		x = amount;
		break;
	case eScrollUp:
		y = -amount;
		break;
	case eScrollDown:
		y = amount;
		break;
	}
	scroll(x, y, minWidth, minHeight, maxWidth, maxHeight);
}

void GLCamera::scroll(float x, float y,
	float minWidth, float minHeight,
	float maxWidth, float maxHeight)
{
	static Vector zvec(0.0f, 0.0f, 1.0f);
	Vector dir = lookAt_ - currentPosition_;
	dir.StoreNormalize();

	x *= zoom_ / 225.0f;
	y *= zoom_ / 225.0f;

	Vector left = dir * zvec;
	left.StoreNormalize();
	Vector up = left * zvec;
	left *= x;
	up *= y;
	
	wantedLookAt_ += left + up;
	lookAt_ = wantedLookAt_;
	currentPosition_ += left + up;

	if (wantedLookAt_[0] < minWidth) wantedLookAt_[0] = minWidth;
	else if (wantedLookAt_[0] > maxWidth) wantedLookAt_[0] = maxWidth;

	if (wantedLookAt_[1] < minHeight) wantedLookAt_[1] = minHeight;
	else if (wantedLookAt_[1] > maxHeight) wantedLookAt_[1] = maxHeight;
}

