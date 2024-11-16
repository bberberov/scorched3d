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

#if !defined(__INCLUDE_OptionsGameh_INCLUDE__)
#define __INCLUDE_OptionsGameh_INCLUDE__

#include <common/OptionEntry.h>

class OptionsGame
{
public:
	OptionsGame();
	virtual ~OptionsGame();

	enum ScoreType
	{
		ScoreWins = 0,
		ScoreKills = 1,
		ScoreMoney = 2
	};
	enum TurnType
	{
		TurnSimultaneous = 0,
		TurnSequentialLoserFirst = 1,
		TurnSequentialRandom = 2,
		TurnSimultaneousNoWait = 3,
		TurnFree = 4,
		TurnFreeTimed = 5
	};
	enum WindForce
	{
		WindRandom = 0,
		WindNone = 1,
		Wind1 = 2,
		Wind2 = 3,
		Wind3 = 4,
		Wind4 = 5,
		Wind5 = 6,
		WindBreezy = 7,
		WindGale = 8
	};
	enum WindType
	{
		WindChangeNever = 0,
		WindChangeSomeTimes = 1,
		WindChangeFrequently = 2,
		WindChangeConstantly = 3,
		WindChangeAlways = 4
	};
	enum WallType
	{
		WallRandom = 0,
		WallConcrete = 1,
		WallBouncy = 2,
		WallWrapAround = 3,
		WallNone = 4,
		WallActive = 5,
		WallInactive = 6
	};
	enum WeapScale
	{
		ScaleSmall = 0,
		ScaleMedium = 1,
		ScaleLarge = 2
	};
	enum ResignType
	{
		ResignStart = 0,
		ResignTimed = 1,
		ResignNone = 2
	};
	enum MovementRestrictionType
	{
		MovementRestrictionNone = 0,
		MovementRestrictionLand = 1,
		MovementRestrictionLandOrAbove = 2
	};
	enum TeamBallanceType
	{
		TeamBallanceNone = 0,
		TeamBallanceAuto = 1,
		TeamBallanceBotsVs = 2,
		TeamBallanceAutoByScore = 3,
		TeamBallanceAutoByBots = 4
	};
	
	const char *getTutorial() { return tutorial_; }
	OptionEntryString &getTutorialEntry() { return tutorial_; }

	int getScorePerMoney() { return scorePerMoney_; }
	OptionEntryInt &getScorePerMoneyEntry() { return scorePerMoney_; }

	int getScorePerAssist() { return scorePerAssist_; }
	OptionEntryInt &getScorePerAssistEntry() { return scorePerAssist_; }

	int getScorePerKill() { return scorePerKill_; }
	OptionEntryInt &getScorePerKillEntry() { return scorePerKill_; }

	int getScorePerResign() { return scorePerResign_; }
	OptionEntryInt &getScorePerResignEntry() { return scorePerResign_; }

	int getScorePerSpectate() { return scorePerSpectate_; }
	OptionEntryInt &getScorePerSpectateEntry() { return scorePerSpectate_; }

	int getScoreWonForRound() { return scoreWonForRound_; }
	OptionEntryInt &getScoreWonForRoundEntry() { return scoreWonForRound_; }

	int getScoreWonForLives() { return scoreWonForLives_; }
	OptionEntryInt &getScoreWonForLivesEntry() { return scoreWonForLives_; }

	int getSkillForRound() { return skillForRound_; }
	OptionEntryInt &getSkillForRoundEntry() { return skillForRound_; }

	int getSkillForMatch() { return skillForMatch_; }
	OptionEntryInt &getSkillForMatchEntry() { return skillForMatch_; }

	int getSkillForResign() { return skillForResign_; }
	OptionEntryInt &getSkillForResignEntry() { return skillForResign_; }

	int getMaxSkillLost() { return maxSkillLost_; }
	OptionEntryInt &getMaxSkillLostEntry() { return maxSkillLost_; }

	int getMaxSkillGained() { return maxSkillGained_; }
	OptionEntryInt &getMaxSkillGainedEntry() { return maxSkillGained_; }

	int getSkillForSelfKill() { return skillForSelfKill_; }
	OptionEntryInt &getSkillForSelfKillEntry() { return skillForSelfKill_; }

	int getSkillForTeamKill() { return skillForTeamKill_; }
	OptionEntryInt &getSkillForTeamKillEntry() { return skillForTeamKill_; }

