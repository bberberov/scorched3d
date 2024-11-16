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

#include <server/ServerAdminCommon.h>
#include <server/ServerCommon.h>
#include <server/ScorchedServer.h>
#include <server/ServerChannelManager.h>
#include <server/ServerSimulator.h>
#include <server/ServerBanned.h>
#include <server/ServerAuthHandler.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <tank/TankState.h>
#include <tank/TankScore.h>
#include <tankai/TankAIAdder.h>
#include <target/TargetLife.h>
#include <common/OptionsScorched.h>
#include <common/OptionsTransient.h>
#include <common/FileLogger.h>
#include <simactions/AdminSimAction.h>

static FileLogger *serverAdminFileLogger = 0;

void ServerAdminCommon::adminLog(const ChannelText &message)
{
	if (!serverAdminFileLogger) 
	{
		char buffer[256];
		snprintf(buffer, 256, "ServerAdminLog-%i-", 
			ScorchedServer::instance()->getOptionsGame().getPortNo());
		serverAdminFileLogger = new FileLogger(buffer);
	}	

	ScorchedServer::instance()->getServerChannelManager().sendText(message, true);

	LangString str = ((ChannelText &)message).getMessage();
	LoggerInfo info(LangStringUtil::convertFromLang(str));
	info.setTime();
	serverAdminFileLogger->logMessage(info);
}

static void internalBanPlayer(ServerAdminSessions::Credential &credential,  
	unsigned int playerId,
	ServerBanned::BannedType type, const char *reason)
{
	Tank *tank = ScorchedServer::instance()->
		getTargetContainer().getTankById(playerId);
	if (tank)
	{
		if (tank->getDestinationId() == 0)
		{
			return;
		}
		unsigned int ipAddress = tank->getIpAddress();
		if (ipAddress != 0)
		{	
			ScorchedServer::instance()->getBannedPlayers().
				addBanned(ipAddress, tank->getTargetName(), 
					tank->getUniqueId(), tank->getSUI(), 
					type, credential.username.c_str(), reason);
			if (type == ServerBanned::Banned)
			{
				ServerCommon::kickPlayer(playerId, "Due to player ban");

				ServerAuthHandler *authHandler =
					ScorchedServer::instance()->getAuthHandler();
				if (authHandler)
				{
					authHandler->banUser(tank->getUniqueId());
				}
			}
		}
	}
}

bool ServerAdminCommon::addPlayer(ServerAdminSessions::Credential &credential, const char *playerType)
{
	if (!credential.hasPermission(ServerAdminSessions::PERMISSION_ADDPLAYER)) return false;
	TankAIAdder::addTankAI(*ScorchedServer::instance(), playerType);
	return true;
}

bool ServerAdminCommon::kickPlayer(ServerAdminSessions::Credential &credential, unsigned int playerId)
{
	if (!credential.hasPermission(ServerAdminSessions::PERMISSION_KICKPLAYER)) return false;

	Tank *targetTank = ScorchedServer::instance()->
		getTargetContainer().getTankById(playerId);
	if (!targetTank) return false;

	adminLog(ChannelText("info",
		"ADMIN_KICK",
		"admin \"{0}\" kicked [p:{1}]",
		credential.username,
		targetTank->getTargetName()));
	ServerCommon::kickPlayer(
		targetTank->getPlayerId(),
		"Kicked by admin");

	return true;
}

bool ServerAdminCommon::poorPlayer(ServerAdminSessions::Credential &credential, unsigned int playerId)
{
	Tank *targetTank = ScorchedServer::instance()->
		getTargetContainer().getTankById(playerId);
	if (!targetTank) return false;

	adminLog(ChannelText("info",
		"ADMIN_POOR",
		"admin \"{0}\" removed all of [p:{1}]'s money",
		credential.username,
		targetTank->getTargetName()));
	targetTank->getScore().setMoney(0);

	return true;
}

