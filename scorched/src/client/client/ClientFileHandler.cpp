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

#include <client/ClientFileHandler.h>
#include <client/ClientState.h>
#include <client/ScorchedClient.h>
#include <engine/ModFiles.h>
#include <engine/ModFileEntryLoader.h>
#include <dialogs/ProgressDialog.h>
#include <common/Logger.h>
#include <common/Defines.h>
#include <common/OptionsScorched.h>
#include <coms/ComsMessageSender.h>
#include <coms/ComsFileMessage.h>
#include <coms/ComsFileAkMessage.h>
#include <lang/LangResource.h>

ClientFileHandler *ClientFileHandler::instance_ = 0;

ClientFileHandler *ClientFileHandler::instance()
{
	if (!instance_)
	{
		instance_ = new ClientFileHandler;
	}
	return instance_;
}

ClientFileHandler::ClientFileHandler() : totalBytes_(0)
{
	ScorchedClient::instance()->getComsMessageHandler().addHandler(
		ComsFileMessage::ComsFileMessageType,
		this);
}

ClientFileHandler::~ClientFileHandler()
{
}

bool ClientFileHandler::processMessage(
	NetMessage &netMessage,
	const char *messageType,
	NetBufferReader &mainreader)
{
	ComsFileMessage message;
	if (!message.readMessage(mainreader)) return false;
	NetBufferReader reader(message.fileBuffer);

	std::map<std::string, ModFileEntry *> &files = 
		ScorchedClient::instance()->getModFiles().getFiles();

	// Read the files one at a time
	for (;;)
	{
		// Read the filename
		// a zero length name means last file
		std::string fileName;
		reader.getFromBuffer(fileName);
		if (fileName.size() == 0) break;

		// Check if we are adding or removing this file
		bool addFile = true;
		reader.getFromBuffer(addFile);
		if (addFile)
		{
			// Read flags
			bool firstChunk = false;
			bool lastChunk = false;
			reader.getFromBuffer(firstChunk);
			reader.getFromBuffer(lastChunk);

			// Read file count
			unsigned int bytesLeft = 0;
			reader.getFromBuffer(bytesLeft);
			if (totalBytes_ == 0) totalBytes_ = bytesLeft;

			// Update progress
			unsigned int doneBytes = totalBytes_ - bytesLeft;
			unsigned int percentage = 0;

			if (doneBytes > 0) {
				percentage = (unsigned int)(((doneBytes / 1024) * 100) / (totalBytes_ / 1024));
			}
			ProgressDialog::instance()->progressChange(
				LANG_RESOURCE_3("DOWNLOADING_FILE", "Downloading mod, {0}% {1}/{2} KB", 
				percentage, (doneBytes / 1024), (totalBytes_ / 1024)), float(percentage));

			// Read the size
			unsigned int uncompressedSize = 0;
			unsigned int chunkSize = 0;
			reader.getFromBuffer(uncompressedSize);
			reader.getFromBuffer(chunkSize);
		
			// The first part
			if (firstChunk)
			{
				recvBuffer_.reset();
			}

			// Add the bytes to the file
			recvBuffer_.addDataToBuffer(
				reader.getBuffer() + reader.getReadSize(), chunkSize);
			reader.setReadSize(reader.getReadSize() + chunkSize);

			// Check if we have finished this file
			if (lastChunk)
			{
				// Finished
				Logger::log(S3D::formatStringBuffer(" %u/%u %s - %i bytes",
					doneBytes,
					totalBytes_,
					fileName.c_str(),
					recvBuffer_.getBufferUsed()));

				// Wrong size
				if (recvBuffer_.getBufferUsed() != uncompressedSize)
				{
					Logger::log(S3D::formatStringBuffer("Downloaded mod file incorrect size \"%s\".\n"
						"Expected %u, got %u.",
						fileName.c_str(), recvBuffer_.getBufferUsed(), uncompressedSize));
					return false;
				}

				// Write file
				if (!ModFileEntryLoader::writeModFile(recvBuffer_, fileName.c_str(),
					ScorchedClient::instance()->getOptionsGame().getMod()))
				{
					Logger::log(S3D::formatStringBuffer("Failed to write mod file \"%s\"",
						fileName.c_str()));
					return false;
				}


				// Remove the file if it already exists
				std::map<std::string, ModFileEntry *>::iterator findItor = 
					files.find(fileName);
				if (findItor != files.end())
				{
					delete (*findItor).second;
					files.erase(findItor);
				}

				// Create a new file
				ModFileEntry *fileEntry = new ModFileEntry(fileName, recvBuffer_.getCrc(), recvBuffer_.getBufferUsed());
				files[fileName] = fileEntry;
			}
		}
		else
		{
			// Remove the file 
			std::map<std::string, ModFileEntry *>::iterator findItor = 
				files.find(fileName);
			if (findItor != files.end())
			{
				ModFileEntryLoader::removeModFile(fileName, 
					ScorchedClient::instance()->getOptionsGame().getMod());

				Logger::log(S3D::formatStringBuffer(" %s - removed",
					fileName.c_str()));

				delete (*findItor).second;
				files.erase(findItor);
			}
		}
	}

	// Send the ak for this file
	ComsFileAkMessage akMessage;
	ComsMessageSender::sendToServer(akMessage);

	return true;
}
