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

#include <events/EventHandlerDataBase.h>
#include <events/EventHandlerDataBaseMySQL.h>
#include <events/EventHandlerDataBasePGSQL.h>
#include <common/OptionsGame.h>
#include <common/OptionsScorched.h>
#include <common/Logger.h>
#include <server/ScorchedServer.h>
#include <weapons/AccessoryStore.h>
#include <weapons/Weapon.h>
#include <net/NetInterface.h>
#include <tank/Tank.h>
#include <tank/TankState.h>
#include <tank/TankScore.h>
#include <tank/TankAvatar.h>
#include <XML/XMLFile.h>
#include <stdlib.h>

EventHandlerDataBase *EventHandlerDataBase::createInstance()
{
	const char *EventHandlerDataBase =
		ScorchedServer::instance()->getOptionsGame().getStatsLogger();
	if (strcmp(EventHandlerDataBase, "mysql") == 0)
	{
		if (strcmp(ScorchedServer::instance()->getOptionsGame().getPublishAddress(),
					"AutoDetect") == 0)
		{
			S3D::dialogExit("EventHandlerDataBase",
				"Stats logging enabled but AutoDetect used for server address");
		}
#ifdef HAVE_MYSQL
		Logger::log( "Created mysql stats logger.");
		EventHandlerDataBaseMySQL *dataBase = new EventHandlerDataBaseMySQL();
		dataBase->connect();
		return dataBase;
#else
		S3D::dialogExit("EventHandlerDataBase",
			"Atempted to create mysql stats logger\n"
			"but mysql support has not been compiled into this\n"
			"scorched3d binary.");
#endif
	}
	else if (strcmp(EventHandlerDataBase, "pgsql") == 0)
	{
		if (strcmp(ScorchedServer::instance()->getOptionsGame().getPublishAddress(),
					"AutoDetect") == 0)
		{
			S3D::dialogExit("EventHandlerDataBase",
				"Stats logging enabled but AutoDetect used for server address");
		}

#ifdef HAVE_PGSQL
		EventHandlerDataBasePGSQL *dataBase = new EventHandlerDataBasePGSQL();
		dataBase->connect();
		return dataBase;
#else
		S3D::dialogExit("EventHandlerDataBase",
			"Atempted to create pgsql stats logger\n"
			"but pgsql support has not been compiled into this\n"
			"scorched3d binary.");
#endif
	}
	else
	{
		Logger::log( "Created null stats logger.");
	}
	return 0;
}

enum EventType
{
	EventKill = 1,
	EventTeamKill = 2,
	EventSelfKill = 3,
	EventResigned = 4,
	EventWon = 5,
	EventOverallWinner = 6,
	EventConnected = 7,
	EventDisconnected = 8,
	EventJoined = 9
};

const char *EventHandlerDataBase::RowResult::getValue(const char *name)
{
	std::map<std::string, unsigned int>::iterator findItor =
		names.find(name);
	if (findItor == names.end()) return 0;
	unsigned int pos = findItor->second;
	return columns[pos].c_str();
}

EventHandlerDataBase::EventHandlerDataBase() : 
	serverid_(0), seriesid_(0), prefixid_(0),
	updateTime_(0)
{
}

EventHandlerDataBase::~EventHandlerDataBase()
{
}

