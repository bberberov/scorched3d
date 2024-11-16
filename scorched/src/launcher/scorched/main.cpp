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

#include <scorched/ScorchedParams.h>
#include <engine/ModDirs.h>
#include <common/OptionsGame.h>
#include <common/ARGParser.h>
#include <common/Defines.h>
#include <graph/OptionsDisplay.h>
#include <wx/wx.h>
#include <wx/utils.h>
#include <locale.h>
#include <math.h>
#include <signal.h>
#include <float.h>
#include <time.h>
#include <common/main.h>

bool newVersion = false;

int main(int argc, char *argv[])
{
	setlocale(LC_ALL, "C");

	// From main.h
	run_main(argc, argv, *ScorchedParams::instance());

	// Read display options from a file
	// **This NEEDS to be after the arg parser**
	if (!OptionsDisplay::instance()->readOptionsFromFile())
	{
		return false;
	}

	// Get this host's description and username
	if (!OptionsDisplay::instance()->getHostDescription()[0])
	{
		OptionsDisplay::instance()->getHostDescriptionEntry().setValue(S3D::getOSDesc());
		OptionsDisplay::instance()->getOnlineUserNameEntry().setValue("Player");
	}

	// Check if this a new version we've seen
	if (S3D::stristr(S3D::ScorchedVersion.c_str(), "BETA"))
	{
		newVersion = true;
	}
	else if (0 != strcmp(S3D::ScorchedVersion.c_str(), OptionsDisplay::instance()->getLastVersionPlayed()))
	{
		newVersion = true;
		OptionsDisplay::instance()->getLastVersionPlayedEntry().setValue(S3D::ScorchedVersion);
	}

	// Write the new options back the the file
	OptionsDisplay::instance()->writeOptionsToFile(ScorchedParams::instance()->getWriteFullOptions());

	// Init SDL
	unsigned int initFlags = SDL_INIT_VIDEO;
	if (ScorchedParams::instance()->getAllowExceptions()) initFlags |= SDL_INIT_NOPARACHUTE;
	if (SDL_Init(initFlags) < 0)
	{
		S3D::dialogMessage(
			scorched3dAppName,
			"Warning: This game uses the SDL library to provide graphics.\n"
			"The graphics section of this library failed to initialize.\n"
			"You will only be able to run a server for this game.");
	}
	else
	{
		ScorchedParams::instance()->getSDLInitVideo() = true;
	}

#ifdef _WIN32
		wxEntry((WXHINSTANCE) (HINSTANCE) GetModuleHandle(NULL),
			(WXHINSTANCE) NULL, "", SW_SHOWNORMAL);
#else
		wxEntry(argc, argv);
#endif

	SDL_Quit();
	return 0; // exit(0)
}
