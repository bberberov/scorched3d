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

#include <webserver/ServerWebHandler.h>
#include <server/ServerLog.h>
#include <server/ScorchedServer.h>
#include <webserver/ServerWebServerUtil.h>
#include <server/ServerCommon.h>
#include <server/ServerAdminCommon.h>
#include <server/ServerAdminSessions.h>
#include <server/ServerParams.h>
#include <server/ServerState.h>
#include <server/ServerBanned.h>
#include <server/ServerChannelManager.h>
#include <landscapedef/LandscapeDefinitionsBase.h>
#include <engine/ModFiles.h>
#include <common/Defines.h>
#include <common/Logger.h>
#include <common/OptionsScorched.h>
#include <common/OptionsTransient.h>
#include <common/FileList.h>
#include <events/EventHandlerDataBase.h>
#include <net/NetInterface.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <tank/TankColorGenerator.h>
#include <tank/TankState.h>
#include <tank/TankScore.h>
#include <tankai/TankAIStore.h>
#include <XML/XMLParser.h>
#include <vector>
#include <algorithm>

struct LogFile
{
	std::string fileName;
	time_t fileTime;
};

static inline bool lt_logfile(const LogFile &o1, const LogFile &o2) 
{ 
	return o1.fileTime < o2.fileTime;
}

static const char *getAdminUserName(std::map<std::string, std::string> &fields)
{
	unsigned int sid = atoi(ServerWebServerUtil::getField(fields, "sid"));
	ServerAdminSessions::SessionParams *session =
		ScorchedServer::instance()->getServerAdminSessions().getSession(sid);
	if (session) return session->credentials.username.c_str();
	return "Unknown";
}

