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

#if !defined(AFX_TankAINone_H__5F21C9C7_0F71_4CCC_ABB9_976CF0A5C5EC__INCLUDED_)
#define AFX_TankAINone_H__5F21C9C7_0F71_4CCC_ABB9_976CF0A5C5EC__INCLUDED_

#include <tankai/TankAI.h>

class TankAINone : public TankAI
{
public:
	TankAINone(unsigned int playerId);
	virtual ~TankAINone();

	virtual TankAINone *createCopy(Tanket *tanket) { return 0; }

	virtual bool parseConfig(TankAIWeaponSets &sets, XMLNode *node) { return false; }

	virtual const char *getName() { return "None"; }
	virtual const char *getDescription() { return "None"; }
	virtual ToolTip *getToolTip() { return 0; }

	virtual void newMatch() {}
	virtual void newGame() {}
	virtual void playMove(unsigned int moveId);
	virtual void buyAccessories(unsigned int moveId);

	// Notification of actions happened
	virtual void tankHurt(Weapon *weapon, float damage, 
		unsigned int damaged, unsigned int firer) {}
	virtual void shotLanded(ScorchedCollisionId collision,
		Weapon *weapon, unsigned int firer, 
		Vector &position) {}

	virtual bool availableForRandom() { return false; }
	virtual bool availableForPlayers() { return false; }
	virtual bool removedPlayer() { return true; }

protected:
	unsigned int playerId_;
};

#endif // !defined(AFX_TankAINone_H__5F21C9C7_0F71_4CCC_ABB9_976CF0A5C5EC__INCLUDED_)
