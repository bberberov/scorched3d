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

#include <GLW/GLWWindowManager.h>
#include <GLW/GLWWindowSkinManager.h>
#include <GLW/GLWSelector.h>
#include <common/Keyboard.h>
#include <common/OptionsScorched.h>
#include <client/ClientParams.h>
#include <client/ClientState.h>
#include <client/ClientWindowSetup.h>
#include <client/ScorchedClient.h>
#include <dialogs/AnimatedBackdropDialog.h>
#include <dialogs/AutoDefenseDialog.h>
#include <dialogs/MainMenuDialog.h>
#include <dialogs/CameraDialog.h>
#include <dialogs/ProfileDialog.h>
#include <dialogs/PlayerInGameDialog.h>
#include <dialogs/QuitDialog.h>
#include <dialogs/LogDialog.h>
#include <dialogs/ConnectDialog.h>
#include <dialogs/AuthDialog.h>
#include <dialogs/BackdropDialog.h>
#include <dialogs/ProgressDialog.h>
#include <dialogs/GiftMoneyDialog.h>
#include <dialogs/BuyAccessoryDialog.h>
#include <dialogs/ScoreDialog.h>
#include <dialogs/SaveDialog.h>
#include <dialogs/KibitzingDialog.h>
#include <dialogs/InventoryDialog.h>
#include <dialogs/SkipDialog.h>
#include <dialogs/SkipAllDialog.h>
#include <dialogs/HUDDialog.h>
#include <dialogs/TipDialog.h>
#include <dialogs/SoundDialog.h>
#include <dialogs/HelpButtonDialog.h>
#include <dialogs/TutorialDialog.h>
#include <dialogs/MessageDialog.h>
#include <dialogs/StartDialog.h>
#include <dialogs/ModSelectDialog.h>
#include <dialogs/ModSubSelectDialog.h>
#include <dialogs/SaveSelectDialog.h>
#include <dialogs/NetworkSelectDialog.h>
#include <dialogs/NetworkChatDialog.h>
#include <dialogs/MsgBoxDialog.h>
#include <dialogs/SettingsSelectDialog.h>
#include <dialogs/SettingsSubSelectDialog.h>
#include <dialogs/AdminCheckDialog.h>
#include <dialogs/AdminAuthDialog.h>
#include <dialogs/AdminDialog.h>
#include <dialogs/PlayerInitialDialog.h>

void ClientWindowSetup::addStateWindows(GLWWindowSkinManager *skinManager,
	unsigned int state, const char *windowState)
{
	std::list<GLWWindowSkin *> allStateWindows = 
		skinManager->getStateWindows(windowState);
	std::list<GLWWindowSkin *>::iterator itor;
	for (itor = allStateWindows.begin();
		itor != allStateWindows.end();
		++itor)
	{
		GLWWindowSkin *window = *itor;
		KeyboardKey *key = 0;
		if (window->getKey()[0]) key = 
			Keyboard::instance()->getKey(window->getKey());
		GLWWindowManager::instance()->addWindow(state, 
			window, key, window->getVisible());
	}
}

