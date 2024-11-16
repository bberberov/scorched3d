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

#include <simactions/TankAccessorySimAction.h>
#include <target/TargetContainer.h>
#include <tanket/TanketAccessories.h>
#include <tank/Tank.h>
#include <tank/TankScore.h>
#include <weapons/AccessoryStore.h>
#include <weapons/EconomyStore.h>
#include <server/ScorchedServer.h>

REGISTER_CLASS_SOURCE(TankAccessorySimAction);

TankAccessorySimAction::TankAccessorySimAction()
{
}

TankAccessorySimAction::TankAccessorySimAction(ComsBuyAccessoryMessage &accessoryMessage) :
	accessoryMessage_(accessoryMessage)
{
}

TankAccessorySimAction::~TankAccessorySimAction()
{
}

bool TankAccessorySimAction::invokeAction(ScorchedContext &context)
{
	unsigned int playerId = accessoryMessage_.getPlayerId();

	// Check tank exists and is alive
	Tank *tank = context.getTargetContainer().getTankById(playerId);
	if (!tank)
	{
		return true;
	}

	// Check that the accessory is valid
	Accessory *accessory = 
		context.getAccessoryStore().
		findByAccessoryId(accessoryMessage_.getAccessoryId());
	if (!accessory)
	{
		return true;
	}

	// The game state and everything is correct
	// Perform the actual add or remove of accessory
	if (accessoryMessage_.getBuy())
	{
		if (!tank->getAccessories().accessoryAllowed(accessory, accessory->getBundle())) return true;
		if (accessory->getNoBuy()) return true;
		if (tank->getScore().getMoney() < accessory->getPrice()) return true;

		if (context.getServerMode())
		{
			ScorchedServer::instance()->getEconomyStore().getEconomy()->accessoryBought(
				tank, accessory->getName());
		}

		// Add the accessory
		tank->getAccessories().add(accessory, accessory->getBundle());
		tank->getScore().setMoney(
			tank->getScore().getMoney() - accessory->getPrice());
	}
	else
	{
		if (tank->getAccessories().getAccessoryCount(accessory) <= 0) return true;

		if (context.getServerMode())
		{
			ScorchedServer::instance()->getEconomyStore().getEconomy()->accessorySold(
				tank, accessory->getName());
		}

		// Remove the accessory
		tank->getAccessories().rm(accessory, 1);
		tank->getScore().setMoney(
			tank->getScore().getMoney() + accessory->getSellPrice());
	}

	return true;
}

bool TankAccessorySimAction::writeMessage(NetBuffer &buffer)
{
	return accessoryMessage_.writeMessage(buffer);
}

bool TankAccessorySimAction::readMessage(NetBufferReader &reader)
{
	return accessoryMessage_.readMessage(reader);
}