bool ServerWebHandler::PlayerHandler::processRequest(
	ServerWebServerIRequest &request,
	std::string &text)
{
	// Check for an add
	const char *addType = ServerWebServerUtil::getField(request.getFields(), "add");
	if (addType)
	{
		ServerAdminCommon::addPlayer(request.getSession()->credentials, addType);
	}

	std::map<unsigned int, Tank *> &tanks = 
		ScorchedServer::instance()->getTargetContainer().getTanks();
	std::map<unsigned int, Tank *>::iterator itor;

	// Check for any action
	const char *action = ServerWebServerUtil::getField(request.getFields(), "action");
	if (action)
	{
		for (itor = tanks.begin(); itor != tanks.end(); ++itor)
		{
			// Is this tank selected
			Tank *tank = (*itor).second;
			if (ServerWebServerUtil::getField(request.getFields(), S3D::formatStringBuffer("player-%u", tank->getPlayerId())))
			{
				if (0 == strcmp(action, "Kick"))
				{
					ServerAdminCommon::kickPlayer(request.getSession()->credentials, tank->getPlayerId());
					break;
				}
				else if (0 == strcmp(action, "Mute"))
				{
					ServerAdminCommon::mutePlayer(request.getSession()->credentials, tank->getPlayerId(), true);
				}
				else if (0 == strcmp(action, "UnMute"))
				{
					ServerAdminCommon::mutePlayer(request.getSession()->credentials, tank->getPlayerId(), false);
				}
				else if (0 == strcmp(action, "Flag"))
				{
					ServerAdminCommon::flagPlayer(request.getSession()->credentials, tank->getPlayerId(),
						ServerWebServerUtil::getField(request.getFields(), "reason"));
				}
				else if (0 == strcmp(action, "Poor"))
				{
					ServerAdminCommon::poorPlayer(request.getSession()->credentials, tank->getPlayerId());
				}
				else if (0 == strcmp(action, "PermMute"))
				{
					ServerAdminCommon::permMutePlayer(request.getSession()->credentials, tank->getPlayerId(),
						ServerWebServerUtil::getField(request.getFields(), "reason"));
				}
				else if (0 == strcmp(action, "UnPermMute"))
				{
					ServerAdminCommon::unpermMutePlayer(request.getSession()->credentials, tank->getPlayerId());
				}
				else if (0 == strcmp(action, "Banned"))
				{
					ServerAdminCommon::banPlayer(request.getSession()->credentials, tank->getPlayerId(),
						ServerWebServerUtil::getField(request.getFields(), "reason"));
				}
				else if (0 == strcmp(action, "Slap"))
				{
					ServerAdminCommon::slapPlayer(request.getSession()->credentials, tank->getPlayerId(), 25.0f);
				}
				else if (0 == strcmp(action, "ChangeName"))
				{
					ServerAdminCommon::changeNamePlayer(request.getSession()->credentials, tank->getPlayerId(), LANG_STRING("ChangeName"));
				}
				else if (0 == strcmp(action, "Kill"))
				{
					ServerAdminCommon::killPlayer(request.getSession()->credentials, tank->getPlayerId());
				}
				else if (0 == strcmp(action, "ShowAliases"))
				{
					if (!request.getSession()->credentials.hasPermission(
						ServerAdminSessions::PERMISSION_ALIASPLAYER)) return true;

					ServerWebServerUtil::getHtmlRedirect(
						S3D::formatStringBuffer("/playersthreaded?sid=%s&action=%s&uniqueid=%s",
							ServerWebServerUtil::getField(request.getFields(), "sid"),
							ServerWebServerUtil::getField(request.getFields(), "action"),
							tank->getUniqueId()), text);
					return true;
				}
				else if (0 == strcmp(action, "ShowIPAliases"))
				{
					if (!request.getSession()->credentials.hasPermission(
						ServerAdminSessions::PERMISSION_ALIASPLAYER)) return true;

					ServerWebServerUtil::getHtmlRedirect(
						S3D::formatStringBuffer("/playersthreaded?sid=%s&action=%s&uniqueid=%s",
							ServerWebServerUtil::getField(request.getFields(), "sid"),
							ServerWebServerUtil::getField(request.getFields(), "action"),
							tank->getUniqueId()), text);
					return true;
				}
				else
				{
					Logger::log(S3D::formatStringBuffer("Unknown web handler player action \"%s\"", action));
				}
			}
		}
	}

	// Player Entries
	std::string players;
	for (itor = tanks.begin(); itor != tanks.end(); ++itor)
	{
		Tank *tank = (*itor).second;
		std::string cleanName(ServerWebServerUtil::htmlLangString(tank->getTargetName()));
		players += S3D::formatStringBuffer(
			"<tr>"
			"<td>dest=%i ip=%s id=%i</td>" // Id
			"<td>%s</td>" // Name
			"<td>%s</td>" // Type
			"<td>%s</td>" // Time
			"<td>%s</td>" // Score
			"<td>%s</td>" // State
			"<td>%s</td>" // Team
			"<td>%i</td>" // Ping
			"<td><input type=\"checkbox\" name=\"player-%u\"></td>" // Select
			"</tr>\n",
			tank->getDestinationId(), NetInterface::getIpName(tank->getIpAddress()), tank->getPlayerId(),
			cleanName.c_str(),
			tank->getTankAI()?tank->getTankAI()->getName():"Human",
			tank->getScore().getTimePlayedString(),
			tank->getScore().getScoreString(),
			tank->getState().getStateString(),
			TankColorGenerator::getTeamName(tank->getTeam()),
			tank->getScore().getPing(),
			tank->getPlayerId()
		);
	}
	request.getFields()["PLAYERS"] = players;
	request.getFields()["NOPLAYERS"] = S3D::formatStringBuffer("%i/%i", tanks.size(), 
		ScorchedServer::instance()->getOptionsGame().getNoMaxPlayers());

	// Add entries
	std::string add;
	std::list<TankAI *> &ais = ScorchedServer::instance()->getTankAIs().getAis();
	std::list<TankAI *>::iterator aiitor;
	for (aiitor = ais.begin();
		aiitor != ais.end();
		++aiitor)
	{
		TankAI *ai = (*aiitor);
		if (ai->availableForPlayers())
		{
			add += S3D::formatStringBuffer(
				"<input type=\"submit\" name=\"add\" value=\"%s\"\n>",
				ai->getName());
		}
	}
	request.getFields()["ADD"] = add;

	return ServerWebServerUtil::getHtmlTemplate(request.getSession(), "player.html", request.getFields(), text);
}

