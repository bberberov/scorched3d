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

#if !defined(__INCLUDE_TanketMovementh_INCLUDE__)
#define __INCLUDE_TanketMovementh_INCLUDE__

#include <actions/Action.h>
#include <common/FixedVector.h>
#include <common/Counter.h>
#include <weapons/Weapon.h>
#include <list>
#include <map>

class TankViewPointProvider;
class VirtualSoundSource;
class WeaponMoveTank;
class Tanket;
class TanketMovement : public Action
{
public:
	TanketMovement(WeaponFireContext &weaponContext,
		WeaponMoveTank *weapon_,
		int positionX, int positionY);
	virtual ~TanketMovement();

	virtual void init();
	virtual void simulate(fixed frameTime, bool &remove);
	virtual std::string getActionDetails();
	virtual std::string getActionType() { return "TanketMovement"; }

protected:
	// A list containing smooth positions
	struct PositionEntry
	{
		PositionEntry(
			int fX, int fY,
			int sX, int sY,
			fixed newx, fixed newy, fixed newa, bool useF) :
		firstX(fX), firstY(fY),
		secondX(sX), secondY(sY),
		x(newx), y(newy), ang(newa), 
		useFuel(useF)
		{}

		int firstX, firstY;
		int secondX, secondY;

		fixed x, y;
		fixed ang;
		bool useFuel;
	};
	WeaponMoveTank *weapon_;
	std::list<PositionEntry> expandedPositions_;
	fixed timePassed_;
	TankViewPointProvider *vPoint_;
	VirtualSoundSource *moveSoundSource_;
	Counter smokeCounter_;

	WeaponFireContext weaponContext_;
	FixedVector startPosition_;
	int positionX_, positionY_;
	unsigned int stepCount_;
	bool remove_, moving_;

	void moveTanket(Tanket *tanket);
	void simulationMove(fixed frameTime);

};

#endif
