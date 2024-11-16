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

#include <server/ServerFileServer.h>
#include <server/ScorchedServer.h>
#include <server/ServerCommon.h>
#include <server/ServerDestinations.h>
#include <common/OptionsScorched.h>
#include <common/Defines.h>
#include <coms/ComsMessageSender.h>
#include <coms/ComsInitializeModMessage.h>
#include <engine/ModFiles.h>
#include <time.h>

ServerFileServer::ServerFileServer() : lastTime_(0), bytesSent_(0)
{
}

ServerFileServer::~ServerFileServer()
{
}

void ServerFileServer::simulate()
{
	int downloadCount = 0;

	// Check how many people are currently downloading
	// also check for any that have finished downloading
	std::map<unsigned int, ServerDestination*> &destinations =
		ScorchedServer::instance()->getServerDestinations().getServerDestinations();
	std::map<unsigned int, ServerDestination*>::iterator itor;
	for (itor = destinations.begin();
		itor != destinations.end();
		++itor)
	{
		unsigned int serverDestinationId = itor->first;
		ServerDestination *destination = itor->second;
		if (destination->getState() == ServerDestination::sDownloadingMod)
		{
			// Does this tank have any more files to send
			if (destination->getMod().getNextFile())
			{
				// Yes, 1 more tank to send to
				downloadCount++;
			}
			else
			{
				// Check if this tank had had a chance to get its mod files
				if (destination->getMod().getInit())
				{
					// Set this tank to finished
					destination->setState(ServerDestination::sInitializingMod);

					// Tell this destination to start initializing
					ComsInitializeModMessage initMessage;
					ComsMessageSender::sendToSingleClient(initMessage,
						serverDestinationId);
				}
			}
		}
	}

	// Do we allow file downloads
	if (ScorchedServer::instance()->getOptionsGame().getModDownloadSpeed() == 0)
	{
		return;
	}

	// If no people are downloading then there is nothing to do
	if (downloadCount == 0) return;

	// Check if this is the next second
	unsigned int theTime = (unsigned int) time(0);
	if (theTime > lastTime_)
	{
		// If so reset the amount that can be sent by this server
		lastTime_ = theTime;
		bytesSent_ = 0;
	}

	// Send bytes to each ready client
	for (itor = destinations.begin();
		itor != destinations.end();
		++itor)
	{
		ServerDestination *destination = itor->second;
		sendToDestination(destination);
	}
}

void ServerFileServer::sendToDestination(ServerDestination *destination)
{
	const unsigned int sizeToSend = ScorchedServer::instance()->getOptionsGame().getModDownloadSpeed() / 10;

	// Check we should be downloading
	if (destination->getState() != ServerDestination::sDownloadingMod)
	{
		return;
	}

	// Make sure we have not sent too much this second already
	if (bytesSent_ + sizeToSend >
		(unsigned int) ScorchedServer::instance()->getOptionsGame().getModDownloadSpeed() + 5)
	{
		return;
	}

	// Check if the client is ready to recieve more
	// bytes and there is some to send and
	// we have not sent to this client this second
	if (!destination->getMod().getReadyToReceive() ||
		!destination->getMod().getNextFile())
	{
		return;
	}

	// Send bytes to this tank
	destination->getMod().setReadyToReceive(false);
	bytesSent_ += sizeToSend;

	sendBytes(destination, sizeToSend);
}

void ServerFileServer::sendBytes(ServerDestination *destination, unsigned int size)
{
	ComsFileMessage message;
	// Fill up the message with files, until it is full
	while (size > 0)
	{
		unsigned int bytesSent = 0;
		if (!sendNextFile(message, destination, size, bytesSent)) break;
		size -= bytesSent;
	}
	// Add any empty file name to signal end of files for this message
	message.fileBuffer.addToBuffer("");

	// Send the message to the client
	ComsMessageSender::sendToSingleClient(message, 
		destination->getDestinationId());

}

bool ServerFileServer::sendNextFile(ComsFileMessage &message,
	ServerDestination *destination, unsigned int size,
	unsigned int &bytesSent)
{
	// Get the next file to send
	ModIdentifierEntry *entry = destination->getMod().getNextFile();
	if (!entry) return false;

	if (entry->addFile) 
	{
		NetBuffer &sendBuffer = destination->getMod().getSendBuffer();

		// Check how much still needs to be sent
		unsigned int sizeSent = entry->length;
		unsigned int sizeLeftToSend = sendBuffer.getBufferUsed() - sizeSent;
		unsigned int sizeToSend = MIN(sizeLeftToSend, size);
		unsigned int bytesLeft = destination->getMod().getTotalLeft();
		bool firstChunk = (sizeSent == 0);
		bool lastChunk = (sizeToSend == sizeLeftToSend);

		// Add the bytes to the buffer
		message.fileBuffer.addToBuffer(entry->fileName);
		message.fileBuffer.addToBuffer(true);
		message.fileBuffer.addToBuffer(firstChunk);
		message.fileBuffer.addToBuffer(lastChunk);
		message.fileBuffer.addToBuffer(bytesLeft);
		message.fileBuffer.addToBuffer(sendBuffer.getBufferUsed());
		message.fileBuffer.addToBuffer(sizeToSend);
		message.fileBuffer.addDataToBuffer(sendBuffer.getBuffer() + entry->length,
			sizeToSend);

		// Update how much we have sent
		entry->length += sizeToSend;
		destination->getMod().setTotalLeft(destination->getMod().getTotalLeft() - sizeToSend);

		// Have we sent the whole file
		if (sizeToSend == sizeLeftToSend)
		{
			// If so remove the file from the list that
			// still needs to be sent
			destination->getMod().rmFile(entry->fileName.c_str());
		}

		bytesSent = sizeToSend;
	} 
	else 
	{
		message.fileBuffer.addToBuffer(entry->fileName);
		message.fileBuffer.addToBuffer(false);

		destination->getMod().rmFile(entry->fileName.c_str());

		bytesSent = 0;
	}
	return true;
}

