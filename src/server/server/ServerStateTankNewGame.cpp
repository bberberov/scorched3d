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

#include <server/ServerStateTankNewGame.h>
#include <server/ServerSimulator.h>
#include <server/ScorchedServer.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <tank/TankState.h>
#include <simactions/TankNewGameSimAction.h>

ServerStateTankNewGame::ServerStateTankNewGame()
{
}

ServerStateTankNewGame::~ServerStateTankNewGame()
{
}

void ServerStateTankNewGame::enterState()
{
	std::map<unsigned int, Tank*> &tanks = 
		ScorchedServer::instance()->getTargetContainer().getTanks();
	std::map<unsigned int, Tank*>::iterator itor;
	for (itor = tanks.begin();
		itor != tanks.end();
		++itor)
	{
		Tank *tank = itor->second;
		TankNewGameSimAction *tankNewGameAction = 
			new TankNewGameSimAction(tank->getPlayerId());
		ScorchedServer::instance()->getServerSimulator().addSimulatorAction(tankNewGameAction);
	}
}

bool ServerStateTankNewGame::simulate()
{
	return (TankNewGameSimAction::getInstanceCount() == 0);
}
