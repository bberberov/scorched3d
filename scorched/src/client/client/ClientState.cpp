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

#include <graph/FrameTimer.h>
#include <graph/ParticleEngine.h>
#include <graph/Main2DCamera.h>
#include <graph/SpeedChange.h>
#include <graph/MainCamera.h>
#include <graph/ShotCountDown.h>
#include <graph/SoftwareMouse.h>
#include <client/ClientState.h>
#include <client/ClientSaveScreenState.h>
#include <client/ScorchedClient.h>
#include <client/ClientSimulator.h>
#include <client/ClientProcessingLoop.h>
#include <client/ClientDisconnected.h>
#include <sound/Sound.h>
#include <tankgraph/RenderTargets.h>
#include <tankgraph/TankKeyboardControl.h>
#include <GLW/GLWWindowManager.h>
#include <GLW/GLWToolTip.h>
#include <landscape/LandscapeStateHandler.h>
#include <landscape/LandscapeMusicManager.h>
#include <GLEXT/GLCameraFrustum.h>
#include <console/ConsoleImpl.h>

void ClientState::addMandatoryComponents(GameState &gameState, unsigned state)
{
	gameState.addStateLoop(state, Main2DCamera::instance(), 
		ClientProcessingLoop::instance());
	gameState.addStateLoop(state, Main2DCamera::instance(), 
		Sound::instance()); // SOUND
	gameState.addStateLoop(state, Main2DCamera::instance(),
		LandscapeMusicManager::instance()); // MUSIC
	gameState.addStateLoop(state, Main2DCamera::instance(), 
		FrameTimer::instance());
	gameState.addStateLoop(state, Main2DCamera::instance(), 
		&ScorchedClient::instance()->getClientSimulator());
}

void ClientState::addWindowManager(GameState &gameState, unsigned state)
{
	addMandatoryComponents(gameState, state);

	gameState.addStateKeyEntry(state, (ConsoleImpl *) Console::instance());
	gameState.addStateEntry(state, GLWWindowManager::instance());
	gameState.addStateLoop(state, Main2DCamera::instance(), 
		GLWWindowManager::instance());
	gameState.addStateMouseDownEntry(state, 
		GameState::MouseButtonLeft | GameState::MouseButtonRight |
		GameState::MouseButtonLeftDoubleClick | GameState::MouseButtonRightDoubleClick, 
		GLWWindowManager::instance());
	gameState.addStateMouseDragEntry(state, 
		GameState::MouseButtonLeft | GameState::MouseButtonRight, 
		GLWWindowManager::instance());
	gameState.addStateMouseUpEntry(state, 
		GameState::MouseButtonLeft | GameState::MouseButtonRight, 
		GLWWindowManager::instance());
	gameState.addStateKeyEntry(state, GLWWindowManager::instance());
	gameState.addStateMouseWheelEntry(state, GLWWindowManager::instance());
	gameState.addStateLoop(state, Main2DCamera::instance(),
		(ConsoleImpl *) Console::instance());
}

