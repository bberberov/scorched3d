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

#include <weapons/Weapon.h>
#include <weapons/AccessoryStore.h>
#include <engine/GameState.h>
#include <engine/ActionController.h>
#include <engine/MainLoop.h>
#include <engine/ObjectGroup.h>
#include <engine/ObjectGroups.h>
#include <engine/ObjectGroupEntry.h>
#include <client/ClientState.h>
#include <client/ScorchedClient.h>
#include <server/ScorchedServer.h>
#include <target/TargetContainer.h>
#include <tanket/TanketAccessories.h>
#include <tank/TankState.h>
#include <tank/TankModel.h>
#include <tank/TankModelContainer.h>
#include <tankai/TankAI.h>
#include <target/TargetParachute.h>
#include <target/TargetShield.h>
#include <target/TargetLife.h>
#include <tankgraph/TankMenus.h>
#include <tankgraph/TankKeyboardControlUtil.h>
#include <GLW/GLWWindowManager.h>
#include <client/ClientParams.h>
#include <graph/OptionsDisplay.h>
#include <common/FileLogger.h>
#include <common/Defines.h>
#include <common/Logger.h>
#include <coms/ComsMessageSender.h>
#include <coms/ComsMessageHandler.h>
#include <landscape/Landscape.h>
#include <landscapemap/LandscapeMaps.h>
#include <tankgraph/RenderTracer.h>
#include <tankgraph/TargetRendererImplTarget.h>
#include <dialogs/MainMenuDialog.h>
#include <dialogs/QuitDialog.h>
#include <dialogs/SkipDialog.h>
#include <dialogs/SaveDialog.h>
#include <sound/SoundUtils.h>
#include <console/ConsoleRuleFnIAdapter.h>
#include <console/ConsoleRuleMethodIAdapter.h>
#include <image/ImageFactory.h>
#include <GLEXT/GLTexture.h>
#include <GLEXT/GLTextureStore.h>
#include <lua/LUAScriptFactory.h>

TankMenus::TankMenus() : logger_("ClientLog")
{
	new ConsoleRuleMethodIAdapter<Landscape>(
		Landscape::instance(), &Landscape::updatePlanTexture, "ResetPlan");
	new ConsoleRuleMethodIAdapter<Landscape>(
		Landscape::instance(), &Landscape::updatePlanATexture, "ResetAPlan");

	new ConsoleRuleMethodIAdapter<TankMenus>(
		this, &TankMenus::resetLandscape, "ResetLandscape");
	new ConsoleRuleMethodIAdapter<TankMenus>(
		this, &TankMenus::clearTracerLines, "ClearTracerLines");
	new ConsoleRuleMethodIAdapter<TankMenus>(
		this, &TankMenus::showTankDetails, "TankDetails");
	new ConsoleRuleMethodIAdapter<TankMenus>(
		this, &TankMenus::showTargetDetails, "TargetDetails");
	new ConsoleRuleMethodIAdapter<TankMenus>(
		this, &TankMenus::showTextureDetails, "TextureDetails");
	new ConsoleRuleMethodIAdapter<TankMenus>(
		this, &TankMenus::logToFile, "LogToFile");
	new ConsoleRuleMethodIAdapter<TankMenus>(
		this, &TankMenus::groupInfo, "GroupInfo");

	//new ConsoleRuleMethodIAdapterEx<TankMenus>(
	//	this, &TankMenus::runScriptConsole, "RunScript");
	new ConsoleRuleFnIBooleanAdapter(
		"ComsMessageLogging", 
		ScorchedClient::instance()->getComsMessageHandler().getMessageLogging());
	new ConsoleRuleFnIBooleanAdapter(
		"StateLogging", 
		ScorchedClient::instance()->getGameState().getStateLogging());
	new ConsoleRuleFnIBooleanAdapter(
		"StateTimeLogging",
		ScorchedClient::instance()->getGameState().getStateTimeLogging());
	new ConsoleRuleFnIBooleanAdapter(
		"AILogging",
		TankAI::getTankAILogging());
	
	if (OptionsDisplay::instance()->getClientLogToFile())
	{
		logToFile();
	}

	unsigned int logState = OptionsDisplay::instance()->getClientLogState();
	if (logState & 0x1)
	{
		ScorchedClient::instance()->getGameState().getStateLogging() = true;
	}
	if (logState & 0x2)
	{
		ScorchedClient::instance()->getComsMessageHandler().getMessageLogging() = true;
	}
}

