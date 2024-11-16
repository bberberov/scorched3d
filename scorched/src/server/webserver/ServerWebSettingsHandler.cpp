////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2004
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
//    You should have received a copy of the GNU General Public License
//    along with Scorched3D; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

#include <webserver/ServerWebSettingsHandler.h>
#include <webserver/ServerWebServerUtil.h>
#include <server/ServerLog.h>
#include <server/ScorchedServer.h>
#include <server/ServerCommon.h>
#include <server/ServerAdminCommon.h>
#include <server/ServerParams.h>
#include <landscapedef/LandscapeDefinitionsBase.h>
#include <common/Defines.h>
#include <common/Logger.h>
#include <common/OptionsScorched.h>
#include <common/OptionsTransient.h>
#include <engine/ModDirs.h>
#include <engine/ModFiles.h>
#include <tankai/TankAINames.h>
#include <XML/XMLParser.h>
#include <vector>
#include <algorithm>

static const char *getField(std::map<std::string, std::string> &fields, const char *field)
{
	std::map<std::string, std::string>::iterator itor = 
		fields.find(field);
	if (itor != fields.end())
	{
		return (*itor).second.c_str();
	}
	return 0;
}

static void setValues(ServerAdminSessions::Credential &credential,
	std::map<std::string, std::string> &fields)
{
	std::list<OptionEntry *>::iterator itor;
	{
		std::list<OptionEntry *> &options = 
			ScorchedServer::instance()->getOptionsGame().
				getChangedOptions().getOptions();
		for (itor = options.begin();
			itor != options.end();
			++itor)
		{
			OptionEntry *entry = (*itor);
			std::map<std::string, std::string>::iterator findItor =
				fields.find(entry->getName());
			if (findItor != fields.end())
			{
				const char *value = (*findItor).second.c_str();
				if (0 != strcmp(entry->getValueAsString(), value))
				{
					entry->setValueFromString(value);

					if (!(entry->getData() & OptionEntry::DataProtected))
					{
						ServerAdminCommon::adminLog(ChannelText("info",
							"CHANGE_SETTING",
							"\"{0}\" settings change {1}=\"{2}\"",
							credential.username,
							entry->getName(),
							entry->getValueAsString()));
					}
				}
			}
		}	
	}
	{
		std::list<OptionEntry *> &options = 
			ScorchedServer::instance()->getOptionsGame().
				getChangedOptions().getPlayerTypeOptions();
		for (itor = options.begin();
			itor != options.end();
			++itor)
		{
			OptionEntry *entry = (*itor);
			std::map<std::string, std::string>::iterator findItor =
				fields.find(entry->getName());
			if (findItor != fields.end())
			{
				const char *value = (*findItor).second.c_str();
				if (0 != strcmp(entry->getValueAsString(), value))
				{
					entry->setValueFromString(value);

					ServerAdminCommon::adminLog(ChannelText("info",
						"CHANGE_SETTING",
						"\"{0}\" settings change {1}=\"{2}\"",
						credential.username,
						entry->getName(),
						entry->getValueAsString()));
				}
			}
		}	
	}
}

static void loadSettings(ServerAdminSessions::Credential &credential)
{
	ScorchedServer::instance()->getOptionsGame().getChangedOptions().
		readOptionsFromFile((char *) ServerParams::instance()->getServerFile());

	ServerAdminCommon::adminLog(ChannelText("info",
		"LOAD_SETTING",
		"\"{0}\" load settings",
		credential.username));
}

static void saveSettings(ServerAdminSessions::Credential &credential)
{
	ScorchedServer::instance()->getOptionsGame().getChangedOptions().
		writeOptionsToFile((char *) ServerParams::instance()->getServerFile(), 
			ServerParams::instance()->getWriteFullOptions());

	ServerAdminCommon::adminLog(ChannelText("info",
		"SAVE_SETTING",
		"\"{0}\" save settings",
		credential.username));
}

bool ServerWebSettingsHandler::SettingsPlayersHandler::processRequest(
	ServerWebServerIRequest &request,
	std::string &text)
{
	if (!request.getSession()->credentials.hasPermission(
		ServerAdminSessions::PERMISSION_ALTERSETTINGS)) return true;

	const char *action = getField(request.getFields(), "action");
	if (action && 0 == strcmp(action, "Load"))
	{
		loadSettings(request.getSession()->credentials);
	}
	else
	{
		setValues(request.getSession()->credentials, request.getFields());
	}

	{
		std::list<std::string> ais;
		TankAINames::loadAIs(ais);
		
		std::string players;
		std::list<OptionEntry *>::iterator itor;
		std::list<OptionEntry *> &options = 
			ScorchedServer::instance()->getOptionsGame().
				getChangedOptions().getPlayerTypeOptions();
		for (itor = options.begin();
			itor != options.end();
			++itor)
		{
			OptionEntry *entry = (*itor);
			
			std::string value = "";

			value.append("<tr><td>");
			value.append(entry->getName());
			value.append("</td><td>");
			value.append(S3D::formatStringBuffer("<select name='%s'>", entry->getName()));
			std::list<std::string>::iterator aiitor;
			for (aiitor = ais.begin(); aiitor != ais.end(); ++aiitor)
			{
				std::string &ai = (*aiitor);
				value.append(S3D::formatStringBuffer("<option %s>%s</option>",
					(0 == strcmp(entry->getValueAsString(), ai.c_str())?"selected":""), 
					ai.c_str()));		
			}
			value.append("</select>");
			value.append("</td></tr>\n");
			
			players.append(value);
		}
		
		request.getFields()["PLAYERS"] = players;
	}	

	if (action && 0 == strcmp(action, "Save"))
	{
		saveSettings(request.getSession()->credentials);
	}

	return ServerWebServerUtil::getHtmlTemplate(request.getSession(), "settingsplayers.html", request.getFields(), text);
}