	int getTeams() { return teams_; }
	OptionEntryInt &getTeamsEntry() { return teams_; }

	int getMinimumLandHeight() { return minimumLandHeight_; }
	OptionEntryInt &getMinimumLandHeightEntry() { return minimumLandHeight_; }

	int getWeaponSpeed() { return weaponSpeed_; }
	OptionEntryInt &getWeaponSpeedEntry() { return weaponSpeed_; }

	int getStartArmsLevel() { return startArmsLevel_; }
	OptionEntryInt &getStartArmsLevelEntry() { return startArmsLevel_; }

	int getEndArmsLevel() { return endArmsLevel_; }
	OptionEntryInt &getEndArmsLevelEntry() { return endArmsLevel_; }

	int getMaxNumberWeapons() { return maxNumberWeapons_; }
	OptionEntryInt &getMaxNumberWeaponsEntry() { return maxNumberWeapons_; }

	int getNoMaxPlayers() { return numberOfPlayers_; }
	OptionEntryInt &getNoMaxPlayersEntry() { return numberOfPlayers_; }

	int getNoMinPlayers() { return numberOfMinPlayers_; }
	OptionEntryInt &getNoMinPlayersEntry() { return numberOfMinPlayers_; }

	int getNoRounds() { return numberOfRounds_; }
	OptionEntryInt &getNoRoundsEntry() { return numberOfRounds_; }

	int getNoTurns() { return maxRoundTurns_; }
	OptionEntryInt &getNoTurnsEntry() { return maxRoundTurns_; }

	int getRemoveBotsAtPlayers() { return removeBotsAtPlayers_; }
	OptionEntryInt &getRemoveBotsAtPlayersEntry() { return removeBotsAtPlayers_; }

	int getStartMoney() { return moneyStarting_; }
	OptionEntryInt &getStartMoneyEntry() { return moneyStarting_; }

	int getInterest() { return moneyInterest_; }
	OptionEntryInt &getInterestEntry() { return moneyInterest_; }

	int getFreeMarketAdjustment() { return freeMarketAdjustment_; }
	OptionEntryInt &getFreeMarketAdjustmentEntry() { return freeMarketAdjustment_; }

	int getFreeMarketLimits() { return freeMarketLimits_; }
	OptionEntryInt &getFreeMarketLimitsEntry() { return freeMarketLimits_; }

	bool getFreeMarketTracksBotPurchases() { return freeMarketTracksBotPurchases_; }
	OptionEntryBool &getFreeMarketTracksBotPurchasesEntry() { return freeMarketTracksBotPurchases_; }

	int getMaxLandscapeSize() { return maxLandscapeSize_; }
	OptionEntryInt &getMaxLandscapeSizeEntry() { return maxLandscapeSize_; }

	int getStartTime() { return startTime_; }
	OptionEntryInt &getStartTimeEntry() { return startTime_; }

	int getShotTime() { return shotTime_; }
	OptionEntryInt &getShotTimeEntry() { return shotTime_; }

	int getAIShotTime() { return aiShotTime_; }
	OptionEntryInt &getAIShotTimeEntry() { return aiShotTime_; }

	int getBuyingTime() { return buyingTime_; }
	OptionEntryInt &getBuyingTimeEntry() { return buyingTime_; }

	int getRemoveTime() { return removeTime_; }
	OptionEntryInt &getRemoveTimeEntry() { return removeTime_; }

	int getRoundTime() { return roundTime_; }
	OptionEntryInt &getRoundTimeEntry() { return roundTime_; }

	int getRoundScoreTime() { return roundScoreTime_; }
	OptionEntryInt &getRoundScoreTimeEntry() { return roundScoreTime_; }

	int getScoreTime() { return scoreTime_; }
	OptionEntryInt &getScoreTimeEntry() { return scoreTime_; }

	int getIdleCycleTime() { return idleCycleTime_; }
	OptionEntryInt &getIdleCycleTimeEntry() { return idleCycleTime_; }

	int getAllowedMissedMoves() { return allowedMissedMoves_; }
	OptionEntryInt &getAllowedMissedMovesEntry() { return allowedMissedMoves_; }

	int getMinFallingDistance() { return minFallingDistance_; }
	OptionEntryInt &getMinFallingDistanceEntry() { return minFallingDistance_; }

	bool getTankFallingDamage() { return tankFallingDamage_; }
	OptionEntryBool &getTankFallingDamageEntry() { return tankFallingDamage_; }

