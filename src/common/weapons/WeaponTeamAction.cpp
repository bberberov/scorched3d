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

#include <weapons/WeaponTeamAction.h>
#include <weapons/AccessoryStore.h>
#include <engine/ActionController.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>

REGISTER_ACCESSORY_SOURCE(WeaponTeamAction);

WeaponTeamAction::WeaponTeamAction()
{
}

WeaponTeamAction::~WeaponTeamAction()
{
	for (int i=0; i<5; i++)
	{
		delete action_[i];
		action_[i] = 0;
	}
}

bool WeaponTeamAction::parseXML(AccessoryCreateContext &context, XMLNode *accessoryNode)
{
	if (!Weapon::parseXML(context, accessoryNode)) return false;

	bool actions = false;
	for (int i=0; i<5; i++)
	{
		action_[i] = 0;

		XMLNode *subNode = 0;
		std::string nodeName = S3D::formatStringBuffer("team%i", i);
		if (accessoryNode->getNamedChild(nodeName.c_str(), subNode, false))
		{
			// Check next weapon is correct type
			AccessoryPart *accessory = context.getAccessoryStore().
				createAccessoryPart(context, parent_, subNode);
			if (!accessory || accessory->getType() != AccessoryPart::AccessoryWeapon)
			{
				return subNode->returnError("Failed to find sub weapon, not a weapon");
			}
			action_[i] = (Weapon*) accessory;
			actions = true;
		}
	}

	if (!actions)
	{
		return accessoryNode->returnError("No actions defined");
	}

	return true;
}

void WeaponTeamAction::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	context.getActionController().addAction(
		new CallbackWeapon("WeaponTeamAction", this, 0, 0, 
			weaponContext, position, velocity));
}

void WeaponTeamAction::weaponCallback(
	ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity,
	unsigned int userData)
{
	Tank *tank = context.getTargetContainer().getTankById(weaponContext.getPlayerId());
	if (!tank) return;

	Weapon *action = action_[tank->getTeam()];
	if (action)
	{
		action->fire(context, weaponContext, position, velocity);
	}
}

