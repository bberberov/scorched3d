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

#include <GLW/GLWTankSlider.h>
#include <GLW/GLWFont.h>
#include <GLEXT/GLState.h>
#include <common/Keyboard.h>
#include <common/ToolTipResource.h>
#include <client/ScorchedClient.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <tank/TankState.h>
#include <tanket/TanketShotInfo.h>
#include <lang/LangResource.h>

REGISTER_CLASS_SOURCE(GLWTankSlider);

GLWTankSlider::GLWTankSlider() : 
	GLWDragger(0.0f, 0.0f, 0.0f, 100.0f)
{
	setHandler(this);
	setToolTip(new ToolTipResource(ToolTip::ToolTipHelp, 
		"POWER", "Power",
		"POWER_CHANGE_TOOLTIP", 
		"Change the power of the current tank\n"
		"by clicking with the left mouse button\n"
		"and dragging up and down.\n"
		"Shift key decreases sensitivity."));
}

GLWTankSlider::~GLWTankSlider()
{
}

void GLWTankSlider::draw()
{
	Tank *currentTank =
		ScorchedClient::instance()->getTargetContainer().getCurrentTank();
	if (currentTank)
	{
		if (currentTank->getState().getState() == TankState::sNormal)
		{
			setCurrent(currentTank->getShotInfo().getPower().asFloat());
		}
	}
	
	GLWDragger::draw();
}

void GLWTankSlider::currentChanged(unsigned int id, float value)
{
	Tank *currentTank =
		ScorchedClient::instance()->getTargetContainer().getCurrentTank();
	if (currentTank)
	{
		if (currentTank->getState().getState() == TankState::sNormal)
		{
			if (id == getId())
			{
				currentTank->getShotInfo().changePower(
					fixed::fromFloat(value), false);
			}
		}
	}
}