	int getMaxClimbingDistance() { return maxClimbingDistance_; }
	OptionEntryInt &getMaxClimbingDistanceEntry() { return maxClimbingDistance_; }

	int getPlayerLives() { return playerLives_; }
	OptionEntryInt &getPlayerLivesEntry() { return playerLives_; }

	int getGravity() { return gravity_; }
	OptionEntryInt &getGravityEntry() { return gravity_; }

	OptionEntryEnum getWindForce() { return windForce_; } // WindForce
	OptionEntryEnum &getWindForceEntry() { return windForce_; } // WindForce

	OptionEntryEnum getWindType() { return windType_; } // WindType
	OptionEntryEnum &getWindTypeEntry() { return windType_; } // WindType

	OptionEntryEnum getWallType() { return wallType_; } // WallType
	OptionEntryEnum &getWallTypeEntry() { return wallType_; } // WallType

	OptionEntryEnum getWeapScale() { return weapScale_; } // WeapScale
	OptionEntryEnum &getWeapScaleEntry() { return weapScale_; } // WeapScale
	
	OptionEntryEnum getTurnType() { return turnType_; } // TurnType
	OptionEntryEnum &getTurnTypeEntry() { return turnType_; } // TurnType
	 
	int getBuyOnRound() { return moneyBuyOnRound_; }
	OptionEntryInt &getBuyOnRoundEntry() { return moneyBuyOnRound_; }

	int getMoneyPerRound() { return moneyPerRound_; }
	OptionEntryInt &getMoneyPerRoundEntry() { return moneyPerRound_; }

	int getMoneyWonForRound() { return moneyWonForRound_; }
	OptionEntryInt &getMoneyWonForRoundEntry() { return moneyWonForRound_; }

	int getMoneyWonForLives() { return moneyWonForLives_; }
	OptionEntryInt &getMoneyWonForLivesEntry() { return moneyWonForLives_; }

	int getMoneyWonPerKillPoint() { return moneyPerKillPoint_; }
	OptionEntryInt &getMoneyWonPerKillPointEntry() { return moneyPerKillPoint_; }

	int getMoneyWonPerMultiKillPoint() { return moneyPerMultiKillPoint_; }
	OptionEntryInt &getMoneyWonPerMultiKillPointEntry() { return moneyPerMultiKillPoint_; }

	int getMoneyWonPerAssistPoint() { return moneyPerAssistPoint_; }
	OptionEntryInt &getMoneyWonPerAssistPointEntry() { return moneyPerAssistPoint_; }

	int getMoneyWonPerHitPoint() { return moneyPerHitPoint_; }
	OptionEntryInt &getMoneyWonPerHitPointEntry() { return moneyPerHitPoint_; }

	bool getMoneyPerHealthPoint() { return moneyPerHealthPoint_; }
	OptionEntryBool &getMoneyPerHealthPointEntry() { return moneyPerHealthPoint_; }

	bool getLimitPowerByHealth() { return limitPowerByHealth_; }
	OptionEntryBool &getLimitPowerByHealthEntry() { return limitPowerByHealth_; }
	
	OptionEntryEnum getTeamBallance() { return teamBallance_; } // TeamBallanceType
	OptionEntryEnum &getTeamBallanceEntry() { return teamBallance_; } // TeamBallanceType

	int getComputersDeathTalk() { return computersDeathTalk_; }
	OptionEntryInt &getComputersDeathTalkEntry() { return computersDeathTalk_; }

	int getComputersAttackTalk() { return computersAttackTalk_; }
	OptionEntryInt &getComputersAttackTalkEntry() { return computersAttackTalk_; }

	const char * getEconomy() { return economy_; }
	OptionEntryStringEnum &getEconomyEntry() { return economy_; }

	const char * getLandscapes() { return landscapes_; }
	OptionEntryString &getLandscapesEntry() { return landscapes_; }

	const char * getStatsLogger() { return statsLogger_; }
	OptionEntryStringEnum &getStatsLoggerEntry() { return statsLogger_; }

	const char * getServerFileLogger() { return serverFileLogger_; }
	OptionEntryStringEnum &getServerFileLoggerEntry() { return serverFileLogger_; }

	const char * getBotNamePrefix() { return botNamePrefix_; }
	OptionEntryString &getBotNamePrefixEntry() { return botNamePrefix_; }