bool ServerWebHandler::PlayerHandlerThreaded::processRequest(
	ServerWebServerIRequest &request,
	std::string &text)
{
	if (!request.getSession()->credentials.hasPermission(
		ServerAdminSessions::PERMISSION_ALIASPLAYER)) return true;

	// Check for any action
	const char *action = ServerWebServerUtil::getField(request.getFields(), "action");
	const char *uniqueid = ServerWebServerUtil::getField(request.getFields(), "uniqueid");
	if (action && uniqueid)
	{
		if (0 == strcmp(action, "ShowAliases"))
		{
			std::list<std::string> aliases;
			EventHandlerDataBase *database = ScorchedServer::instance()->getEventHandlerDataBase();
			if (database)
			{
				aliases = database->getAliases(uniqueid);
			}
			std::string lines = ServerWebServerUtil::concatLines(aliases);
			return ServerWebServerUtil::getHtmlMessage(
				request.getSession(), 
				"ShowAliases", lines.c_str(), request.getFields(), text);
		}
		else if (0 == strcmp(action, "ShowIPAliases"))
		{
			std::list<std::string> aliases;
			EventHandlerDataBase *database = ScorchedServer::instance()->getEventHandlerDataBase();
			if (database)
			{
				aliases = database->getIpAliases(uniqueid);
			}
			std::string lines = ServerWebServerUtil::concatLines(aliases);
			return ServerWebServerUtil::getHtmlMessage(
				request.getSession(), 
				"ShowIPAliases", lines.c_str(), request.getFields(), text);
		}
	}

	return ServerWebServerUtil::getHtmlTemplate(request.getSession(), "player.html", request.getFields(), text);
}

bool ServerWebHandler::LogHandler::processRequest(
	ServerWebServerIRequest &request,
	std::string &text)
{
	if (!request.getSession()->credentials.hasPermission(
		ServerAdminSessions::PERMISSION_VIEWLOGS)) return true;

	std::deque<ServerLog::ServerLogEntry> &entries = 
		ServerLog::instance()->getEntries();

	const int pagesize = 20;
	int start = (int(entries.size()) / pagesize) * pagesize;
	const char *page = ServerWebServerUtil::getField(request.getFields(), "page");
	if (page) start = atoi(page);
	else start = entries.size() - pagesize;

	// Log entries
	std::string log;

	int min = MAX(start, 0);
	int max = MIN((int) entries.size(), start + pagesize);
	for (int i=min; i<max; i++)
	{
		std::string cleanText;
		XMLNode::removeSpecialChars(entries[i].text, cleanText);
		log += S3D::formatStringBuffer(
			"<tr>"
			"<td><font size=-1>%u</font></td>"
			"<td><font size=-1>%s</font></td>"
			"</tr>\n",
			i,
			cleanText.c_str());
	}
	request.getFields()["LOG"] = log;

	// Pages
	std::string pages;
	for (int i =0; i<=int(entries.size())/pagesize-1; i++)	
	{
		pages +=
			S3D::formatStringBuffer("<a href='?sid=%s&page=%i'>%i - %i</a>&nbsp;",
				request.getFields()["sid"].c_str(), 
				i * pagesize, i * pagesize, i * pagesize + pagesize - 1);
	}
	pages +=
		S3D::formatStringBuffer("<a href='?sid=%s'>Last</a>",
			request.getFields()["sid"].c_str());
	request.getFields()["PAGES"] = pages;

        const char *refreshRate = ServerWebServerUtil::getField(request.getFields(), "RefreshRate");
        int refreshSeconds = 0;
        if (refreshRate) refreshSeconds = atoi(refreshRate);
        else request.getFields()["RefreshRate"]="0";

	if (refreshSeconds > 0)
        request.getFields()["Meta"] = S3D::formatStringBuffer("<meta  HTTP-EQUIV=\"Refresh\" CONTENT=\"%d;URL=%s?sid=%s&RefreshRate=%s\">", refreshSeconds, 
			request.getUrl(),request.getFields()["sid"].c_str(),refreshRate);

	return ServerWebServerUtil::getHtmlTemplate(request.getSession(), "log.html", request.getFields(), text);
}

