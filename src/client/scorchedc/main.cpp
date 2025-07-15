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

#include <client/ClientParams.h>
#include <client/ScorchedClient.h>
#include <client/ClientMain.h>
#include <server/ScorchedServer.h>
#include <engine/ModDirs.h>
#include <common/OptionsScorched.h>
#include <common/ARGParser.h>
#include <common/Defines.h>
#include <common/OptionsTransient.h>
#include <graph/OptionsDisplay.h>
#include <locale.h>
#include <math.h>
#include <signal.h>
#include <float.h>
#include <time.h>
#include <common/main.h>
#include <SDL/SDL.h>

void checkLaunchFile(char *progPath)
{
	if (!ClientParams::instance()->getNonParam()[0]) return;

	// Read launcher file
	FILE *in = fopen(ClientParams::instance()->getNonParam(), "r");
	if (!in)
	{
		S3D::dialogExit(scorched3dAppName, "Failed to open launchfile");
	}
	char buffer[2048];
	if (!fgets(buffer, 2048, in))
	{
		S3D::dialogExit(scorched3dAppName, "Failed to read launchfile");
	}
	fclose(in);

	// Set the launch
	ClientParams::instance()->setConnect(buffer);
}

int main(int argc, char *argv[])
{
	setlocale(LC_ALL, "C");

	// From main.h
	run_main(argc, argv, *ClientParams::instance());
	checkLaunchFile(argv[0]);

	// Read display options from a file
	// **This NEEDS to be after the arg parser**
	if (!OptionsDisplay::instance()->readOptionsFromFile())
	{
		return 0;
	}

	// Get this host's description and username
	if (!OptionsDisplay::instance()->getHostDescription()[0])
	{
		OptionsDisplay::instance()->getHostDescriptionEntry().setValue(S3D::getOSDesc());
		OptionsDisplay::instance()->getOnlineUserNameEntry().setValue("Player");
	}

	// Write the new options back the the file
	OptionsDisplay::instance()->writeOptionsToFile(ClientParams::instance()->getWriteFullOptions());

	// Init SDL
	unsigned int initFlags = SDL_INIT_VIDEO;
	if (ClientParams::instance()->getAllowExceptions()) initFlags |= SDL_INIT_NOPARACHUTE;
	if (SDL_Init(initFlags) < 0)
	{
		S3D::dialogExit(
			scorched3dAppName,
			"This game uses the SDL library to provide graphics.\n"
			"The graphics section of this library failed to initialize.");
	}

	if (!ClientMain::clientMain()) exit(64);

	// Write display options back to the file
	// in case they have been changed by this client (in game by the console)
	OptionsDisplay::instance()->writeOptionsToFile(ClientParams::instance()->getWriteFullOptions());

	SDL_Quit();
	return 0; // exit(0)
}
