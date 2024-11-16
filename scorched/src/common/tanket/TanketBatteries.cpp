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

#include <tanket/TanketBatteries.h>
#include <tanket/Tanket.h>
#include <tanket/TanketAccessories.h>
#include <weapons/Accessory.h>

TanketBatteries::TanketBatteries(ScorchedContext &context) :
	context_(context),
	tanket_(0)
{
}

TanketBatteries::~TanketBatteries()
{
}

void TanketBatteries::newMatch()
{
}

void TanketBatteries::changed()
{
}

Accessory *TanketBatteries::getBatteryAccessory()
{
	std::list<Accessory *> &result =
		tanket_->getAccessories().getAllAccessoriesByType(
			AccessoryPart::AccessoryBattery);
	if (result.empty()) return 0;
	return result.front();
}

int TanketBatteries::getNoBatteries()
{
	Accessory *battery = getBatteryAccessory();
	if (!battery) return 0;
	return tanket_->getAccessories().getAccessoryCount(battery);
}

bool TanketBatteries::canUse()
{
	int count = getNoBatteries();
	if (count == -1) return true;
	if (count == 0) return false;

	std::list<Accessory *> &result =
		tanket_->getAccessories().getAllAccessoriesByType(
			AccessoryPart::AccessoryBattery);
	if (result.empty()) return false;
	if (count >= result.front()->getUseNumber()) return true;
	return false;
}
