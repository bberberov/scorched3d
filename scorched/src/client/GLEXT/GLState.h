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

#if !defined(AFX_GLSTATE_H__32B0E2D0_566D_4438_94E4_B12FE82430B1__INCLUDED_)
#define AFX_GLSTATE_H__32B0E2D0_566D_4438_94E4_B12FE82430B1__INCLUDED_

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include "glew.h"
#else
#include <GL/glew.h>
#endif

class GLState  
{
public:
	enum State
	{
		TEXTURE_ON = 0x1,
		TEXTURE_OFF = 0x2,
		BLEND_ON = 0x4,
		BLEND_OFF = 0x8,
		DEPTH_ON = 0x10,
		DEPTH_OFF = 0x20,
		CUBEMAP_ON = 0x40,
		CUBEMAP_OFF = 0x80,
		NORMALIZE_ON = 0x100,
		NORMALIZE_OFF = 0x200,
		LIGHTING_ON = 0x400,
		LIGHTING_OFF = 0x800,
		LIGHT1_ON = 0x1000,
		LIGHT1_OFF = 0x2000,
		ALPHATEST_ON = 0x4000,
		ALPHATEST_OFF = 0x8000
	};

	GLState(unsigned wantedState);
	virtual ~GLState();

	static void setBaseState(unsigned baseState);
	static const char *getStateString();
	static unsigned int getState() { return currentState_; }
	static void setState(unsigned wanted);

	static unsigned int getStateSwitches() { return stateSwitches_; }
	static void resetStateSwitches() { stateSwitches_ = 0; }

protected:
	static unsigned currentState_;
	static unsigned int stateSwitches_;
	unsigned returnState_;


};

#endif // !defined(AFX_GLSTATE_H__32B0E2D0_566D_4438_94E4_B12FE82430B1__INCLUDED_)