void ClientState::addStandardComponents(GameState &gameState, unsigned state)
{
	gameState.addStateKeyEntry(state, SpeedChange::instance());
	gameState.addStateEntry(state, &RenderTargets::instance()->render3D);
	gameState.addStateLoop(state, 
		MainCamera::instance(), GLCameraFrustum::instance());
	gameState.addStateLoop(state, 
		MainCamera::instance(), new LandscapeStateLandHandler());
	gameState.addStateLoop(state,
		MainCamera::instance(), &RenderTargets::instance()->render3D);
	gameState.addStateLoop(state, 
		MainCamera::instance(), new LandscapeStateWaterHandler());
	gameState.addStateLoop(state, 
		MainCamera::instance(), new LandscapeStateObjectsHandler());
	gameState.addStateLoop(state, MainCamera::instance(),
		&ScorchedClient::instance()->getClientSimulator().actionControllerGameState);
	gameState.addStateLoop(state, MainCamera::instance(), 
		&ScorchedClient::instance()->getParticleEngine());
	gameState.addStateLoop(state, 
		MainCamera::instance(), &MainCamera::instance()->precipitation_);
	gameState.addStateLoop(state, 
		Main2DCamera::instance(), &RenderTargets::instance()->render2D);
	gameState.addStateLoop(state, 
		Main2DCamera::instance(), SpeedChange::instance());
	gameState.addStateLoop(state,
		Main2DCamera::instance(), ShotCountDown::instance());
	addWindowManager(gameState, state);
	gameState.addStateLoop(state, Main2DCamera::instance(), 
		GLWToolTip::instance());
	gameState.addStateLoop(state, Main2DCamera::instance(), 
		SoftwareMouse::instance());
	gameState.addStateMouseDownEntry(state,
		GameState::MouseButtonLeft | 
		GameState::MouseButtonRight | 
		GameState::MouseButtonMiddle, MainCamera::instance());
	gameState.addStateMouseUpEntry(state,
		GameState::MouseButtonLeft | 
		GameState::MouseButtonRight | 
		GameState::MouseButtonMiddle, MainCamera::instance());
	gameState.addStateMouseDragEntry(state, 
		GameState::MouseButtonLeft | 
		GameState::MouseButtonRight | 
		GameState::MouseButtonMiddle, MainCamera::instance());
	gameState.addStateMouseWheelEntry(state, 
		MainCamera::instance());
	gameState.addStateKeyEntry(state, 
		MainCamera::instance());
	gameState.addStateLoop(state, 
		Main2DCamera::instance(), &MainCamera::instance()->saveScreen_);
}

