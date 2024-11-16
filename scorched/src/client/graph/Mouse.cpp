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

#include <GLEXT/GLViewPort.h>
#include <graph/Mouse.h>
#include <graph/OptionsDisplay.h>
#include <common/OptionsScorched.h>
#include <client/ScorchedClient.h>
#include <engine/GameState.h>

Mouse *Mouse::instance_ = 0;

Mouse *Mouse::instance()
{
	if (!instance_) {
		instance_ = new Mouse;
	}

	return instance_;
}

Mouse::Mouse() : mouse_sensitivity_(120)
{

}

Mouse::~Mouse()
{

}



void Mouse::mouseDown(SDL_Event &event)
{
	int defaultY = (int) (event.button.y * GLViewPort::getHeightMult());
	if (!OptionsDisplay::instance()->getSwapYAxis())
	{
		defaultY = GLViewPort::getHeight() - defaultY;
	}

	switch (event.button.button) {
	case SDL_BUTTON_LEFT:
		ScorchedClient::instance()->getGameState().
		    mouseDown(GameState::MouseButtonLeft,
				(int) (event.button.x * GLViewPort::getWidthMult()),
				defaultY);
		break;
	case SDL_BUTTON_MIDDLE:
		ScorchedClient::instance()->getGameState().
		    mouseDown(GameState::MouseButtonMiddle,
			      (int) (event.button.x * GLViewPort::getWidthMult()),
			      defaultY);
		break;
	case SDL_BUTTON_RIGHT:
		ScorchedClient::instance()->getGameState().
		    mouseDown(GameState::MouseButtonRight,
			      (int) (event.button.x * GLViewPort::getWidthMult()),
			      defaultY);
		break;
	case 4:
		ScorchedClient::instance()->getGameState().
			mouseWheel(-mouse_sensitivity_);
		break;
	case 5:
		ScorchedClient::instance()->getGameState().
			mouseWheel(mouse_sensitivity_);
		break;
	default:
		break;
	}
}

void Mouse::mouseUp(SDL_Event &event)
{
	int defaultY = (int) (event.button.y * GLViewPort::getHeightMult());
	if (!OptionsDisplay::instance()->getSwapYAxis())
	{
		defaultY = GLViewPort::getHeight() - defaultY;
	}

	switch (event.button.button) {
	case SDL_BUTTON_LEFT:
		ScorchedClient::instance()->getGameState().
			mouseUp(GameState::MouseButtonLeft,
				(int) (event.button.x * GLViewPort::getWidthMult()),
				defaultY);
		break;
	case SDL_BUTTON_MIDDLE:
		ScorchedClient::instance()->getGameState().
		    mouseUp(GameState::MouseButtonMiddle,
			    (int) (event.button.x * GLViewPort::getWidthMult()),
				defaultY);
		break;
	case SDL_BUTTON_RIGHT:
		ScorchedClient::instance()->getGameState().
			mouseUp(GameState::MouseButtonRight,
				(int) (event.button.x * GLViewPort::getWidthMult()),
				defaultY);
		break;
	default:
		break;
	}
}

void Mouse::mouseMove(SDL_Event &event)
{
	int defaultY = (int) (event.button.y * GLViewPort::getHeightMult());
	if (!OptionsDisplay::instance()->getSwapYAxis())
	{
		defaultY = GLViewPort::getHeight() - defaultY;
	}

	ScorchedClient::instance()->getGameState().
		mouseMove(
			(int) (event.button.x * GLViewPort::getWidthMult()),
			defaultY);
}

void Mouse::processMouseEvent(SDL_Event & event)
{
	switch (event.type) {
	case SDL_MOUSEBUTTONDOWN:
		mouseDown(event);
		break;
	case SDL_MOUSEBUTTONUP:
		mouseUp(event);
		break;
	case SDL_MOUSEMOTION:
		mouseMove(event);
		break;
	}
}
