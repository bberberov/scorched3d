////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011, 2026
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

#ifndef __INCLUDE_TankAICurrent_hpp_INCLUDE__
#define __INCLUDE_TankAICurrent_hpp_INCLUDE__

#include <tankai/TankAI.hpp>
#include <tankai/TankAICurrentWeapons.hpp>
#include <tankai/TankAICurrentDefenses.hpp>
#include <tankai/TankAICurrentMove.hpp>

class TankAICurrent : public TankAI
{
public:
	TankAICurrent();

	virtual TankAI* createCopy( Tanket* tanket );

	virtual bool parseConfig( TankAIWeaponSets& sets, XMLNode* node );

	// Inherited from TankAI
	virtual void newMatch();
	virtual void newGame();
	virtual void playMove( unsigned int moveId );
	virtual void buyAccessories( unsigned int moveId );

	// Notification of actions happened
	virtual void tankHurt( Weapon* weapon, float damage, unsigned int damaged, unsigned int firer );
	virtual void shotLanded( ScorchedCollisionId collision, Weapon* weapon, unsigned int firer, Vector& position );

protected:
	Tanket*               tanket_;
	TankAICurrentWeapons  wantedWeapons_;
	TankAICurrentDefenses defenses_;
	TankAICurrentMove     move_;

	void setTanket( Tanket* tanket );
	void resign();
};

#endif  // __INCLUDE_TankAICurrent_hpp_INCLUDE__
