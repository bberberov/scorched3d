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

#include <client/ScorchedClient.h>
#include <client/ClientSimulator.h>
#include <engine/MainLoop.h>
#include <engine/GameState.h>
#include <graph/MainCamera.h>
#include <graph/ParticleEngine.h>
#include <graph/OptionsDisplay.h>
#include <target/TargetSpace.h>
#include <coms/ComsSimulateMessage.h>
#include <coms/ComsNetStatMessage.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscape/GraphicalLandscapeMap.h>

TargetSpace *ScorchedClient::targetSpace_ = new TargetSpace();

ScorchedClient *ScorchedClient::instance_ = 0;

ScorchedClient *ScorchedClient::instance()
{
	if (!instance_)
	{
		instance_ = new ScorchedClient;
	}
	return instance_;
}

ScorchedClient::ScorchedClient() : 
	ScorchedContext("Client")
{
	targetSpace_->setContext(this);

	mainLoop_ = new MainLoop();
	mainLoop_->clear();

	gameState = new GameState(mainLoop_, "Client");
	mainLoop_->addMainLoop(gameState);

	clientSimulator_ = new ClientSimulator();
	clientSimulator_->setScorchedContext(this);

	new ComsMessageHandlerIAdapter<ClientSimulator>(
		clientSimulator_, &ClientSimulator::processComsSimulateMessage,
		ComsSimulateMessage::ComsSimulateMessageType,
		getComsMessageHandler());
	new ComsMessageHandlerIAdapter<ClientSimulator>(
		clientSimulator_, &ClientSimulator::processNetStatMessage,
		ComsNetStatMessage::ComsNetStatMessageType,
		getComsMessageHandler());

	// Calculate how many particles we can see
	int numberOfBilboards = 6000;
	if (OptionsDisplay::instance()->getEffectsDetail() == 0) 
		numberOfBilboards = 100;
	else if (OptionsDisplay::instance()->getEffectsDetail() == 2) 
		numberOfBilboards = 10000;
	particleEngine_ = new ParticleEngine(
		&MainCamera::instance()->getCamera(), 
		numberOfBilboards);

	getLandscapeMaps().getGroundMaps().getHeightMap().setGraphicalMap(
		new GraphicalLandscapeMap());
	getLandscapeMaps().getRoofMaps().getRoofMap().setGraphicalMap(
		new GraphicalLandscapeMap());
}

ScorchedClient::~ScorchedClient()
{
	targetSpace_->clear();
}

Simulator &ScorchedClient::getSimulator() 
{ 
	return *clientSimulator_; 
}
