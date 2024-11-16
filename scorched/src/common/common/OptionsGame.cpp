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

#include <common/OptionsGame.h>
#include <common/Defines.h>
#include <string.h>

static OptionEntryEnum::EnumEntry scoreEnum[] =
{
	{ "ScoreWins", OptionsGame::ScoreWins },
	{ "ScoreKills", OptionsGame::ScoreKills },
	{ "ScoreMoney", OptionsGame::ScoreMoney },
	{ "", -1 }
};

static OptionEntryEnum::EnumEntry turnEnum[] =
{
	{ "TurnSimultaneous", OptionsGame::TurnSimultaneous },
	{ "TurnSimultaneousNoWait", OptionsGame::TurnSimultaneousNoWait },
	{ "TurnSequentialLoserFirst", OptionsGame::TurnSequentialLoserFirst },
	{ "TurnSequentialRandom", OptionsGame::TurnSequentialRandom },
	{ "TurnFree", OptionsGame::TurnFree },
	{ "TurnFreeTimed", OptionsGame::TurnFreeTimed },
	{ "", -1 }
};

static OptionEntryEnum::EnumEntry windForceEnum[] =
{
	{ "WindRandom", OptionsGame::WindRandom },
	{ "WindNone", OptionsGame::WindNone },
	{ "Wind1", OptionsGame::Wind1 },
	{ "Wind2", OptionsGame::Wind2 },
	{ "Wind3", OptionsGame::Wind3 },
	{ "Wind4", OptionsGame::Wind4 },
	{ "Wind5", OptionsGame::Wind5 },
	{ "WindBreezy", OptionsGame::WindBreezy },
	{ "WindGale", OptionsGame::WindGale },
	{ "", -1 }
};

static OptionEntryEnum::EnumEntry windTypeEnum[] =
{
	{ "WindChangeNever", OptionsGame::WindChangeNever },
	{ "WindChangeSomeTimes", OptionsGame::WindChangeSomeTimes },
	{ "WindChangeFrequently", OptionsGame::WindChangeFrequently },
	{ "WindChangeConstantly", OptionsGame::WindChangeConstantly },
	{ "WindChangeAlways", OptionsGame::WindChangeAlways },
	{ "", -1 }
};

static OptionEntryEnum::EnumEntry wallEnum[] =
{
	{ "WallRandom", OptionsGame::WallRandom },
	{ "WallConcrete", OptionsGame::WallConcrete },
	{ "WallBouncy", OptionsGame::WallBouncy },
	{ "WallWrapAround", OptionsGame::WallWrapAround },
	{ "WallNone", OptionsGame::WallNone },
	{ "WallActive", OptionsGame::WallActive },
	{ "WallInactive", OptionsGame::WallInactive },
	{ "", -1 }
};

static OptionEntryEnum::EnumEntry weapScaleEnum[] =
{
	{ "ScaleSmall", OptionsGame::ScaleSmall },
	{ "ScaleMedium", OptionsGame::ScaleMedium },
	{ "ScaleLarge", OptionsGame::ScaleLarge },
	{ "", -1 }
};

static OptionEntryEnum::EnumEntry resignEnum[] =
{
	{ "ResignStart", OptionsGame::ResignStart },
	{ "ResignTimed", OptionsGame::ResignTimed },
	{ "ResignNone", OptionsGame::ResignNone },
	{ "", -1 }
};

static OptionEntryEnum::EnumEntry movementRestrictionEnum[] =
{
	{ "MovementRestrictionNone", OptionsGame::MovementRestrictionNone },
	{ "MovementRestrictionLand", OptionsGame::MovementRestrictionLand },
	{ "MovementRestrictionLandOrAbove", OptionsGame::MovementRestrictionLandOrAbove },
	{ "", -1 }
};

static OptionEntryEnum::EnumEntry teamBallanceEnum[] =
{
	{ "TeamBallanceNone", OptionsGame::TeamBallanceNone },
	{ "TeamBallanceAuto", OptionsGame::TeamBallanceAuto },
	{ "TeamBallanceBotsVs", OptionsGame::TeamBallanceBotsVs },
	{ "TeamBallanceAutoByScore", OptionsGame::TeamBallanceAutoByScore },
	{ "TeamBallanceAutoByBots", OptionsGame::TeamBallanceAutoByBots },
	{ "", -1 }
};