bool ServerWebHandler::LogFileHandler::processRequest(
	ServerWebServerIRequest &request,
	std::string &text)
{
	if (!request.getSession()->credentials.hasPermission(
		ServerAdminSessions::PERMISSION_VIEWLOGS)) return true;

	std::deque<ServerLog::ServerLogEntry> &entries = 
		ServerLog::instance()->getEntries();

	const char *logFilename = ServerWebServerUtil::getField(request.getFields(), "filename");
	if (logFilename &&
		// Disallow directory backtracking
		!strstr(logFilename, "..") &&
		!strstr(logFilename, ":") &&
		!strstr(logFilename, "/") &&
		!strstr(logFilename, "\\"))
	{
		// We've requested to view a log file
		// So load the file and display it
		text.append(S3D::formatStringBuffer(
			"HTTP/1.1 200 OK\r\n"
			"Server: Scorched3D\r\n"
			"Content-Type: text/plain\r\n"
			"Content-Disposition: filename=%s.txt\r\n"
			"Connection: Close\r\n"
			"\r\n", logFilename));

		std::string filename = S3D::getLogFile(logFilename);
		std::string file = ServerWebServerUtil::getFile(filename);
		request.getFields()["FILE"] = file;

		return ServerWebServerUtil::getTemplate(request.getSession(), "logfile.html", request.getFields(), text);
	}
	else
	{
		// We've requested to see the list of current log files

		// Check to see if we want to search these files
		const char *search = 0;
		const char *action = ServerWebServerUtil::getField(request.getFields(), "action");
		if (action && 0 == strcmp(action, "Search"))
		{
			search = ServerWebServerUtil::getField(request.getFields(), "search");
		}

		// Iterator through all of the log files in the logs directory
		std::vector<LogFile> logFiles;
		std::string portNumber = S3D::formatStringBuffer("%i",
			ScorchedServer::instance()->getOptionsGame().getPortNo());
		std::string dirName = S3D::getLogFile("");
		FileList dir(dirName, "*.log", false);
		if (dir.getStatus())
		{
			std::list<std::string> &files = dir.getFiles();
			std::list<std::string>::iterator itor;
			for (itor = files.begin();
				itor!= files.end();
				++itor)
			{
				const std::string &fileName = (*itor).c_str();
				const std::string &fullFilename = S3D::getLogFile(fileName);

				// Only show files from this server (this port)
				if (0 == strstr(fileName.c_str(), portNumber.c_str())) continue;

				// If searching is enabled check to see if this file contains 
				// the specified string
				if (search)
				{
					std::string file = ServerWebServerUtil::getFile(fullFilename);
					if (0 == strstr(file.c_str(), search)) continue;
				}

				// Add this file to the list of files to view
				LogFile logFile;
				logFile.fileName = fileName;
				logFile.fileTime = S3D::fileModTime(fullFilename);
				logFiles.push_back(logFile);
			}
		}

		// Sort the list of files to view
		std::sort(logFiles.begin(), logFiles.end(), lt_logfile);

		// Create the table that will show the list of log files
		std::string log;
		{
			std::vector<LogFile>::iterator itor;
			for (itor = logFiles.begin();
				itor != logFiles.end();
				++itor)
			{
				LogFile &logFile = *itor;
				const char *fileName = logFile.fileName.c_str();
				log += S3D::formatStringBuffer(
					"<tr>"
					"<td><font size=-1><a href=?filename=%s&sid=%s>%s</a></font></td>"
					"<td><font size=-1>%s</font></td>"
					"</tr>\n",
					fileName,
					request.getFields()["sid"].c_str(),
					fileName,
					ctime(&logFile.fileTime));
			}
		}
		request.getFields()["LOG"] = log;

		return ServerWebServerUtil::getHtmlTemplate(request.getSession(), "logfiles.html", request.getFields(), text);
	}
	return false;
}

