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

#include <landscape/LandscapePoints.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscape/MapPoints.h>
#include <client/ScorchedClient.h>
#include <common/OptionsTransient.h>
#include <graph/ModelRendererSimulator.h>
#include <graph/ModelRendererMesh.h>
#include <GLEXT/GLGlobalState.h>

LandscapePoints::LandscapePoints()
{
}

LandscapePoints::~LandscapePoints()
{
}

void LandscapePoints::draw()
{
	GLGlobalState currentState(GLState::TEXTURE_OFF);
	for (int i=0; i<(int) points_.size(); i++)
	{
		Vector &current = points_[i];
		current[2] = 
			ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getHeight(
				(int) current[0], (int) current[1]).asFloat();
		glPushMatrix();
			glTranslatef(current[0], current[1], current[2]);
			glScalef(0.15f, 0.15f, 0.15f);
			switch(ScorchedClient::instance()->getOptionsTransient().getWallType())
			{
			case OptionsTransient::wallWrapAround:
				MapPoints::instance()->getBorderModelWrap()->draw();
				break;
			case OptionsTransient::wallBouncy:
				MapPoints::instance()->getBorderModelBounce()->draw();
				break;
			case OptionsTransient::wallConcrete:
				MapPoints::instance()->getBorderModelConcrete()->draw();
				break;
			default:
				break;
			}
		glPopMatrix();
	}
}

void LandscapePoints::generate()
{
	points_.clear();

	int arenaX = ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getArenaX();
	int arenaY = ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getArenaY();
	int arenaWidth = ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getArenaWidth();
	int arenaHeight = ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getArenaHeight();

	int pointsX = arenaWidth / 32; // Each point is 32 units appart
	int pointsY = arenaHeight / 32; // Each point is 32 units appart
	
	int i;
	for (i=0; i<=pointsX; i++)
	{
		int pos = 32 * i;

		points_.push_back(Vector(arenaX + pos, arenaY));
		points_.push_back(Vector(arenaX + pos, arenaY + arenaHeight));
	}
	for (i=1; i<=pointsY-1; i++)
	{
		int pos = 32 * i;

		points_.push_back(Vector(arenaX, arenaY + pos));
		points_.push_back(Vector(arenaX + arenaWidth, arenaY + pos));
	}
}