static OptionEntryStringEnum::EnumEntry economyEnum[] =
{
	{ "EconomyFreeMarket" },
	{ "EconomyNone" },
	{ "" }
};

static OptionEntryStringEnum::EnumEntry serverFileLoggerEnum[] =
{
	{ "none" },
	{ "file" },
	{ "" }
};

static OptionEntryStringEnum::EnumEntry statsLoggerEnum[] =
{
	{ "none" },
	{ "file" },
	{ "mysql" },
	{ "" }
};

static OptionEntryStringEnum::EnumEntry authHandlerEnum[] =
{
	{ "none" },
	{ "prefered" },
	{ "forumlogin" },
	{ "minkills" },
	{ "" }
};

OptionsGame::OptionsGame() :
	tutorial_(options_, "Tutorial",
		"The tutorial to load for this game", 0, ""),
	scorePerMoney_(options_, "ScorePerMoney",
		"The amount of score awarded for 100 money units earned", 0, 0, 0, 100, 2),
	scorePerAssist_(options_, "ScorePerAssist",
		"The amount of score awarded for each kill assist", 0, 2, 0, 50, 1),
	scorePerKill_(options_, "ScorePerKill",
		"The amount of score awarded for each kill", 0, 10, 0, 50, 1),
	scorePerResign_(options_, "ScorePerResign",
		"The amount of score awarded for each resign", 0, 0, -50, 50, 1),
	scorePerSpectate_(options_, "ScorePerSpectate",
		"The amount of score awarded for each move to spectator (only when tank is still alive)", 0, 0, -10, 50, 1),
	scoreWonForRound_(options_, "ScoreWonForRound",
		"The amount of score awarded for round won", 0, 250, 0, 500, 10),
	scoreWonForLives_(options_, "ScoreWonForLives",
		"The amount of score awarded for lives left", 0, 0, 0, 500, 10),
	skillForRound_(options_, "SkillForRound",
		"The amount of skill awarded for winning the round", 0, 0, 0, 100, 5),
	skillForMatch_(options_, "SkillForMatch",
		"The amount of skill awarded for winning the match", 0, 0, 0, 100, 5),
	skillForResign_(options_, "SkillForResign",
		"The amount of skill awarded for resigning", 0, 0, -50, 50, 5),
	maxSkillLost_(options_, "MaxSkillLost",
		"The maximum amount of skill you can lose for being killed", 0, 10, 0, 100, 5),
	maxSkillGained_(options_, "MaxSkillGained",
		"The maximum amount of skill you can gain for killing someone", 0, 10, 0, 100, 5),
	skillForSelfKill_(options_, "SkillForSelfKill",
		"The amount of skill you lose for being killed by yourself", 0, -25, -50, 0, 5),
	skillForTeamKill_(options_, "SkillForTeamKill",
		"The amount of skill you lose for killing a team memeber", 0, -25, -50, 0, 5),
	teams_(options_, "Teams",
		"The number of teams (1 is no teams)", 0, 1, 1, 4, 1),
	minimumLandHeight_(options_, "MinimumLandHeight",
		"The minimum height the ground can reach", 0, 0),
	startArmsLevel_(options_, "StartArmsLevel",
		"The largest weapon type allowed (on round 1)", 0, 10, 0, 10, 1),
	endArmsLevel_(options_, "EndArmsLevel",
		"The largest weapon type allowed (on the last round)", 0, 10, 0, 10, 1),
	maxNumberWeapons_(options_, "MaxNumberWeapons",
		"The number of each weapon the player is allowed", 0, 90, 0, 500, 10),
	weaponSpeed_(options_, "WeaponSpeed",
		"The speed of projectiles and rollers", 0, 15000, 2500, 40000, 2500),
	maxLandscapeSize_(options_, "MaxLandscapeSize",
		"The largest landscape (in bytes) that will be sent to the clients", 0, 200000),
	startTime_(options_, "StartTime", 
		"The amount of time before a new game starts", 0, 0, 0, 90, 5),
	shotTime_(options_, "ShotTime", 
		"The amount of time each player has for each shot", 0, 0, 0, 90, 5),
	aiShotTime_(options_, "AIShotTime", 
		"The amount of time each computer controller player has for each shot", 0, 10, 0, 90, 2),
	buyingTime_(options_, "BuyingTime", 
		"The amount of time each player has to buy weapons and use auto defense", 0, 0, 0, 90, 5),
	removeTime_(options_, "RemoveTime", 
		"The amount of time each player has before being removed when disconected", 0, 10, 0, 25, 1),
	allowedMissedMoves_(options_, "AllowedMissedMoves",
		"The number of moves a player is allowed to miss (due to the shot timer)", 0, 0, 0, 10, 1),
	roundScoreTime_(options_, "RoundScoreTime", 
		"The amount of time to show the end of each round for", 0, 5, 0, 30, 1),
	idleCycleTime_(options_, "IdleCycleTime", 
		"The time before a new map is loaded when no one is playing", 0, 900, 45, 1800, 45),
	scoreTime_(options_, "ScoreTime", 
		"The amount of time to show the end of match scores for", 0, 15, 0, 90, 5),
	roundTime_(options_, "RoundTime", 
		"The amount of time for each round", 0, 0, 0, 900, 30),
	depricatedKeepAliveTime_(options_, "KeepAliveTime",
		"The amount of time between each client keep alive message", OptionEntry::DataDepricated, 2, 0, 30, 1),
	depricatedKeepAliveTimeoutTime_(options_, "KeepAliveTimeoutTIme",
		"The amount of time the server will allow without receiving a keep alive message", OptionEntry::DataDepricated, 0, 0, 90, 5),
	depricatedIdleKickTime_(options_, "IdleKickTime",
		"The amount of time to give clients to respond after level loading before kicking them", OptionEntry::DataDepricated, 60, 0, 90, 5),
	depricatedIdleShotKickTime_(options_, "IdleShotKickTime",
		"The amount of time to give clients to respond after shots before kicking them", OptionEntry::DataDepricated, 45, 0, 90, 5),
	minFallingDistance_(options_, "MinFallingDistance",
		"The minimum distance that can be made with no damage (value is divided by 10)", 0, 5, 0, 100, 5),
	tankFallingDamage_(options_, "TankFallingDamage", 
		"If tanks tank damage from falling", 0, true),
	maxClimbingDistance_(options_, "MaxClimbingDistance",
		"The maximum distance that a tank can climb per movement square (divided by 10)", 0, 8, 0, 20, 1),
	playerLives_(options_, "PlayerLives",
		"The number of lives that each player tank has", 0, 1, 0, 10, 1),
	gravity_(options_, "Gravity",
		"The gravity used by the physics engine", 0, -10, -25, 0, 1),
	maxRoundTurns_(options_, "MaxNumberOfRoundTurns",
		"The maximum number of turns all players are allowed in each round (0 is infinite)", 0, 15, 0, 50, 1),
	numberOfRounds_(options_, "NumberOfRounds", 
		"The number of rounds to play in each game", 0, 5, 1, 50, 1),
	numberOfPlayers_(options_, "NumberOfPlayers", 
		"The maximum number of players to allow", 0, 2, 2, 24, 1),
	numberOfMinPlayers_(options_, "NumberOfMinPlayers", 
		"The minimum number of players to allow", 0, 2, 2, 24, 1),
	removeBotsAtPlayers_(options_, "RemoveBotsAtPlayers",
		"The number of players to allow before remvoing bots", 0, 0, 0, 24, 1),
	limitPowerByHealth_(options_, "LimitPowerByHealth", 
		"Show power is limited by the amount of health a player has", 0, true),
	residualPlayers_(options_, "ResidualPlayers",
		"Players that leave will have the same state when reconnecting", 0, true),
	delayedDefenseActivation_(options_, "DelayedDefenseActivation",
		"Changes to shields are only seen after the aiming phase.", 0, false),
	autoSendSyncCheck_(options_, "AutoSendSyncCheck",
		"Automatically send a sync check at the end of each shot", 0, false),
	actionSyncCheck_(options_, "ActionSyncCheck",
		"Gather enhanced action syncchecking", 0, false),
	actionMovementSyncCheck_(options_, "ActionMovementSyncCheck",
		"Gather enhanced action syncchecking for target movements", 0, false),
	actionRandomSyncCheck_(options_, "ActionRandomSyncCheck",
		"Gather enhanced action syncchecking for random number generation", 0, false),
	actionCollisionSyncCheck_(options_, "ActionCollisionSyncCheck",
		"Gather enhanced action syncchecking for particle collisions", 0, false),
	targetPlacementSyncCheck_(options_, "TargetPlacementSyncCheck",
		"Gather enhanced action syncchecking for initial target placements", 0, false),
	weaponSyncCheck_(options_, "WeaponSyncCheck",
		"Gather enhanced action syncchecking for weapon invocations", 0, false),
	accessoryNameSyncCheck_(options_, "AccessoryNameSyncCheck",
		"Also send the accessory names when sending the tank sync check", 0, false),
	resignMode_(options_, "ResignMode",
		"When does a players resign take place", 0, int(ResignTimed), resignEnum),
	movementRestriction_(options_, "MovementRestriction",
		"Where a tank is allowed to move to", 0, int(MovementRestrictionNone), movementRestrictionEnum),
	teamBallance_(options_, "TeamBallance",
		"The mode of team auto-ballancing performed for team games", 0, int(TeamBallanceNone), teamBallanceEnum),
	turnType_(options_, "TurnType", 
		"The player turn mode", 0, 	int(TurnSequentialLoserFirst), turnEnum), // Data, default, min, max
	moneyBuyOnRound_(options_, "MoneyBuyOnRound", 
		"The first round players are allowed to buy on", 0 ,2, 1, 50, 1),
	moneyWonForRound_(options_, "MoneyWonForRound", 
		"The money awarded for each won round", 0, 5000, 0, 100000, 1000),
	moneyWonForLives_(options_, "MoneyWonForLives",
		"The money awarded for each life remaining", 0, 5000, 0, 100000, 1000),
	moneyPerKillPoint_(options_, "MoneyWonPerKillPoint", 
		"The money awarded for each person killed * weapon used", 0, 750, 0, 2000, 50),
	moneyPerMultiKillPoint_(options_, "MoneyWonPerMultiKillPoint", 
		"The extra money awarded for each person multi-killed * weapon used * multi-kill", 0, 0, 0, 2000, 50),
	moneyPerAssistPoint_(options_, "MoneyWonPerAssistPoint", 
		"The money awarded for each kill assist * weapon used", 0, 250, 0, 2000, 50),
	moneyPerHitPoint_(options_, "MoneyWonPerHitPoint", 
		"The money awarded for each person hit * weapon used", 0, 250, 0, 2000, 50),
	moneyPerRound_(options_, "MoneyPerRound", 
		"The money given at the end of each round", 0, 0, 0, 250000, 5000),
	moneyPerHealthPoint_(options_, "MoneyPerHealthPoint",
		"The money awarded is proportional to the amount of health removed", 0, true),
	moneyStarting_(options_, "MoneyStarting", 
		"The money each player starts with", 0, 10000, 0, 500000, 10000),
	moneyInterest_(options_, "MoneyInterest", 
		"The interest awarded at the end of each round", 0, 15, 0, 100, 5),
	freeMarketAdjustment_(options_, "FreeMarketAdjustment",
		"The scale of the adjustment changes made by the free market", 0, 100),
	freeMarketLimits_(options_, "FreeMarketLimits",
		"The scale of the max/min prices (*1.5) allowed by the free market e.g. 2 is a 300% increase/reduction", 0, 2),
	freeMarketTracksBotPurchases_(options_, "FreeMarketTrackBotPurchases",
		"Allows the purchases bots (ais) make to be tracked by the free market", 0, false),
	windForce_(options_, "WindForce", 
		"The force of the wind", 0, int(WindRandom), windForceEnum),
	windType_(options_, "WindType", 
		"When the wind changes", 0, int(WindChangeNever), windTypeEnum),
	wallType_(options_, "WallType", 
		"The type of walls allowed", 0, int(WallRandom), wallEnum),
	weapScale_(options_, "WeaponScale", 
		"The scale of the weapons used", 0, int(ScaleMedium), weapScaleEnum),
	statsLogger_(options_, "StatsLogger",
		"The type of player stats to be logged", 0, "none", statsLoggerEnum),
	serverFileLogger_(options_, "ServerLogger",
	"The type of server events to be logged to file", 0, "none", serverFileLoggerEnum),
	portNo_(options_, "PortNo", 
		"The port to start the server on", 0, S3D::ScorchedPort),
	managementPortNo_(options_, "ManagementPortNo", 
		"The port to start the managament web server on (0 is management off)", 0, 0),
	serverName_(options_, "ServerName", 
		"The name of the server to start", 0, "No Name"),
	mod_(options_, "Mod", 
		"The name of currently running Scorched3D mod", 0, "none"),
	motd_(options_, "MOTD",
		"The message of the day", 0, 
		"Scorched3D : Copyright 2011 Gavin Camp\n"
		"For updates and news visit:\n"
		"http://www.scorched3d.co.uk",
		true),
	modDownloadSpeed_(options_, "ModDownloadSpeed",
		"Max download speed of mods in bytes per second (0 is no mod download)", 0, 0, 0, 500000, 5000),
	maxAvatarSize_(options_, "MaxAvatarSize",
		"Max size allowed for avatars in bytes (0 is no avatars)", 0, 5000),
	economy_(options_, "Economy", 
		"Speicifies the name of the economy to use", 0, "EconomyFreeMarket", economyEnum),
	landscapes_(options_, "Landscapes", 
		"Colon seperated list of landscape names", 0, ""),
	serverPassword_(options_, "ServerPassword", 
		"The password for this server (empty password is no password)", OptionEntry::DataProtected, ""),
	allowSameIP_(options_, "AllowSameIP",
		"Allow scorched clients from same machine/NAT router to connect.", 0, true),
	allowSameUniqueId_(options_, "AllowSameUniqueId",
		"Allow scorched clients with same unique id to connect.", 0, true),
	publishServer_(options_, "PublishServer",
		"Allow other scorched net clients to see this server.  Do not use for LAN games.", 0, false),
	useUPnP_(options_, "UseUPnP",
		"Try to automaticaly add external port forwarding rules using UPnP", 0, false),
	useUPnPLogging_(options_, "UseUPnPLogging",
		"Turn on packet logging for UPnP", 0, false),
	publishAddress_(options_, "PublishAddress",
		"IP address to publish to scorched net clients (auto-detected if not given).", 0, "AutoDetect"),
	botNamePrefix_(options_, "BotNamePrefix", 
		"Prepend and bot name with the specified text", 0, "(Bot) "),
	giveAllWeapons_(options_, "GiveAllWeapons",
		"Start the game will all the weapons", 0, false),
	registeredUserNames_(options_, "RegisteredUserNames",
		"Only allow authenticated players to use their own player names", 0, false),
	allowMultiLingualChat_(options_, "AllowMultiLingualChat",
		"Allow internaltional (non-latin) characters in chat", 0, true),
	allowMultiLingualNames_(options_, "AllowMultiLingualNames",
		"Allow internaltional (non-latin) characters in names", 0, true),
	authHandler_(options_, "AuthHandler",
		"Only allow authenticated players to connect", 0, "none", authHandlerEnum),
	authHandlerParam1_(options_, "AuthHandlerParam1",
		"First parameter for the authentication handler", 0, ""),
	authHandlerParam2_(options_, "AuthHandlerParam2",
		"Second parameter for the authentication handler", 0, ""),
	cycleMaps_(options_, "CycleMaps",
		"Cycle through the maps instead of choosing them using a random probablity", 0, false),
	randomizeBotNames_(options_, "RandomizeBotNames",
		"Choose random bot names instread of sequential names", 0, false),
	computersDeathTalk_(options_, "ComputersDeathTalk",
		"The percentage chance the computers will say something when killed", 0, 100, 0, 100, 10),
	computersAttackTalk_(options_, "ComputersAttackTalk",
		"The percentage chance the computers will say something when shooting", 0, 25, 0, 100, 10),
	debugFeatures_(options_, "DebugFeatures",
		"Set to \"true\" to enable debugging features such as InfoGrid", 0, false),
	waitForShotsBeforeShowingScore_(options_, "WaitForShotsBeforeShowingScore",
		"Wait for all shots to be played out before showing the score dialog", 0, true),

	// Deprecated options
	depricatedMasterListServer_(options_, "MasterListServer",
		"The master list server for scorched3d", OptionEntry::DataDepricated, "scorched3d.sourceforge.net"),
	depricatedMasterListServerURI_(options_, "MasterListServerURI",
		"The URI on the master list server for scorched3d", OptionEntry::DataDepricated, "/scorched"),
	depricatedServerAdminPassword_(options_, "ServerAdminPassword", 
		"The admin password for this server (empty password is no access)", OptionEntry::DataDepricated, ""),
	depricatedMaxArmsLevel_(options_, "MaxArmsLevel",
		"", OptionEntry::DataDepricated, 10, 0, 10, 1),
	depricatedAutoBallanceTeams_(options_, "AutoBallanceTeams",
		"", OptionEntry::DataDepricated, true),
	depricatedScoreType_(options_, "ScoreType",
		"How the winnder is choosen", OptionEntry::DataDepricated, 0, scoreEnum)
{
	char buffer[128];
	for (int i=0; i<24; i++)
	{
		snprintf(buffer, 128, "PlayerType%i", i+1);
		playerType_[i] = new OptionEntryString(playerTypeOptions_,
			buffer,
			"The type of the player e.g. human, computer etc..", 0,
			"Human");
	}
}

