////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011, 2025
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

#if !defined(__INCLUDE_Napalmh_INCLUDE__)
#define __INCLUDE_Napalmh_INCLUDE__

#include <actions/Action.h>
#include <weapons/WeaponNapalm.h>
#include <common/Counter.h>
#include <landscapemap/DeformLandscape.h>
#include <list>
#include <vector>
#include <set>

class Target;
class TankViewPointProvider;
class GLTextureSet;
class Napalm : public Action
{
public:
	struct NapalmEntry
	{
		NapalmEntry(int x, int y, int o, int p) :
			posX(x),
			posY(y),
			offset(o),
			pset(p)
		{}

		int posX;
		int posY;
		int offset;
		int pset;
	};

	Napalm(int x, int y, Weapon *weapon, NapalmParams *params, WeaponFireContext &weaponContext);
	virtual ~Napalm();

	virtual void init();
	virtual void simulate(fixed frameTime, bool &remove);
	virtual std::string getActionDetails();
	virtual std::string getActionType() { return "Napalm"; }

	unsigned int getPlayerId() { return weaponContext_.getPlayerId(); }
	NapalmParams *getParams() { return params_; }

protected:
	TankViewPointProvider *vPoint_;
	Weapon *weapon_;
	NapalmParams *params_;
	WeaponFireContext weaponContext_;
	Counter counter_;
	GLTextureSet *set_;

	// Not sent by wire
	int particleSet_;
	int startX_;
	int startY_;
	fixed totalTime_;
	fixed hurtTime_;
	fixed napalmTime_;
	std::set<unsigned int> burnedTargets_;
	std::set<unsigned int> edgePoints_;
	std::map<unsigned int, int> napalmPointsCount_;
	std::list<NapalmEntry *> napalmPoints_;

	fixed getHeight(int x, int y);
	void simulateAddStep();
	void simulateAddEdge(int x, int y);
	void simulateRmStep();
	void simulateDamage();
	void addBurnAction(Target *target);

};

#endif