bool EventHandlerDataBase::connect()
{
	XMLFile file;
	std::string fileName = S3D::getSettingsFile(S3D::formatStringBuffer("mysql-%i.xml",
		ScorchedServer::instance()->getOptionsGame().getPortNo()));

	std::string host, port, user, passwd, db, prefix;
	if (!file.readFile(fileName) ||
		!file.getRootNode())
	{
		S3D::dialogExit("Stats Logging",
			S3D::formatStringBuffer(
			"Failed to parse %s settings file. Error: %s", 
			fileName.c_str(),
			file.getParserError()));
		return false;
	}

	if (!file.getRootNode()->getNamedChild("host", host) ||
		!file.getRootNode()->getNamedChild("port", port) ||
		!file.getRootNode()->getNamedChild("user", user) ||
		!file.getRootNode()->getNamedChild("passwd", passwd) ||
		!file.getRootNode()->getNamedChild("db", db) ||
		!file.getRootNode()->getNamedChild("prefix", prefix)) 
	{
		S3D::dialogExit("Stats Logging", 
			S3D::formatStringBuffer(
			"Failed to parse %s settings file.", fileName.c_str()));
		return false;
	}

	// Create the database connection
	if (!connectDatabase(host.c_str(), port.c_str(), 
		user.c_str(), passwd.c_str(), 
		db.c_str()))
	{
		S3D::dialogExit("Stats Logging",
			"Failed to connect to stats database");
		return false;
	}

	// Add event types
	runQuery("INSERT INTO scorched3d_eventtypes "
		"(eventtype, name) VALUES "
		"(%i, \"KILL\"), "
		"(%i, \"TEAMKILL\"), "
		"(%i, \"SELFKILL\"), "
		"(%i, \"WON\"), "
		"(%i, \"OVERALLWINNER\"), "
		"(%i, \"CONNECTED\"), "
		"(%i, \"DISCONNECTED\"), "
		"(%i, \"JOINED\"), "
		"(%i, \"RESIGNED\"); ",
		EventKill, EventTeamKill, EventSelfKill,
		EventWon, EventOverallWinner, 
		EventConnected, EventDisconnected, 
		EventJoined, EventResigned);

	// Get/allocate the prefixid
	std::list<EventHandlerDataBase::RowResult> prefixRows =
		runSelectQuery("SELECT prefixid FROM scorched3d_prefixs "
		"WHERE prefix = \"%s\";",
		prefix.c_str());
	if (!prefixRows.empty())
	{
		std::list<EventHandlerDataBase::RowResult>::iterator itor;
		for (itor = prefixRows.begin();
			itor != prefixRows.end();
			++itor)
		{
			EventHandlerDataBase::RowResult &rowResult = (*itor);
			prefixid_ = atoi(rowResult.columns[0].c_str());
		}
	}
	if (prefixid_ == 0)
	{
		if (runQuery("INSERT INTO scorched3d_prefixs "
			"(prefix) VALUES(\"%s\");",
			prefix.c_str()))
		{
			prefixid_ = getLastInsertId();
		}
	}

	// Get/allocate the server id
	std::list<EventHandlerDataBase::RowResult> serverIdRows =
		runSelectQuery("SELECT serverid, displaystats FROM scorched3d_servers "
		"WHERE name = \"%s\";",
		ScorchedServer::instance()->getOptionsGame().getServerName());
	if (!serverIdRows.empty())
	{
		std::list<EventHandlerDataBase::RowResult>::iterator itor;
		for (itor = serverIdRows.begin();
			itor != serverIdRows.end();
			++itor)
		{
			EventHandlerDataBase::RowResult &rowResult = (*itor);
			serverid_ = atoi(rowResult.columns[0].c_str());
			displayStats_ = (atoi(rowResult.columns[1].c_str()) != 0);
		}
	}
	if (serverid_ == 0)
	{
		if (runQuery("INSERT INTO scorched3d_servers "
			"(name, published) VALUES(\"%s\", \"%s\");",
			ScorchedServer::instance()->getOptionsGame().getServerName(),
			ScorchedServer::instance()->getOptionsGame().getPublishAddress()))
		{
			serverid_ = getLastInsertId();
			displayStats_ = true;
		}
	}

	// Get/allocate the series id
	std::list<EventHandlerDataBase::RowResult> seriesIdRows =
		runSelectQuery("SELECT seriesid FROM scorched3d_series "
		"WHERE type = 0;");
	if (!seriesIdRows.empty())
	{
		std::list<EventHandlerDataBase::RowResult>::iterator itor;
		for (itor = seriesIdRows.begin();
			itor != seriesIdRows.end();
			++itor)
		{
			EventHandlerDataBase::RowResult &rowResult = (*itor);
			seriesid_ = atoi(rowResult.columns[0].c_str());
		}
	}
	if (seriesid_ == 0)
	{
		if (runQuery("INSERT INTO scorched3d_series "
			"(started, ended) VALUES(NOW(), NOW());"))
		{
			seriesid_ = getLastInsertId();
		}
	}

	// Add this server in this series
	runQuery("INSERT INTO scorched3d_statssource "
		"(serverid, prefixid, seriesid) VALUES "
		"(%i, %i, %i);", 
		serverid_, prefixid_, seriesid_);


	// Add all the weapons
	std::list<Accessory *> weapons = 
		ScorchedServer::instance()->getAccessoryStore().getAllAccessories();
	std::list<Accessory *>::iterator itor;	
	for (itor = weapons.begin();
		itor != weapons.end();
		++itor)
	{
		Accessory *accessory = *itor;

		int weaponId = 0;
		std::list<EventHandlerDataBase::RowResult> weaponIdRows =
			runSelectQuery("SELECT weaponid FROM scorched3d_weapons "
				"WHERE name = \"%s\" AND seriesid = %i AND prefixid = %i;", 
				accessory->getName(),
				seriesid_,
				prefixid_);
		if (!weaponIdRows.empty())
		{
			std::list<EventHandlerDataBase::RowResult>::iterator itor;
			for (itor = weaponIdRows.begin();
				itor != weaponIdRows.end();
				++itor)
			{
				EventHandlerDataBase::RowResult &rowResult = (*itor);
				weaponId = atoi(rowResult.columns[0].c_str());
			}
		}

		if (weaponId == 0)
		{
			if (runQuery("INSERT INTO scorched3d_weapons "
				"(seriesid, prefixid, name, description, armslevel, cost, bundlesize, icon) "
				"VALUES(%i, %i, \"%s\", \"%s\", %i, %i, %i, \"%s\");", 
				seriesid_,
				prefixid_,
				accessory->getName(), 
				accessory->getDescription(),
				accessory->getArmsLevel(),
				accessory->getOriginalPrice(),
				accessory->getBundle(),
				accessory->getIconName()))
			{
				weaponId = getLastInsertId();
			}
		}
		else
		{
			runQuery("UPDATE scorched3d_weapons SET "
				"description = \"%s\", "
				"armslevel = %i, "
				"cost = %i, "
				"bundlesize = %i, "
				"icon = \"%s\" "
				"WHERE name = \"%s\" AND seriesid = %i AND prefixid = %i;", 
				accessory->getDescription(),
				accessory->getArmsLevel(),
				accessory->getOriginalPrice(),
				accessory->getBundle(),
				accessory->getIconName(),
				accessory->getName(),
				seriesid_,
				prefixid_);
		}

		weaponId_[accessory->getName()] = weaponId;
	}

	Logger::log(S3D::formatStringBuffer("database stats logger started, prefix=%i, server=%i, series=%i",
		prefixid_, serverid_, seriesid_));
	periodicUpdate();

	return true;
}

