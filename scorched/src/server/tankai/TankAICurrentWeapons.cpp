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

#include <server/ScorchedServer.h>
#include <tankai/TankAICurrentWeapons.h>
#include <XML/XMLNode.h>
#include <stdlib.h>

TankAICurrentWeapons::TankAICurrentWeapons() : 
	currentWeaponSet_(0)
{
}

TankAICurrentWeapons::~TankAICurrentWeapons()
{
}

TankAIWeaponSets::WeaponSet *TankAICurrentWeapons::getCurrentWeaponSet()
{
	if (!currentWeaponSet_)
	{
		currentWeaponSet_ = weaponSets_[rand() % weaponSets_.size()];
	}
	return currentWeaponSet_;
}

bool TankAICurrentWeapons::parseConfig(TankAIWeaponSets &sets, XMLNode *node)
{
	std::string weaponset;
	while (node->getNamedChild("weaponset", weaponset, false))
	{
		TankAIWeaponSets::WeaponSet *weaponSet = 
			sets.getWeaponSet(weaponset.c_str());
		if (!weaponSet) node->returnError(
			S3D::formatStringBuffer("Cannot find weapon set \"%s\"", weaponset.c_str()));
		weaponSets_.push_back(weaponSet);
	}
	if (weaponSets_.empty())
	{
		return node->returnError("Must define at least one weaponset");
	}

	return node->failChildren();	
}

void TankAICurrentWeapons::buyWeapons(TankAIWeaponSets::WeaponSetAccessories &tankAccessories, bool lastRound)
{
	getCurrentWeaponSet()->buyWeapons(tankAccessories, lastRound);
}