	bool getGiveAllWeapons() { return giveAllWeapons_; }
	OptionEntryBool &getGiveAllWeaponsEntry() { return giveAllWeapons_; }

	bool getCycleMaps() { return cycleMaps_; }
	OptionEntryBool &getCycleMapsEntry() { return cycleMaps_; }

	int getResignMode() { return resignMode_; }
	OptionEntryInt &getResignModeEntry() { return resignMode_; }

	OptionEntryEnum getMovementRestriction() { return movementRestriction_; } // MovementRestrictionType
	OptionEntryEnum &getMovementRestrictionEntry() { return movementRestriction_; } // MovementRestrictionType

	bool getRandomizeBotNames() { return randomizeBotNames_; }
	OptionEntryBool &getRandomizeBotNamesEntry() { return randomizeBotNames_; }

	int getPortNo() { return portNo_; }
	OptionEntryInt &getPortNoEntry() { return portNo_; }

	int getManagementPortNo() { return managementPortNo_; }
	OptionEntryInt &getManagementPortNoEntry() { return managementPortNo_; }

	const char * getMod() { return mod_; }
	OptionEntryString &getModEntry() { return mod_; }

	const char * getMOTD() { return motd_; }
	OptionEntryString &getMOTDEntry() { return motd_; }

	int getModDownloadSpeed() { return modDownloadSpeed_; }
	OptionEntryInt &getModDownloadSpeedEntry() { return modDownloadSpeed_; }

	int getMaxAvatarSize() { return maxAvatarSize_; }
	OptionEntryInt &getMaxAvatarSizeEntry() { return maxAvatarSize_; }

	const char * getServerName() { return serverName_; }
	OptionEntryString &getServerNameEntry() { return serverName_; }

	const char * getServerPassword() { return serverPassword_; }
	OptionEntryString &getServerPasswordEntry() { return serverPassword_; }

	OptionEntryString &getPlayerType(int no) { DIALOG_ASSERT(no<24); return *playerType_[no]; }

	const char * getPublishAddress() { return publishAddress_; }
	OptionEntryString &getPublishAddressEntry() { return publishAddress_; }

	bool getAllowSameIP() { return allowSameIP_; }
	OptionEntryBool &getAllowSameIPEntry() { return allowSameIP_; }

	bool getAllowSameUniqueId() { return allowSameUniqueId_; }
	OptionEntryBool &getAllowSameUniqueIdEntry() { return allowSameUniqueId_; }

	bool getPublishServer() { return publishServer_; }
	OptionEntryBool &getPublishServerEntry() { return publishServer_; }

	bool getUseUPnP() { return useUPnP_; }
	OptionEntryBool &getUseUPnPEntry() { return useUPnP_; }

	bool getUseUPnPLogging() { return useUPnPLogging_; }
	OptionEntryBool &getUseUPnPLoggingEntry() { return useUPnPLogging_; }

	bool getResidualPlayers() { return residualPlayers_; }
	OptionEntryBool &getResidualPlayersEntry() { return residualPlayers_; }

	bool getDelayedDefenseActivation() { return delayedDefenseActivation_; }
	OptionEntryBool &getDelayedDefenseActivationEntry() { return delayedDefenseActivation_; }

	bool getAutoSendSyncCheck() { return autoSendSyncCheck_; }
	OptionEntryBool &getAutoSendSyncCheckEntry() { return autoSendSyncCheck_; }

	bool getWaitForShotsBeforeShowingScore() { return waitForShotsBeforeShowingScore_; }
	OptionEntryBool &getWaitForShotsBeforeShowingScoreEntry() { return waitForShotsBeforeShowingScore_; }

	bool getActionSyncCheck() { return actionSyncCheck_; }
	OptionEntryBool &getActionSyncCheckEntry() { return actionSyncCheck_; }

	bool getActionRandomSyncCheck() { return actionRandomSyncCheck_; }
	OptionEntryBool &getActionRandomSyncCheckEntry() { return actionRandomSyncCheck_; }

	bool getActionMovementSyncCheck() { return actionMovementSyncCheck_; }
	OptionEntryBool &getActionMovementSyncCheckEntry() { return actionMovementSyncCheck_; }

	bool getActionCollisionSyncCheck() { return actionCollisionSyncCheck_; }
	OptionEntryBool &getActionCollisionSyncCheckEntry() { return actionCollisionSyncCheck_; }