bool ServerWebHandler::GameHandler::processRequest(
	ServerWebServerIRequest &request,
	std::string &text)
{
	// Check for any action
	const char *action = ServerWebServerUtil::getField(request.getFields(), "action");
	if (action)
	{
		if (0 == strcmp(action, "NewGame"))
		{
			ServerAdminCommon::newGame(request.getSession()->credentials);
		}
		else if (0 == strcmp(action, "KillAll"))
		{
			ServerAdminCommon::killAll(request.getSession()->credentials);
		}
	}

	std::map<unsigned int, Tank *> &tanks = 
		ScorchedServer::instance()->getTargetContainer().getTanks();
	request.getFields()["PLAYERS"] = S3D::formatStringBuffer("%i/%i", tanks.size(), 
		ScorchedServer::instance()->getOptionsGame().getNoMaxPlayers());

	request.getFields()["STATE"] = (ScorchedServer::instance()->getServerState().getState() > 
		ServerState::ServerMatchCountDownState)?"Playing":"Waiting";

	request.getFields()["ROUND"] = S3D::formatStringBuffer("%i/%i",
		ScorchedServer::instance()->getOptionsTransient().getCurrentRoundNo(),
		ScorchedServer::instance()->getOptionsGame().getNoRounds());
	
	request.getFields()["BI"] = S3D::formatStringBuffer("%uK", NetInterface::getBytesIn() / 1000);
	request.getFields()["BO"] = S3D::formatStringBuffer("%uK", NetInterface::getBytesOut() / 1000);
	request.getFields()["P"] = S3D::formatStringBuffer("%u", NetInterface::getPings());
	request.getFields()["C"] = S3D::formatStringBuffer("%u", NetInterface::getConnects());

	return ServerWebServerUtil::getHtmlTemplate(request.getSession(), "game.html", request.getFields(), text);
}

bool ServerWebHandler::ServerHandler::processRequest(
	ServerWebServerIRequest &request,
	std::string &text)
{
	// Check for any action
	const char *action = ServerWebServerUtil::getField(request.getFields(), "action");
	if (action)
	{
		if (0 == strcmp(action, "Stop Server"))
		{
			ServerAdminCommon::stopServer(request.getSession()->credentials);
		}
		else if (0 == strcmp(action, "Stop Server When Empty"))
		{
			ServerAdminCommon::stopServerWhenEmpty(request.getSession()->credentials);
		}
		else if (0 == strcmp(action, "Set Logging"))
		{
			ServerAdminCommon::setLogging(request.getSession()->credentials,
				(0 == strcmp(request.getFields()["MessageLogging"].c_str(), "on")));
		}
	}

	bool &messageLogging = ScorchedServer::instance()->
		getComsMessageHandler().getMessageLogging();
	{
		request.getFields()["MESSAGELOGGING"] = S3D::formatStringBuffer(
			"<input type='radio' name='MessageLogging' %s value='on'>On</input>"
			"<input type='radio' name='MessageLogging' %s value='off'>Off</input>",
			(messageLogging?"checked":""),
			(!messageLogging?"checked":""));
	}

	request.getFields()["STATE"] = (ScorchedServer::instance()->getServerState().getState() > 
		ServerState::ServerMatchCountDownState)?"Playing":"Waiting";
	request.getFields()["VERSION"] = S3D::formatStringBuffer("%s (%s) - Built %s", 
		S3D::ScorchedVersion.c_str(), 
		S3D::ScorchedProtocolVersion.c_str(), 
		S3D::ScorchedBuildTime.c_str());
	request.getFields()["STARTTIME"] = S3D::getStartTime();
	request.getFields()["EXITEMPTY"] = (ServerCommon::getExitEmpty()?"True":"False");

	return ServerWebServerUtil::getHtmlTemplate(request.getSession(), "server.html", request.getFields(), text);
}

