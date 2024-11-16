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

#include <actions/ShowScoreAction.h>
#include <engine/ScorchedContext.h>
#include <server/ScorchedServer.h>
#include <server/ServerState.h>
#include <target/TargetContainer.h>
#include <tank/Tank.h>
#include <tank/TankScore.h>
#include <common/Logger.h>
#ifndef S3D_SERVER
#include <client/ScorchedClient.h>
#include <client/ClientState.h>
#include <client/ClientSaveScreenState.h>
#endif

bool ShowScoreAction::finalScoreStatic_ = false;

ShowScoreAction::ShowScoreAction(fixed scoreTime, bool finalScore) :
	Action(true),
	scoreTime_(scoreTime),
	finalScore_(finalScore)
{
}

ShowScoreAction::~ShowScoreAction()
{
}

void ShowScoreAction::init()
{
	finalScoreStatic_ = finalScore_;

#ifndef S3D_SERVER
	if (!context_->getServerMode())
	{
#endif
		if (finalScore_)
		{
			Logger::log("Final scores -------");
			std::map<unsigned int, Tank *> &tanks =
				context_->getTargetContainer().getTanks();
			std::map<unsigned int, Tank *>::iterator itor;
			for (itor = tanks.begin();
				itor != tanks.end();
				++itor)
			{
				Tank *tank = (*itor).second;
				Logger::log(S3D::formatStringBuffer("%s - %s",
					tank->getCStrName().c_str(),
					tank->getScore().getScoreString()));
			}
			Logger::log("--------------------");
		}
#ifndef S3D_SERVER
	}
#endif

#ifndef S3D_SERVER
	if (!context_->getServerMode())
	{
		ClientSaveScreenState::instance()->saveScreen();

		ScorchedClient::instance()->getGameState().stimulate(ClientState::StimWait);
		ScorchedClient::instance()->getGameState().stimulate(ClientState::StimScore);
	}
#endif
}

void ShowScoreAction::simulate(fixed frameTime, bool &remove)
{
	scoreTime_ -= frameTime;

	if (scoreTime_ <= 0)
	{
		remove = true;
		if (context_->getServerMode())
		{
			ScorchedServer::instance()->getServerState().scoreFinished();
		}
	}

	Action::simulate(frameTime, remove);
}
