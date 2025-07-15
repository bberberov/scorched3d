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

#include <weapons/WeaponGiveLives.h>
#include <weapons/AccessoryStore.h>
#include <engine/ActionController.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <tank/TankState.h>
#include <common/Defines.h>
#include <common/ChannelManager.h>
#include <lang/LangResource.h>

REGISTER_ACCESSORY_SOURCE(WeaponGiveLives);

WeaponGiveLives::WeaponGiveLives()
{

}

WeaponGiveLives::~WeaponGiveLives()
{

}

bool WeaponGiveLives::parseXML(AccessoryCreateContext &context, XMLNode *accessoryNode)
{
	if (!Weapon::parseXML(context, accessoryNode)) return false;

	if (!accessoryNode->getNamedChild("lives", lives_)) return false;

	return true;
}

void WeaponGiveLives::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	context.getActionController().addAction(
		new CallbackWeapon("WeaponGiveLives", this, 0, 0, 
			weaponContext, position, velocity));
}

void WeaponGiveLives::weaponCallback(
	ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity,
	unsigned int userData)
{
	Tank *tank = context.getTargetContainer().getTankById(weaponContext.getPlayerId());
	if (!tank) return;

	if (tank->getAlive() &&
		tank->getState().getMaxLives() > 0)
	{
		tank->getState().setLives(
			MAX(tank->getState().getLives() + lives_, 1));

		{
			if (lives_ > 0)
			{
				ChannelText text("combat", 
					LANG_RESOURCE_2("TANK_GET_LIVE",
					"[p:{0}] has received {1} extra live(s)", 
					tank->getTargetName(), 
					S3D::formatStringBuffer("%i", lives_)));
				ChannelManager::showText(context, text);
			}
			else
			{
				ChannelText text("combat", 
					LANG_RESOURCE_2("TANK_LOST_LIVE",
					"[p:{0}] has lost {1} extra live(s)", 
					tank->getTargetName(), 
					S3D::formatStringBuffer("%i", -lives_)));
				ChannelManager::showText(context, text);
			}
		}
	}
}

