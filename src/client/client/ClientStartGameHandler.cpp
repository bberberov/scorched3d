////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011, 2025
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

#include <client/ClientStartGameHandler.hpp>
#include <client/ClientState.hpp>
#include <client/ScorchedClient.hpp>
#include <landscape/Landscape.hpp>
#include <landscapemap/LandscapeMaps.hpp>
#include <engine/ActionController.hpp>
#include <target/TargetContainer.hpp>
#include <tanket/TanketAccessories.hpp>
#include <tank/Tank.hpp>
#include <tank/TankCamera.hpp>
#include <graph/MainCamera.hpp>
#include <graph/OptionsDisplay.hpp>
#include <common/OptionsTransient.hpp>
#include <common/OptionsScorched.hpp>
#include <common/Defines.hpp>
#include <sound/SoundUtils.hpp>
#include <weapons/Accessory.hpp>

ClientStartGameHandler *ClientStartGameHandler::instance_ = nullptr;

ClientStartGameHandler *ClientStartGameHandler::instance()
{
	if (nullptr == instance_)
	{
		instance_ = new ClientStartGameHandler();
	}
	return instance_;
}

ClientStartGameHandler::ClientStartGameHandler()
{
}

ClientStartGameHandler::~ClientStartGameHandler()
{
}

void ClientStartGameHandler::startGame(TankStartMoveSimAction *action)
{
	Tank *tank = ScorchedClient::instance()->getTargetContainer().
		getTankById(action->getPlayerId());
	if (!tank)
	{
		return;
	}

	ScorchedClient::instance()->getTargetContainer().setCurrentPlayerId(
		action->getPlayerId());
	Tank *current = ScorchedClient::instance()->getTargetContainer().getCurrentTank();
	if (!current) 
	{
		return;
	}

	// Set the camera back to this players camera position
	if (OptionsDisplay::instance()->getStorePlayerCamera())
	{
		int counter = 0;
		unsigned int currentDestinationId = ScorchedClient::instance()->
			getTargetContainer().getCurrentDestinationId();
		std::map<unsigned int, Tank *> &tanks = ScorchedClient::instance()->
			getTargetContainer().getTanks();
		std::map<unsigned int, Tank *>::iterator itor;
		for (itor = tanks.begin();
			itor != tanks.end();
			++itor)
		{
			Tank *tank = itor->second;
			if (tank->getDestinationId() == currentDestinationId &&
				tank->getAlive())
			{
				counter++;
			}
		}

		// Only reset the position if there is more than one human player
		// from the same destination
		if (counter > 1)
		{
			MainCamera::instance()->getTarget().getCamera().setLookAt(current->getCamera().getCameraLookAt());
			Vector rotation = current->getCamera().getCameraRotation();
			MainCamera::instance()->getTarget().getCamera().movePosition(rotation[0], rotation[1], rotation[2]);
			MainCamera::instance()->getTarget().setCameraType((TargetCamera::CamType) current->getCamera().getCameraType());
		}
	}

	// Ensure that the landscape is set to the "proper" texture
	Landscape::instance()->restoreLandscapeTexture();

	// make sound to tell client a new game is commencing
	CACHE_SOUND(playSound, S3D::getModFile("data/wav/misc/play.wav"));
	SoundUtils::playRelativeSound(VirtualSoundPriority::eText, playSound);

	// Stimulate into the new game state
	ScorchedClient::instance()->getGameState().stimulate(ClientState::StimWait);
	if (action->getBuying())
	{
		ScorchedClient::instance()->getGameState().stimulate(
			ClientState::StimBuyWeapons);
	}
	else
	{
		ScorchedClient::instance()->getGameState().stimulate(
			ClientState::StimPlaying);

		Accessory *currentWeapon = current->getAccessories().getWeapons().getCurrent();
		if (currentWeapon && 
			currentWeapon->getPositionSelect() != Accessory::ePositionSelectNone)
		{
			std::list<Accessory *> &entries =
				current->getAccessories().getAllAccessoriesByGroup("weapon");
			if (!entries.empty()) currentWeapon = entries.front();
		}
		current->getAccessories().getWeapons().setWeapon(currentWeapon);
	}
}
