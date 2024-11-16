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

#ifndef _GAMMA_H_
#define _GAMMA_H_

#include <SDL/SDL.h>

struct GammaSettings
{
	Uint16 Red  [256];	
	Uint16 Green[256];
	Uint16 Blue [256];
};

class Gamma
{
public:
	static Gamma *instance();

	// should be between 0.3f, 4.0f
	bool save();
	bool set();

	void reset();

protected:
	static Gamma *instance_;

private:
	Gamma();
	virtual ~Gamma();

	GammaSettings savedSettings_;
};

#endif /* _GAMMA_H_ */
