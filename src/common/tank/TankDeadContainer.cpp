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

#include <tank/TankDeadContainer.h>
#include <tank/TankState.h>
#include <tank/TankScore.h>
#include <tanket/TanketAccessories.h>
#include <net/NetBuffer.h>
#include <server/ScorchedServer.h>
#include <common/OptionsScorched.h>
#include <common/Logger.h>

TankDeadContainer::TankDeadContainer() 
{

}

TankDeadContainer::~TankDeadContainer()
{
	std::map<std::string, NetBuffer *>::iterator itor;
	for (itor = deadTanks_.begin();
		itor != deadTanks_.end();
		++itor)
	{
		delete itor->second;
	}
	deadTanks_.clear();
}

void TankDeadContainer::addDeadTank(Tank *tank, const std::string &storedName)
{
	// Find/create buffer
	NetBuffer *buffer = 0;
	std::map<std::string, NetBuffer *>::iterator finditor =
		deadTanks_.find(storedName);
	if (finditor != deadTanks_.end())
	{
		buffer = finditor->second;
	}
	else
	{
		buffer = new NetBuffer();
	}
	buffer->reset();
	
	// Write to buffer
	if (!tank->getAccessories().writeMessage(*buffer, true) ||
		!tank->getScore().writeMessage(*buffer))
	{
		Logger::log("ERROR: Failed to update residual player info (write)");
		return;
	}

	// Save buffer
	deadTanks_[storedName] = buffer;
}

void TankDeadContainer::getDeadTank(TankNewMatchSimAction *simAction, const std::string &storedName)
{
	// Get the buffer
	std::map<std::string, NetBuffer *>::iterator finditor =
		deadTanks_.find(storedName);
	if (finditor == deadTanks_.end()) return;
	NetBuffer *buffer = finditor->second;

	simAction->getScoreNetBuffer().reset();
	simAction->getScoreNetBuffer().addDataToBuffer(buffer->getBuffer(), buffer->getBufferUsed());

	deadTanks_.erase(finditor);
	delete buffer;
}

void TankDeadContainer::getDeadTank(TankAddSimAction *simAction, const std::string &storedName)
{
	// Get the buffer
	std::map<std::string, NetBuffer *>::iterator finditor =
		deadTanks_.find(storedName);
	if (finditor == deadTanks_.end()) return;
	NetBuffer *buffer = finditor->second;

	simAction->getScoreNetBuffer().reset();
	simAction->getScoreNetBuffer().addDataToBuffer(buffer->getBuffer(), buffer->getBufferUsed());
}

void TankDeadContainer::clearTanks()
{
	std::map<std::string, NetBuffer *>::iterator itor;
	for (itor = deadTanks_.begin();
		itor != deadTanks_.end();
		++itor)
	{
		NetBuffer *current = (*itor).second;
		delete current;
	}
	deadTanks_.clear();
}

