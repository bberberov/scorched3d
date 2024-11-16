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

#include <engine/ScorchedContext.h>
#include <engine/GameState.h>
#include <engine/ActionController.h>
#include <engine/Simulator.h>
#include <engine/ModFiles.h>
#include <engine/ObjectGroups.h>
#include <net/NetInterface.h>
#include <coms/ComsMessageHandler.h>
#include <common/OptionsTransient.h>
#include <target/TargetContainer.h>
#include <tank/TankTeamScore.h>
#include <tank/TankModelStore.h>
#include <tanket/TanketTypes.h>
#include <target/TargetSpace.h>
#include <movement/TargetMovement.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscapedef/LandscapeDefinitions.h>
#include <weapons/AccessoryStore.h>
#include <tankai/TankAIStrings.h>
#include <lua/LUAScriptHook.h>
#include <events/EventController.h>

ScorchedContext::ScorchedContext(const char *name)
{
	objectGroups_ = new ObjectGroups();
	accessoryStore_ = new AccessoryStore();
	targetContainer_ = new TargetContainer();
	landscapeMaps_ = new LandscapeMaps();
	comsMessageHandler_ = new ComsMessageHandler(name);
	netInterface_ = (NetInterface *) 0;
	optionsGame_ = new OptionsScorched();
	optionsTransient_ = new OptionsTransient(*optionsGame_);
	modFiles_ = new ModFiles();
	landscapes_ = new LandscapeDefinitions();
	tanketTypes_ = new TanketTypes();
	tankModelStore_ = new TankModelStore();
	tankTeamScore_ = new TankTeamScore();
	targetMovement_ = new TargetMovement();
	luaScriptFactory_ = new LUAScriptFactory();
	luaScriptHook_ = new LUAScriptHook(luaScriptFactory_,
		name[0]=='S'?"server":"client",
		name[0]=='S'?S3D::getSettingsFile("serverhooks"):S3D::getSettingsFile("clienthooks"));
	tankAIStrings_ = new TankAIStrings();
	eventController_ = new EventController();

	luaScriptFactory_->setContext(this);
}

ScorchedContext::~ScorchedContext()
{
	if (netInterface_) getNetInterface().stop();

	delete landscapeMaps_;
	delete comsMessageHandler_;
	delete netInterface_;
	delete optionsGame_;
	delete optionsTransient_;
	delete modFiles_;
	delete accessoryStore_;
	delete landscapes_;
	delete targetContainer_;
	delete tanketTypes_;
	delete tankModelStore_;
	delete tankTeamScore_;
	delete targetMovement_;
	delete luaScriptFactory_;
	delete luaScriptHook_;
	delete tankAIStrings_;
	delete objectGroups_;
	delete eventController_;
}

ActionController &ScorchedContext::getActionController()
{
	return getSimulator().getActionController();
}
