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

#include <engine/SaveGame.h>
#include <common/Logger.h>
#include <common/OptionsTransient.h>
#include <server/ScorchedServer.h>
#include <server/ServerSimulator.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <tank/TankState.h>

bool SaveGame::saveFile(const std::string &fileName) 
{
	// Create the save message
	ComsLoadLevelMessage &message = 
		ScorchedServer::instance()->getServerSimulator().getLevelMessage();
	NetBuffer saveBuffer;
	saveBuffer.addToBuffer(S3D::ScorchedProtocolVersion);
	if (!message.writeMessage(saveBuffer)) return false;

	// Save the message
	FILE *out = fopen(fileName.c_str(), "wb");
	if (out == 0) return false;
	if (fwrite(saveBuffer.getBuffer(), 1, saveBuffer.getBufferUsed(), out) != 
		saveBuffer.getBufferUsed())
	{
		fclose(out);
		return false;
	}
	fclose(out);

	return true;
}

bool SaveGame::loadFile(const std::string &fileName, ComsLoadLevelMessage &message)
{
	NetBuffer loadBuffer;
	FILE *in = fopen(fileName.c_str(), "rb");
	if (!in)
	{
		Logger::log("Failed to open file");
		return false;
	}
	unsigned char readBuf[512];
	while (unsigned int size = (unsigned int) fread(readBuf, sizeof(unsigned char), 512, in))
	{
		loadBuffer.addDataToBuffer(readBuf, size);
	}
	fclose(in);

	NetBufferReader reader(loadBuffer);
	std::string version;
	if (!reader.getFromBuffer(version)) 
	{
		Logger::log("Failed to read version");
		return false;
	}
	if (version != S3D::ScorchedProtocolVersion)
	{
		Logger::log("Cannot load a saved game from a previous version of Scorched3D");
		return false;
	}
	if (!message.readMessage(reader)) 
	{
		Logger::log("Failed to read saved game message");
		return false;
	}

	return true;
}

bool SaveGame::loadState(const std::string &fileName)
{
	ComsLoadLevelMessage message;
	if (!loadFile(fileName, message)) return false;

	if (!message.loadState(ScorchedServer::instance()->getContext(), false)) return false;
	return true;
}

bool SaveGame::loadTargets(const std::string &fileName)
{
	ComsLoadLevelMessage message;
	if (!loadFile(fileName, message)) return false;

	if (!message.loadState(ScorchedServer::instance()->getContext())) return false;
	if (!message.loadTanks(ScorchedServer::instance()->getContext())) return false;

	std::map<unsigned int, Tank *> tanks = 
		ScorchedServer::instance()->getTargetContainer().getTanks();
	std::map<unsigned int, Tank *>::iterator itor;
	for (itor = tanks.begin();
		itor != tanks.end();
		++itor)
	{
		Tank *tank = (*itor).second;
		if (tank->getDestinationId() == 0)
		{
			tank->getState().setState(TankState::sDead);
		}
	}

	OptionsTransient &options = ScorchedServer::instance()->getOptionsTransient();
	options.setCurrentRoundNo(options.getCurrentRoundNo() - 1);

	return true;
}