unsigned int EventHandlerDataBase::getAchievementId(const std::string &name)
{
	unsigned int achievementId = 0;
	std::list<EventHandlerDataBase::RowResult> achievementIdRows =
		runSelectQuery("SELECT achievementid FROM scorched3d_achievement_names "
				"WHERE name = \"%s\";", 
				name.c_str());
	if (!achievementIdRows.empty())
	{
		achievementId = atoi(achievementIdRows.front().columns[0].c_str());
	} 
	else
	{
		if (runQuery("INSERT INTO scorched3d_achievement_names "
				"(name) "
				"VALUES(\"%s\");", 
				name.c_str()))
		{
			achievementId = getLastInsertId();
		}
	}
	return achievementId;
}

unsigned int EventHandlerDataBase::getAchievementRank(unsigned int achievementId, unsigned int playerId)
{
	unsigned int result = 0;
	std::list<EventHandlerDataBase::RowResult> achievementRows =
		runSelectQuery("SELECT achievementrank FROM scorched3d_achievements "
				"WHERE achievementid = %u AND playerid = %u;", 
				achievementId, playerId);
	if (!achievementRows.empty())
	{
		result = atoi(achievementRows.front().columns[0].c_str());
	} 
	return result;
}

void EventHandlerDataBase::assignAchievementRank(unsigned int playerId, unsigned int achievementId, unsigned int rank)
{
	std::list<EventHandlerDataBase::RowResult> achievementRows =
		runSelectQuery("SELECT achievementrank FROM scorched3d_achievements "
				"WHERE achievementid = %u AND playerid = %u;", 
				achievementId, playerId);
	if (!achievementRows.empty())
	{
		runQuery("UPDATE scorched3d_achievements SET "
				"achievementrank = %u "
				"WHERE achievementid = %u AND playerid = %u;", 
				rank, achievementId, playerId);
	} 
	else
	{
		runQuery("INSERT INTO scorched3d_achievements "
				"(achievementid, playerid, achievementrank) "
				"VALUES(%u, %u, %u);", 
				achievementId, playerId, rank);
	}
}

void EventHandlerDataBase::addIpAliases(int playerId, 
	std::set<int> &currentPlayers, std::list<std::string> &results)
{
	currentPlayers.insert(playerId);
	addAliases(playerId, results);

	std::list<std::string> ipaddresses;
	std::list<EventHandlerDataBase::RowResult> ipaddressesRows =
		runSelectQuery("SELECT ipaddress FROM scorched3d_ipaddress "
			"WHERE playerid = %i;", playerId);
	if (!ipaddresses.empty())
	{
		std::list<EventHandlerDataBase::RowResult>::iterator itor;
		for (itor = ipaddressesRows.begin();
			itor != ipaddressesRows.end();
			++itor)
		{
			EventHandlerDataBase::RowResult &rowResult = (*itor);
			ipaddresses.push_back(rowResult.columns[0]);
		}
	}

	std::list<std::string>::iterator itor;
	for (itor = ipaddresses.begin();
		itor != ipaddresses.end();
		++itor)
	{
		const char *ipaddress = (*itor).c_str();
		std::list<int> newplayers;

		std::list<EventHandlerDataBase::RowResult> ipaddressRows =
			runSelectQuery("SELECT playerid FROM scorched3d_ipaddress "
				"WHERE ipaddress = \"%s\";", ipaddress);
		if (!ipaddressRows.empty())
		{
			std::list<EventHandlerDataBase::RowResult>::iterator itor;
			for (itor = ipaddressRows.begin();
				itor != ipaddressRows.end();
				++itor)
			{
				EventHandlerDataBase::RowResult &rowResult = (*itor);
				int newplayerid = atoi(rowResult.columns[0].c_str());
				if (currentPlayers.find(newplayerid) == currentPlayers.end())
				{
					newplayers.push_back(newplayerid);
				}
			}
		}

		std::list<int>::iterator itor2;
		for (itor2 = newplayers.begin();
			itor2 != newplayers.end();
			++itor2)
		{
			addIpAliases((*itor2), currentPlayers, results);
		}
	}
}

std::string EventHandlerDataBase::getTopRanks()
{
	const char *columns = 
		"rank, kills, deaths, selfkills, teamkills, shots, wins, "
		"overallwinner, resigns, gamesplayed, timeplayed, roundsplayed, "
		"moneyearned, skill, name";

	std::string stringResult;
	std::list<EventHandlerDataBase::RowResult> rankRows =
		runSelectQuery(
			"select %s from scorched3d_stats "
			"left join scorched3d_players on scorched3d_stats.playerid = "
			"scorched3d_players.playerid where seriesid=%i and prefixid=%i "
			"order by skill desc limit 0,50",
			columns, seriesid_, prefixid_);

	stringResult.append("<table>");

	std::string cols(columns);
	char *token = strtok((char *) cols.c_str(), " ");
	stringResult.append("<tr>");
	while(token != 0)
	{
		stringResult.append("<td><b>").append(token).append("</b></td>");
		token = strtok(0, " ");
	}
	stringResult.append("</tr>");

	if (!rankRows.empty())
	{
		std::list<EventHandlerDataBase::RowResult>::iterator itor;
		for (itor = rankRows.begin();
			itor != rankRows.end();
			++itor)
		{
			EventHandlerDataBase::RowResult &result = (*itor);
			stringResult.append("<tr>");
			for (unsigned int i=0; i<result.columns.size(); i++)
			{
				stringResult.append("<td>").append(result.columns[i]).append("</td>");
			}
			stringResult.append("</tr>");
		}
	}
	stringResult.append("</table>");
	return S3D::formatStringBuffer("%s", stringResult.c_str());
}

