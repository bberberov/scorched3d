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

#if !defined(AFX_GLCAMERA_H__A41E0133_3B1F_11D4_BCBB_00A0C9A4CA3E__INCLUDED_)
#define AFX_GLCAMERA_H__A41E0133_3B1F_11D4_BCBB_00A0C9A4CA3E__INCLUDED_

#include <GLEXT/GLState.h>
#include <common/Line.h>

/**
A class that wraps setting and moving the current viewport.
It sets the projection matrix and the modelview matrix such
that any OpenGL calls will be "looking" at the given position.
*/
class GLCamera
{
public:
	typedef float (*MinHeightFunc)(int, int, void *);
	typedef float (*MaxHeightFunc)(int, int, void *);

	/**
	Create the camera.
	The camera has a width and height that it will use for its
	viewport.
	*/
	GLCamera(GLsizei windowWidth, GLsizei windowHeight);
	virtual ~GLCamera();

	/**
	Sets a function that can be used to limit the cameras minimum
	height at a specified position.  This can be used for example
	to prevent the camera from entering the landscape.
	*/
	void setMinHeightFunc(MinHeightFunc func, void *heightData = 0);
	void setMaxHeightFunc(MaxHeightFunc func, void *heightData = 0);
	/** 
	Turns the user of the height function on or off.
	See setHeightFunc.
	*/
	void setUseHeightFunc(bool toggle);
	/**
	Sets the position that the camera will point at.
	The camera will gradualy move to look at this position unless
	the instant flag is given in which case the camera will
	move instantly.
	*/
	void setLookAt(Vector &lookAt, bool instant = false);

	/**
	Sets the position that the camera will look from.
	*/
	void setCurrentPos(Vector &pos);
	/**
	Sets the position that the camera will look from.
	This position is relative to the current look at position.
	The camera will gradualy move to look from this position unless
	the instant flag is given in which case the camera will
	move instantly.
	*/
	void setOffSet(Vector &offSet, bool instant = false);
	Vector &getOffSet() { return wantedOffset_; }

	/**
	Changes the current viewport size (w, h dimension) of the viewport.
	The viewport is the area of the window that is drawn to.
	*/
	void setWindowSize(GLsizei windowWidth, GLsizei windowHeight);
	/**
	Sets the current viewport location (x, y position) of the viewport.
	The viewport is the area of the window that is drawn to.
	*/
	void setWindowOffset(GLsizei windowLeft, GLsizei windowTop);

	/**
	Causes the current model and projection matrixs to be replaced
	so they are "looking" from the from position to the to position.
	*/
	void draw();
	/**
	Causes the camera to move if gradual movements are being made.
	*/
	void simulate(float frameTime = 0.02f);
	/**
	Causes the camera to shake the viewport randomly.
	This can be used to simulate ground shake.
	e.g. during large explosions.
	The larger the shake the longer the camera will shake.
	*/
	void addShake(float shake);

	/**
	Moves the current look from position to a new location.
	The current look from position in overwritten.
	The look from position is relative to the look at position.
	XY = horizontal rotation for the new point from the old
	YZ = vertical rotation for the new point from the old
	Z = Zoom closeness 
	*/
	void movePosition(float XY, float YZ, float Z);
	/**
	Moves the current look from position to a new location.
	This position is relative to the current look from position.
	The look from position is relative to the look at position.
	XY = horizontal rotation for the new point from the old
	YZ = vertical rotation for the new point from the old
	Z = Zoom closeness 
	*/
	void movePositionDelta(float XY, float YZ, float Z);
	/**
	Uses the current matrixs to turn a two 2D points into
	a 3D line.
	*/
	bool getDirectionFromPt(GLfloat ptX, GLfloat ptY, Line &direction);

	/**
	Get the point the camera is currently looking from
	*/
	Vector &getCurrentPos() { return currentPosition_; }
	/**
	Get the point the camera is currently looking at (observing)
	*/
	Vector &getLookAt() { return lookAt_; }
	/**
	Get the speed of movement of the camera looking from position
	*/
	Vector &getVelocity() { return velocity_; }

	/**
	Returns the current camera horizontal rotation.
	As set by the move position methods.
	*/
	float getRotationXY() { return rotationXY_; }
	/**
	Returns the current camera vertical rotation.
	As set by the move position methods.
	*/
	float getRotationYZ() { return rotationYZ_; }
	/**
	Returns the current camera zoom.
	As set by the move position methods.
	*/
	float getZoom() { return zoom_; }

	/**
	Scrolls the camera along the ground, in the direction given.
	*/
	enum ScrollDir
	{
		eScrollLeft,
		eScrollRight,
		eScrollUp,
		eScrollDown
	};
	void scroll(ScrollDir direction, float minWidth, float minHeight, 
		float maxWidth, float maxHeight, float amount);
	void scroll(float x, float y, float minWidth, float minHeight, 
		float maxWidth, float maxHeight);

	static GLCamera *getCurrentCamera() { return currentCamera_; }

protected:
	static GLCamera *currentCamera_;
	GLsizei windowW_, windowH_;
	GLsizei windowL_, windowT_;
	GLfloat windowAspect_;
	GLfloat rotationXY_, rotationYZ_, zoom_;
	float shake_;
	float totalTime_;
	Vector shakeV_;
	bool useHeightFunc_;
	Vector lookAt_;
	Vector wantedLookAt_;
	Vector wantedOffset_;
	Vector currentPosition_;
	Vector velocity_;
	MinHeightFunc minHeightFunc_;
	MaxHeightFunc maxHeightFunc_;
	void *minHeightData_;
	void *maxHeightData_;

	virtual void calculateWantedOffset();
	virtual void moveViewport(Vector &lookFrom, Vector &lookAt);

};

#endif // !defined(AFX_GLCAMERA_H__A41E0133_3B1F_11D4_BCBB_00A0C9A4CA3E__INCLUDED_)