	bool getTargetPlacementSyncCheck() { return targetPlacementSyncCheck_; }
	OptionEntryBool &getTargetPlacementSyncCheckEntry() { return targetPlacementSyncCheck_; }

	bool getWeaponSyncCheck() { return weaponSyncCheck_; }
	OptionEntryBool &getWeaponSyncCheckEntry() { return weaponSyncCheck_; }

	bool getAccessoryNameSyncCheck() { return accessoryNameSyncCheck_; }
	OptionEntryBool &getAccessoryNameSyncCheckEntry() { return accessoryNameSyncCheck_; }

	const char *getAuthHandler() { return authHandler_; }
	OptionEntryStringEnum &getAuthHandlerEntry() { return authHandler_; }

	const char *getAuthHandlerParam1() { return authHandlerParam1_; }
	OptionEntryString &getAuthHandlerParam1Entry() { return authHandlerParam1_; }

	const char *getAuthHandlerParam2() { return authHandlerParam2_; }
	OptionEntryString &getAuthHandlerParam2Entry() { return authHandlerParam2_; }

	bool getRegisteredUserNames() { return registeredUserNames_; }
	OptionEntryBool &getRegisteredUserNamesEntry() { return registeredUserNames_; }

	bool getAllowMultiLingualChat() { return allowMultiLingualChat_; }
	OptionEntryBool &getAllowMultiLingualChatEntry() { return allowMultiLingualChat_; }

	bool getAllowMultiLingualNames() { return allowMultiLingualNames_; }
	OptionEntryBool &getAllowMultiLingualNamesEntry() { return allowMultiLingualNames_; }

	bool getDebugFeatures() { return debugFeatures_; }
	OptionEntryBool &getDebugFeaturesEntry() { return debugFeatures_; }

	// Fns used to save or restore the state of the options
	std::list<OptionEntry *> &getOptions();
	std::list<OptionEntry *> &getPlayerTypeOptions(); 
	virtual bool writeOptionsToFile(const std::string &filePath, bool allOptions);
	virtual bool readOptionsFromFile(const std::string &filePath);
	virtual bool writeToBuffer(NetBuffer &buffer, bool useProtected, bool usePlayerTypes);
	virtual bool readFromBuffer(NetBufferReader &reader, bool useProtected, bool usePlayerTypes);

protected:
	std::list<OptionEntry *> options_;
	std::list<OptionEntry *> playerTypeOptions_;