bool ServerAdminCommon::banPlayer(ServerAdminSessions::Credential &credential, unsigned int playerId, const char *reason)
{
	if (!credential.hasPermission(ServerAdminSessions::PERMISSION_BANPLAYER)) return false;

	Tank *targetTank = ScorchedServer::instance()->
		getTargetContainer().getTankById(playerId);
	if (!targetTank) return false;

	adminLog(ChannelText("info",
		"ADMIN_BAN",
		"admin \"{0}\" banned [p:{1}]",
		credential.username,
		targetTank->getTargetName()));
	internalBanPlayer(
		credential,
		targetTank->getPlayerId(), 
		ServerBanned::Banned, 
		reason);

	return true;
}

bool ServerAdminCommon::slapPlayer(ServerAdminSessions::Credential &credential, unsigned int playerId, float slap)
{
	Tank *targetTank = ScorchedServer::instance()->
		getTargetContainer().getTankById(playerId);
	if (!targetTank) return false;

	adminLog(ChannelText("info",
		"ADMIN_SLAP",
		"admin \"{0}\" slapped [p:{1}] for {2} life",
		credential.username,
		targetTank->getTargetName(),
		slap));

	AdminSimAction *action = new AdminSimAction(AdminSimAction::eSlap, playerId, fixed(int(slap)));
	ScorchedServer::instance()->getServerSimulator().addSimulatorAction(action);

	return true;
}

bool ServerAdminCommon::changeNamePlayer(ServerAdminSessions::Credential &credential, unsigned int playerId, const LangString &newName)
{
	Tank *targetTank = ScorchedServer::instance()->
		getTargetContainer().getTankById(playerId);
	if (!targetTank) return false;

	adminLog(ChannelText("info",
		"ADMIN_CHANGE_NAME",
		"admin \"{0}\" changed the name of [p:{1}] to {2}",
		credential.username,
		targetTank->getTargetName(),
		newName));

	AdminSimAction *action = new AdminSimAction(playerId, newName);
	ScorchedServer::instance()->getServerSimulator().addSimulatorAction(action);

	return true;
}

bool ServerAdminCommon::killPlayer(ServerAdminSessions::Credential &credential, unsigned int playerId)
{
	Tank *targetTank = ScorchedServer::instance()->
		getTargetContainer().getTankById(playerId);
	if (!targetTank) return false;

	adminLog(ChannelText("info",
		"ADMIN_KILL",
		"admin \"{0}\" killed [p:{1}]",
		credential.username,
		targetTank->getTargetName()));

	AdminSimAction *action = new AdminSimAction(AdminSimAction::eKill, playerId, 0);
	ScorchedServer::instance()->getServerSimulator().addSimulatorAction(action);

	return true;
}

bool ServerAdminCommon::flagPlayer(ServerAdminSessions::Credential &credential, unsigned int playerId, const char *reason)
{
	Tank *targetTank = ScorchedServer::instance()->
		getTargetContainer().getTankById(playerId);
	if (!targetTank) return false;

	adminLog(ChannelText("info",
		"ADMIN_FLAG",
		"admin \"{0}\" flagged [p:{1}] for attention",
		credential.username,
		targetTank->getTargetName()));
	internalBanPlayer(
		credential,
		targetTank->getPlayerId(), 
		ServerBanned::Flagged, 
		reason);

	return true;
}

bool ServerAdminCommon::mutePlayer(ServerAdminSessions::Credential &credential, unsigned int playerId, bool mute)
{
	Tank *targetTank = ScorchedServer::instance()->
		getTargetContainer().getTankById(playerId);
	if (!targetTank) return false;

	adminLog(ChannelText("info",
		mute?"ADMIN_MUTE":"ADMIN_UNMUTE",
		mute?"admin \"{0}\" muted [p:{1}]":"admin \"{0}\" unmuted [p:{1}]",
		credential.username,
		targetTank->getTargetName()));
	targetTank->getState().setMuted(mute); 

	return true;
}

