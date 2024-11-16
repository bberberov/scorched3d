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


#if !defined(__INCLUDE_ClientStateh_INCLUDE__)
#define __INCLUDE_ClientStateh_INCLUDE__

#include <engine/GameState.h>

namespace ClientState  
{
	enum Stimulus
	{
		StimOptions = 1,
		StimConnect,
		StimLoadFiles,
		StimLoadLevel,
		StimWaitNoLandscape,
		StimWait,
		StimBuyWeapons,
		StimAutoDefense,
		StimPlaying,
		StimDisconnected,
		StimGameStopped,
		StimScore
	};

	enum State
	{
		StateOptions = 1,
		StateConnect,
		StateDisconnected,
		StateLoadFiles,
		StateLoadLevel,
		StateWaitNoLandscape,
		StateWait,
		StateBuyWeapons,
		StateAutoDefense,
		StatePlaying,
		StateScore
	};

	void setupGameState();
	void addWindowManager(GameState &gameState, unsigned state);
	void addStandardComponents(GameState &gameState, unsigned state);
	void addMandatoryComponents(GameState &gameState, unsigned state);
};

#endif
