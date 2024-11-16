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

#include <landscape/LandscapeStateHandler.h>
#include <landscape/Landscape.h>

LandscapeStateLandHandler::LandscapeStateLandHandler() :
	GameStateI("LandscapeLand")
{
}

LandscapeStateLandHandler::~LandscapeStateLandHandler()
{
}

void LandscapeStateLandHandler::draw(const unsigned state)
{
	Landscape::instance()->calculateVisibility();
	Landscape::instance()->drawShadows();
	Landscape::instance()->drawLand();
}

void LandscapeStateLandHandler::simulate(const unsigned state, float frameTime)
{
	Landscape::instance()->simulate(frameTime);
}

LandscapeStateWaterHandler::LandscapeStateWaterHandler() :
	GameStateI("LandscapeWater")
{
}

LandscapeStateWaterHandler::~LandscapeStateWaterHandler()
{
}

void LandscapeStateWaterHandler::draw(const unsigned state)
{
	Landscape::instance()->drawWater();
}

void LandscapeStateWaterHandler::simulate(const unsigned state, float frameTime)
{
}

LandscapeStateObjectsHandler::LandscapeStateObjectsHandler() :
	GameStateI("LandscapeObjects")
{
}

LandscapeStateObjectsHandler::~LandscapeStateObjectsHandler()
{
}

void LandscapeStateObjectsHandler::draw(const unsigned state)
{
	Landscape::instance()->drawObjects();
}

void LandscapeStateObjectsHandler::simulate(const unsigned state, float frameTime)
{
}
