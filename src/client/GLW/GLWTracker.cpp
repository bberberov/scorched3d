////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011, 2025
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

#include <GLW/GLWTracker.hpp>
#include <GLW/GLWFont.hpp>
#include <GLW/GLWToolTip.hpp>
#include <GLEXT/GLState.hpp>
#include <common/Keyboard.hpp>
#include <common/ToolTipResource.hpp>
#include <graph/OptionsDisplay.hpp>
#include <client/ScorchedClient.hpp>
#include <target/TargetContainer.hpp>
#include <tank/Tank.hpp>
#include <tank/TankState.hpp>
#include <tanket/TanketShotInfo.hpp>
#include <lang/LangResource.hpp>

REGISTER_CLASS_SOURCE(GLWTracker);

GLWTracker::GLWTracker(float x, float y, float w, float range) :
	GLWidget(x, y, w, w),
	handler_(nullptr),
	dragging_(false),
	currentX_(0.0f), currentY_(0.0f),
	range_(range)
{}

GLWTracker::~GLWTracker()
{}

void GLWTracker::mouseDown(int button, float x, float y, bool &skipRest)
{
	if (inBox(x, y, x_, y_, w_, h_))
	{
		skipRest = true;
		dragging_ = true;
	}
}

void GLWTracker::mouseUp(int button, float x, float y, bool &skipRest)
{
	dragging_ = false;
}

void GLWTracker::mouseDrag(int button, float mx, float my, float x, float y, bool &skipRest)
{
	if (dragging_)
	{
		float rangeMult = 1.0f;
		unsigned int keyState = 
			Keyboard::instance()->getKeyboardState();
		if (keyState & KMOD_LSHIFT) rangeMult = 0.5f;

		currentX_ -= x / w_ * range_ * rangeMult;
		if (OptionsDisplay::instance()->getInvertElevation())
		{
			rangeMult = -rangeMult;
		}
		currentY_ -= y /w_ * range_ * rangeMult;
		if (handler_) handler_->currentChanged(getId(), currentX_, currentY_);

		skipRest = true;
	}
}

REGISTER_CLASS_SOURCE(GLWTankTracker);

GLWTankTracker::GLWTankTracker() :
	GLWTracker(0.0f, 0.0f, 0.0f, 100.0f)
{
	setHandler(this);
	setToolTip(new ToolTipResource(ToolTip::ToolTipHelp, 
		"ROTATION_ELEVATION", "Rotation/Elevation",
		"ROTATION_ELEVATION_TOOLTIP", 
		"Change the rotation and elevation of the\n"
		"current tank by clicking with the left\n"
		"mouse button and dragging up and down,\n"
		"left and right.\n"
		"Shift key decreases sensitivity."));
}

GLWTankTracker::~GLWTankTracker()
{}

void GLWTankTracker::draw()
{
	Tank *currentTank = ScorchedClient::instance()->getTargetContainer().getCurrentTank();
	if (currentTank)
	{
		if (currentTank->getState().getState() == TankState::sNormal)
		{
			setCurrentX(currentTank->getShotInfo().getRotationGunXY().asFloat());
			setCurrentY(currentTank->getShotInfo().getRotationGunYZ().asFloat());
		}
	}
	GLWTracker::draw();
}

void GLWTankTracker::currentChanged(unsigned int id, float valueX, float valueY)
{
	Tank *currentTank = ScorchedClient::instance()->getTargetContainer().getCurrentTank();
	if (currentTank)
	{
		if (currentTank->getState().getState() == TankState::sNormal)
		{
			if (id == getId())
			{
				currentTank->getShotInfo().rotateGunXY(fixed::fromFloat(valueX), false);
				currentTank->getShotInfo().rotateGunYZ(fixed::fromFloat(valueY), false);
			}
		}
	}
}