void ClientWindowSetup::addCommonComponents(GLWWindowSkinManager *skinManager, 
	unsigned state)
{
	addStateWindows(skinManager, state, "all");

	KEYBOARDKEY("HUD_ITEMS", hudItemsKey);
	GLWWindowManager::instance()->addWindow(state, 
 		HUDDialog::instance(), hudItemsKey, false);

	KEYBOARDKEY("TIP_ITEMS", tipItemsKey);
	GLWWindowManager::instance()->addWindow(state, 
 		TipDialog::instance(), tipItemsKey, false);

	KEYBOARDKEY("SHOW_SOUND_DIALOG", showSoundKey);
	GLWWindowManager::instance()->addWindow(state, 
 		SoundDialog::instance(), showSoundKey, false);

	KEYBOARDKEY("SHOW_INVENTORY_DIALOG", showInvKey);
	GLWWindowManager::instance()->addWindow(state, 
 		InventoryDialog::instance(), showInvKey, false);

	KEYBOARDKEY("SHOW_KIBITZ_DIALOG", kibitzKey);
	GLWWindowManager::instance()->addWindow(state, 
		KibitzingDialog::instance(), kibitzKey, false);

	if (!ClientParams::instance()->getConnectedToServer())
	{
		KEYBOARDKEY("SHOW_KILL_DIALOG", killKey);
		GLWWindowManager::instance()->addWindow(state, 
 			QuitDialog::instance(), killKey, false);
		KEYBOARDKEY("SHOW_SAVE_DIALOG", saveKey);
		GLWWindowManager::instance()->addWindow(state, 
 			SaveDialog::instance(), saveKey, false);
	}

	KEYBOARDKEY("SHOW_CAMERA_DIALOG", cameraKey);
	GLWWindowManager::instance()->addWindow(state, 
		CameraDialog::instance(), cameraKey, false);

	if (state != ClientState::StateScore)
	{
		KEYBOARDKEY("SHOW_SCORE_DIALOG", scoreKey);
		GLWWindowManager::instance()->addWindow(state, 
			ScoreDialog::instance(), scoreKey, false);
	}
	GLWWindowManager::instance()->addWindow(state,
		MessageDialog::instance(), 0, true);
	GLWWindowManager::instance()->addWindow(state, 
		MainMenuDialog::instance(), 0, true);

	addMessageComponents(skinManager, state);

	if (state != ClientState::StateBuyWeapons &&
		state != ClientState::StateAutoDefense)
	{
		KEYBOARDKEY("SHOW_TEAM_DIALOG", teamKey);
		GLWWindowManager::instance()->addWindow(state,
			PlayerInGameDialog::instance(), teamKey, false);
	}

	GLWWindowManager::instance()->addWindow(state, 
		AdminCheckDialog::instance(), 0, false);
	GLWWindowManager::instance()->addWindow(state, 
		AdminDialog::instance(), 0, false);
	GLWWindowManager::instance()->addWindow(state, 
		AdminAuthDialog::instance(), 0, false);

	if (ScorchedClient::instance()->getOptionsGame().getTutorial()[0])
	{
		GLWWindowManager::instance()->addWindow(state, 
 			TutorialDialog::instance(), 0, true);
	}
}

void ClientWindowSetup::addMessageComponents(GLWWindowSkinManager *skinManager, unsigned state)
{
	KEYBOARDKEY("SHOW_QUIT_DIALOG", quitKey);
	GLWWindowManager::instance()->addWindow(state, 
 		QuitDialog::instance(), quitKey, false);

	GLWWindowManager::instance()->addWindow(state,
		GLWSelector::instance(), 0, true);
	GLWWindowManager::instance()->addWindow(state,
		MsgBoxDialog::instance(), 0, false);
	GLWWindowManager::instance()->addWindow(state,
		TextBoxDialog::instance(), 0, false);
}