std::string EventHandlerDataBase::getPlayerInfo(const char *player)
{
	std::string stringResult;
	std::list<EventHandlerDataBase::RowResult> playerRows =
		runSelectQuery("select "
			"scorched3d_names.playerid as playerid, "
			"scorched3d_names.name as name, "
			"uniqueid from "
			"scorched3d_names left join "
			"scorched3d_players on scorched3d_players.playerid = scorched3d_names.playerid "
			"where LOCATE(LOWER(\"%s\"), LOWER(scorched3d_names.name)) != 0 limit 0,50",
			player);
	if (!playerRows.empty())
	{
		std::list<EventHandlerDataBase::RowResult>::iterator itor;
		for (itor = playerRows.begin();
			itor != playerRows.end();
			++itor)
		{
			EventHandlerDataBase::RowResult &result = (*itor);
			for (unsigned int i=0; i<result.columns.size(); i++)
			{
				stringResult.append(result.columns[i]);
				if (i < result.columns.size() - 1) stringResult.append(",");
				else stringResult.append("\n");
			}
		}
	}
	
	return S3D::formatStringBuffer("%s", stringResult.c_str());
}

void EventHandlerDataBase::combinePlayers(unsigned int player1, unsigned int player2)
{
	// Check these players exist
	std::list<EventHandlerDataBase::RowResult> player1Rows =
		runSelectQuery("select name, uniqueid from "
		"scorched3d_players where playerid=%i", 
		player1);
	std::list<EventHandlerDataBase::RowResult> player2Rows =
		runSelectQuery("select name, uniqueid from "
		"scorched3d_players where playerid=%i", 
		player2);
	if (player1Rows.empty()) return;
	if (player2Rows.empty()) return;

	// Find all results from player2
	std::list<EventHandlerDataBase::RowResult> player2Results =
		runSelectQuery("select * from scorched3d_stats where playerid=%i", 
		player2);
	std::list<EventHandlerDataBase::RowResult>::iterator itor;
	for (itor = player2Results.begin();
		itor != player2Results.end();
		++itor)
	{
		EventHandlerDataBase::RowResult &player2Result = *itor;
		const char *prefixId = player2Result.getValue("prefixid");
		const char *seriesId = player2Result.getValue("seriesid");
		if (prefixId && seriesId)
		{
			// Find all results from player1 in the same prefix and series
			std::list<EventHandlerDataBase::RowResult> player1Results =
				runSelectQuery("select * from scorched3d_stats where playerid=%i AND "
				"prefixid=%s and seriesid=%s", 
				player1, prefixId, seriesId);
			if (player1Results.empty())
			{
				// There are no results for player1, just move the results
				runQuery("update scorched3d_stats set playerid=%i where "
					"playerid=%i and prefixid=%s and seriesid=%s", 
					player1, player2,
					prefixId, seriesId);
			}
			else
			{
				// There are results for player1, combine the results
				EventHandlerDataBase::RowResult &player1Result = player1Results.front();
				std::map<std::string, unsigned int>::iterator itor;
				for (itor = player1Result.names.begin();
					itor != player1Result.names.end();
					++itor)
				{
					std::string name = itor->first;
					std::string value1 = player1Result.getValue(name.c_str());
					std::string value2 = player2Result.getValue(name.c_str());
					
					std::string query = "update scorched3d_stats ";
					if (name == "playerid" ||
						name == "prefixid" ||
						name == "seriesid" ||
						name == "lastconnected" ||
						name == "rank" ||
						name == "skill")
					{
						// Ignore
					}
					else
					{
						int v1 = atoi(value1.c_str());
						int v2 = atoi(value2.c_str());
						int value = v1 + v2;
						query.append(S3D::formatStringBuffer("set %s=%i ", name.c_str(), value));
					}
					query.append(
						S3D::formatStringBuffer("where playerid=%i and prefixid=%s and seriesid=%s", 
							player1, prefixId, seriesId));
					runQuery(query.c_str());
				}
			}
		}
	}

	// Remove player2
	runQuery("delete from scorched3d_players where playerid=%i", player2);
	runQuery("delete from scorched3d_stats where playerid=%i", player2);
	runQuery("delete from scorched3d_names where playerid=%i", player2);
	runQuery("delete from scorched3d_ipaddresses where playerid=%i", player2);
}

std::list<std::string> EventHandlerDataBase::getIpAliases(const char *unqiueId)
{
	std::list<std::string> results;
	int playerId = getPlayerId(unqiueId);
	if (playerId == 0) return results;

	std::set<int> currentPlayers;
	addIpAliases(playerId, currentPlayers, results);
	
	return results;
}

static bool findInList(std::list<std::string> &results, 
	const char *name)
{
	std::list<std::string>::iterator itor;
	for (itor = results.begin();
		itor != results.end();
		++itor)
	{
		if (0 == strcmp(name, (*itor).c_str())) return true;
	}
	return false;
}

void EventHandlerDataBase::addAliases(int playerId, 
	std::list<std::string> &results)
{
	// Make sure the last used name is in the list and
	// it is always first
	{
		std::list<EventHandlerDataBase::RowResult> nameRows =
			runSelectQuery("SELECT name FROM scorched3d_players "
				"WHERE playerid = %i;", playerId);
		if (!nameRows.empty())
		{
			std::list<EventHandlerDataBase::RowResult>::iterator itor;
			for (itor = nameRows.begin();
				itor != nameRows.end();
				++itor)
			{
				EventHandlerDataBase::RowResult &rowResult = (*itor);
				if (!findInList(results, rowResult.columns[0].c_str()))
				{
					results.push_back(rowResult.columns[0].c_str());
				}
			}
		}
	}

	// Add all other UNIQUE aliases
	{
		std::list<EventHandlerDataBase::RowResult> nameRows =
			runSelectQuery("SELECT name FROM scorched3d_names "
				"WHERE playerid = %i;", playerId);
		if (!nameRows.empty())
		{
			std::list<EventHandlerDataBase::RowResult>::iterator itor;
			for (itor = nameRows.begin();
				itor != nameRows.end();
				++itor)
			{
				EventHandlerDataBase::RowResult &rowResult = (*itor);
				if (!findInList(results, rowResult.columns[0].c_str()))
				{
					results.push_back(rowResult.columns[0].c_str());
				}
			}
		}
	}
}

