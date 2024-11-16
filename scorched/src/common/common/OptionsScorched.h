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

#if !defined(__INCLUDE_OptionsScorchedh_INCLUDE__)
#define __INCLUDE_OptionsScorchedh_INCLUDE__

#include <common/OptionsGame.h>

#define GENERIC_GETTER(x) \
	{ \
	if (levelOptions_.get##x##Entry().isChangedValue()) return levelOptions_.get##x(); \
	else return mainOptions_.get##x(); \
	};

#define OPTIONSTRING_GETTER(x) const char *get##x() GENERIC_GETTER(x)
#define OPTIONINT_GETTER(x) int get##x() GENERIC_GETTER(x)
#define OPTIONENUM_GETTER(x) OptionEntryEnum get##x() GENERIC_GETTER(x)
#define OPTIONSTRINGENUM_GETTER(x) const char *get##x() GENERIC_GETTER(x)
#define OPTIONBOOL_GETTER(x) bool get##x() GENERIC_GETTER(x)
#define OPTIONFLOAT_GETTER(x) bool get##x() GENERIC_GETTER(x)
#define OPTIONVECTOR_GETTER(x) bool get##x() GENERIC_GETTER(x)

class ScorchedContext;
class LandscapeDefinition;
class LandscapeInclude;
class OptionsScorched
{
public:
	OptionsScorched();
	virtual ~OptionsScorched();

	OPTIONINT_GETTER(PortNo);
	OPTIONINT_GETTER(ManagementPortNo);

	OPTIONSTRING_GETTER(Tutorial);
	OPTIONINT_GETTER(Teams);
	OPTIONINT_GETTER(MinimumLandHeight);
	OPTIONINT_GETTER(WeaponSpeed);
	OPTIONINT_GETTER(StartArmsLevel);
	OPTIONINT_GETTER(EndArmsLevel);
	OPTIONINT_GETTER(MaxNumberWeapons);
	OPTIONINT_GETTER(NoMaxPlayers);
	OPTIONINT_GETTER(NoMinPlayers);
	OPTIONINT_GETTER(NoRounds);
	OPTIONINT_GETTER(NoTurns);
	OPTIONINT_GETTER(RemoveBotsAtPlayers);
	OPTIONINT_GETTER(MaxLandscapeSize);
	OPTIONINT_GETTER(AllowedMissedMoves);
	OPTIONINT_GETTER(MinFallingDistance);
	OPTIONBOOL_GETTER(TankFallingDamage);
	OPTIONINT_GETTER(MaxClimbingDistance);
	OPTIONINT_GETTER(PlayerLives);
	OPTIONINT_GETTER(Gravity);

	OPTIONINT_GETTER(StartTime);
	OPTIONINT_GETTER(ShotTime);
	OPTIONINT_GETTER(AIShotTime);
	OPTIONINT_GETTER(BuyingTime);
	OPTIONINT_GETTER(RoundTime);
	OPTIONINT_GETTER(RemoveTime);
	OPTIONINT_GETTER(RoundScoreTime);
	OPTIONINT_GETTER(ScoreTime);
	OPTIONINT_GETTER(IdleCycleTime);

	OPTIONENUM_GETTER(WindForce);
	OPTIONENUM_GETTER(WindType);
	OPTIONENUM_GETTER(WallType);
	OPTIONENUM_GETTER(WeapScale);
	OPTIONENUM_GETTER(TurnType);
	OPTIONENUM_GETTER(TeamBallance);
	OPTIONENUM_GETTER(MovementRestriction);

	OPTIONINT_GETTER(ScorePerMoney);
	OPTIONINT_GETTER(ScorePerAssist);
	OPTIONINT_GETTER(ScorePerKill);
	OPTIONINT_GETTER(ScorePerResign);
	OPTIONINT_GETTER(ScorePerSpectate);
	OPTIONINT_GETTER(ScoreWonForRound);
	OPTIONINT_GETTER(ScoreWonForLives);

	OPTIONINT_GETTER(SkillForRound);
	OPTIONINT_GETTER(SkillForMatch);
	OPTIONINT_GETTER(SkillForResign);
	OPTIONINT_GETTER(MaxSkillLost);
	OPTIONINT_GETTER(MaxSkillGained);
	OPTIONINT_GETTER(SkillForSelfKill);
	OPTIONINT_GETTER(SkillForTeamKill);

	OPTIONINT_GETTER(StartMoney);
	OPTIONINT_GETTER(Interest);
	OPTIONINT_GETTER(FreeMarketAdjustment);
	OPTIONINT_GETTER(FreeMarketLimits);
	OPTIONBOOL_GETTER(FreeMarketTracksBotPurchases);
	OPTIONINT_GETTER(BuyOnRound);
	OPTIONINT_GETTER(MoneyPerRound);
	OPTIONINT_GETTER(MoneyWonForRound);
	OPTIONINT_GETTER(MoneyWonForLives);
	OPTIONINT_GETTER(MoneyWonPerKillPoint);
	OPTIONINT_GETTER(MoneyWonPerMultiKillPoint);
	OPTIONINT_GETTER(MoneyWonPerAssistPoint);
	OPTIONINT_GETTER(MoneyWonPerHitPoint);
	OPTIONBOOL_GETTER(MoneyPerHealthPoint);
	OPTIONBOOL_GETTER(LimitPowerByHealth);
	OPTIONBOOL_GETTER(GiveAllWeapons);
	OPTIONSTRINGENUM_GETTER(Economy);
	OPTIONINT_GETTER(ResignMode);

	OPTIONINT_GETTER(ComputersDeathTalk);
	OPTIONINT_GETTER(ComputersAttackTalk);
	OPTIONSTRING_GETTER(BotNamePrefix);
	OPTIONBOOL_GETTER(RandomizeBotNames);
	OPTIONBOOL_GETTER(WaitForShotsBeforeShowingScore);

	OPTIONBOOL_GETTER(CycleMaps);
	OPTIONSTRING_GETTER(Landscapes);

	OPTIONSTRING_GETTER(StatsLogger);
	OPTIONSTRING_GETTER(ServerFileLogger);
	
	OPTIONSTRING_GETTER(Mod);
	OPTIONSTRING_GETTER(MOTD);
	OPTIONINT_GETTER(ModDownloadSpeed);
	OPTIONINT_GETTER(MaxAvatarSize);
	OPTIONSTRING_GETTER(ServerName);
	OPTIONSTRING_GETTER(ServerPassword);
	OPTIONSTRING_GETTER(PublishAddress);

	OPTIONBOOL_GETTER(AllowMultiLingualChat);
	OPTIONBOOL_GETTER(AllowMultiLingualNames);
	OPTIONBOOL_GETTER(AllowSameIP);
	OPTIONBOOL_GETTER(AllowSameUniqueId);
	OPTIONBOOL_GETTER(PublishServer);
	OPTIONBOOL_GETTER(UseUPnP);
	OPTIONBOOL_GETTER(UseUPnPLogging);
	OPTIONBOOL_GETTER(ResidualPlayers);
	OPTIONBOOL_GETTER(DelayedDefenseActivation);
	OPTIONBOOL_GETTER(AutoSendSyncCheck);
	OPTIONBOOL_GETTER(ActionSyncCheck);
	OPTIONBOOL_GETTER(WeaponSyncCheck);
	OPTIONBOOL_GETTER(AccessoryNameSyncCheck);
	OPTIONBOOL_GETTER(TargetPlacementSyncCheck);
	OPTIONBOOL_GETTER(ActionMovementSyncCheck);
	OPTIONBOOL_GETTER(ActionRandomSyncCheck);
	OPTIONBOOL_GETTER(ActionCollisionSyncCheck);

	OPTIONSTRING_GETTER(AuthHandler);
	OPTIONSTRING_GETTER(AuthHandlerParam1);
	OPTIONSTRING_GETTER(AuthHandlerParam2);
	OPTIONBOOL_GETTER(RegisteredUserNames);
	OPTIONBOOL_GETTER(DebugFeatures);

	OptionEntryString &getPlayerType(int no) { DIALOG_ASSERT(no<24); return mainOptions_.getPlayerType(no); }

	OptionsGame &getChangedOptions() { return changedOptions_; }
	OptionsGame &getMainOptions() { return mainOptions_; }
	OptionsGame &getLevelOptions() { return levelOptions_; }

	void updateLevelOptions(ScorchedContext &context, LandscapeDefinition &defn);
	void updateChangeSet();
	bool commitChanges();

protected:
	OptionsGame mainOptions_;
	OptionsGame changedOptions_;
	OptionsGame levelOptions_;

	void updateLevelOptions(std::vector<LandscapeInclude *> &options,
		std::map<std::string, OptionEntry *> &values);
};

#endif
