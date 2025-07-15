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

#if !defined(AFX_TankAICurrentDefenses_H__5F21C9C7_0F71_4CCC_ABB9_976CF0A5C5EC__INCLUDED_)
#define AFX_TankAICurrentDefenses_H__5F21C9C7_0F71_4CCC_ABB9_976CF0A5C5EC__INCLUDED_

#include <tankai/TankAIWeaponSets.h>

class TankAICurrentDefenses
{
public:
	TankAICurrentDefenses();
	virtual ~TankAICurrentDefenses();

	virtual bool parseConfig(XMLNode *node);

	void raiseDefenses(Tanket *tanket);

	bool getUseBatteries() { return useBatteries_; }

protected:
	bool useParachutes_;
	bool useShields_;
	bool useBatteries_;

	void selectFirstShield(Tanket *tanket);
	void selectFirstParachute(Tanket *tanket);
	void parachutesUpDown(Tanket *tanket, unsigned int paraId);
	void shieldsUpDown(Tanket *tanket, unsigned int shieldId);
};

#endif // !defined(AFX_TankAICurrentDefenses_H__5F21C9C7_0F71_4CCC_ABB9_976CF0A5C5EC__INCLUDED_)