bool ServerWebHandler::BannedHandler::processRequest(
	ServerWebServerIRequest &request,
	std::string &text)
{
	if (!request.getSession()->credentials.hasPermission(
		ServerAdminSessions::PERMISSION_BANPLAYER)) return true;

	const char *action = ServerWebServerUtil::getField(request.getFields(), "action");
	if (action && 0 == strcmp(action, "Load")) 
		ScorchedServer::instance()->getBannedPlayers().load(true);

	const char *selected = ServerWebServerUtil::getField(request.getFields(), "selected");
	std::string banned;
	std::list<ServerBanned::BannedRange> &bannedIps = 
		ScorchedServer::instance()->getBannedPlayers().getBannedIps();
	std::list<ServerBanned::BannedRange>::iterator itor;
	for (itor = bannedIps.begin();
		itor != bannedIps.end();
		++itor)
	{
		ServerBanned::BannedRange &range = (*itor);
		std::string mask = NetInterface::getIpName(range.mask);

		std::map<unsigned int, ServerBanned::BannedEntry>::iterator ipitor;
		for (ipitor = range.ips.begin();
			ipitor != range.ips.end();
			++ipitor)
		{
			unsigned int ip = (*ipitor).first;
			ServerBanned::BannedEntry &entry = (*ipitor).second;
			std::string ipName = NetInterface::getIpName(ip);

			if (selected && 0 == strcmp(selected, ipName.c_str()))
			{
				entry.type = ServerBanned::NotBanned;
			}

			std::string cleanName = ServerWebServerUtil::htmlLangString(entry.name);
			banned += S3D::formatStringBuffer("<tr><td>%s</td><td>%s</td><td>%s</td>"
				"<td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td>"
				"<td><input type=\"checkbox\" name=\"selected\" value=\"%s\"></td>" // Select
				"</tr>",
				(entry.bantime?ctime(&entry.bantime):""),
				cleanName.c_str(),
				entry.uniqueid.c_str(),
				entry.SUI.c_str(),
				ServerBanned::getBannedTypeStr(entry.type),
				ipName.c_str(), mask.c_str(),
				entry.adminname.c_str(), entry.reason.c_str(),
				ipName.c_str());
		}
	}
	request.getFields()["BANNED"] = banned;

	if (action && 0 == strcmp(action, "Save")) 
		ScorchedServer::instance()->getBannedPlayers().save();

	return ServerWebServerUtil::getHtmlTemplate(request.getSession(), "banned.html", request.getFields(), text);
}

bool ServerWebHandler::ModsHandler::processRequest(
	ServerWebServerIRequest &request,
	std::string &text)
{
	std::string modfiles;
	std::map<std::string, ModFileEntry *> &modFiles = 
		ScorchedServer::instance()->getModFiles().getFiles();
	std::map<std::string, ModFileEntry *>::iterator itor;
	for (itor = modFiles.begin();
		itor != modFiles.end();
		++itor)
	{
		ModFileEntry *entry = (*itor).second;
		std::string cleanFileName;
		std::string dirtyFileName(entry->getFileName());
		XMLNode::removeSpecialChars(dirtyFileName, cleanFileName);
		modfiles += S3D::formatStringBuffer("<tr><td>%s</td><td>%u</td><td>%u</td></tr>",
			cleanFileName.c_str(),
			entry->getUncompressedSize(),
			entry->getUncompressedCrc());
	}
	request.getFields()["MODFILES"] = modfiles;

	return ServerWebServerUtil::getHtmlTemplate(request.getSession(), "mods.html", request.getFields(), text);
}

static void addUser(std::string &admins, ServerAdminSessions::Credential &crendential)
{
	std::string permissions;
	std::set<std::string>::iterator permitor;
	for (permitor = crendential.permissions.begin();
		permitor != crendential.permissions.end();
		)
	{
		permissions.append(*permitor);

		++permitor;
		if (permitor != crendential.permissions.end()) permissions.append(", ");
	}

	admins += S3D::formatStringBuffer(
		"<tr>"
		"<td>%s</td>" // Name
		"<td>%s</td>" // Permisions
		"</tr>\n",
		crendential.username.c_str(),
		permissions.c_str()
	);
}

