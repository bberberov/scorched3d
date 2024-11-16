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

#include <actions/CallbackWeapon.h>
#include <weapons/AccessoryStore.h>

CallbackWeapon::CallbackWeapon(
	const char *name,
	WeaponCallback *callback,
	fixed delay, unsigned int callbackData,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity) :
	Action(weaponContext.getInternalContext().getReferenced()),
	callback_(callback),
	delay_(delay),
	callbackData_(callbackData),
	position_(position),
	velocity_(velocity),
	weaponContext_(weaponContext),
	totalTime_(0)
{

}

CallbackWeapon::~CallbackWeapon()
{
}

void CallbackWeapon::init()
{
}

void CallbackWeapon::simulate(fixed frameTime, bool &remove)
{
	totalTime_ += frameTime;
	if (totalTime_ > delay_)
	{
		callback_->weaponCallback(
			*context_, weaponContext_, position_, velocity_, callbackData_);
		remove = true;
	}

	Action::simulate(frameTime, remove);
}

std::string CallbackWeapon::getActionDetails()
{
	if (callback_) return callback_->getParent()->getName();
	return "";
}