std::list<std::string> EventHandlerDataBase::getAliases(const char *unqiueId)
{
	std::list<std::string> results;
	int playerId = getPlayerId(unqiueId);
	if (playerId == 0) return results;
	addAliases(playerId, results);

	return results;
}

void EventHandlerDataBase::gameStart(std::list<Tank *> &tanks)
{
	runQuery("UPDATE scorched3d_series SET games = games + 1, "
		"ended = NOW() WHERE seriesid = %i;",
		seriesid_);

	std::list<Tank *>::iterator itor;
	for (itor = tanks.begin();
		itor != tanks.end();
		++itor)
	{
		Tank *tank = *itor;
		if (tank->getState().getTankPlaying())
		{
			runQuery("UPDATE scorched3d_stats SET gamesplayed=gamesplayed+1 "
				"WHERE playerid = %i AND prefixid = %i AND seriesid = %i;", 
				playerId_[tank->getUniqueId()],
				prefixid_,
				seriesid_);
		}
	}
}

void EventHandlerDataBase::roundStart(std::list<Tank *> &tanks)
{
	runQuery("UPDATE scorched3d_series SET rounds = rounds + 1, "
		"ended = NOW() WHERE seriesid = %i;",
		seriesid_);

	std::list<Tank *>::iterator itor;
	for (itor = tanks.begin();
		itor != tanks.end();
		++itor)
	{
		Tank *tank = *itor;
		if (tank->getState().getTankPlaying())
		{
			runQuery("UPDATE scorched3d_stats SET roundsplayed=roundsplayed+1 "
				"WHERE playerid = %i AND prefixid = %i AND seriesid = %i;", 
				playerId_[tank->getUniqueId()],
				prefixid_,
				seriesid_);
		}
	}
}

void EventHandlerDataBase::tankFired(Tank *firedTank, Weapon *weapon)
{
	runQuery("UPDATE scorched3d_stats SET shots=shots+1 "
		"WHERE playerid = %i AND prefixid = %i AND seriesid = %i;", 
		playerId_[firedTank->getUniqueId()],
		prefixid_,
		seriesid_);

	runQuery("UPDATE scorched3d_weapons SET shots=shots+1 "
		"WHERE weaponid = \"%i\" AND prefixid = %i AND seriesid = %i;", 
		weaponId_[weapon->getParent()->getName()],
		prefixid_,
		seriesid_);
}

void EventHandlerDataBase::tankResigned(Tank *tank)
{
    runQuery("INSERT INTO scorched3d_events "
		"(prefixid, seriesid, eventtype, playerid, otherplayerid, weaponid, eventtime) "
		"VALUES(%i, %i, %i, %i, 0, 0, NOW());",
		prefixid_, seriesid_,
		EventResigned,
		playerId_[tank->getUniqueId()]);

	runQuery("UPDATE scorched3d_stats SET resigns=resigns+1 "
		"WHERE playerid = %i AND prefixid = %i AND seriesid = %i;", 
		playerId_[tank->getUniqueId()],
		prefixid_,
		seriesid_);
}

void EventHandlerDataBase::periodicUpdate(Tank *tank)
{
	if (tank->getState().getTankPlaying())
	{
		unsigned int playerId = playerId_[tank->getUniqueId()];

		runQuery("UPDATE scorched3d_stats SET "
			"timeplayed=timeplayed+%i, moneyearned=moneyearned+%i, scoreearned=scoreearned+%i "
			"WHERE playerid = %i AND prefixid = %i AND seriesid = %i;", 
			((unsigned int) tank->getScore().getTimePlayedStat()),
			tank->getScore().getTotalMoneyEarnedStat(), 
			tank->getScore().getTotalScoreEarnedStat(),
			playerId,
			prefixid_,
			seriesid_);
	}
}

void EventHandlerDataBase::periodicUpdate()
{
	time_t currentTime = time(0);
	if (currentTime - updateTime_ > 60 * 60 * 12) // 12 hrs
	{
		updateTime_ = currentTime;
		Logger::log(S3D::formatStringBuffer("statslogger database starting periodics"));

		// Cleanup orphaned avatars
		std::list<EventHandlerDataBase::RowResult> binaryRows =
			runSelectQuery("select binaryid, count(binaryid) "
				"from scorched3d_binary left join "
				"scorched3d_players on binaryid = avatarid group by avatarid");
		if (!binaryRows.empty())
		{
			std::list<EventHandlerDataBase::RowResult>::iterator itor;
			for (itor = binaryRows.begin();
				itor != binaryRows.end();
				++itor)
			{
				EventHandlerDataBase::RowResult &rowResult = (*itor);
				if (rowResult.columns[1] == "0")
				{
					runQuery("delete from scorched3d_binary where binaryid = %s",
						rowResult.columns[0].c_str());
				}
			}
		}

		// Generate ranks
		std::list<EventHandlerDataBase::RowResult>::iterator playerItor;
		std::list<EventHandlerDataBase::RowResult> playerRows =
			runSelectQuery(
				"SELECT playerid, skill from scorched3d_stats "
				"WHERE seriesid=%u and prefixid=%u order by skill desc",
				seriesid_,
				prefixid_);

		int rank = 1;
		for (playerItor = playerRows.begin();
			playerItor != playerRows.end();
			++playerItor, rank++)
		{
			EventHandlerDataBase::RowResult &playerRow = *playerItor;
			runQuery("UPDATE scorched3d_stats SET rank=%i "
				"WHERE seriesid=%u and prefixid=%u and playerid=%s",
				rank, 
				seriesid_,
				prefixid_,
				playerRow.columns[0].c_str());
		}

		Logger::log(S3D::formatStringBuffer("statslogger database finished periodics"));
	}
}

