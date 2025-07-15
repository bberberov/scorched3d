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

#include <tank/TankWeaponSwitcher.h>
#include <tank/Tank.h>
#include <target/TargetContainer.h>
#include <target/TargetLife.h>
#include <weapons/AccessoryStore.h>
#include <weapons/WeaponMoveTank.h>
#ifndef S3D_SERVER
	#include <client/ScorchedClient.h>
	#include <client/ClientState.h>
	#include <landscape/Landscape.h>
#endif
#include <landscapemap/LandscapeMaps.h>
#include <landscapemap/MovementMap.h>
#include <lang/LangResource.h>
#include <common/ChannelManager.h>

TankWeaponSwitcher::TankWeaponSwitcher() 
{
}

TankWeaponSwitcher::~TankWeaponSwitcher()
{
}

void TankWeaponSwitcher::switchWeapon(ScorchedContext &context, Tanket *tanket, Accessory *currentWeapon, Accessory *newWeapon) 
{
#ifndef S3D_SERVER
	if (!context.getServerMode() && tanket->getType() == Target::TypeTank)
	{
		Tank *tank = (Tank *) tanket;

		// Only show this information on this tanks client
		if (ScorchedClient::instance()->getTargetContainer().getCurrentDestinationId() ==
			tank->getDestinationId() &&
			ScorchedClient::instance()->getGameState().getState() == ClientState::StatePlaying)
		{

		// Turn off fuel display (if any)
		if (newWeapon && 
			newWeapon->getPositionSelect() == Accessory::ePositionSelectNone)
		{
			Landscape::instance()->restoreLandscapeTexture();
		}
		// Turn on selection display (if any)
		if (newWeapon &&
			newWeapon->getPositionSelect() != Accessory::ePositionSelectNone)
		{
			if (newWeapon->getPositionSelect() == Accessory::ePositionSelectFuel)
			{
				WeaponMoveTank *moveWeapon = (WeaponMoveTank *)
					context.getAccessoryStore().findAccessoryPartByAccessoryId(
						newWeapon->getAccessoryId(), "WeaponMoveTank");
				if (moveWeapon)
				{
					MovementMap mmap(
						tank, 
						context);
					mmap.calculateAllPositions(mmap.getFuel(moveWeapon));
					mmap.movementTexture();	
				}
			}
			else if (newWeapon->getPositionSelect() == Accessory::ePositionSelectFuelLimit)
			{
				MovementMap mmap(
					tank, 
					context);
				mmap.calculateAllPositions(fixed(newWeapon->getPositionSelectLimit()));
				mmap.movementTexture();	
			}
			else if (newWeapon->getPositionSelect() == Accessory::ePositionSelectLimit)
			{
				MovementMap::limitTexture(tank->getLife().getTargetPosition(), 
					newWeapon->getPositionSelectLimit());
			}

			ChannelText text("banner",
				LANG_RESOURCE_1(
					"GROUND_WEAPON_ACTIVATE", 
					"Click ground to activate {0}",
					newWeapon->getName()));
			ChannelManager::showText(ScorchedClient::instance()->getContext(), 
				text);
		}

		}
	}
#endif
}
