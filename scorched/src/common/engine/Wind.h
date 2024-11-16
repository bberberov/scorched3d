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

#if !defined(AFX_Wind_H__86995B4A_478E_4CFE_BD4C_79128DE51904__INCLUDED_)
#define AFX_Wind_H__86995B4A_478E_4CFE_BD4C_79128DE51904__INCLUDED_

#include <common/FixedVector.h>
#include <engine/ScorchedContext.h>

class Wind
{
public:
	Wind();
	virtual ~Wind();

	void setScorchedContext(ScorchedContext *context) { context_ = context; }
	void newLevel();
	void simulate(fixed frameTime);

	fixed getWindAngle() { return windAngle_; }
	fixed getWindSpeed() { return windSpeed_; }
	FixedVector &getWindDirection() { return windDirection_; }
	FixedVector &getWindStartingDirection() { return windStartingDirection_; }
	bool getWindOn() { return (windSpeed_ > fixed(0)); }

protected:
	ScorchedContext *context_;
	fixed windAngle_, windStartAngle_, windSpeed_;
	fixed windChangeTime_;
	FixedVector windDirection_, windStartingDirection_;

	void updateDirection();
	void updateChangeTime();
};

#endif // !defined(AFX_Wind_H__86995B4A_478E_4CFE_BD4C_79128DE51904__INCLUDED_)