EventHandlerDataBase::TankRank EventHandlerDataBase::tankRank(Tank *tank)
{
	TankRank result(tank->getPlayerId());

	if (!displayStats_) return result;

	// Try to determine this players sql playerid
	std::list<EventHandlerDataBase::RowResult> skillRows =
		runSelectQuery("SELECT skill FROM scorched3d_stats "
		"WHERE playerid = %i AND prefixid = %i AND seriesid = %i;", 
		playerId_[tank->getUniqueId()],
		prefixid_,
		seriesid_);
	if (!skillRows.empty())
	{
		std::list<EventHandlerDataBase::RowResult>::iterator itor;
		for (itor = skillRows.begin();
			itor != skillRows.end();
			++itor)
		{
			EventHandlerDataBase::RowResult &rowResult = (*itor);
			result.setSkill(atoi(rowResult.columns[0].c_str()));
		}

		std::list<EventHandlerDataBase::RowResult> countRows =
			runSelectQuery("SELECT count(*) FROM scorched3d_stats "
			"WHERE skill > \"%i\" AND prefixid = %i AND seriesid = %i;", 
			result.getSkill(),
			prefixid_,
			seriesid_);
		if (!countRows.empty())
		{
			std::list<EventHandlerDataBase::RowResult>::iterator itor;
			for (itor = countRows.begin();
				itor != countRows.end();
				++itor)
			{
				EventHandlerDataBase::RowResult &rowResult = (*itor);
				result.setRank(atoi(rowResult.columns[0].c_str()) + 1);
			}
		}
	}

	return result;
}

int EventHandlerDataBase::getPlayerId(const char *uniqueId)
{
	std::map<std::string, int>::iterator itor = playerId_.find(uniqueId);
	if (itor != playerId_.end()) return itor->second;

	// Try to determine this players sql playerid
	int playerId = 0;
	std::list<EventHandlerDataBase::RowResult> playerIdRows =
		runSelectQuery("SELECT playerid FROM scorched3d_players "
			"WHERE uniqueid = \"%s\";", uniqueId);
	if (!playerIdRows.empty())
	{
		std::list<EventHandlerDataBase::RowResult>::iterator itor;
		for (itor = playerIdRows.begin();
			itor != playerIdRows.end();
			++itor)
		{
			EventHandlerDataBase::RowResult &rowResult = (*itor);
			playerId = atoi(rowResult.columns[0].c_str());
			playerId_[uniqueId] = playerId;
		}
	}

	return playerId;
}

unsigned int EventHandlerDataBase::getStatsId(const char *uniqueId)
{
	int id = getPlayerId(uniqueId);
	return id;
}

std::string EventHandlerDataBase::allocateId() 
{ 
	const char possibleChars [] = {
		'1', '2', '3', '4', '5', '6', '7', '8', '9',
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
		'J', 'K', 'L', 'M', 'N', 'P', 'Q', 'R', 'S',
		'T', 'U', 'V', 'W', 'X', 'Y', 'Z' };
	char buffer[128];

	do
	{
		int pos = 0;
		for (int j=0; j<3; j++)
		{
			for (int i=0; i<8; i++)
			{
				buffer[pos++] = possibleChars[rand() % 33];
			}
			buffer[pos++] = '-';
		}
		buffer[pos - 1] = '\0';

	} while (getPlayerId(buffer) != 0);
	return buffer;
}

void EventHandlerDataBase::addInfo(Tank *tank)
{
	char playerName[1024];
	escapeString(playerName, 
		tank->getCStrName().c_str(),
		(unsigned long) tank->getCStrName().size());

	// Add the players name (may fail if duplicates)
	runQuery("INSERT INTO scorched3d_names (playerid, name, count) VALUES "
		"(%i, \"%s\", 0);", 
		playerId_[tank->getUniqueId()], 
		playerName);
	runQuery("UPDATE scorched3d_names SET count=count+1 WHERE "
		"playerid=%i AND name=\"%s\";", 
		playerId_[tank->getUniqueId()], 
		playerName);

	// Add the ipaddress (may fail if duplicates)
	runQuery("INSERT INTO scorched3d_ipaddress (playerid, ipaddress, count) VALUES "
		"(%i, \"%s\", 0);",
		playerId_[tank->getUniqueId()], 
		NetInterface::getIpName(tank->getIpAddress()));
	runQuery("UPDATE scorched3d_ipaddress SET count=count+1 WHERE "
		"playerid=%i AND ipaddress=\"%s\";",
		playerId_[tank->getUniqueId()], 
		NetInterface::getIpName(tank->getIpAddress()));

	// Update last username etc
	runQuery("UPDATE scorched3d_players SET "
		"name=\"%s\", ipaddress=\"%s\" "
		"WHERE playerid = %i;",
		playerName, 
		NetInterface::getIpName(tank->getIpAddress()),
		playerId_[tank->getUniqueId()]);
}