bool ServerWebHandler::SessionsHandler::processRequest(
	ServerWebServerIRequest &request,
	std::string &text)
{
	// Sessions Entries
	{
		std::string sessions;
		std::map<unsigned int, ServerAdminSessions::SessionParams> &sessionparams = 
			ScorchedServer::instance()->getServerAdminSessions().getAllSessions();
		std::map<unsigned int, ServerAdminSessions::SessionParams>::iterator itor;
		for (itor = sessionparams.begin(); itor != sessionparams.end(); ++itor)
		{
			ServerAdminSessions::SessionParams &params = (*itor).second;

			time_t theTime = (time_t) params.sessionTime;
			const char *timeStr = ctime(&theTime);
			sessions += S3D::formatStringBuffer(
				"<tr>"
				"<td>%s</td>" // Name
				"<td>%s</td>" // Time
				"<td>%s</td>" // Ip
				"</tr>\n",
				params.credentials.username.c_str(),
				timeStr,
				params.ipAddress.c_str()
			);
		}
		request.getFields()["SESSIONS"] = sessions;
	}

	// List of admins
	{
		std::string admins;
		addUser(admins, ScorchedServer::instance()->getServerAdminSessions().getLocalUserCredentials());
		std::list<ServerAdminSessions::Credential> creds;
		std::list<ServerAdminSessions::Credential>::iterator itor;
		ScorchedServer::instance()->getServerAdminSessions().getAllCredentials(creds);
		for (itor = creds.begin();
			itor != creds.end();
			++itor)
		{
			ServerAdminSessions::Credential &crendential = (*itor);
			addUser(admins, crendential);
		}
		request.getFields()["ADMINS"] = admins;
	}

	return ServerWebServerUtil::getHtmlTemplate(request.getSession(), "sessions.html", request.getFields(), text);
}

bool ServerWebHandler::AccountHandler::processRequest(
	ServerWebServerIRequest &request,
	std::string &text)
{
	request.getFields()["USERNAME"] = request.getSession()->credentials.username;

	const char *setpassword = ServerWebServerUtil::getField(request.getFields(), "setpassword");
	const char *oldpassword = ServerWebServerUtil::getField(request.getFields(), "oldpassword");
	const char *newpassword = ServerWebServerUtil::getField(request.getFields(), "newpassword");
	if (setpassword && oldpassword && newpassword)
	{
		if (request.getSession()->credentials.username == 
			ScorchedServer::instance()->getServerAdminSessions().getLocalUserCredentials().username)
		{
			request.getFields()["MESSAGE"] = "<b>Cannot set local connection password</b><br/>";
		}
		else
		{
			if (ScorchedServer::instance()->getServerAdminSessions().setPassword(
				request.getSession()->credentials.username.c_str(),
				oldpassword,
				newpassword))
			{
				request.getFields()["MESSAGE"] = "<b>New Password Set</b><br/>";
			}
			else
			{
				request.getFields()["MESSAGE"] = "<b>Wrong old password!</b><br/>";
			}
		}
	}

	return ServerWebServerUtil::getHtmlTemplate(request.getSession(), "account.html", request.getFields(), text);
}

bool ServerWebHandler::StatsHandler::processRequest(
	ServerWebServerIRequest &request,
	std::string &text)
{
	std::string message;
	EventHandlerDataBase *database = ScorchedServer::instance()->getEventHandlerDataBase();

	const char *find;
	const char *action = ServerWebServerUtil::getField(request.getFields(), "action");
	if (action && 
		(0 == strcmp(action, "Find")) &&
		(find = ServerWebServerUtil::getField(request.getFields(), "find")))
	{
		message.append("<b>Players</b>\n");
		if (database)
		{
			message.append(database->getPlayerInfo(find));
		}
	}
	else if (action && (0 == strcmp(action, "Combine")))
	{
		const char *player1 = ServerWebServerUtil::getField(request.getFields(), "player1");
		const char *player2 = ServerWebServerUtil::getField(request.getFields(), "player2");
		if (player1 && player2)
		{
			int p1 = atoi(player1);
			int p2 = atoi(player2);
			if (p1 && p2)
			{
				if (database)
				{
					database->combinePlayers(p1, p2);
					message.append("<b>Combined</b>\n");
				}
				else 
				{
					message.append("<b>No DataBase</b>\n");
				}
			}
		}
	}
	else
	{
		message.append("<b>Ranks</b>\n");
		if (database)
		{
			message.append(database->getTopRanks());
		}		
	}

	int pos;
	while ((pos = message.find("\n")) > 0)
	{
		message.replace(pos, 1, "<br>");
	}

	request.getFields()["RANKS"] = message;

	return ServerWebServerUtil::getHtmlTemplate(request.getSession(), "stats.html", request.getFields(), text);
}