TankMenus::~TankMenus()
{

}

void TankMenus::groupInfo()
{
	std::map<std::string, ObjectGroup *> *groups = 
		ScorchedClient::instance()->getObjectGroups().getGroups();
	if (groups)
	{
		std::map<std::string, ObjectGroup*>::iterator itor;
		for (itor = groups->begin();
			itor != groups->end();
			++itor)
		{
			const std::string &name = itor->first;
			ObjectGroup *entry = itor->second;
			Logger::log(S3D::formatStringBuffer("Group %s count %i", 
				name.c_str(),
				entry->getObjectCount()));
		}
	}
}

void TankMenus::logToFile()
{
	Logger::addLogger(&logger_);
}

void TankMenus::showTextureDetails()
{
	Console::instance()->addLine(false,
		S3D::formatStringBuffer("Textures use %u bytes", 
		GLTexture::getTextureSpace()));
	Console::instance()->addLine(false,
		S3D::formatStringBuffer("There are %u texture references", 
		GLTextureStore::instance()->getReferenceCount()));
}

void TankMenus::resetLandscape()
{
	Landscape::instance()->recalculateLandscape();
	Landscape::instance()->recalculateRoof();
}

void TankMenus::clearTracerLines()
{
	RenderTracer::instance()->clearTracerLines();
}

void TankMenus::showInventory()
{
	std::map<unsigned int, Tank *> &tanks = 
		ScorchedClient::instance()->getTargetContainer().getTanks();
	std::map<unsigned int, Tank *>::iterator itor;
	for (itor = tanks.begin();
		itor != tanks.end();
		++itor)
	{
		Tank *tank = (*itor).second;
		Console::instance()->addLine(false,
			S3D::formatStringBuffer("--%s------------------------------------",
			tank->getCStrName().c_str()));

		std::list<Accessory *> accessories;
		tank->getAccessories().getAllAccessories(accessories);
		ScorchedClient::instance()->getAccessoryStore().sortList(accessories, AccessoryStore::SortName);

		std::list<Accessory *>::iterator aitor;
		for (aitor = accessories.begin();
			aitor != accessories.end();
			++aitor)
		{
			Accessory *accessory = (*aitor);

			Console::instance()->addLine(false,
				S3D::formatStringBuffer("%s - %i", accessory->getName(), 
				tank->getAccessories().getAccessoryCount(accessory)));
		}

		Console::instance()->addLine(false,
			"----------------------------------------------------");
	}
}

void TankMenus::showTargetDetails()
{
	std::map<std::string, unsigned int> results;
	std::map<unsigned int, Target *> &targets = 
		ScorchedClient::instance()->getTargetContainer().getTargets();
	std::map<unsigned int, Target *>::iterator itor;
	for (itor = targets.begin();
		itor != targets.end();
		++itor)
	{
		Target *target = (*itor).second;

		std::string name = target->getCStrName();
		if (target->getType() != Target::TypeTank &&
			target->getRenderer() &&
			name.empty())
		{
			TargetRendererImplTarget *renderer =
				(TargetRendererImplTarget *) target->getRenderer();
			name = renderer->getModelId().getMeshName();
		}

		if (results.find(name) == results.end()) results[name] = 1;
		else results[name]++;
	}

	char buffer[1024];
	Console::instance()->addLine(false,
		"--Target Dump-----------------------------------------");
	std::map<std::string, unsigned int>::iterator resultItor;
	for (resultItor = results.begin();
		resultItor != results.end();
		++resultItor)
	{
		snprintf(buffer, 1024, "\"%s\" - %u", resultItor->first.c_str(), resultItor->second);
		Console::instance()->addLine(false, buffer);
	}
	snprintf(buffer, 1024, "TOTAL - %u", targets.size());
	Console::instance()->addLine(false, buffer);
	Console::instance()->addLine(false,
		"----------------------------------------------------");
}

