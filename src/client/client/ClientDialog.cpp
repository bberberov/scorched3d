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

#include <client/ClientDialog.h>
#include <client/LoadPNG.h>
#include <graph/GLSetup.h>
#include <graph/MainCamera.h>
#include <graph/Main2DCamera.h>
#include <graph/OptionsDisplay.h>
#include <graph/Display.h>
#include <graph/Gamma.h>
#include <client/ClientParams.h>
#include <GLEXT/GLStateExtension.h>
#include <common/OptionsTransient.h>
#include <common/OptionsScorched.h>
#include <common/Defines.h>
#include <stdio.h>
#include <time.h>

extern char scorched3dAppName[128];

bool createScorchedWindow()
{
	int width = OptionsDisplay::instance()->getScreenWidth();
	int height = OptionsDisplay::instance()->getScreenHeight();
	bool fullscreen = OptionsDisplay::instance()->getFullScreen();

	SDL_WM_SetCaption(scorched3dAppName, "Scorched3D");
	std::string iconFile = S3D::getDataFile("data/images/tank2.png");

	SDL_Surface *icon = EXT_LoadPNG_RW(SDL_RWFromFile(iconFile.c_str(), "rb"));
	SDL_WM_SetIcon(icon, NULL);
	SDL_FreeSurface(icon);

	if (!Display::instance()->changeSettings(width,height,fullscreen)) 
	{
		S3D::dialogMessage("Scorched 3D Display", 
			"ERROR: Failed to set the display mode.\n"
			"Ensure that no other application is exclusively using the graphics hardware.\n"
			"Ensure that the current desktop mode has at least 24 bits colour depth.\n");
		return false;
	}

	int windowWidth = 1024;
	int windowHeight = 768;
	switch (OptionsDisplay::instance()->getDialogSize())
	{
	case 0:
		windowWidth = 1600;
		break;
	case 1:
		windowWidth = 1152;
		break;
	case 3:
		windowWidth = 800;
		break;
	}
	windowHeight = (height * windowWidth) / width;

	MainCamera::instance()->getCamera().setWindowSize(width, height);
	Main2DCamera::instance()->getViewPort().setWindowSize(windowWidth, windowHeight, 
		width, height);

	Gamma::instance()->save();
	Gamma::instance()->set();

	GLSetup::setup();

	if (OptionsDisplay::instance()->getOpenGLWarnings() &&
		GLStateExtension::isSoftwareOpenGL())
	{
		S3D::dialogMessage("Scorched 3D Display", 
			S3D::formatStringBuffer(
			"Warning: This computer is not using the graphics card to renderer OpenGL.\n"
			"This may cause the game to play very slowly!\n\n"
			"Please update your graphics drivers from the appropriate website.\n\n"
			"OpenGL Vendor : %s\nOpenGL Renderer : %s",
			glGetString(GL_VENDOR), glGetString(GL_RENDERER)));
	}

	return true;
}
