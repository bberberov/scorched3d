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

#if !defined(__INCLUDE_ScorchedClienth_INCLUDE__)
#define __INCLUDE_ScorchedClienth_INCLUDE__

#include <engine/ScorchedContext.h>

class MainLoop;
class ParticleEngine;
class GameState;
class SimulatorGameState;
class ClientSimulator;
class ScorchedClient : public ScorchedContext
{
public:
	static ScorchedClient *instance();

	virtual bool getServerMode() { return false; }

	virtual Simulator &getSimulator();
	virtual TargetSpace &getTargetSpace() { return *targetSpace_; }
	MainLoop &getMainLoop() { return *mainLoop_; }
	ScorchedContext &getContext() { return *this; }
	ParticleEngine &getParticleEngine() { return *particleEngine_; }
	GameState &getGameState() { return *gameState; }
	ClientSimulator &getClientSimulator() { return *clientSimulator_; }

protected:
	static ScorchedClient *instance_;
	static TargetSpace *targetSpace_;
	MainLoop *mainLoop_;
	ParticleEngine* particleEngine_;
	GameState *gameState;
	ClientSimulator *clientSimulator_;

private:
	ScorchedClient();
	virtual ~ScorchedClient();
};

#endif
