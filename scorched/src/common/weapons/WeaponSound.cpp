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

#include <weapons/WeaponSound.h>
#include <engine/ActionController.h>
#include <actions/SoundAction.h>
#include <common/Defines.h>
#include <stdlib.h>

REGISTER_ACCESSORY_SOURCE(WeaponSound);

WeaponSound::WeaponSound() : 
	gain_(1), relative_(false),
	rolloff_(1), referenceDistance_(75)
{

}

WeaponSound::~WeaponSound()
{

}

const char *WeaponSound::getSound()
{
	std::string &sound = sounds_[rand() % sounds_.size()];
	return sound.c_str();
}

bool WeaponSound::parseXML(AccessoryCreateContext &context, XMLNode *accessoryNode)
{
	if (!Weapon::parseXML(context, accessoryNode)) return false;

	std::string sound;
	while (accessoryNode->getNamedChild("sound", sound, false))
	{
		if (!S3D::checkDataFile(sound.c_str())) return false;
		sounds_.push_back(sound);
	}
	accessoryNode->getNamedChild("gain", gain_, false);
	accessoryNode->getNamedChild("rolloff", rolloff_, false);
	accessoryNode->getNamedChild("referencedistance", referenceDistance_, false);
	accessoryNode->getNamedChild("relative", relative_, false);
	return true;
}

void WeaponSound::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	context.getActionController().addAction(
		new SoundAction(position, this));
}
