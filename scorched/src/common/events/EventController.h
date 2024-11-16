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

#if !defined(__INCLUDE_EventControllerh_INCLUDE__)
#define __INCLUDE_EventControllerh_INCLUDE__

#include <list>

class Tank;
class Weapon;
class EventHandler;
class EventController
{
public:
	EventController();
	virtual ~EventController();

	void addEventHandler(EventHandler *handler);

	void periodicUpdate();
	void periodicUpdate(Tank *tank);

	void gameStart(std::list<Tank *> &tanks);
	void roundStart(std::list<Tank *> &tanks);

	void tankConnected(Tank *tank);
	void tankDisconnected(Tank *tank);
	void tankJoined(Tank *tank);

	void tankFired(Tank *firedTank, Weapon *weapon);
	void tankResigned(Tank *resignedTank);

	void tankKilled(Tank *firedTank, Tank *deadTank, Weapon *weapon);
	void tankTeamKilled(Tank *firedTank, Tank *deadTank, Weapon *weapon);
	void tankSelfKilled(Tank *firedTank, Weapon *weapon);

	void tankWon(Tank *tank);
	void tankOverallWinner(Tank *tank);

private:
	std::list<EventHandler *> handlers_;
};

#endif