void EventHandlerDataBase::tankConnected(Tank *tank)
{
	// We don't have a player id, create one
	int playerId = getPlayerId(tank->getUniqueId());
	if (playerId == 0)
	{
		if (runQuery("INSERT INTO scorched3d_players (uniqueid) "
			"VALUES(\"%s\");",
			tank->getUniqueId()))
		{
			playerId = getLastInsertId();
			Logger::log(S3D::formatStringBuffer("Add new stats user \"%i\"", playerId));
		}
	}
	else
	{
		Logger::log(S3D::formatStringBuffer("Found stats user \"%i\"", playerId));
	}

	// Create the players stats entry if it does not exist
	std::list<EventHandlerDataBase::RowResult> playerIdRows =
		runSelectQuery("SELECT playerid FROM scorched3d_stats "
		"WHERE playerid = %i AND prefixid = %i AND seriesid = %i;", 
		playerId,
		prefixid_,
		seriesid_);
	if (playerIdRows.empty())
	{
		runQuery("INSERT INTO scorched3d_stats (playerid, prefixid, seriesid) "
			"VALUES(%i, %i, %i);",
			playerId,
			prefixid_,
			seriesid_);
	}

	// Store this new player id
	playerId_[tank->getUniqueId()] = playerId;

	// Add name and ip address
	addInfo(tank);

	// Connecting events
	runQuery("INSERT INTO scorched3d_events "
		"(prefixid, seriesid, eventtype, playerid, otherplayerid, weaponid, eventtime) "
		"VALUES(%i, %i, %i, %i, 0, 0, NOW());",
		prefixid_, seriesid_,
		EventConnected,
		playerId_[tank->getUniqueId()]);

	// Connecting stats
	runQuery("UPDATE scorched3d_players SET osdesc=\"%s\" "
		"WHERE playerid = %i;", 
		tank->getHostDesc(),
		playerId);
	runQuery("UPDATE scorched3d_stats SET connects=connects+1, "
		"lastconnected=NOW() "
		"WHERE playerid = %i AND prefixid = %i AND seriesid = %i;", 
		playerId,
		prefixid_,
		seriesid_);
}

void EventHandlerDataBase::tankJoined(Tank *tank)
{
	// Joined events
	runQuery("INSERT INTO scorched3d_events "
		"(prefixid, seriesid, eventtype, playerid, otherplayerid, weaponid, eventtime) "
		"VALUES(%i, %i, %i, %i, 0, 0, NOW());",
		prefixid_, seriesid_,
		EventJoined,
		playerId_[tank->getUniqueId()]);

	// add new info
	addInfo(tank);

	// Add the avatar
	if (tank->getAvatar().getName()[0])
	{
		char buffer[32];
		for (int i=0; i<30; i++)
		{
			buffer[i] = tank->getAvatar().getName()[i];
			if (!buffer[i]) break;
		}
		buffer[30] = '\0';

		int binaryid = 0;
		unsigned int crc = tank->getAvatar().getCrc();
		std::list<EventHandlerDataBase::RowResult> binaryIdRows =
			runSelectQuery("SELECT binaryid FROM scorched3d_binary "
			"WHERE name = \"%s\" AND crc = %u;", 
			buffer,
			crc);
		if (!binaryIdRows.empty())
		{
			std::list<EventHandlerDataBase::RowResult>::iterator itor;
			for (itor = binaryIdRows.begin();
				itor != binaryIdRows.end();
				++itor)
			{
				EventHandlerDataBase::RowResult &rowResult = (*itor);
				binaryid = atoi(rowResult.columns[0].c_str());
			}
		}

		if (binaryid == 0)
		{
			char *to = new char[tank->getAvatar().getFile().getBufferUsed() * 2];
            escapeString(to, 
				tank->getAvatar().getFile().getBuffer(),
				tank->getAvatar().getFile().getBufferUsed());
			if (runQuery("INSERT INTO scorched3d_binary "
				"(name, crc, length, data) "
				"VALUES(\"%s\", %u, %u, \"%s\");", 
				buffer,
				crc,
				tank->getAvatar().getFile().getBufferUsed(),
				to))
			{
				binaryid = getLastInsertId();
			}

			delete [] to;
		}

		// Set the avatar id
		runQuery("UPDATE scorched3d_players SET avatarid = %i "
			"WHERE playerid = %i;", 
			binaryid,
			playerId_[tank->getUniqueId()]);
	}
}

int EventHandlerDataBase::getKillCount(const char *uniqueId)
{
	int kills = 0;
	int playerId = getPlayerId(uniqueId);
	if (playerId != 0) 
	{
		std::list<EventHandlerDataBase::RowResult> killsRows =
			runSelectQuery("SELECT kills FROM scorched3d_stats "
			"WHERE playerid = %i;",
			playerId);
		if (!killsRows.empty())
		{
			std::list<EventHandlerDataBase::RowResult>::iterator itor;
			for (itor = killsRows.begin();
				itor != killsRows.end();
				++itor)
			{
				EventHandlerDataBase::RowResult &rowResult = (*itor);
				kills += atoi(rowResult.columns[0].c_str());
			}
		}
	}

	return kills;
}

void EventHandlerDataBase::tankDisconnected(Tank *tank)
{
	// Leaving events
	runQuery("INSERT INTO scorched3d_events "
		"(prefixid, seriesid, eventtype, playerid, otherplayerid, weaponid, eventtime) "
		"VALUES(%i, %i, %i, %i, 0, 0, NOW());",
		prefixid_, seriesid_,
		EventDisconnected,
		playerId_[tank->getUniqueId()]);

	periodicUpdate(tank);
}

