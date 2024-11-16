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

#if !defined(AFX_TANKSORT_H__21F68DAD_9352_4673_9E24_B869AD5645AE__INCLUDED_)
#define AFX_TANKSORT_H__21F68DAD_9352_4673_9E24_B869AD5645AE__INCLUDED_

#include <list>
#include <lang/LangString.h>
#include <engine/ScorchedContext.h>

class Tank;
class TankScore;
namespace TankSort
{
	struct SortOnScore
	{
		bool operator()(const Tank *x, const Tank *y, ScorchedContext &context) const;
	};

	int compare(ScorchedContext &context,
		const LangString &nameX, TankScore &scoreX,
		const LangString &nameY, TankScore &scoreY);

	int getWinningTeam(ScorchedContext &context);
	void getSortedTanks(std::list<Tank *> &list, ScorchedContext &context);
	void getSortedTanksIds(ScorchedContext &context, std::list<unsigned int> &list);
};

#endif // !defined(AFX_TANKSORT_H__21F68DAD_9352_4673_9E24_B869AD5645AE__INCLUDED_)
