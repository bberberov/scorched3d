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

#if !defined(__INCLUDE_EventHandlerDataBaseh_INCLUDE__)
#define __INCLUDE_EventHandlerDataBaseh_INCLUDE__

#include <list>
#include <map>
#include <vector>
#include <set>
#include <string>
#include <events/EventHandler.h>

class EventHandlerDataBase : public EventHandler
{
public:
	struct TankRank
	{
		TankRank(unsigned int playerId) : 
			playerId_(playerId), rank_(-1), skill_(-1) {}
		TankRank(unsigned int playerId, int rank, int skill) : 
			playerId_(playerId), rank_(rank), skill_(skill) {}

		int getRank() { return rank_; }
		int getSkill() { return skill_; }
		unsigned int getPlayerId() { return playerId_; }

		void setRank(int rank) { rank_ = rank; }
		void setSkill(int skill) { skill_ = skill; }
		void setPlayerId(int playerId) { playerId_ = playerId; }

	protected:
		unsigned int playerId_;
		int rank_;
		int skill_;
	};

	static EventHandlerDataBase *createInstance();

	EventHandlerDataBase();
	virtual ~EventHandlerDataBase();

	int getKillCount(const char *uniqueId);
	std::list<std::string> getAliases(const char *unqiueId);
	std::list<std::string> getIpAliases(const char *unqiueId);
	TankRank tankRank(Tank *tank);
	std::string allocateId();
	unsigned int getStatsId(const char *uniqueId);
	std::string getTopRanks();
	std::string getPlayerInfo(const char *player);
	void combinePlayers(unsigned int player1, unsigned int player2);

	unsigned int getAchievementId(const std::string &name);
	unsigned int getAchievementRank(unsigned int achievementId, unsigned int playerId);

	void assignAchievementRank(unsigned int playerId, unsigned int achievementId, unsigned int rank);

	virtual void periodicUpdate();
	virtual void periodicUpdate(Tank *tank);

	virtual void gameStart(std::list<Tank *> &tanks);
	virtual void roundStart(std::list<Tank *> &tanks);

	virtual void tankConnected(Tank *tank);
	virtual void tankDisconnected(Tank *tank);
	virtual void tankJoined(Tank *tank);

	virtual void tankFired(Tank *firedTank, Weapon *weapon);
	virtual void tankResigned(Tank *resignedTank);

	virtual void tankKilled(Tank *firedTank, Tank *deadTank, Weapon *weapon);
	virtual void tankTeamKilled(Tank *firedTank, Tank *deadTank, Weapon *weapon);
	virtual void tankSelfKilled(Tank *firedTank, Weapon *weapon);

	virtual void tankWon(Tank *tank);
	virtual void tankOverallWinner(Tank *tank);

protected:
	struct RowResult
	{
		std::vector<std::string> columns;
		std::map<std::string, unsigned int> names;

		const char *getValue(const char *name);
	};

	// Overridden by new implementations
	virtual bool runQuery(const char *, ...) = 0;
	virtual std::list<RowResult> runSelectQuery(const char *, ...) = 0;
	virtual bool connectDatabase(const char *host, const char *port,
		const char *user, const char *passwd, 
		const char *db) = 0;

	virtual int getLastInsertId() = 0;
	virtual void escapeString(char *to, const char *from, unsigned long length) = 0;

	// Default stuff
	time_t updateTime_;
	int serverid_;
	int seriesid_;
	int prefixid_;
	bool displayStats_;

	std::map<std::string, int> playerId_;
	std::map<std::string, int> weaponId_;

	bool connect();
	int getPlayerId(const char *uniqueId);

	void addInfo(Tank *tank);
	void addAliases(int playerId, 
		std::list<std::string> &results);
	void addIpAliases(int playerId, 
		std::set<int> &currentPlayers, std::list<std::string> &result);
};

#endif

