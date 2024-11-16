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

#include <tankai/TankAIWeaponSets.h>
#include <tank/Tank.h>
#include <tank/TankScore.h>
#include <server/ScorchedServer.h>
#include <server/ServerSimulator.h>
#include <common/OptionsScorched.h>
#include <common/OptionsTransient.h>
#include <weapons/AccessoryStore.h>
#include <simactions/TankAccessorySimAction.h>
#include <XML/XMLFile.h>

TankAIWeaponSets::WeaponSetAccessories::WeaponSetAccessories(Tanket *tanket) :
	tankAccessories(ScorchedServer::instance()->getContext())
{
	tankAccessories.setTanket(tanket);
	NetBuffer buffer;
	tanket->getAccessories().writeMessage(buffer, true);
	NetBufferReader reader(buffer);
	tankAccessories.readMessage(reader);
	if (tanket->getType() == Target::TypeTank)
	{
		tankMoney = ((Tank *) tanket)->getScore().getMoney();
	}
	else
	{
		tankMoney = 0;
	}
	tankId = tanket->getPlayerId();
}

TankAIWeaponSets::TankAIWeaponSets()
{
}

TankAIWeaponSets::~TankAIWeaponSets()
{
}

bool TankAIWeaponSets::parseConfig()
{
	XMLFile file;
	std::string fileName = S3D::getModFile("data/tankaiweaponsets.xml");
	if (!file.readFile(fileName.c_str()))
	{
		S3D::dialogMessage("TankAIWeaponSets", S3D::formatStringBuffer(
			"Failed to parse \"%s\":%s\n", 
			fileName.c_str(),
			file.getParserError()));
		return false;
	}
	if (!file.getRootNode())
	{
		S3D::dialogMessage("TankAIWeaponSets", S3D::formatStringBuffer(
			"Failed to weapon sets definition file \"%s\"",
				fileName.c_str()));
		return false;		
	}

	XMLNode *weaponsetNode = 0;
	while (file.getRootNode()->getNamedChild("weaponset", weaponsetNode, false))
	{
		WeaponSet weaponSet;
		if (!weaponSet.parseConfig(weaponsetNode)) return false;
		weaponSets_[weaponSet.name] = weaponSet;
	}

	return file.getRootNode()->failChildren();
}

TankAIWeaponSets::WeaponSet *TankAIWeaponSets::getWeaponSet(const char *name)
{
	std::map<std::string, WeaponSet>::iterator findItor =
		weaponSets_.find(name);
	if (findItor == weaponSets_.end()) return 0;

	return &findItor->second;
}

bool TankAIWeaponSets::WeaponSet::parseConfig(XMLNode *node)
{
	if (!node->getNamedChild("name", name)) return false;
	XMLNode *setNode = 0, *weaponNode = 0;
	if (!node->getNamedChild("set", setNode)) return false;
	while (setNode->getNamedChild("weapon", weaponNode, false))
	{
		WeaponSetEntry weapon;
		if (!weapon.parseConfig(weaponNode)) return false;
		weapons.push_back(weapon);
	}

	return node->failChildren();
}

void TankAIWeaponSets::WeaponSet::buyWeapons(WeaponSetAccessories &tankAccessories, bool lastRound)
{
	for (;;)
	{
		// Get all of the weapons we can buy
		std::multimap<unsigned int, WeaponSetEntry *> potentialWeapons;
		std::vector<WeaponSetEntry>::iterator itor;
		for (itor = weapons.begin();
			itor != weapons.end();
			++itor)
		{
			WeaponSetEntry &weapon = *itor;
			if (weapon.weaponValid(tankAccessories, lastRound))
			{
				potentialWeapons.insert(
					std::pair<const unsigned int, WeaponSetEntry *>
						(weapon.prioritybuy, &weapon));
			}
		}
		if (potentialWeapons.empty()) break;

		// Choose one of the potential weapons
		// from the list of the highest priority
		std::vector<WeaponSetEntry *> priorityWeapons;
		std::multimap<unsigned int, WeaponSetEntry *>::reverse_iterator ritor;
		for (ritor = potentialWeapons.rbegin();
			ritor != potentialWeapons.rend();
			++ritor)
		{
			WeaponSetEntry *weapon = ritor->second;
			priorityWeapons.push_back(weapon);
			if (weapon->prioritybuy < priorityWeapons.back()->prioritybuy) break;
		}
		WeaponSetEntry *choosenWeapon = priorityWeapons[rand() % priorityWeapons.size()];
		Accessory *choosenAccessory = choosenWeapon->accessory;

		// Buy this weapon
		tankAccessories.tankAccessories.add(choosenAccessory, choosenAccessory->getBundle(), false);
		tankAccessories.tankMoney -= choosenAccessory->getPrice();
		if (tankAccessories.tankMoney < 0) tankAccessories.tankMoney = 0;

		ComsBuyAccessoryMessage buyMessage(
			tankAccessories.tankId, choosenAccessory->getAccessoryId(), true);
		TankAccessorySimAction *buyAction = new TankAccessorySimAction(buyMessage);
		ScorchedServer::instance()->getServerSimulator().addSimulatorAction(buyAction);
	}
}

