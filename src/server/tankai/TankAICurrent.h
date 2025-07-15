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

#if !defined(AFX_TankAICurrent_H__5F21C9C7_0F71_4CCC_ABB9_976CF0A5C5EC__INCLUDED_)
#define AFX_TankAICurrent_H__5F21C9C7_0F71_4CCC_ABB9_976CF0A5C5EC__INCLUDED_

#include <tankai/TankAI.h>
#include <tankai/TankAICurrentWeapons.h>
#include <tankai/TankAICurrentDefenses.h>
#include <tankai/TankAICurrentMove.h>

class TankAICurrent : public TankAI
{
public:
	TankAICurrent();
	virtual ~TankAICurrent();

	virtual TankAI *createCopy(Tanket *tanket);

	virtual bool parseConfig(TankAIWeaponSets &sets, XMLNode *node);

	// Inherited from TankAI
	virtual void newMatch();
	virtual void newGame();
	virtual void playMove(unsigned int moveId);
	virtual void buyAccessories(unsigned int moveId);

	// Notification of actions happened
	virtual void tankHurt(Weapon *weapon, float damage, 
		unsigned int damaged, unsigned int firer);
	virtual void shotLanded(ScorchedCollisionId collision,
		Weapon *weapon, unsigned int firer, 
		Vector &position);

protected:
	Tanket *tanket_;
	TankAICurrentWeapons wantedWeapons_;
	TankAICurrentDefenses defenses_;
	TankAICurrentMove move_;

	void setTanket(Tanket *tanket);
	void resign();

};

#endif // !defined(AFX_TankAICurrent_H__5F21C9C7_0F71_4CCC_ABB9_976CF0A5C5EC__INCLUDED_)
