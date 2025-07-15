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

#include <weapons/WeaponTypeAction.h>
#include <weapons/AccessoryStore.h>
#include <engine/ActionController.h>
#include <target/TargetContainer.h>
#include <tanket/Tanket.h>
#include <tanket/TanketType.h>

REGISTER_ACCESSORY_SOURCE(WeaponTypeAction);

WeaponTypeAction::WeaponTypeAction()
{
}

WeaponTypeAction::~WeaponTypeAction()
{
	std::map<std::string, Weapon *>::iterator itor;
	for (itor = actions_.begin();
		itor != actions_.end();
		++itor)
	{
		delete itor->second;
	}
	actions_.clear();
}

bool WeaponTypeAction::parseXML(AccessoryCreateContext &context, XMLNode *accessoryNode)
{
	if (!Weapon::parseXML(context, accessoryNode)) return false;

	std::list<XMLNode *> children = accessoryNode->getChildren(); // Copy
	std::list<XMLNode *>::iterator itor;
	for (itor = children.begin();
		itor != children.end();
		++itor)
	{
		XMLNode *node = (*itor);

		XMLNode *tmpNode = 0;
		accessoryNode->getNamedChild(node->getName(), tmpNode); // Just to remove child

		// Check next weapon is correct type
		AccessoryPart *accessory = context.getAccessoryStore().
			createAccessoryPart(context, parent_, node);
		if (!accessory || accessory->getType() != AccessoryPart::AccessoryWeapon)
		{
			return node->returnError("Failed to find sub weapon, not a weapon");
		}
		actions_[node->getName()] = (Weapon*) accessory;
	}

	if (actions_.empty())
	{
		return accessoryNode->returnError("No actions defined");
	}

	return true;
}

void WeaponTypeAction::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	context.getActionController().addAction(
		new CallbackWeapon("WeaponTypeAction", this, 0, 0, 
			weaponContext, position, velocity));
}

void WeaponTypeAction::weaponCallback(
	ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity,
	unsigned int userData)
{
	Tanket *tanket = context.getTargetContainer().getTanketById(weaponContext.getPlayerId());
	if (!tanket) return;

	std::map<std::string, Weapon *>::iterator itor = 
		actions_.find(tanket->getTanketType()->getName());
	if (itor == actions_.end()) return;

	Weapon *action = (*itor).second;
	if (action)
	{
		action->fire(context, weaponContext, position, velocity);
	}
}