Accessory *TankAIWeaponSets::WeaponSet::getTankAccessoryByType(
	Tanket *tanket, const char *getType)
{
	DIALOG_ASSERT(WeaponSetEntry::checkType(getType));

	WeaponSetEntry *result = 0;

	std::vector<WeaponSetEntry>::iterator itor;
	for (itor = weapons.begin();
		itor != weapons.end();
		++itor)
	{
		WeaponSetEntry &current = *itor;
		if (current.type == getType)
		{
			if ((10 - current.accessory->getArmsLevel()) <=
				ScorchedServer::instance()->getOptionsTransient().getArmsLevel() ||
				ScorchedServer::instance()->getOptionsGame().getGiveAllWeapons())
			{
				if (tanket->getAccessories().canUse(current.accessory))
				{
					if (!result ||
						result->priorityuse < current.priorityuse)
					{
						result = &current;
					}
				}
			}
		}
	}

	return (result?result->accessory:0);
}

bool TankAIWeaponSets::WeaponSetEntry::parseConfig(XMLNode *node)
{
	std::string name;
	if (!node->getNamedChild("name", name)) return false;
	accessory = ScorchedServer::instance()->getAccessoryStore().
		findByPrimaryAccessoryName(name.c_str());
	if (!accessory)
	{
		return node->returnError(
			S3D::formatStringBuffer("Unknown accessory %s", name.c_str()));
	}

	if (!node->getNamedChild("buymin", buymin)) return false;
	if (!node->getNamedChild("buymax", buymax)) return false;
	if (!node->getNamedChild("moneymin", moneymin)) return false;
	if (!node->getNamedChild("moneymax", moneymax)) return false;
	if (!node->getNamedChild("prioritybuy", prioritybuy)) return false;
	if (!node->getNamedChild("priorityuse", priorityuse)) return false;
	if (!node->getNamedChild("type", type)) return false;

	if (!checkType(type.c_str()))
	{
		return node->returnError(
			S3D::formatStringBuffer("Unknown type %s", type.c_str()));
	}

	return node->failChildren();
}

bool TankAIWeaponSets::WeaponSetEntry::checkType(const char *type)
{
	if (0 != strcmp(type, "explosionhuge") &&
		0 != strcmp(type, "explosionlarge") &&
		0 != strcmp(type, "explosionsmall") &&
		0 != strcmp(type, "uncover") &&
		0 != strcmp(type, "digger") &&
		0 != strcmp(type, "roller") &&
		0 != strcmp(type, "napalm") &&
		0 != strcmp(type, "laser") &&
		0 != strcmp(type, "shield") &&
		0 != strcmp(type, "other") &&
		0 != strcmp(type, "fuel") &&
		0 != strcmp(type, "autodefense") &&
		0 != strcmp(type, "parachute"))
	{
		return false;
	}
	return true;
}

bool TankAIWeaponSets::WeaponSetEntry::weaponValid(
	WeaponSetAccessories &tankAccessories, bool lastRound)
{
	if (!tankAccessories.tankAccessories.accessoryAllowed(accessory, accessory->getBundle()))
	{
		return false;
	}

	int currentMoney = tankAccessories.tankMoney;
	if (currentMoney < accessory->getPrice()) return false;

	if (type == "autodefense")
	{
		// Don't buy autodefense when simultaneous
		if (ScorchedServer::instance()->getOptionsGame().getTurnType() == 
			OptionsGame::TurnSimultaneous)
		{
			return false;
		}
	}

	if (!lastRound)
	{
		int currentCount = tankAccessories.tankAccessories.getAccessoryCount(accessory);
		if (currentCount > buymin) return false;
		if ((currentMoney < moneymin && moneymin != 0) || 
			(currentMoney > moneymax && moneymax != 0)) return false;
	}
	
	return true;
}
