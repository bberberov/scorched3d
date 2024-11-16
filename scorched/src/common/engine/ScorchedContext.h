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

#if !defined(__INCLUDE_ScorchedContexth_INCLUDE__)
#define __INCLUDE_ScorchedContexth_INCLUDE__

class AccessoryStore;
class ActionController;
class TargetSpace;
class TargetContainer;
class TargetMovement;
class LandscapeMaps;
class ComsMessageHandler;
class NetInterface;
class OptionsScorched;
class OptionsTransient;
class ModFiles;
class LandscapeDefinitions;
class TargetContainer;
class TanketContainer;
class TankModelStore;
class TanketTypes;
class TankTeamScore;
class LUAScriptFactory;
class LUAScriptHook;
class Simulator;
class TankAIStrings;
class ObjectGroups;
class EventController;

class ScorchedContext
{
public:
	ScorchedContext(const char *name);
	virtual ~ScorchedContext();

	virtual bool getServerMode() = 0;

	void setNetInterface(NetInterface *i) { netInterface_ = i; }
	NetInterface &getNetInterface() { return *netInterface_; }
	bool getNetInterfaceValid() { return netInterface_ != 0; }

	virtual TargetSpace &getTargetSpace() = 0;
	virtual Simulator &getSimulator() = 0;
	AccessoryStore &getAccessoryStore() { return *accessoryStore_; }
	TargetContainer &getTargetContainer() { return *targetContainer_; }
	TargetMovement &getTargetMovement() { return *targetMovement_; }
	TankTeamScore &getTankTeamScore() { return *tankTeamScore_; }
	ActionController &getActionController();
	LandscapeMaps &getLandscapeMaps() { return *landscapeMaps_; }
	OptionsScorched &getOptionsGame() { return *optionsGame_; }
	OptionsTransient &getOptionsTransient() { return *optionsTransient_; }
	ComsMessageHandler &getComsMessageHandler() { return *comsMessageHandler_; }
	ModFiles &getModFiles() { return *modFiles_; }
	LandscapeDefinitions &getLandscapes() { return *landscapes_; }
	TanketTypes &getTanketTypes() { return *tanketTypes_; }
	TankModelStore &getTankModels() { return *tankModelStore_; }
	LUAScriptFactory &getLUAScriptFactory() { return *luaScriptFactory_; }
	LUAScriptHook &getLUAScriptHook() { return *luaScriptHook_; }
	TankAIStrings &getTankAIStrings() { return *tankAIStrings_; }
	ObjectGroups &getObjectGroups() { return *objectGroups_; }
	EventController &getEventController() { return *eventController_; }
	
protected:
	LandscapeMaps *landscapeMaps_;
	ComsMessageHandler *comsMessageHandler_;
	NetInterface *netInterface_;
	OptionsScorched *optionsGame_;
	OptionsTransient *optionsTransient_;
	ModFiles *modFiles_;
	AccessoryStore *accessoryStore_;
	LandscapeDefinitions *landscapes_;
	TargetContainer *targetContainer_;
	TankModelStore *tankModelStore_;
	TankTeamScore *tankTeamScore_;
	TanketTypes *tanketTypes_;
	TargetMovement *targetMovement_;
	LUAScriptFactory *luaScriptFactory_;
	LUAScriptHook *luaScriptHook_;
	TankAIStrings *tankAIStrings_;
	ObjectGroups *objectGroups_;
	EventController *eventController_;
};

#endif
