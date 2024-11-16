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

#include <GLW/GLWTankFire.h>
#include <client/ScorchedClient.h>
#include <tankgraph/TankKeyboardControlUtil.h>
#include <common/ToolTipResource.h>
#include <lang/LangResource.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <tank/TankState.h>

REGISTER_CLASS_SOURCE(GLWTankFire);

GLWTankFire::GLWTankFire() :
	GLWidget(0.0f, 0.0f, 0.0f, 0.0f)
{
	setToolTip(new ToolTipResource(ToolTip::ToolTipHelp, 
		"FIRE_CURRENT_WEAPON", "Fire Current Weapon",
		"FIRE_CURRENT_WEAPON_TOOLTIP", 
		"Fires the current tanks currently selected\n"
		"weapon."));
}

GLWTankFire::~GLWTankFire()
{

}

void GLWTankFire::mouseDown(int button, float x, float y, bool &skipRest)
{
	if (inBox(x, y, x_, y_, w_, h_))
	{
		dragging_ = true;
	}
}

void GLWTankFire::mouseUp(int button, float x, float y, bool &skipRest)
{
	if (dragging_)
	{
		if (inBox(x, y, x_, y_, w_, h_))
		{
			Tank *currentTank =
				ScorchedClient::instance()->getTargetContainer().getCurrentTank();
			if (currentTank)
			{
				if (currentTank->getState().getState() == 
					TankState::sNormal)
				{
					TankKeyboardControlUtil::fireShot(currentTank);
				}
			}
		}
	}
	dragging_ = false;
}

