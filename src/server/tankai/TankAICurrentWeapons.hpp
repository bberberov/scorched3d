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

#if !defined(AFX_TankAICurrentWeapons_H__5F21C9C7_0F71_4CCC_ABB9_976CF0A5C5EC__INCLUDED_)
#define AFX_TankAICurrentWeapons_H__5F21C9C7_0F71_4CCC_ABB9_976CF0A5C5EC__INCLUDED_

#include <tankai/TankAIWeaponSets.h>

class TankAICurrentWeapons
{
public:
	TankAICurrentWeapons();
	virtual ~TankAICurrentWeapons();

	virtual bool parseConfig(TankAIWeaponSets &sets, XMLNode *node);

	void buyWeapons(TankAIWeaponSets::WeaponSetAccessories &tankAccessories, bool lastRound);

	TankAIWeaponSets::WeaponSet *getCurrentWeaponSet();

protected:
	std::vector<TankAIWeaponSets::WeaponSet *> weaponSets_;
	TankAIWeaponSets::WeaponSet *currentWeaponSet_;
};

#endif // !defined(AFX_TankAICurrentWeapons_H__5F21C9C7_0F71_4CCC_ABB9_976CF0A5C5EC__INCLUDED_)
