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

#include <simactions/TankTeamBallanceSimAction.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <tank/TankScore.h>
#include <tank/TankState.h>
#include <tank/TankModelStore.h>
#include <tank/TankColorGenerator.h>
#include <tank/TankModelContainer.h>
#include <tanket/TanketType.h>
#include <engine/Simulator.h>
#include <common/OptionsScorched.h>
#include <common/ChannelManager.h>
#include <algorithm>

static inline bool lt_score(const Tank *o1, const Tank *o2) 
{ 
	return ((Tank*)o1)->getScore().getScore() > ((Tank *)o2)->getScore().getScore();
}

static inline bool lt_bots(const Tank *o1, const Tank *o2) 
{ 
	return ((Tank*)o1)->getDestinationId() > ((Tank *)o2)->getDestinationId();
}

REGISTER_CLASS_SOURCE(TankTeamBallanceSimAction);

TankTeamBallanceSimAction::TankTeamBallanceSimAction()
{
}

TankTeamBallanceSimAction::~TankTeamBallanceSimAction()
{
}

bool TankTeamBallanceSimAction::invokeAction(ScorchedContext &context)
{
	// Make sure everyone is in a team if they should be
	std::map<unsigned int, Tank *> &playingTanks = 
		context.getTargetContainer().getTanks();
	std::map<unsigned int, Tank *>::iterator mainitor;
	for (mainitor = playingTanks.begin();
		 mainitor != playingTanks.end();
		 ++mainitor)
	{
		Tank *current = (*mainitor).second;
		if (current->getState().getTankPlaying())
		{
			if (context.getOptionsGame().getTeams() > 1 &&
				current->getTeam() == 0) 
			{
				current->setTeam(1); 
			}
			if (context.getOptionsGame().getTeams() == 1 &&
				current->getTeam() > 0) 
			{
				current->setTeam(0); 
			}
		}
	}

	// Do we check teams ballance
	if (context.getOptionsGame().getTeams() != 1)
	{
		// Check for team ballance types
		switch (context.getOptionsGame().getTeamBallance())
		{
			case OptionsGame::TeamBallanceAuto:
			case OptionsGame::TeamBallanceAutoByScore:
			case OptionsGame::TeamBallanceAutoByBots:
				checkTeamsAuto(context);
				break;
			case OptionsGame::TeamBallanceBotsVs:
				checkTeamsBotsVs(context);
				break;
			default:
				break;
		}
	}

	// Make sure everyone is using a team model
	for (mainitor = playingTanks.begin();
		 mainitor != playingTanks.end();
		 ++mainitor)
	{
		Tank *current = (*mainitor).second;
		if (current->getState().getTankPlaying() &&
			current->getModelContainer().getTankModel())
		{
			// Check if this model is allowed for this team
			if (!current->getModelContainer().getTankModel()->availableForTank(current))
			{
				TankModel *model = context.getTankModels().getRandomModel(
					current->getTeam(), current->getDestinationId() == 0,
					current->getTanketType()->getName());
				current->getModelContainer().setTankModelName(model->getName());
			}
		}
	}

	return true;
}

bool TankTeamBallanceSimAction::writeMessage(NetBuffer &buffer)
{
	return true;
}
bool TankTeamBallanceSimAction::readMessage(NetBufferReader &reader)
{
	return true;
}

void TankTeamBallanceSimAction::minMaxTeams(ScorchedContext &context,
	std::vector<Tank *> *teamPlayers,
	std::vector<Tank *> *&minPlayers,
	std::vector<Tank *> *&maxPlayers)
{
	// Clear all team lists
	for (int i=0; i<context.getOptionsGame().getTeams(); i++)
	{
		teamPlayers[i].clear();
	}

	// Count players in each team
	std::map<unsigned int, Tank *> &playingTanks = 
		context.getTargetContainer().getTanks();
	std::map<unsigned int, Tank *>::iterator mainitor;
	for (mainitor = playingTanks.begin();
		 mainitor != playingTanks.end();
		 ++mainitor)
	{
		Tank *current = (*mainitor).second;
		if (current->getState().getTankPlaying())
		{
			if (current->getTeam() > 0)
			{
				teamPlayers[current->getTeam() - 1].push_back(current);
			}
		}
	}

	// Find the teams with the min and max players in them
	minPlayers = &teamPlayers[0];
	maxPlayers = &teamPlayers[0];
	for (int i=0; i<context.getOptionsGame().getTeams(); i++)
	{
		if (teamPlayers[i].size() < minPlayers->size()) minPlayers = &teamPlayers[i];
		if (teamPlayers[i].size() > maxPlayers->size()) maxPlayers = &teamPlayers[i];			
	}
}

void TankTeamBallanceSimAction::checkTeamsAuto(ScorchedContext &context)
{
	// Find the teams with the min and max players in them
	std::vector<Tank *> teamPlayers[4], *minPlayers = 0, *maxPlayers = 0;

	// Is the difference between the min and max teams >= 2 players
	for (minMaxTeams(context, teamPlayers, minPlayers, maxPlayers);
		maxPlayers->size() - minPlayers->size() >= 2;
		minMaxTeams(context, teamPlayers, minPlayers, maxPlayers))
	{
		// Sort teams (if needed)
		if (context.getOptionsGame().getTeamBallance() ==
			OptionsGame::TeamBallanceAutoByScore)
		{
			std::sort(minPlayers->begin(), minPlayers->end(), lt_score); 
			std::sort(maxPlayers->begin(), maxPlayers->end(), lt_score); 
		}
		else if (context.getOptionsGame().getTeamBallance() ==
			OptionsGame::TeamBallanceAutoByBots)
		{
			std::sort(minPlayers->begin(), minPlayers->end(), lt_bots); 
			std::sort(maxPlayers->begin(), maxPlayers->end(), lt_bots); 
		}
	
		// Find out which team has the least players
		for (int i=0; i<context.getOptionsGame().getTeams(); i++)
		{	
			if (minPlayers == &teamPlayers[i])
			{
				// Ballance the teams
				Tank *tank = maxPlayers->back();
				setTeam(context, tank, i+1);
			}
		}
	}
}

void TankTeamBallanceSimAction::checkTeamsBotsVs(ScorchedContext &context)
{
	std::map<unsigned int, Tank *> &playingTanks = 
		context.getTargetContainer().getTanks();
	std::map<unsigned int, Tank *>::iterator mainitor;
	for (mainitor = playingTanks.begin();
		 mainitor != playingTanks.end();
		 ++mainitor)
	{
		Tank *current = (*mainitor).second;
		if (current->getState().getTankPlaying())
		{
			if (current->getDestinationId() == 0) setTeam(context, current, 1);
			else setTeam(context, current, 2);
		}
	}
}

void TankTeamBallanceSimAction::setTeam(ScorchedContext &context, Tank *tank, unsigned int team)
{
	if (tank->getTeam() != team)
	{
		// Tell people whats going on
		ChannelText text("info",
			"MOVING_PLAYER",
			"Moving player [p:{0}] to team {1}",
			tank->getTargetName(), 
			TankColorGenerator::getTeamName(team));
		ChannelManager::showText(context, text);

		tank->setTeam(team);
	}
}
