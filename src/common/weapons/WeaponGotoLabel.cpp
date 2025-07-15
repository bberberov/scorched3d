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

#include <weapons/WeaponGotoLabel.h>
#include <weapons/AccessoryStore.h>

REGISTER_ACCESSORY_SOURCE(WeaponGotoLabel);

WeaponGotoLabel::WeaponGotoLabel()
{
}

WeaponGotoLabel::~WeaponGotoLabel()
{
}

bool WeaponGotoLabel::parseXML(AccessoryCreateContext &context, XMLNode *accessoryNode)
{
	if (!Weapon::parseXML(context, accessoryNode)) return false;

	std::string label;
	if (!accessoryNode->getNamedChild("label", label)) return false;
	if (!accessoryNode->getNamedChild("count", count_)) return false;

	weaponLabel_ = context.getLabel(label.c_str());
	if (!weaponLabel_)
	{
		return accessoryNode->returnError("Failed to find the named label");
	}

	return true;
}

void WeaponGotoLabel::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	int doneCount = weaponContext.getInternalContext().getIncLabelCount(getAccessoryPartId());
	if (doneCount <= count_)
	{
		weaponLabel_->fire(context, weaponContext, position, velocity);
	}
}