void TankMenus::showTankDetails()
{
	std::map<unsigned int, Tank *> &tanks = 
		ScorchedClient::instance()->getTargetContainer().getTanks();
	Tank *currentTank = 
		ScorchedClient::instance()->getTargetContainer().getCurrentTank();

	Console::instance()->addLine(false,
		"--Tank Dump-----------------------------------------");
		
	std::map<unsigned int, Tank *>::iterator itor;
	for (itor = tanks.begin();
		itor != tanks.end();
		++itor)
	{
		Tank *tank = (*itor).second;

		const char *description = "Unknown";
		if (!ClientParams::instance()->getConnectedToServer())
		{
			if (ScorchedServer::serverStarted())
			{
				Tank *otherTank = ScorchedServer::instance()->
					getTargetContainer().getTankById(tank->getPlayerId());
				if (otherTank && !otherTank->getTankAI())
				{
					description = "Human";
				}
				else if (otherTank && otherTank->getTankAI())
				{
					description = otherTank->getTankAI()->getName();
				}
			}
		}

		char buffer[1024];
		snprintf(buffer, 1024, "%u %c %8s - \"%10s\" (%s)", 
			tank->getPlayerId(),
			currentTank == tank?'>':' ',
			description,
			tank->getCStrName().c_str(), 
			tank->getModelContainer().getTankModel()->getName());
		Console::instance()->addLine(false, buffer);
	}

	Console::instance()->addLine(false,
		"----------------------------------------------------");
}

// Player Menus

TankMenus::PlayerMenu::PlayerMenu()
{
	GLMenuItem bar(LANG_STRING("----------"));
	bar.setSeperator();

	Image *map = new Image(ImageFactory::loadImage(
		S3D::eDataLocation,
		"data/images/setting.bmp",
		"data/images/settinga.bmp",
		false));
	DIALOG_ASSERT(map->getBits());
	MainMenuDialog::instance()->addMenu(LANG_RESOURCE("GAME", "Game"), 
		"Game",
		LANG_RESOURCE("GAME_MENU", "Options affecting the game"),
		32, 
		ClientState::StatePlaying, this, map);

	MainMenuDialog::instance()->addMenuItem("Game", 
		GLMenuItem(LANG_RESOURCE("CLEAR_TRACERS", "Clear Tracers"),
		new ToolTip(ToolTip::ToolTipHelp, 
			LANG_RESOURCE("CLEAR_TRACERS", "Clear Tracers"), 
			LANG_RESOURCE("CLEAR_TRACERS_TOOLTIP", "Remove all tracer lines."))));

	MainMenuDialog::instance()->addMenuItem("Game", bar);

	MainMenuDialog::instance()->addMenuItem("Game", 
		GLMenuItem(LANG_RESOURCE("SKIP_MOVE", "Skip Move"),
		new ToolTip(ToolTip::ToolTipHelp, 
			LANG_RESOURCE("SKIP_MOVE", "Skip Move"), 
			LANG_RESOURCE("SKIP_MOVE_TOOLTIP", "Player forfits this move."))));
	MainMenuDialog::instance()->addMenuItem("Game", 
		GLMenuItem(LANG_RESOURCE("RESIGN_ROUND", "Resign Round"),
		new ToolTip(ToolTip::ToolTipHelp, 
			LANG_RESOURCE("RESIGN_ROUND", "Resign Round"), 
			LANG_RESOURCE("RESIGN_ROUND_TOOLTIP", 
			"Player resigns from this round.\n"
			"Player takes no part in the rest of the round."))));

	MainMenuDialog::instance()->addMenuItem("Game", bar);

	MainMenuDialog::instance()->addMenuItem("Game", 
		GLMenuItem(LANG_RESOURCE("EXIT_GAME", "Exit Game"),
		new ToolTip(ToolTip::ToolTipHelp, 
			LANG_RESOURCE("EXIT_GAME", "Exit Game"), 
			LANG_RESOURCE("EXIT_GAME_TOOLTIP", "Stop Playing Scorched."))));
	if (!ClientParams::instance()->getConnectedToServer())
	{
		MainMenuDialog::instance()->addMenuItem("Game",
			GLMenuItem(LANG_RESOURCE("MASS_TANK_KILL", "Mass Tank Kill"),
			new ToolTip(ToolTip::ToolTipHelp, 
				LANG_RESOURCE("MASS_TANK_KILL", "Mass Tank Kill"),
				LANG_RESOURCE("MASS_TANK_KILL_TOOLTIP", 
				"Kill all tanks.\nStarts the next round."))));

		MainMenuDialog::instance()->addMenuItem("Game", bar);

		MainMenuDialog::instance()->addMenuItem("Game",
			GLMenuItem(LANG_RESOURCE("SAVE", "Save"),
			new ToolTip(ToolTip::ToolTipHelp, 
				LANG_RESOURCE("SAVE", "Save"),
				LANG_RESOURCE("SAVE_TOOLTIP", "Save this game."))));
	}
}

