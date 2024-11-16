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

#include <target/TargetContainer.h>
#include <target/Target.h>
#include <tank/Tank.h>
#include <tank/TankState.h>
#include <tanket/Tanket.h>

TargetContainer::TargetContainer() :
	playerId_(0), 
	destinationId_(0),
	roundId_(0),
	currentPlayer_(0)
{
}

TargetContainer::~TargetContainer()
{
	std::map<unsigned int, Target *>::iterator itor;
	for (itor = targets_.begin();
		itor != targets_.end();
		++itor)
	{
		delete itor->second;
	}
	targets_.clear();
}

void TargetContainer::addTarget(Target *target)
{
	std::map<unsigned int, Target *>::iterator findItor = 
		targets_.find(target->getPlayerId());
	if (findItor != targets_.end())
	{
		Target *original = (*findItor).second;
		S3D::dialogExit("Scorched3D",
			S3D::formatStringBuffer("Duplicate target %u being added to container.\n"
			"Original :%s, this %s",
			target->getPlayerId(),
			original->getCStrName().c_str(),
			target->getCStrName().c_str()));
	}

	switch (target->getType())
	{
	case Target::TypeTank:
		tanks_[target->getPlayerId()] = (Tank *) target;
	case Target::TypeTanket:
		tankets_[target->getPlayerId()] = (Tanket *) target;
	case Target::TypeTarget:
		targets_[target->getPlayerId()] = target;
	}
}

Target *TargetContainer::removeTarget(unsigned int playerId)
{
    std::map<unsigned int, Target *>::iterator itor =
		targets_.find(playerId);
	if (itor == targets_.end()) return 0;

	Target *target = (*itor).second;
	targets_.erase(itor);

	switch (target->getType())
	{
	case Target::TypeTank:
		tanks_.erase(playerId);
		if (currentPlayer_ == target)
		{
			currentPlayer_ = 0;
		}
		// Note: No break
	case Target::TypeTanket:
		tankets_.erase(playerId);
		// Note: No break
	case Target::TypeTarget:
		break;
	}
	return target;
}

Target *TargetContainer::getTargetById(unsigned int id)
{
	std::map<unsigned int, Target *>::iterator mainitor =
		targets_.find(id);
	if (mainitor != targets_.end())
	{
		Target *target = (*mainitor).second;
		DIALOG_ASSERT(target->getPlayerId() == id);

		return target;
	}
	return 0;
}

Tanket *TargetContainer::getTanketById(unsigned int id)
{
	std::map<unsigned int, Tanket *>::iterator mainitor =
		tankets_.find(id);
	if (mainitor != tankets_.end())
	{
		Tanket *tanket = (*mainitor).second;
		DIALOG_ASSERT(tanket->getPlayerId() == id);

		return tanket;
	}
	return 0;
}

Tank *TargetContainer::getTankById(unsigned int id)
{
	std::map<unsigned int, Tank *>::iterator mainitor =
		tanks_.find(id);
	if (mainitor != tanks_.end())
	{
		Tank *tank = (*mainitor).second;
		DIALOG_ASSERT(tank->getPlayerId() == id);

		return tank;
	}
	return 0;
}

Tank *TargetContainer::getTankByName(const LangString &name)
{
	std::map<unsigned int, Tank *>::iterator mainitor;
	for (mainitor = tanks_.begin();
		mainitor != tanks_.end();
		++mainitor)
	{
		Tank *tank = (*mainitor).second;
		if (tank->getTargetName() == name) return tank;
	}
	return 0;
}

void TargetContainer::setCurrentPlayerId(unsigned int pid) 
{ 
	playerId_ = pid; 
	if (playerId_)
	{
		currentPlayer_ = getTankById(playerId_);
	}
	else 
	{
		currentPlayer_ = 0;
	}
}

int TargetContainer::teamCount()
{
	int team1 = 0;
	int team2 = 0;
	int team3 = 0;
	int team4 = 0;

	std::map<unsigned int, Tank *>::iterator mainitor;
	for (mainitor = tanks_.begin();
		mainitor != tanks_.end();
		++mainitor)
	{
		Tank *current = (*mainitor).second;
		if (current->getState().getTankPlaying() &&
			current->getState().getLives() > 0)
		{
			if (current->getTeam() == 1) team1=1;
			if (current->getTeam() == 2) team2=1;
			if (current->getTeam() == 3) team3=1;
			if (current->getTeam() == 4) team4=1;
		}
	}
	return team1 + team2 + team3 + team4;
}

int TargetContainer::aliveCount()
{
	int alive = 0;
	std::map<unsigned int, Tank *>::iterator mainitor;
	for (mainitor = tanks_.begin();
		mainitor != tanks_.end();
		++mainitor)
	{
		Tank *current = (*mainitor).second;
		if (current->getState().getTankPlaying() &&
			current->getState().getLives() > 0)
		{
			alive++;
		}
	}
	return alive;
}

int TargetContainer::getNoOfNonSpectatorTanks()
{
	int count = 0;
	std::map<unsigned int, Tank *>::iterator mainitor;
	for (mainitor = tanks_.begin();
		mainitor != tanks_.end();
		++mainitor)
	{
		Tank *current = (*mainitor).second;
		if (current->getState().getTankPlaying()) count++;
	}
	return count;
}
