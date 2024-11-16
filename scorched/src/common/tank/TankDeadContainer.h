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

#if !defined(AFX_TankDeadContainer_H__56AF98E2_E188_45EC_AA25_1865ADBBA3F1__INCLUDED_)
#define AFX_TankDeadContainer_H__56AF98E2_E188_45EC_AA25_1865ADBBA3F1__INCLUDED_

#include <map>
#include <tank/Tank.h>
#include <simactions/TankNewMatchSimAction.h>
#include <simactions/TankAddSimAction.h>

class TankDeadContainer  
{
public:
	TankDeadContainer();
	virtual ~TankDeadContainer();

	void clearTanks();
	void addDeadTank(Tank *tank, const std::string &storedName);
	void getDeadTank(TankNewMatchSimAction *simAction, const std::string &storedName);
	void getDeadTank(TankAddSimAction *simAction, const std::string &storedName);

protected:
	std::map<std::string, NetBuffer *> deadTanks_;

};

#endif // !defined(AFX_TankDeadContainer_H__56AF98E2_E188_45EC_AA25_1865ADBBA3F1__INCLUDED_)