	OptionEntryBoundedInt weaponSpeed_;
	OptionEntryBoundedInt startArmsLevel_;
	OptionEntryBoundedInt endArmsLevel_;
	OptionEntryBoundedInt shotTime_;
	OptionEntryBoundedInt aiShotTime_;
	OptionEntryBoundedInt startTime_;
	OptionEntryBoundedInt buyingTime_;
	OptionEntryBoundedInt removeTime_;
	OptionEntryBoundedInt roundTime_;
	OptionEntryBoundedInt roundScoreTime_;
	OptionEntryBoundedInt scoreTime_;
	OptionEntryBoundedInt idleCycleTime_;
	OptionEntryBoundedInt allowedMissedMoves_;
	OptionEntryBoundedInt numberOfRounds_;
	OptionEntryBoundedInt maxRoundTurns_;
	OptionEntryBoundedInt maxNumberWeapons_;
	OptionEntryBoundedInt gravity_;
	OptionEntryBoundedInt minFallingDistance_;
	OptionEntryBool tankFallingDamage_;
	OptionEntryBoundedInt maxClimbingDistance_;
	OptionEntryBoundedInt playerLives_;
	OptionEntryBoundedInt teams_;
	OptionEntryBoundedInt numberOfPlayers_;
	OptionEntryBoundedInt numberOfMinPlayers_;
	OptionEntryBoundedInt removeBotsAtPlayers_;
	OptionEntryBoundedInt computersDeathTalk_;
	OptionEntryBoundedInt computersAttackTalk_;
	OptionEntryBoundedInt moneyBuyOnRound_;
	OptionEntryBoundedInt moneyWonForRound_;
	OptionEntryBoundedInt moneyWonForLives_;
	OptionEntryBoundedInt moneyPerKillPoint_;
	OptionEntryBoundedInt moneyPerMultiKillPoint_;
	OptionEntryBoundedInt moneyPerAssistPoint_;
	OptionEntryBoundedInt moneyPerHitPoint_;
	OptionEntryBoundedInt moneyPerRound_;
	OptionEntryBool moneyPerHealthPoint_;
	OptionEntryBoundedInt scorePerMoney_;
	OptionEntryBoundedInt scorePerAssist_;
	OptionEntryBoundedInt scorePerKill_;
	OptionEntryBoundedInt scorePerResign_;
	OptionEntryBoundedInt scorePerSpectate_;
	OptionEntryBoundedInt scoreWonForRound_;
	OptionEntryBoundedInt scoreWonForLives_;
	OptionEntryInt maxLandscapeSize_;
	OptionEntryInt freeMarketAdjustment_;
	OptionEntryInt freeMarketLimits_;
	OptionEntryBool freeMarketTracksBotPurchases_;
	OptionEntryInt minimumLandHeight_;
	OptionEntryBoundedInt skillForRound_;
	OptionEntryBoundedInt skillForMatch_;
	OptionEntryBoundedInt skillForResign_;
	OptionEntryBoundedInt maxSkillLost_;
	OptionEntryBoundedInt maxSkillGained_;
	OptionEntryBoundedInt skillForSelfKill_;
	OptionEntryBoundedInt skillForTeamKill_;
	OptionEntryBoundedInt moneyStarting_;
	OptionEntryEnum teamBallance_;
	OptionEntryBoundedInt moneyInterest_;
	OptionEntryBool limitPowerByHealth_;
	OptionEntryString tutorial_;
	OptionEntryBool cycleMaps_;
	OptionEntryEnum resignMode_;
	OptionEntryEnum movementRestriction_;
	OptionEntryEnum turnType_;
	OptionEntryEnum windForce_;
	OptionEntryEnum windType_;
	OptionEntryEnum wallType_;
	OptionEntryEnum weapScale_;
	OptionEntryBoundedInt modDownloadSpeed_;
	OptionEntryInt maxAvatarSize_;
	OptionEntryString mod_;
	OptionEntryString motd_;
	OptionEntryStringEnum economy_;
	OptionEntryString landscapes_;
	OptionEntryStringEnum statsLogger_;
	OptionEntryStringEnum serverFileLogger_;
	OptionEntryBool waitForShotsBeforeShowingScore_;

	// Server only options
	OptionEntryString botNamePrefix_;
	OptionEntryBool actionSyncCheck_;
	OptionEntryBool actionMovementSyncCheck_;
	OptionEntryBool actionRandomSyncCheck_;
	OptionEntryBool actionCollisionSyncCheck_;
	OptionEntryBool targetPlacementSyncCheck_;
	OptionEntryBool weaponSyncCheck_;
	OptionEntryBool accessoryNameSyncCheck_;
	OptionEntryBool autoSendSyncCheck_;
	OptionEntryBool residualPlayers_;
	OptionEntryBool delayedDefenseActivation_;
	OptionEntryBool randomizeBotNames_;
	OptionEntryBool giveAllWeapons_;
	OptionEntryBool registeredUserNames_;
	OptionEntryBool allowMultiLingualChat_;
	OptionEntryBool allowMultiLingualNames_;
	OptionEntryStringEnum authHandler_;
	OptionEntryString authHandlerParam1_;
	OptionEntryString authHandlerParam2_;
	OptionEntryString serverName_;
	OptionEntryString *playerType_[24];
	OptionEntryString serverPassword_;
	OptionEntryInt portNo_;
	OptionEntryInt managementPortNo_;
	OptionEntryString publishAddress_;
	OptionEntryBool publishServer_;
	OptionEntryBool useUPnP_;
	OptionEntryBool useUPnPLogging_;
	OptionEntryBool allowSameIP_;
	OptionEntryBool allowSameUniqueId_;
	OptionEntryBool debugFeatures_;

	// Depricated (old) 
	OptionEntryBoundedInt depricatedIdleKickTime_;
	OptionEntryBoundedInt depricatedIdleShotKickTime_;
	OptionEntryBoundedInt depricatedKeepAliveTime_;
	OptionEntryBoundedInt depricatedKeepAliveTimeoutTime_;
	OptionEntryEnum depricatedScoreType_;
	OptionEntryBool depricatedAutoBallanceTeams_;
	OptionEntryBoundedInt depricatedMaxArmsLevel_;
	OptionEntryString depricatedServerAdminPassword_;
	OptionEntryString depricatedMasterListServer_;
	OptionEntryString depricatedMasterListServerURI_;

};

#endif
