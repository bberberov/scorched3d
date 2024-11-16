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

#include <events/EventHandler.h>

EventHandler::EventHandler()
{

}

EventHandler::~EventHandler()
{
}

void EventHandler::periodicUpdate()
{
}

void EventHandler::periodicUpdate(Tank *tank)
{
}

void EventHandler::gameStart(std::list<Tank *> &tanks)
{
}

void EventHandler::roundStart(std::list<Tank *> &tanks)
{
}

void EventHandler::tankConnected(Tank *tank)
{
}

void EventHandler::tankDisconnected(Tank *tank)
{
}


void EventHandler::tankJoined(Tank *tank)
{
}

void EventHandler::tankFired(Tank *firedTank, Weapon *weapon)
{
}

void EventHandler::tankResigned(Tank *resignedTank)
{
}

void EventHandler::tankKilled(Tank *firedTank, Tank *deadTank, Weapon *weapon)
{
}

void EventHandler::tankTeamKilled(Tank *firedTank, Tank *deadTank, Weapon *weapon)
{
}

void EventHandler::tankSelfKilled(Tank *firedTank, Weapon *weapon)
{
}

void EventHandler::tankWon(Tank *tank)
{
}

void EventHandler::tankOverallWinner(Tank *tank)
{
}
