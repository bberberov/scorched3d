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

#if !defined(AFX_PlayMovesSimAction_H__2C00E711_B337_4665_AB54_C6661FD67E5D__INCLUDED_)
#define AFX_PlayMovesSimAction_H__2C00E711_B337_4665_AB54_C6661FD67E5D__INCLUDED_

#include <simactions/SimAction.h>
#include <coms/ComsPlayedMoveMessage.h>
#include <list>

class Tanket;
class PlayMovesSimAction : public SimAction
{
public:
	PlayMovesSimAction();
	PlayMovesSimAction(unsigned int moveId, bool timeoutPlayers, bool referenced);
	virtual ~PlayMovesSimAction();

	void addMove(ComsPlayedMoveMessage *message);

	virtual bool invokeAction(ScorchedContext &context);

	virtual bool writeMessage(NetBuffer &buffer);
	virtual bool readMessage(NetBufferReader &reader);

REGISTER_CLASS_HEADER(PlayMovesSimAction);
protected:
	unsigned int moveId_;
	bool timeoutPlayers_, referenced_;
	std::list<ComsPlayedMoveMessage *> messages_;

	void tankTimedOut(ScorchedContext &context, 
		Tanket *tanket);
	void tankFired(ScorchedContext &context,
		Tanket *tanket, ComsPlayedMoveMessage &message);
	void tankResigned(ScorchedContext &context,
		Tanket *tanket, ComsPlayedMoveMessage &message);
};

#endif // !defined(AFX_PlayMovesSimAction_H__2C00E711_B337_4665_AB54_C6661FD67E5D__INCLUDED_)
