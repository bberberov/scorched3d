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

#include <tanket/TanketWeapon.h>
#include <tanket/Tanket.h>
#include <tanket/TanketAccessories.h>
#include <engine/ScorchedContext.h>
#include <common/Defines.h>
#include <weapons/Accessory.h>

TanketWeapon::TanketWeapon(ScorchedContext &context) : 
	currentWeapon_(0), context_(context),
	tanket_(0), weaponSwitcher_(0)
{
}

TanketWeapon::~TanketWeapon()
{
}

void TanketWeapon::newMatch()
{
	setCurrentWeapon(0);
}

void TanketWeapon::changed()
{
	if (!tanket_->getAccessories().canUse(currentWeapon_) ||
		currentWeapon_ == 0)
	{
		setCurrentWeapon(0);
		std::list<Accessory *> &result =
			tanket_->getAccessories().getAllAccessoriesByGroup("weapon");
		std::list<Accessory *>::iterator itor;
		for (itor = result.begin();
			itor != result.end();
			++itor)
		{
			if (tanket_->getAccessories().canUse(*itor))
			{
				setWeapon(*itor);
				break;
			}
		}
	}
}

bool TanketWeapon::setWeapon(Accessory *wp)
{
	if (tanket_->getAccessories().canUse(wp))
	{
		setCurrentWeapon(wp);
		return true;
	}
	return false;
}

Accessory *TanketWeapon::getCurrent()
{
	return currentWeapon_;
}

void TanketWeapon::setCurrentWeapon(Accessory *wp)
{
	if (weaponSwitcher_) weaponSwitcher_->switchWeapon(context_, tanket_, currentWeapon_, wp);

	currentWeapon_ = wp;
}

const char *TanketWeapon::getWeaponString()
{
	if (!getCurrent()) return "";

	static char buffer[256];
	int count = tanket_->getAccessories().getAccessoryCount(getCurrent());
	snprintf(buffer, 256, ((count>0)?"%s (%i)":"%s (In)"),
		getCurrent()->getName(), count);
	return buffer;
}
