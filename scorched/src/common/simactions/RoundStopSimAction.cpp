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

#include <simactions/RoundStopSimAction.h>
#include <simactions/TankStopMoveSimAction.h>
#include <target/TargetContainer.h>
#ifndef S3D_SERVER
#include <graph/ShotCountDown.h>
#endif

REGISTER_CLASS_SOURCE(RoundStopSimAction);

RoundStopSimAction::RoundStopSimAction()
{
}

RoundStopSimAction::~RoundStopSimAction()
{
}

bool RoundStopSimAction::invokeAction(ScorchedContext &context)
{
	// Stop the current round
	context.getTargetContainer().setCurrentRoundId(0);

	// Stop any tanks from making any moves
	std::map<unsigned int, Tanket *> &tankets =
		context.getTargetContainer().getTankets();
	std::map<unsigned int, Tanket *>::iterator tankItor;
	for (tankItor = tankets.begin();
		tankItor != tankets.end();
		++tankItor)
	{
		TankStopMoveSimAction::stopMove(context, tankItor->second);
	}

	if (!context.getServerMode())
	{
#ifndef S3D_SERVER
		ShotCountDown::instance()->hideRoundTime();
#endif
	}

	return true;
}

bool RoundStopSimAction::writeMessage(NetBuffer &buffer)
{
	return true;
}

bool RoundStopSimAction::readMessage(NetBufferReader &reader)
{
	return true;
}