bool ServerAdminCommon::permMutePlayer(ServerAdminSessions::Credential &credential, unsigned int playerId, const char *reason)
{
	Tank *targetTank = ScorchedServer::instance()->
		getTargetContainer().getTankById(playerId);
	if (!targetTank) return false;

	adminLog(ChannelText("info",
		"ADMIN_PERMMUTE",
		"admin \"{0}\" permanently muted [p:{1}]",
		credential.username,
		targetTank->getTargetName()));
	internalBanPlayer(
		credential,
		targetTank->getPlayerId(),
		ServerBanned::Muted, 
		reason);
	targetTank->getState().setMuted(true);

	return true;
}

bool ServerAdminCommon::unpermMutePlayer(ServerAdminSessions::Credential &credential, unsigned int playerId)
{
	Tank *targetTank = ScorchedServer::instance()->
		getTargetContainer().getTankById(playerId);
	if (!targetTank) return false;

	adminLog(ChannelText("info",
		"ADMIN_UNPERMMUTE",
		"admin \"{0}\" removed permanently muting of [p:{1}]",
		credential.username,
		targetTank->getTargetName()));
	internalBanPlayer(
		credential,
		targetTank->getPlayerId(),
		ServerBanned::NotBanned, 
		"");
	targetTank->getState().setMuted(false);

	return true;
}

bool ServerAdminCommon::newGame(ServerAdminSessions::Credential &credential)
{
	if (!credential.hasPermission(
		ServerAdminSessions::PERMISSION_ALTERGAME)) return false;

	adminLog(ChannelText("info",
		"ADMIN_NEW_GAME",
		"admin \"{0}\" started a new game",
		credential.username));

	AdminSimAction *action = new AdminSimAction(AdminSimAction::eNewGame, 0, 0);
	ScorchedServer::instance()->getServerSimulator().addSimulatorAction(action);

	return true;
}

bool ServerAdminCommon::killAll(ServerAdminSessions::Credential &credential)
{
	if (!credential.hasPermission(
		ServerAdminSessions::PERMISSION_ALTERGAME)) return false;

	adminLog(ChannelText("info",
		"ADMIN_KILL_ALL",
		"admin \"{0}\" killed all players",
		credential.username));

	AdminSimAction *action = new AdminSimAction(AdminSimAction::eKillAll, 0, 0);
	ScorchedServer::instance()->getServerSimulator().addSimulatorAction(action);

	return true;
}

bool ServerAdminCommon::stopServer(ServerAdminSessions::Credential &credential)
{
	if (!credential.hasPermission(
		ServerAdminSessions::PERMISSION_ALTERSERVER)) return false;

	adminLog(ChannelText("info",
		"ADMIN_STOP_SERVER",
		"admin \"{0}\" stopping the server",
		credential.username));

	exit(0);

	return true;
}

bool ServerAdminCommon::stopServerWhenEmpty(ServerAdminSessions::Credential &credential)
{
	if (!credential.hasPermission(
		ServerAdminSessions::PERMISSION_ALTERSERVER)) return false;

	adminLog(ChannelText("info",
		"ADMIN_STOP_SERVER_WHEN_EMPTY",
		"admin \"{0}\" stopping the server when empty",
		credential.username));

	ServerCommon::getExitEmpty() = true;

	return true;
}

bool ServerAdminCommon::setLogging(ServerAdminSessions::Credential &credential, bool logging)
{
	if (!credential.hasPermission(
		ServerAdminSessions::PERMISSION_ALTERSERVER)) return false;

	adminLog(ChannelText("info",
		"SET_LOGGING",
		"admin \"{0}\" set logging",
		credential.username));

	ScorchedServer::instance()->getComsMessageHandler().getMessageLogging() = logging;

	return true;
}

bool ServerAdminCommon::adminSay(ServerAdminSessions::Credential &credential,
	const char *channel, const char *text)
{
	LangString langString(LANG_STRING(text));

	ChannelText channelText(channel, langString);
	channelText.setAdminPlayer(credential.username.c_str());
	ScorchedServer::instance()->getServerChannelManager().sendText(channelText, true);

	return true;
}
