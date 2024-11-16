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

#include <coms/ComsHaveModFilesMessage.h>

ComsMessageType ComsHaveModFilesMessage::ComsHaveModFilesMessageType("ComsHaveModFilesMessageType");

ComsHaveModFilesMessage::ComsHaveModFilesMessage() :
	ComsMessage(ComsHaveModFilesMessageType)
{
}

ComsHaveModFilesMessage::~ComsHaveModFilesMessage()
{
}

bool ComsHaveModFilesMessage::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer((int) files_.size());
	std::list<ModIdentifierEntry>::iterator itor;
	for (itor = files_.begin();
		itor != files_.end();
		++itor)
	{
		ModIdentifierEntry &entry = *itor;
		buffer.addToBuffer(entry.fileName);
		buffer.addToBuffer(entry.length);
		buffer.addToBuffer(entry.crc);
	}

	return true;
}

bool ComsHaveModFilesMessage::readMessage(NetBufferReader &reader)
{
	int fileSize = 0;
	if (!reader.getFromBuffer(fileSize)) return false;
	for (int i=0; i<fileSize; i++)
	{
		ModIdentifierEntry entry;
		reader.getFromBuffer(entry.fileName);
		reader.getFromBuffer(entry.length);
		reader.getFromBuffer(entry.crc);
		files_.push_back(entry);
	}
	return true;
}
