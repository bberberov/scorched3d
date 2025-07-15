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

#if !defined(__INCLUDE_TankAIWeaponSetsh_INCLUDE__)
#define __INCLUDE_TankAIWeaponSetsh_INCLUDE__

#include <tanket/TanketAccessories.h>

#include <vector>
#include <map>
#include <string>

class XMLNode;
class TankAIWeaponSets
{
public:
	TankAIWeaponSets();
	virtual ~TankAIWeaponSets();

	bool parseConfig();

	class WeaponSetAccessories 
	{
	public:
		WeaponSetAccessories(Tanket *tanket);

		int tankMoney;
		unsigned int tankId;
		TanketAccessories tankAccessories;
	};

	class WeaponSetEntry
	{
	public:
		Accessory *accessory;
		int buymin, buymax;
		int moneymin, moneymax;
		int prioritybuy, priorityuse;
		std::string type;

		bool parseConfig(XMLNode *node);
		bool weaponValid(WeaponSetAccessories &tankAccessories, bool lastRound);
		static bool checkType(const char *type);
	};

	class WeaponSet
	{
	public:
		std::string name;
		std::vector<WeaponSetEntry> weapons;

		bool parseConfig(XMLNode *node);
		void buyWeapons(WeaponSetAccessories &tankAccessories, bool lastRound);
		Accessory *getTankAccessoryByType(Tanket *tanket, const char *type);
	};

	WeaponSet *getWeaponSet(const char *name);

protected:
	std::map<std::string, WeaponSet> weaponSets_;
};

#endif