void TankMenus::PlayerMenu::menuSelection(const char* menuName, 
	const int position, GLMenuItem &item)
{
	Tank *firstTank = ScorchedClient::instance()->getTargetContainer().getCurrentTank();
	if (firstTank)
	{
		switch (position)
		{
		case 0:
			RenderTracer::instance()->clearTracerLines();
			RenderTracer::instance()->clearTracers();
			break;
		case 2:
			GLWWindowManager::instance()->showWindow(
				SkipDialog::instance()->getId());
			break;
		case 3:
			GLWWindowManager::instance()->showWindow(
				SkipDialog::instance()->getId());
			break;
		case 5:
			GLWWindowManager::instance()->showWindow(
				QuitDialog::instance()->getId());
			break;
		case 6:
			GLWWindowManager::instance()->showWindow(
				QuitDialog::instance()->getId());
			break;
		case 8:
			GLWWindowManager::instance()->showWindow(
				SaveDialog::instance()->getId());
			break;
		}
	}
}

bool TankMenus::PlayerMenu::getEnabled(const char* menuName)
{
	if (ScorchedClient::instance()->getGameState().getState() 
		!= ClientState::StatePlaying) return false;

	Tank *firstTank = ScorchedClient::instance()->getTargetContainer().getCurrentTank();
	if (firstTank)
	{
		return (firstTank->getState().getState() == TankState::sNormal);
	}
	return false;
}

// Accessory Menus
TankMenus::AccessoryMenu::AccessoryMenu()
{
	Image *map = new Image(ImageFactory::loadImage(
		S3D::eDataLocation,
		"data/images/bomb.bmp",
		"data/images/bomba.bmp",
		false));
	DIALOG_ASSERT(map->getBits());
	MainMenuDialog::instance()->addMenu(LANG_RESOURCE("WEAPONS", "Weapons"), 
		"Weapons", 
		LANG_RESOURCE("WEAPONS_WINDOW", "Change the current weapon and enable defenses"),
		32, 
		ClientState::StatePlaying, this, map);
}