bool ServerWebSettingsHandler::SettingsLandscapeHandler::processRequest(
	ServerWebServerIRequest &request,
	std::string &text)
{
	if (!request.getSession()->credentials.hasPermission(
		ServerAdminSessions::PERMISSION_ALTERSETTINGS)) return true;

	const char *action = getField(request.getFields(), "action");
	if (action && 0 == strcmp(action, "Load"))
	{
		loadSettings(request.getSession()->credentials);
	}

	LandscapeDefinitionsBase landscapeDefinitions;
	landscapeDefinitions.readLandscapeDefinitions();
	std::list<LandscapeDefinitionsEntry> &defns =
		landscapeDefinitions.getAllLandscapes();
	std::list<LandscapeDefinitionsEntry>::iterator itor;

	OptionsGame &optionsGame = 
		ScorchedServer::instance()->getOptionsGame().getChangedOptions();

	// Set new options if any 
	if (action && (
		0 == strcmp(action, "Save") || 
		0 == strcmp(action, "Set") || 
		0 == strcmp(action, "Select All") || 
		0 == strcmp(action, "Select None")
		))
	{
		std::string landscapesString = "";
		if (0 != strcmp(action, "Select None"))
		{
			for (itor = defns.begin();
				itor != defns.end();
				++itor)
			{
				LandscapeDefinitionsEntry &dfn = *itor;
		
				const char *setting = getField(request.getFields(), dfn.name.c_str());
				if (0 == strcmp(action, "Select All") || 
					(setting && 0 == strcmp(setting, "on")))
				{
					if (!landscapesString.empty()) landscapesString.append(":");
					landscapesString.append(dfn.name.c_str());
				}
			}
		}
		else
		{
			landscapesString = " ";
		}
		optionsGame.getLandscapesEntry().setValue(landscapesString.c_str());

		ServerAdminCommon::adminLog(ChannelText("info",
			"CHANGE_SETTING",
			"\"{0}\" settings change {1}=\"{2}\"",
			request.getSession()->credentials.username,
			optionsGame.getLandscapesEntry().getName(),
			optionsGame.getLandscapesEntry().getValueAsString()));
	}

	// Read the current options
	std::string landscapes;
	for (itor = defns.begin();
		itor != defns.end();
		++itor)
	{
		LandscapeDefinitionsEntry &dfn = *itor;

		bool enabled = landscapeDefinitions.landscapeEnabled(
			optionsGame, dfn.name.c_str());

		std::string value = S3D::formatStringBuffer(
			"<input type='radio' name='%s' %s value='on'>On</input>"
			"<input type='radio' name='%s' %s value='off'>Off</input>",
			dfn.name.c_str(), (enabled?"checked":""),
			dfn.name.c_str(), (!enabled?"checked":""));

		landscapes.append(S3D::formatStringBuffer("<tr><td>%s</td><td>%s</td><td>%s</td></tr>\n",
			dfn.name.c_str(), 
			(dfn.description.c_str()[0]?dfn.description.c_str():"-"), 
			value.c_str()));
	}
	request.getFields()["LANDSCAPES"] = landscapes;

	if (action && 0 == strcmp(action, "Save"))
	{
		saveSettings(request.getSession()->credentials);
	}

	return ServerWebServerUtil::getHtmlTemplate(request.getSession(), "settingslandscape.html", request.getFields(), text);
}

