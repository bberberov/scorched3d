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

#if !defined(AFX_GLGlobalState_H__32B0E2D0_566D_4438_94E4_B12FE82430B1__INCLUDED_)
#define AFX_GLGlobalState_H__32B0E2D0_566D_4438_94E4_B12FE82430B1__INCLUDED_

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <GLEXT/GLState.h>

class GLGlobalState  
{
public:
	GLGlobalState(unsigned wantedState);
	virtual ~GLGlobalState();

	static void setState(unsigned wanted);

protected:
	static GLState *state_;
	static unsigned int stateStack_;
};

#endif // !defined(AFX_GLGlobalState_H__32B0E2D0_566D_4438_94E4_B12FE82430B1__INCLUDED_)
