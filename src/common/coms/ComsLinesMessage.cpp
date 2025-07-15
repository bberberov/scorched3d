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

#include <coms/ComsLinesMessage.h>

ComsMessageType ComsLinesMessage::ComsLinesMessageType("ComsLinesMessageType");

ComsLinesMessage::ComsLinesMessage(unsigned int playerId) : 
	ComsMessage(ComsLinesMessageType),
	playerId_(playerId)
{
}

ComsLinesMessage::~ComsLinesMessage()
{
}

bool ComsLinesMessage::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer(playerId_);
	buffer.addToBuffer((int) lines_.size());
	std::list<Vector>::iterator itor;
	for (itor = lines_.begin();
		itor != lines_.end();
		++itor)
	{
		Vector &v = (*itor);
		buffer.addToBuffer(v);
	}
	return true;
}

bool ComsLinesMessage::readMessage(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(playerId_)) return false;
	int t = 0;
	if (!reader.getFromBuffer(t)) return false; 
	Vector v;
	for (int i=0; i<t; i++)
	{
		if (!reader.getFromBuffer(v)) return false;
		lines_.push_back(v);
	}

	return true;
}