bool ServerWebSettingsHandler::SettingsAllHandler::processRequest(
	ServerWebServerIRequest &request,
	std::string &text)
{
	if (!request.getSession()->credentials.hasPermission(
		ServerAdminSessions::PERMISSION_ALTERSETTINGS)) return true;

	std::list<OptionEntry *>::iterator itor;
	std::list<OptionEntry *> &options = 
		ScorchedServer::instance()->getOptionsGame().
			getChangedOptions().getOptions();

	const char *action = getField(request.getFields(), "action");
	if (action && 0 == strcmp(action, "Load"))
	{
		loadSettings(request.getSession()->credentials);
	}
	else
	{
		// Check if any changes have been made
		setValues(request.getSession()->credentials, request.getFields());
	}

	// Show the current settings
	std::string settings;
	for (itor = options.begin();
		itor != options.end();
		++itor)
	{
		OptionEntry *entry = (*itor);
		if (!(entry->getData() & OptionEntry::DataDepricated))
		{
			std::string value;
			ServerWebServerUtil::generateSettingValue(entry, value);
			
			bool different = (0 != strcmp(entry->getValueAsString(), 
				entry->getDefaultValueAsString()));
			std::string desc = entry->getDescription();
			desc.append("<br>Default value : ");
			if (different) desc.append("<b>");
			desc.append(entry->getDefaultValueAsString());
			if (different) desc.append("</b>");

			settings += S3D::formatStringBuffer("<tr><td>%s</td>"
				"<td><font size=-1>%s</font></td>"
				"<td>%s</td></tr>\n",
				entry->getName(),
				desc.c_str(),
				value.c_str());
		}
	}

	request.getFields()["SETTINGS"] = settings;

	if (action && 0 == strcmp(action, "Save"))
	{
		saveSettings(request.getSession()->credentials);
	}

	return ServerWebServerUtil::getHtmlTemplate(request.getSession(), "settingsall.html", request.getFields(), text);
}

bool ServerWebSettingsHandler::SettingsMainHandler::processRequest(
	ServerWebServerIRequest &request,
	std::string &text)
{
	if (!request.getSession()->credentials.hasPermission(
		ServerAdminSessions::PERMISSION_ALTERSETTINGS)) return true;

	std::list<OptionEntry *>::iterator itor;
	std::list<OptionEntry *> &options = 
		ScorchedServer::instance()->getOptionsGame().
			getChangedOptions().getOptions();

	const char *action = getField(request.getFields(), "action");
	if (action && 0 == strcmp(action, "Load"))
	{
		loadSettings(request.getSession()->credentials);
	}
	else
	{
		// Check if any changes have been made
		setValues(request.getSession()->credentials, request.getFields());
	}

	if (action && 0 == strcmp(action, "Save"))
	{
		saveSettings(request.getSession()->credentials);
	}

	return ServerWebServerUtil::getHtmlTemplate(request.getSession(), "settingsmain.html", request.getFields(), text);
}

bool ServerWebSettingsHandler::SettingsModHandler::processRequest(
	ServerWebServerIRequest &request,
	std::string &text)
{
	if (!request.getSession()->credentials.hasPermission(
		ServerAdminSessions::PERMISSION_ALTERSETTINGS)) return true;

	std::list<OptionEntry *>::iterator itor;
	std::list<OptionEntry *> &options = 
		ScorchedServer::instance()->getOptionsGame().
			getChangedOptions().getOptions();

	const char *action = getField(request.getFields(), "action");
	if (action && 0 == strcmp(action, "Load"))
	{
		loadSettings(request.getSession()->credentials);
	}
	else
	{
		// Check if any changes have been made
		setValues(request.getSession()->credentials, request.getFields());
	}

	// Import/upload a mod (if specified)
	if (!request.getParts().empty())
	{
		std::map<std::string, NetMessage *>::iterator modupload =
			request.getParts().find("modupload");
		if (modupload != request.getParts().end())
		{
			NetMessage *message = (*modupload).second;
			ModFiles files;
			std::string mod;
			if (files.importModFiles(mod, message->getBuffer()))
			{
				return ServerWebServerUtil::getHtmlMessage(
					request.getSession(), 
					"Mod Upload", 
					S3D::formatStringBuffer("Successfuly uploaded and imported mod %s",
					(mod[0]?mod.c_str():"Unknown")), 
					request.getFields(), text);
			}
			else
			{
				return ServerWebServerUtil::getHtmlMessage(
					request.getSession(), 
					"Mod Upload", 
					"Failed to load mod files from network", 
					request.getFields(), text);
			}
		}
	}

	// Display the list of mods, and the displayed the currently selected mod
	ModDirs modDirs;
	if (modDirs.loadModDirs())
	{
		OptionEntryString &modOption = 
			ScorchedServer::instance()->getOptionsGame().getChangedOptions().getModEntry();

		std::string mods;
		std::list<ModInfo>::iterator itor;
		for (itor = modDirs.getDirs().begin();
			itor != modDirs.getDirs().end();
			++itor)
		{
			ModInfo &modInfo = (*itor);

			mods.append(S3D::formatStringBuffer("<tr><td>%s</td><td>%s</td><td><input type='radio' name='%s' %s value='%s'></input></td>\n",
				modInfo.getName(),
				modInfo.getDescription(),
				modOption.getName(),
				((0 == strcmp(modOption.getValueAsString(), modInfo.getName()))?"checked":""),
				modInfo.getName()));
		}

		request.getFields()["MODS"] = mods;
	}

	if (action && 0 == strcmp(action, "Save"))
	{
		saveSettings(request.getSession()->credentials);
	}

	return ServerWebServerUtil::getHtmlTemplate(request.getSession(), "settingsmod.html", request.getFields(), text);
}