OptionsGame::~OptionsGame()
{
	std::list<OptionEntry *>::iterator itor;
	for (itor = playerTypeOptions_.begin();
		itor != playerTypeOptions_.end();
		++itor)
	{
		OptionEntry *entry = *itor;
		delete entry;
	}
}

bool OptionsGame::writeToBuffer(NetBuffer &buffer,
	bool useProtected,
	bool usePlayerTypes)
{
	std::list<OptionEntry *> saveOptions;
	if (usePlayerTypes) saveOptions = playerTypeOptions_;
	std::list<OptionEntry *>::iterator itor;
	for (itor = options_.begin();
		itor != options_.end();
		++itor)
	{
		OptionEntry *entry = *itor;
		saveOptions.push_back(entry);
	}

	if (!OptionEntryHelper::writeToBuffer(
		saveOptions, buffer, useProtected)) return false;
	return true;
}

bool OptionsGame::readFromBuffer(NetBufferReader &reader,
	bool useProtected,
	bool usePlayerTypes)
{
	std::list<OptionEntry *> saveOptions;
	if (usePlayerTypes) saveOptions = playerTypeOptions_;
	std::list<OptionEntry *>::iterator itor;
	for (itor = options_.begin();
		itor != options_.end();
		++itor)
	{
		OptionEntry *entry = *itor;
		saveOptions.push_back(entry);
	}

	if (!OptionEntryHelper::readFromBuffer(
		saveOptions, reader, useProtected)) return false;
	return true;
}

bool OptionsGame::writeOptionsToFile(const std::string &filePath, bool allOptions)
{
	std::list<OptionEntry *> saveOptions = 
		playerTypeOptions_; // Note: The players are also saved
	std::list<OptionEntry *>::iterator itor;
	for (itor = options_.begin();
		itor != options_.end();
		++itor)
	{
		OptionEntry *entry = *itor;
		saveOptions.push_back(entry);
	} 

	if (!OptionEntryHelper::writeToFile(saveOptions, filePath, allOptions)) return false;
	return true;
}

bool OptionsGame::readOptionsFromFile(const std::string &filePath)
{
	std::list<OptionEntry *> saveOptions = 
		playerTypeOptions_; // Note: The players are also saved
        std::list<OptionEntry *>::iterator itor;
        for (itor = options_.begin();
                itor != options_.end();
                ++itor)
        {
                OptionEntry *entry = *itor;
                saveOptions.push_back(entry);
        } 

	if (!OptionEntryHelper::readFromFile(saveOptions, filePath)) return false;
	return true;
}

std::list<OptionEntry *> &OptionsGame::getPlayerTypeOptions()
{
	return playerTypeOptions_;
}

std::list<OptionEntry *> &OptionsGame::getOptions()
{
	return options_;
}