// This is called before any mod has been loaded
void ClientWindowSetup::setupStartWindows(GLWWindowSkinManager *skinManager)
{
	
	KEYBOARDKEY("SHOW_SOUND_DIALOG", showSoundKey);

	// StateOptions
	GLWWindowManager::instance()->removeState(ClientState::StateOptions);
	GLWWindowManager::instance()->addWindow(ClientState::StateOptions, 
		AnimatedBackdropDialog::instance(), 0, true);
	GLWWindowManager::instance()->addWindow(ClientState::StateOptions, 
 		SoundDialog::instance(), showSoundKey, false);
	GLWWindowManager::instance()->addWindow(ClientState::StateOptions, 
		StartDialog::instance(), 0, true);
	GLWWindowManager::instance()->addWindow(ClientState::StateOptions,
		ModSelectDialog::instance(), 0, false);
	GLWWindowManager::instance()->addWindow(ClientState::StateOptions,
		ModSubSelectDialog::instance(), 0, false);
	GLWWindowManager::instance()->addWindow(ClientState::StateOptions,
		SaveSelectDialog::instance(), 0, false);
	GLWWindowManager::instance()->addWindow(ClientState::StateOptions,
		NetworkSelectDialog::instance(), 0, false);
	GLWWindowManager::instance()->addWindow(ClientState::StateOptions,
		NetworkChatDialog::instance(), 0, false);
	GLWWindowManager::instance()->addWindow(ClientState::StateOptions,
		SettingsSelectDialog::instance(), 0, false);
	GLWWindowManager::instance()->addWindow(ClientState::StateOptions,
		SettingsSubSelectDialog::instance(), 0, false);
	GLWWindowManager::instance()->addWindow(ClientState::StateOptions, 
		MainMenuDialog::instance(), 0, true);
	addMessageComponents(skinManager, ClientState::StateOptions);

	// StateConnect
	GLWWindowManager::instance()->removeState(ClientState::StateConnect);
	GLWWindowManager::instance()->addWindow(ClientState::StateConnect, 
		BackdropDialog::instance(), 0, true);
	GLWWindowManager::instance()->addWindow(ClientState::StateConnect, 
 		SoundDialog::instance(), showSoundKey, false);
	GLWWindowManager::instance()->addWindow(ClientState::StateConnect, 
		ProgressDialog::instance(), 0, true);
	GLWWindowManager::instance()->addWindow(ClientState::StateConnect, 
		ConnectDialog::instance(), 0, true);
	GLWWindowManager::instance()->addWindow(ClientState::StateConnect, 
		AuthDialog::instance(), 0, false);
	GLWWindowManager::instance()->addWindow(ClientState::StateConnect, 
		MainMenuDialog::instance(), 0, true);
	addMessageComponents(skinManager, ClientState::StateConnect);

	// StateDisconnected
	GLWWindowManager::instance()->removeState(ClientState::StateDisconnected);
	GLWWindowManager::instance()->addWindow(ClientState::StateDisconnected, 
		BackdropDialog::instance(), 0, true);
	GLWWindowManager::instance()->addWindow(ClientState::StateDisconnected, 
 		SoundDialog::instance(), showSoundKey, false);
	GLWWindowManager::instance()->addWindow(ClientState::StateDisconnected, 
		LogDialog::instance(), 0, true);
	GLWWindowManager::instance()->addWindow(ClientState::StateDisconnected, 
		MainMenuDialog::instance(), 0, true);
	addMessageComponents(skinManager, ClientState::StateDisconnected);

	// StateLoadFiles
	GLWWindowManager::instance()->removeState(ClientState::StateLoadFiles);
	GLWWindowManager::instance()->addWindow(ClientState::StateLoadFiles,
		BackdropDialog::instance(), 0, true);
	GLWWindowManager::instance()->addWindow(ClientState::StateOptions, 
 		SoundDialog::instance(), showSoundKey, false);
	addStateWindows(skinManager, ClientState::StateLoadFiles, "start");
	GLWWindowManager::instance()->addWindow(ClientState::StateLoadFiles,
		ProgressDialog::instance(), 0, true);
	GLWWindowManager::instance()->addWindow(ClientState::StateLoadFiles, 
		MainMenuDialog::instance(), 0, true);
	addMessageComponents(skinManager, ClientState::StateLoadFiles);
}

