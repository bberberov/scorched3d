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

#include <stdio.h>
#include <graph/Display.h>
#include <graph/OptionsDisplay.h>
#include <common/Defines.h>

Display *Display::instance_ = 0;

Display *Display::instance()
{
	if (!instance_)
	{
		instance_ = new Display;
	}

	return instance_;
}

Display::Display()
{

}

Display::~Display()
{
}


bool Display::changeSettings(int width, int height, bool full)
{
	// set opengl double buffering 
	int doubleBuffer = OptionsDisplay::instance()->getDoubleBuffer()?1:0;
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, doubleBuffer);

	// set opengl component size 
	int componentSize = OptionsDisplay::instance()->getColorComponentSize();
	SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, componentSize);
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, componentSize);
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, componentSize);
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, componentSize);

	if (OptionsDisplay::instance()->getAntiAlias() > 0)
	{
		// Anti-aliased
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 
			OptionsDisplay::instance()->getAntiAlias());
	}
	
	// At least 24 bits depth buffer
	int depthBufferBits = OptionsDisplay::instance()->getDepthBufferBits();
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, depthBufferBits );

	// create display surface 
	int videoFlags = SDL_OPENGL | SDL_ANYFORMAT; // | SDL_RESIZABLE;  
	int flags = ( full ? videoFlags|SDL_FULLSCREEN : videoFlags);
	int bpp = OptionsDisplay::instance()->getBitsPerPixel();

	// Try to create suface
	surface = SDL_SetVideoMode( width, height, bpp, flags);

	// If this fails
	// Hack, to check if 16bit depth will work instead if 24 bits specified
	if (!surface &&
		depthBufferBits == 24)
	{
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
		surface = SDL_SetVideoMode( width, height, bpp, flags);
	}

	// And if this fails
	if (!surface)
	{
		char buffer[256];
		SDL_VideoDriverName(buffer, 256);
		S3D::dialogMessage("Display", S3D::formatStringBuffer(
			"ERROR: Failed to set video mode.\n"
			"Error Message: %s\n"
			"----------------------------\n"
			"Requested Display Mode:-\n"
			"Driver=%s\n"
			"Resolution=%ix%ix%i %s\n" 
			"DepthBuffer=%i\n"
			"DoubleBuffer=%s\n"
			"ColorComponentSize=%i\n",
			SDL_GetError(),
			buffer, 
			width, height, 
			OptionsDisplay::instance()->getBitsPerPixel(),
			(full?"(fullscreen)":"(windowed)"), 
			OptionsDisplay::instance()->getDepthBufferBits(),
			OptionsDisplay::instance()->getDoubleBuffer()?"On":"Off",
			OptionsDisplay::instance()->getColorComponentSize()));
		return false;
	}
	return true;
}

