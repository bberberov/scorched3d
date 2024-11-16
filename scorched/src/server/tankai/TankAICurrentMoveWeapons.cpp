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

#include <tankai/TankAICurrentMoveWeapons.h>
#include <tanket/Tanket.h>
#include <tanket/TanketShotInfo.h>
#include <target/TargetShield.h>
#include <target/TargetParachute.h>
#include <target/TargetLife.h>
#include <server/ScorchedServer.h>
#include <landscapedef/LandscapeTex.h>
#include <landscapemap/LandscapeMaps.h>
#include <weapons/Accessory.h>
#include <weapons/WeaponNapalm.h>
#include <weapons/Shield.h>

TankAICurrentMoveWeapons::TankAICurrentMoveWeapons(Tanket *tanket, 
	Tanket *targetTanket,
	TankAIWeaponSets::WeaponSet *weapons)
{
	// Check if this target has shields
	shield = 0;
	if (targetTanket->getShield().getCurrentShield())
	{
		shield = (Shield *) targetTanket->getShield().
			getCurrentShield()->getAction();
	}

	// Normal weapons
	small = weapons->getTankAccessoryByType(tanket, "explosionsmall");
	large = weapons->getTankAccessoryByType(tanket, "explosionlarge");
	roller = weapons->getTankAccessoryByType(tanket, "roller");
	uncover = weapons->getTankAccessoryByType(tanket, "uncover");

	// Check for being under water
	fixed waterHeight = -10;
	LandscapeTex &tex = *ScorchedServer::instance()->getLandscapeMaps().getDefinitions().getTex();
	if (tex.border->getType() == LandscapeTexType::eWater)
	{
		LandscapeTexBorderWater *water = 
			(LandscapeTexBorderWater *) tex.border;

		waterHeight = water->height;
	}
	napalm = 0;
	if (targetTanket->getLife().getTargetPosition()[2] > waterHeight)
	{
		napalm = weapons->getTankAccessoryByType(tanket, "napalm");
	}

	// Check for laser proof shields
	laser = 0;
	if (!shield ||
		shield->getLaserProof() == Shield::ShieldLaserProofNone)
	{
		laser = weapons->getTankAccessoryByType(tanket, "laser");
	}

	// Check for having parachutes
	// Cheating!!, need a better way for this
	digger = 0;
	if (!targetTanket->getParachute().getCurrentParachute())
	{
		digger = weapons->getTankAccessoryByType(tanket, "digger");
	}
}

TankAICurrentMoveWeapons::~TankAICurrentMoveWeapons()
{
}