void EventHandlerDataBase::tankKilled(Tank *firedTank, Tank *deadTank, Weapon *weapon)
{
	runQuery("INSERT INTO scorched3d_events "
		"(prefixid, seriesid, eventtype, playerid, otherplayerid, weaponid, eventtime) "
		"VALUES(%i, %i, %i, %i, %i, %i, NOW());",
		prefixid_, seriesid_,
		EventKill,
		playerId_[firedTank->getUniqueId()], 
		playerId_[deadTank->getUniqueId()], 
		weaponId_[weapon->getParent()->getName()]);

	runQuery("UPDATE scorched3d_stats SET kills=kills+1, skill=%i "
		"WHERE playerid = %i AND prefixid = %i AND seriesid = %i;", 
		firedTank->getScore().getSkill(),
		playerId_[firedTank->getUniqueId()],
		prefixid_,
		seriesid_);
		
	runQuery("UPDATE scorched3d_stats SET deaths=deaths+1, skill=%i "
		"WHERE playerid = %i AND prefixid = %i AND seriesid = %i;", 
		deadTank->getScore().getSkill(),
		playerId_[deadTank->getUniqueId()],
		prefixid_,
		seriesid_);

	runQuery("UPDATE scorched3d_weapons SET kills=kills+1 "
		"WHERE weaponid = \"%i\" AND prefixid = %i AND seriesid = %i;", 
		weaponId_[weapon->getParent()->getName()],
		prefixid_,
		seriesid_);
}

void EventHandlerDataBase::tankTeamKilled(Tank *firedTank, Tank *deadTank, Weapon *weapon)
{
	runQuery("INSERT INTO scorched3d_events "
		"(prefixid, seriesid, eventtype, playerid, otherplayerid, weaponid, eventtime) "
		"VALUES(%i, %i, %i, %i, %i, %i, NOW());",
		prefixid_, seriesid_,
		EventTeamKill,
		playerId_[firedTank->getUniqueId()],
		playerId_[deadTank->getUniqueId()], 
		weaponId_[weapon->getParent()->getName()]);

	runQuery("UPDATE scorched3d_stats SET teamkills=teamkills+1, skill=%i  "
		"WHERE playerid = %i AND prefixid = %i AND seriesid = %i;", 
		firedTank->getScore().getSkill(),
		playerId_[firedTank->getUniqueId()],
		prefixid_,
		seriesid_);
		
	runQuery("UPDATE scorched3d_stats SET deaths=deaths+1 "
		"WHERE playerid = %i AND prefixid = %i AND seriesid = %i;", 
		playerId_[deadTank->getUniqueId()],
		prefixid_,
		seriesid_);

	runQuery("UPDATE scorched3d_weapons SET kills=kills+1 "
		"WHERE weaponid = \"%i\" AND prefixid = %i AND seriesid = %i;", 
		weaponId_[weapon->getParent()->getName()],
		prefixid_,
		seriesid_);
}

void EventHandlerDataBase::tankSelfKilled(Tank *firedTank, Weapon *weapon)
{
	runQuery("INSERT INTO scorched3d_events "
		"(prefixid, seriesid, eventtype, playerid, otherplayerid, weaponid, eventtime) "
		"VALUES(%i, %i, %i, %i, 0, %i, NOW());",
		prefixid_, seriesid_, 
		EventSelfKill,
		playerId_[firedTank->getUniqueId()],
		weaponId_[weapon->getParent()->getName()]);

	runQuery("UPDATE scorched3d_stats SET selfkills=selfkills+1, deaths=deaths+1, skill=%i "
		"WHERE playerid = %i AND prefixid = %i AND seriesid = %i;", 
		firedTank->getScore().getSkill(),
		playerId_[firedTank->getUniqueId()],
		prefixid_,
		seriesid_);

	runQuery("UPDATE scorched3d_weapons SET kills=kills+1 "
		"WHERE weaponid = \"%i\" AND prefixid = %i AND seriesid = %i;", 
		weaponId_[weapon->getParent()->getName()],
		prefixid_,
		seriesid_);
}

void EventHandlerDataBase::tankWon(Tank *tank)
{
	runQuery("INSERT INTO scorched3d_events "
		"(prefixid, seriesid, eventtype, playerid, otherplayerid, weaponid, eventtime) "
		"VALUES(%i, %i, %i, %i, 0, 0, NOW());",
		prefixid_, seriesid_,
		EventWon,
		playerId_[tank->getUniqueId()]);

	runQuery("UPDATE scorched3d_stats SET wins=wins+1, skill=%i  "
		"WHERE playerid = %i AND prefixid = %i AND seriesid = %i;", 
		tank->getScore().getSkill(),
		playerId_[tank->getUniqueId()],
		prefixid_,
		seriesid_);
}

void EventHandlerDataBase::tankOverallWinner(Tank *tank)
{
	runQuery("INSERT INTO scorched3d_events "
		"(prefixid, seriesid, eventtype, playerid, otherplayerid, weaponid, eventtime) "
		"VALUES(%i, %i, %i, %i, 0, 0, NOW());",
		prefixid_, seriesid_,
		EventOverallWinner,
		playerId_[tank->getUniqueId()]);

	runQuery("UPDATE scorched3d_stats SET overallwinner=overallwinner+1, skill=%i "
		"WHERE playerid = %i AND prefixid = %i AND seriesid = %i;",
		tank->getScore().getSkill(),
		playerId_[tank->getUniqueId()],
		prefixid_,
		seriesid_);
}
