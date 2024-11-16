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

#include <server/ServerHaveModFilesHandler.h>
#include <server/ServerChannelManager.h>
#include <server/ServerDestinations.h>
#include <server/ScorchedServer.h>
#include <server/ServerCommon.h>
#include <engine/ModFiles.h>
#include <coms/ComsHaveModFilesMessage.h>
#include <common/OptionsScorched.h>
#include <common/Defines.h>
#include <common/Logger.h>
#include <time.h>

ServerHaveModFilesHandler::ServerHaveModFilesHandler(ComsMessageHandler &comsMessageHandler)
{
	comsMessageHandler.addHandler(
		ComsHaveModFilesMessage::ComsHaveModFilesMessageType,
		this);
}

ServerHaveModFilesHandler::~ServerHaveModFilesHandler()
{
}

bool ServerHaveModFilesHandler::processMessage(
	NetMessage &netMessage,
	const char *messageType,
	NetBufferReader &reader)
{
	ComsHaveModFilesMessage message;
	if (!message.readMessage(reader)) return false;

	unsigned int destinationId = netMessage.getDestinationId();

	std::list<ModIdentifierEntry> neededEntries;
	unsigned int neededLength = 0;

	// Build up a list of all of the files this client has
	std::map<std::string, ModIdentifierEntry*> hasEntries;
	{
		std::list<ModIdentifierEntry>::iterator hasItor;
		for (hasItor = message.getFiles().begin();
			hasItor != message.getFiles().end();
			++hasItor)
		{
			ModIdentifierEntry *entry = &(*hasItor);
			hasEntries[entry->fileName] = entry;
		}
	}

	// Build up a list of mod files that this client needs
	// and does not already have
	{
		std::map<std::string, ModFileEntry *> &modFiles = 
			ScorchedServer::instance()->getModFiles().getFiles();
		std::map<std::string, ModFileEntry *>::iterator itor;
		for (itor = modFiles.begin();
			itor != modFiles.end();
			++itor)
		{
			const std::string &fileName = (*itor).first;
			ModFileEntry *fileEntry = (*itor).second;

			// See if this file exists (or is correct) for this client
			std::map<std::string, ModIdentifierEntry*>::iterator hasEntryItor =
				hasEntries.find(fileName);
			if (hasEntryItor == hasEntries.end() ||
				hasEntryItor->second->crc != fileEntry->getUncompressedCrc() ||
				hasEntryItor->second->length != fileEntry->getUncompressedSize())
			{
#ifdef _DEBUG
				if (hasEntryItor == hasEntries.end())
				{
					ServerCommon::serverLog(
						S3D::formatStringBuffer("Mod download \"%s\" new file",
							fileName.c_str()));
				}
				else if (hasEntryItor->second->length != fileEntry->getUncompressedSize())
				{
					ServerCommon::serverLog(
						S3D::formatStringBuffer("Mod download \"%s\" size difference %u %u",
						fileName.c_str(),
						hasEntryItor->second->length, 
						fileEntry->getUncompressedSize()));
				}
				else if (hasEntryItor->second->crc != fileEntry->getUncompressedCrc())
				{
					ServerCommon::serverLog(
						S3D::formatStringBuffer("Mod download \"%s\" crc difference %u %u",
						fileName.c_str(),
						hasEntryItor->second->crc, 
						fileEntry->getUncompressedCrc()));
				}
#endif //_DEBUG

				ModIdentifierEntry newEntry(true,
					fileEntry->getFileName(),
					0, fileEntry->getUncompressedCrc());
				neededEntries.push_back(newEntry);
				neededLength += fileEntry->getUncompressedSize();
			}

			// Say that this file will be sent to the client
			if (hasEntryItor != hasEntries.end()) {
				hasEntries.erase(hasEntryItor);
			}
		}

		std::map<std::string, ModIdentifierEntry*>::iterator hasItor;
		for (hasItor = hasEntries.begin();
			hasItor != hasEntries.end();
			++hasItor)
		{
#ifdef _DEBUG
			ServerCommon::serverLog(
				S3D::formatStringBuffer("Mod download \"%s\" removing file",
					hasItor->first.c_str()));
#endif //_DEBUG

			ModIdentifierEntry newEntry(false,
				hasItor->first,
				0, 0);
			neededEntries.push_back(newEntry);
		}
	}

	if (neededEntries.empty())
	{
		// No files need downloading
		ScorchedServer::instance()->getServerChannelManager().sendText(
			ChannelText("info",
				"NO_MOD_FILES",
				"No mod files need downloading"),
			netMessage.getDestinationId(), 
			false);
	}
#ifndef S3D_SERVER
	else
	{
		// Do a sanity check that single player games don't need to download
		// any mod files.  As the server and client is the same process and
		// should be using the same mod directory.
		S3D::dialogExit("ModFiles",
			"ERROR: Single player client required mod files");
	}
#else
	else if (ScorchedServer::instance()->getOptionsGame().getModDownloadSpeed() == 0)
	{
		// If this server does not allow file downloads tell the client
		// and disconnect them
		std::string reason = S3D::formatStringBuffer(
				"This server requires the \"%s\" Scorched3D mod.\n"
				"The server does not allow in game file downloads.\n"
				"You must download and install this mod before you\n"
				"can connect to this server.",
				ScorchedServer::instance()->getOptionsGame().getMod());
		ServerCommon::kickDestination(destinationId, reason);
	}
#endif

	// Set the files to download in this tanks profile
	ServerDestination *destination =
		ScorchedServer::instance()->getServerDestinations().getDestination(
			netMessage.getDestinationId());
	if (!destination) return false;

	// and for each needed entry
	std::list<ModIdentifierEntry>::iterator neededItor;
	for (neededItor = neededEntries.begin();
		neededItor != neededEntries.end();
		++neededItor)
	{
		ModIdentifierEntry &entry = (*neededItor);
		destination->getMod().addFile(entry);
	}
	destination->getMod().setInit(true);
	destination->getMod().setTotalLeft(neededLength);

	return true;
}
