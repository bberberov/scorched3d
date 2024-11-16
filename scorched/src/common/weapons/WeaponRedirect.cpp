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

#include <weapons/WeaponRedirect.h>
#include <weapons/AccessoryStore.h>
#include <engine/ActionController.h>
#include <common/Defines.h>
#include <tank/TankLib.h>
#include <math.h>

REGISTER_ACCESSORY_SOURCE(WeaponRedirect);

WeaponRedirect::WeaponRedirect() :
	hredirect_("WeaponRedirect::hredirect", 0), vredirect_("WeaponRedirect::vredirect", 0),
	habs_(false), vabs_(false),
	nextAction_(0)
{

}

WeaponRedirect::~WeaponRedirect()
{
	delete nextAction_;
	nextAction_ = 0;
}

bool WeaponRedirect::parseXML(AccessoryCreateContext &context, XMLNode *accessoryNode)
{
	if (!Weapon::parseXML(context, accessoryNode)) return false;

	if (!accessoryNode->getNamedChild("hredirect", hredirect_)) return false;
	if (!accessoryNode->getNamedChild("habs", habs_)) return false;
	if (!accessoryNode->getNamedChild("vredirect", vredirect_)) return false;
	if (!accessoryNode->getNamedChild("vabs", vabs_)) return false;

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

void WeaponRedirect::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	fixed currentMag = velocity.Magnitude();
	fixed currenth = (atan2x(velocity[1], velocity[0]) / fixed::XPI * 180) - 90;
	fixed dist = (velocity[0] * velocity[0] + velocity[1] * velocity[1]).sqrt();
	fixed currentv = atan2x(dist, velocity[2]) / fixed::XPI * 180;

	if (habs_) currenth = hredirect_.getValue(context);	// call NumberParser::getValue
	else currenth += hredirect_.getValue(context);		// to evaluate at runtime
	if (vabs_) currentv = vredirect_.getValue(context);
	else currentv += vredirect_.getValue(context);
	
	FixedVector newVelocity = TankLib::getVelocityVector(currenth, fixed(90) - currentv);
	newVelocity.StoreNormalize();
	newVelocity *= currentMag;
	
	nextAction_->fire(context, weaponContext, position, newVelocity);

}