void TankMenus::AccessoryMenu::menuSelection(const char* menuName, 
	const int position, GLMenuItem &item)
{
	Accessory *accessory = (Accessory *) item.getUserData();
	Tank *tank = ScorchedClient::instance()->getTargetContainer().getCurrentTank();
	if (tank && accessory)
	{
		switch (accessory->getType())
		{
		case AccessoryPart::AccessoryParachute:
			TankKeyboardControlUtil::parachutesUpDown(
				tank->getPlayerId(),
				(tank->getParachute().getCurrentParachute()==accessory)?
				0:accessory->getAccessoryId());
			break;
		case AccessoryPart::AccessoryShield:
			TankKeyboardControlUtil::shieldsUpDown(
				tank->getPlayerId(),
				(tank->getShield().getCurrentShield()==accessory)?
				0:accessory->getAccessoryId());
			break;
		case AccessoryPart::AccessoryWeapon:
			tank->getAccessories().getWeapons().setWeapon(accessory);
			break;
		case AccessoryPart::AccessoryBattery:
			if (tank->getLife().getLife() < 
				tank->getLife().getMaxLife())
			{
				TankKeyboardControlUtil::useBattery(
					tank->getPlayerId(), 
					accessory->getAccessoryId());
			}
			break;
		case AccessoryPart::AccessoryAutoDefense:
			default:
			break;
		}
	}
}

bool TankMenus::AccessoryMenu::getMenuItems(const char* menuName, 
											std::list<GLMenuItem> &result)
{
	Tank *firstTank = ScorchedClient::instance()->getTargetContainer().getCurrentTank();
	if (!firstTank) return true;

	std::string lastGroup;
	bool firstIteration = true;
	std::set<std::string> tabGroups = ScorchedClient::instance()->
		getAccessoryStore().getTabGroupNames();
	std::set<std::string>::iterator groupitor;
	for (groupitor = tabGroups.begin();
		groupitor != tabGroups.end();
		++groupitor)
	{
		const char *group = (*groupitor).c_str();
		std::list<Accessory *> weapons = ScorchedClient::instance()->
			getAccessoryStore().getAllAccessoriesByTabGroup(
				group,
				OptionsDisplay::instance()->getAccessorySortKey());

		std::list<Accessory *>::iterator itor;
		for (itor = weapons.begin();
			itor != weapons.end();
			++itor)
		{
			Accessory *accessory = (*itor);
			if (!firstTank->getAccessories().canUse(accessory)) continue;

			bool sel = false;
			switch (accessory->getType())
			{
			case AccessoryPart::AccessoryParachute:
				sel = (firstTank->getParachute().getCurrentParachute() == accessory);
				break;
			case AccessoryPart::AccessoryShield:
				sel = (firstTank->getShield().getCurrentShield() == accessory);
				break;
			case AccessoryPart::AccessoryWeapon:
				sel = (firstTank->getAccessories().getWeapons().getCurrent() == accessory);
				break;
			case AccessoryPart::AccessoryAutoDefense:
				sel = true;
				break;
			default:
			case AccessoryPart::AccessoryBattery:
				sel = false;
				break;
			}

			if (!firstIteration &&
				0 != strcmp(lastGroup.c_str(), accessory->getTabGroupName()))
			{
				GLMenuItem bar(LANG_STRING("----------"));
				bar.setSeperator();
				result.push_back(bar);
			}
			lastGroup = accessory->getTabGroupName();
			firstIteration = false;

			result.push_back(
				GLMenuItem(
					firstTank->getAccessories().getAccessoryAndCountString(accessory), 
					&accessory->getToolTip(), 
					sel,
					&accessory->getTexture(),
					accessory));
		}
	}
	return true;
}

bool TankMenus::AccessoryMenu::getEnabled(const char* menuName)
{
	if (ScorchedClient::instance()->getGameState().getState() != 
		ClientState::StatePlaying) return false;

	Tank *firstTank = ScorchedClient::instance()->getTargetContainer().getCurrentTank();
	if (firstTank)
	{
		return (firstTank->getState().getState() == TankState::sNormal);
	}
	return false;
}

void TankMenus::runScriptConsole(std::list<ConsoleRuleValue> list)
{
	/*
	list.pop_front();
	if (!list.empty())
	{
		const char *fileName = (char *) list.begin()->rule.c_str();
		LUAScript *script = 
			ScorchedClient::instance()->getLUAScriptFactory().createScript();

		std::string luaErrorString;
		if (!script->loadFromFile(fileName, luaErrorString))
		{
			Logger::log(S3D::formatStringBuffer("Script returned an error : %s",
				luaErrorString.c_str()));
		}

		delete script;
	}
	*/
}