// This is called after the correct mod has been loaded
void ClientWindowSetup::setupGameWindows(GLWWindowSkinManager *skinManager)
{
	KEYBOARDKEY("SHOW_SKIP_DIALOG", skipKey);
	KEYBOARDKEY("SHOW_SOUND_DIALOG", showSoundKey);

	// StateWaitNoLandscape
	GLWWindowManager::instance()->removeState(ClientState::StateWaitNoLandscape);
	GLWWindowManager::instance()->addWindow(ClientState::StateWaitNoLandscape,
		BackdropDialog::instance(), 0, true);
	addStateWindows(skinManager, ClientState::StateWaitNoLandscape, "start");
	GLWWindowManager::instance()->addWindow(ClientState::StateWaitNoLandscape, 
		ScoreDialog::instance2(), 0, true);
	GLWWindowManager::instance()->addWindow(ClientState::StateWaitNoLandscape,
		PlayerInGameDialog::instance(), 0, false);
	GLWWindowManager::instance()->addWindow(ClientState::StateWaitNoLandscape,
		PlayerInitialDialog::instance(), 0, false);
	GLWWindowManager::instance()->addWindow(ClientState::StateWaitNoLandscape, 
 		SoundDialog::instance(), showSoundKey, false);
	GLWWindowManager::instance()->addWindow(ClientState::StateWaitNoLandscape, 
		MainMenuDialog::instance(), 0, true);
	addMessageComponents(skinManager, ClientState::StateWaitNoLandscape);
	if (ScorchedClient::instance()->getOptionsGame().getTutorial()[0])
	{
		GLWWindowManager::instance()->addWindow(ClientState::StateWaitNoLandscape, 
 			TutorialDialog::instance(), 0, true);
	}

	// StateLoadLevel
	GLWWindowManager::instance()->removeState(ClientState::StateLoadLevel);
	GLWWindowManager::instance()->addWindow(ClientState::StateLoadLevel,
		BackdropDialog::instance(), 0, true);
	addStateWindows(skinManager, ClientState::StateLoadLevel, "start");
	GLWWindowManager::instance()->addWindow(ClientState::StateLoadLevel,
		ProgressDialog::instance(), 0, true);
	GLWWindowManager::instance()->addWindow(ClientState::StateLoadLevel, 
 		SoundDialog::instance(), showSoundKey, false);

	// StateWait
	GLWWindowManager::instance()->removeState(ClientState::StateWait);
	addCommonComponents(skinManager, ClientState::StateWait);

	// StateBuyWeapons
	GLWWindowManager::instance()->removeState(ClientState::StateBuyWeapons);
	addCommonComponents(skinManager, ClientState::StateBuyWeapons);
	GLWWindowManager::instance()->addWindow(ClientState::StateBuyWeapons, 
		BuyAccessoryDialog::instance(), 0, true);
	GLWWindowManager::instance()->addWindow(ClientState::StateBuyWeapons, 
		GiftMoneyDialog::instance(), 0, false);

	// StateAutoDefense
	GLWWindowManager::instance()->removeState(ClientState::StateAutoDefense);
	addCommonComponents(skinManager, ClientState::StateAutoDefense);
	GLWWindowManager::instance()->addWindow(ClientState::StateAutoDefense, 
		AutoDefenseDialog::instance(), 0, false);

	// StatePlaying
	GLWWindowManager::instance()->removeState(ClientState::StatePlaying);
	addStateWindows(skinManager, ClientState::StatePlaying, "playing");
	KEYBOARDKEY("SHOW_PROFILE_DIALOG", profileKey);
	GLWWindowManager::instance()->addWindow(ClientState::StatePlaying, 
		ProfileDialog::instance(), profileKey, false);
	GLWWindowManager::instance()->addWindow(ClientState::StatePlaying, 
			SkipDialog::instance(), skipKey, false);
	GLWWindowManager::instance()->addWindow(ClientState::StatePlaying, 
			SkipAllDialog::instance(), 0, false);
	addCommonComponents(skinManager, ClientState::StatePlaying);

	// StateScore
	GLWWindowManager::instance()->removeState(ClientState::StateScore);
	addCommonComponents(skinManager, ClientState::StateScore);
	GLWWindowManager::instance()->addWindow(ClientState::StateScore,
		ScoreDialog::instance2(), 0, true);
}
