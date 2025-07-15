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

#include <events/EventController.h>
#include <events/EventHandler.h>

EventController::EventController()
{

}

EventController::~EventController()
{
	std::list<EventHandler *>::iterator itor, end = handlers_.end();
	for (itor = handlers_.begin();
		itor != end;
		++itor)
	{
		delete *itor;
	}
	handlers_.clear();
}

void EventController::addEventHandler(EventHandler *handler)
{
	handlers_.push_back(handler);
}

void EventController::periodicUpdate()
{
	std::list<EventHandler *>::iterator itor, end = handlers_.end();
	for (itor = handlers_.begin();
		itor != end;
		++itor)
	{
		(*itor)->periodicUpdate();
	}
}

void EventController::periodicUpdate(Tank *tank)
{
	std::list<EventHandler *>::iterator itor, end = handlers_.end();
	for (itor = handlers_.begin();
		itor != end;
		++itor)
	{
		(*itor)->periodicUpdate(tank);
	}
}

void EventController::gameStart(std::list<Tank *> &tanks)
{
	std::list<EventHandler *>::iterator itor, end = handlers_.end();
	for (itor = handlers_.begin();
		itor != end;
		++itor)
	{
		(*itor)->gameStart(tanks);
	}

}

void EventController::roundStart(std::list<Tank *> &tanks)
{
	std::list<EventHandler *>::iterator itor, end = handlers_.end();
	for (itor = handlers_.begin();
		itor != end;
		++itor)
	{
		(*itor)->roundStart(tanks);
	}
}

void EventController::tankConnected(Tank *tank)
{
	std::list<EventHandler *>::iterator itor, end = handlers_.end();
	for (itor = handlers_.begin();
		itor != end;
		++itor)
	{
		(*itor)->tankConnected(tank);
	}
}

void EventController::tankDisconnected(Tank *tank)
{
	std::list<EventHandler *>::iterator itor, end = handlers_.end();
	for (itor = handlers_.begin();
		itor != end;
		++itor)
	{
		(*itor)->tankDisconnected(tank);
	}
}


void EventController::tankJoined(Tank *tank)
{
	std::list<EventHandler *>::iterator itor, end = handlers_.end();
	for (itor = handlers_.begin();
		itor != end;
		++itor)
	{
		(*itor)->tankJoined(tank);
	}
}

void EventController::tankFired(Tank *firedTank, Weapon *weapon)
{
	std::list<EventHandler *>::iterator itor, end = handlers_.end();
	for (itor = handlers_.begin();
		itor != end;
		++itor)
	{
		(*itor)->tankFired(firedTank, weapon);
	}
}

void EventController::tankResigned(Tank *resignedTank)
{
	std::list<EventHandler *>::iterator itor, end = handlers_.end();
	for (itor = handlers_.begin();
		itor != end;
		++itor)
	{
		(*itor)->tankResigned(resignedTank);
	}
}

void EventController::tankKilled(Tank *firedTank, Tank *deadTank, Weapon *weapon)
{
	std::list<EventHandler *>::iterator itor, end = handlers_.end();
	for (itor = handlers_.begin();
		itor != end;
		++itor)
	{
		(*itor)->tankKilled(firedTank, deadTank, weapon);
	}
}

void EventController::tankTeamKilled(Tank *firedTank, Tank *deadTank, Weapon *weapon)
{
	std::list<EventHandler *>::iterator itor, end = handlers_.end();
	for (itor = handlers_.begin();
		itor != end;
		++itor)
	{
		(*itor)->tankTeamKilled(firedTank, deadTank, weapon);
	}
}

void EventController::tankSelfKilled(Tank *firedTank, Weapon *weapon)
{
	std::list<EventHandler *>::iterator itor, end = handlers_.end();
	for (itor = handlers_.begin();
		itor != end;
		++itor)
	{
		(*itor)->tankSelfKilled(firedTank, weapon);
	}
}

void EventController::tankWon(Tank *tank)
{
	std::list<EventHandler *>::iterator itor, end = handlers_.end();
	for (itor = handlers_.begin();
		itor != end;
		++itor)
	{
		(*itor)->tankWon(tank);
	}
}

void EventController::tankOverallWinner(Tank *tank)
{
	std::list<EventHandler *>::iterator itor, end = handlers_.end();
	for (itor = handlers_.begin();
		itor != end;
		++itor)
	{
		(*itor)->tankOverallWinner(tank);
	}
}
