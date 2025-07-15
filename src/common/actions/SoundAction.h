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

#if !defined(__INCLUDE_SoundActionh_INCLUDE__)
#define __INCLUDE_SoundActionh_INCLUDE__

#include <actions/Action.h>
#include <common/FixedVector.h>

class WeaponSound;
class SoundAction : public Action
{
public:
	SoundAction(FixedVector &position = FixedVector::getNullVector(), 
		WeaponSound *weapon = 0);
	virtual ~SoundAction();

	virtual void init();
	virtual void simulate(fixed frameTime, bool &remove);
	virtual std::string getActionDetails();
	virtual std::string getActionType() { return "SoundAction"; }

protected:
	WeaponSound *weapon_;
	FixedVector position_;

};

#endif // __INCLUDE_SoundActionh_INCLUDE__
