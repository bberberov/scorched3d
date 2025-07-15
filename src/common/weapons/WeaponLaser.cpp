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

#include <weapons/WeaponLaser.h>
#include <weapons/AccessoryStore.h>
#include <actions/Laser.h>
#include <engine/ActionController.h>

REGISTER_ACCESSORY_SOURCE(WeaponLaser);

WeaponLaser::WeaponLaser() :
	minimumHurt_("WeaponLaser::minimumHurt"), maximumHurt_("WeaponLaser::maximumHurt"),
	minimumDistance_("WeaponLaser::minimumDistance"), maximumDistance_("WeaponLaser::maximumDistance"),
	hurtRadius_("WeaponLaser::hurtRadius"),
	totalTime_("WeaponLaser::totalTime")
{
}

WeaponLaser::~WeaponLaser()
{
}

bool WeaponLaser::parseXML(AccessoryCreateContext &context, XMLNode *accessoryNode)
{
	if (!Weapon::parseXML(context, accessoryNode)) return false;

	if (!accessoryNode->getNamedChild("minimumhurt", minimumHurt_)) return false;
	if (!accessoryNode->getNamedChild("maximumhurt", maximumHurt_)) return false;
	if (!accessoryNode->getNamedChild("hurtradius", hurtRadius_)) return false;
	if (!accessoryNode->getNamedChild("minimumdistance", minimumDistance_)) return false;
	if (!accessoryNode->getNamedChild("maximumdistance", maximumDistance_)) return false;
	if (!accessoryNode->getNamedChild("totaltime", totalTime_)) return false;

	if (!laserParams_.parseXML(accessoryNode)) return false;

	return true;
}

void WeaponLaser::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &velocity)
{
	LaserParams *params = new LaserParams(laserParams_);
	params->setMinimumHurt(minimumHurt_.getValue(context));
	params->setMaximumHurt(maximumHurt_.getValue(context));
	params->setHurtRadius(hurtRadius_.getValue(context));
	params->setMinimumDistance(minimumDistance_.getValue(context));
	params->setMaximumDistance(maximumDistance_.getValue(context));
	params->setTotalTime(totalTime_.getValue(context));

	context.getActionController().addAction(
		new Laser(this, params, position, velocity, weaponContext));
}