void ClientState::setupGameState()
{
	GameState &gameState = ScorchedClient::instance()->getGameState();
	gameState.clear();

	// StateOptions
	addWindowManager(gameState, StateOptions);
	gameState.addStateLoop(StateOptions, 
		Main2DCamera::instance(), GLWToolTip::instance());
	gameState.addStateLoop(StateOptions, 
		Main2DCamera::instance(), SoftwareMouse::instance());
	gameState.addStateStimulus(StateOptions, 
		StimDisconnected, StateDisconnected);
	gameState.addStateStimulus(StateOptions, 
		StimConnect, StateConnect);

	// Set the start state
	gameState.setState(StateOptions);

	// StateConnect
	addWindowManager(gameState, StateConnect);
	gameState.addStateLoop(StateConnect, 
		Main2DCamera::instance(), SoftwareMouse::instance());
	gameState.addStateStimulus(StateConnect, 
		StimDisconnected, StateDisconnected);
	gameState.addStateStimulus(StateConnect, 
		StimGameStopped, StateConnect);
	gameState.addStateStimulus(StateConnect, 
		StimOptions, StateOptions);
	gameState.addStateStimulus(StateConnect, 
		StimLoadFiles, StateLoadFiles);
	
	// StateLoadFiles
	addWindowManager(gameState, StateLoadFiles);
	gameState.addStateLoop(StateLoadFiles, 
		Main2DCamera::instance(), GLWToolTip::instance());
	gameState.addStateLoop(StateLoadFiles, 
		Main2DCamera::instance(), SoftwareMouse::instance());
	gameState.addStateStimulus(StateLoadFiles, 
		StimGameStopped, StateConnect);
	gameState.addStateStimulus(StateLoadFiles, 
		StimDisconnected, StateDisconnected);
	gameState.addStateStimulus(StateLoadFiles, 
		StimWait, StateWait);
	gameState.addStateStimulus(StateLoadFiles, 
		StimLoadLevel, StateLoadLevel);

	// StateLoadLevel
	addWindowManager(gameState, StateLoadLevel);
	gameState.addStateLoop(StateLoadLevel, 
		Main2DCamera::instance(), GLWToolTip::instance());
	gameState.addStateLoop(StateLoadLevel, 
		Main2DCamera::instance(), SoftwareMouse::instance());
	gameState.addStateStimulus(StateLoadLevel, 
		StimGameStopped, StateConnect);
	gameState.addStateStimulus(StateLoadLevel, 
		StimDisconnected, StateDisconnected);
	gameState.addStateStimulus(StateLoadLevel, 
		StimWait, StateWait);
	gameState.addStateStimulus(StateLoadLevel, 
		StimWaitNoLandscape, StateWaitNoLandscape);
	gameState.addStateStimulus(StateLoadLevel, 
		StimLoadLevel, StateLoadLevel);

	// StateWaitNoLandscape
	addWindowManager(gameState, StateWaitNoLandscape);
	gameState.addStateLoop(StateWaitNoLandscape, 
		Main2DCamera::instance(), GLWToolTip::instance());
	gameState.addStateLoop(StateWaitNoLandscape, 
		Main2DCamera::instance(), SoftwareMouse::instance());
	gameState.addStateStimulus(StateWaitNoLandscape, 
		StimWait, StateWait);
	gameState.addStateStimulus(StateWaitNoLandscape, 
		StimLoadLevel, StateLoadLevel);
	gameState.addStateStimulus(StateWaitNoLandscape, 
		StimDisconnected, StateDisconnected);

	// StateWait
	addStandardComponents(gameState, StateWait);
	gameState.addStateEntry(StateWait, 
		TankKeyboardControl::instance());
	gameState.addStateStimulus(StateWait, 
		StimDisconnected, StateDisconnected);
	gameState.addStateStimulus(StateWait, 
		StimAutoDefense, StateWait);
	gameState.addStateStimulus(StateWait, 
		StimBuyWeapons, StateBuyWeapons);
	gameState.addStateStimulus(StateWait, 
		StimPlaying, StatePlaying);
	gameState.addStateStimulus(StateWait, 
		StimWait, StateWait);
	gameState.addStateStimulus(StateWait, 
		StimScore, StateScore);
	gameState.addStateStimulus(StateWait, 
		StimLoadLevel, StateLoadLevel);

	// StateBuyWeapons
	addStandardComponents(gameState, StateBuyWeapons);
	gameState.addStateStimulus(StateBuyWeapons, 
		StimDisconnected, StateDisconnected);
	gameState.addStateStimulus(StateBuyWeapons, 
		StimAutoDefense, StateAutoDefense);
	gameState.addStateStimulus(StateBuyWeapons, 
		StimWait, StateWait);
	gameState.addStateStimulus(StateBuyWeapons, 
		StimLoadLevel, StateLoadLevel);

	// StateAutoDefense
	addStandardComponents(gameState, StateAutoDefense);
	gameState.addStateStimulus(StateAutoDefense, 
		StimDisconnected, StateDisconnected);
	gameState.addStateStimulus(StateAutoDefense, 
		StimWait, StateWait);
	gameState.addStateStimulus(StateAutoDefense, 
		StimLoadLevel, StateLoadLevel);

	// StatePlaying
	addStandardComponents(gameState, StatePlaying);
	gameState.addStateKeyEntry(StatePlaying, 
		TankKeyboardControl::instance());
	gameState.addStateStimulus(StatePlaying, 
		StimDisconnected, StateDisconnected);
	gameState.addStateStimulus(StatePlaying, 
		StimWait, StateWait);
	gameState.addStateStimulus(StatePlaying, 
		StimLoadLevel, StateLoadLevel);

	// StateScore
	addStandardComponents(gameState, StateScore);
	gameState.addStateLoop(StateScore,
		MainCamera::instance(), ClientSaveScreenState::instance());
	gameState.addStateStimulus(StateScore, 
		StimDisconnected, StateDisconnected);
	gameState.addStateStimulus(StateScore, 
		StimWait, StateWait);
	gameState.addStateStimulus(StateScore, 
		StimLoadLevel, StateLoadLevel);

	// StateDisconnected
	gameState.addStateEntry(StateDisconnected, new ClientDisconnected());
	gameState.addStateStimulus(StateDisconnected, 
		StimOptions, StateOptions);
}
