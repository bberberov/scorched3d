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

#include <weapons/WeaponGroupSelect.h>
#include <weapons/AccessoryStore.h>
#include <engine/Simulator.h>
#include <engine/ObjectGroups.h>
#include <engine/ObjectGroup.h>
#include <engine/ObjectGroupEntry.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscapedef/LandscapeTex.h>
#include <target/Target.h>
#include <target/TargetLife.h>

REGISTER_ACCESSORY_SOURCE(WeaponGroupSelect);

WeaponGroupSelect::WeaponGroupSelect() :
	nextAction_(0)
{

}

WeaponGroupSelect::~WeaponGroupSelect()
{
	delete nextAction_;
	nextAction_ = 0;
}

bool WeaponGroupSelect::parseXML(AccessoryCreateContext &context,XMLNode *accessoryNode)
{
	if (!Weapon::parseXML(context, accessoryNode)) return false;

	if (!accessoryNode->getNamedChild("groupname", groupName_)) return false;

	XMLNode *subNode = 0;
	if (!accessoryNode->getNamedChild("nextaction", subNode)) return false;
	
	// Check next weapon is correct type
	AccessoryPart *accessory = context.getAccessoryStore().
		createAccessoryPart(context, parent_, subNode);
	if (!accessory || accessory->getType() != AccessoryPart::AccessoryWeapon)
	{
		return subNode->returnError("Failed to find sub weapon, not a weapon");
	}
	nextAction_ = (Weapon*) accessory;

	return true;
}

void WeaponGroupSelect::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	// Find the group to select the objects in
	ObjectGroup *objectGroup = weaponContext.getInternalContext().getLocalGroups().getGroup(groupName_.c_str());
	if (!objectGroup) objectGroup = context.getObjectGroups().getGroup(groupName_.c_str());
	if (!objectGroup) return;

	// Select the object
	int objectCount = objectGroup->getObjectCount();
	if (objectCount == 0) return;
	unsigned int object = context.getSimulator().getRandomGenerator().getRandUInt("WeaponGroupSelect") % objectCount;
	ObjectGroupEntry *entry = objectGroup->getObjectByPos(object);

	FixedVector newPosition = entry->getPosition();
	FixedVector newVelocity = entry->getVelocity();
	nextAction_->fire(context, weaponContext, newPosition, newVelocity);
}
