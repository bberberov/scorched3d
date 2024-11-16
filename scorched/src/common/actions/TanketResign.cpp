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

#include <actions/TanketResign.h>
#include <actions/Resurrection.h>
#include <engine/ScorchedContext.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <tank/TankState.h>
#include <tank/TankScore.h>
#include <tank/TankTeamScore.h>
#include <common/Defines.h>
#include <common/ChannelManager.h>
#include <common/OptionsScorched.h>
#include <events/EventController.h>
#include <lang/LangResource.h>

TanketResign::TanketResign(unsigned int playerId, fixed resignTime, bool referenced) :
	Action(referenced),
	resignTime_(resignTime),
	playerId_(playerId),
	stateChangeCount_(0)
{
}

TanketResign::~TanketResign()
{
}

void TanketResign::init()
{
	Tank *tank = context_->getTargetContainer().getTankById(playerId_);
	if (tank) stateChangeCount_ = tank->getState().getStateChangeCount();

#ifndef S3D_SERVER
	if (!context_->getServerMode())
	{
		if (tank && resignTime_ > 0)
		{
			ChannelText text("combat",
				LANG_RESOURCE_2(
					"TANK_RESIGNED_TIMED",
					"[p:{0}] resigning from round in {1} seconds", 
					tank->getTargetName(),
					resignTime_.asString()));
			ChannelManager::showText(*context_, text);
		}
	}
#endif // #ifndef S3D_SERVER	
}

void TanketResign::simulate(fixed frameTime, bool &remove)
{
	resignTime_ -= frameTime;

	if (resignTime_ <= 0)
	{
		remove = true;

		Tanket *tanket = context_->getTargetContainer().getTanketById(playerId_);
		if (tanket)
		{
			if (tanket->getType() == Target::TypeTank)
			{
				Tank *tank = (Tank *) tanket;
				if (tank->getState().getStateChangeCount() == stateChangeCount_ &&
					tank->getAlive())
				{
					// update player assists when this player resigns
					int moneyPerAssist = 
						context_->getOptionsGame().getMoneyWonPerAssistPoint() *
							5;
					int scorePerAssist = context_->getOptionsGame().getScorePerAssist();

					// Update assists
					std::set<unsigned int> &hurtBy = 
						tank->getScore().getHurtBy();
					std::set<unsigned int>::iterator itor;
					for (itor = hurtBy.begin();
						itor != hurtBy.end();
						++itor)
					{
						unsigned int hurtByPlayer = (*itor);
						Tank *hurtByTank = 
							context_->getTargetContainer().getTankById(hurtByPlayer);
						if (!hurtByTank) continue;

						// Only score when the tank does not hurt itself
						if (hurtByTank == tank) continue;

						// or a team member
						if ((context_->getOptionsGame().getTeams() > 1) &&
							(hurtByTank->getTeam() == tank->getTeam())) continue;

						// Update assist score
						hurtByTank->getScore().setAssists(
							hurtByTank->getScore().getAssists() + 1);
						hurtByTank->getScore().setMoney(
							hurtByTank->getScore().getMoney() + moneyPerAssist);
						hurtByTank->getScore().setScore(
							hurtByTank->getScore().getScore() + scorePerAssist);

						if (hurtByTank->getTeam() > 0)
						{
							context_->getTankTeamScore().addScore(
								scorePerAssist, hurtByTank->getTeam());
						}
					}

					// Update score
					int scorePerResign = context_->getOptionsGame().getScorePerResign();
					tank->getScore().setScore(
						tank->getScore().getScore() + scorePerResign);

					// Update skill
					int skillPerResign = context_->getOptionsGame().getSkillForResign();
					tank->getScore().setSkill(
						tank->getScore().getSkill() + skillPerResign);

					// tank is dead
					tank->getState().setState(TankState::sDead);

					// This tank has lost a life
					if (tank->getState().getMaxLives() > 0)
					{
						tank->getState().setLives(
							tank->getState().getLives() - 1);
					}
					Resurrection::checkResurection(context_, tank);

					context_->getEventController().tankResigned(tank);

		#ifndef S3D_SERVER
					if (!context_->getServerMode())
					{
						ChannelText text("combat",
							LANG_RESOURCE_1(
								"TANK_RESIGNED",
								"[p:{0}] resigned from round", 
								tank->getTargetName()));
						ChannelManager::showText(*context_, text);
					}
		#endif // #ifndef S3D_SERVER
				}
			}
			else
			{
				Target *target = context_->getTargetContainer().removeTarget(tanket->getPlayerId());
				delete target;
			}
		}
	}

	Action::simulate(frameTime, remove);
}

std::string TanketResign::getActionDetails()
{
	return S3D::formatStringBuffer("%u %s", playerId_, resignTime_.asQuickString());
}
