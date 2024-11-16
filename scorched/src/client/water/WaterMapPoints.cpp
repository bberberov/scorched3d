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

#include <water/WaterMapPoints.h>
#include <water/Water2Patches.h>
#include <graph/ModelRendererSimulator.h>
#include <graph/ModelRendererMesh.h>
#include <landscape/MapPoints.h>
#include <landscapemap/LandscapeMaps.h>
#include <GLEXT/GLGlobalState.h>
#include <client/ScorchedClient.h>
#include <common/OptionsTransient.h>

WaterMapPoints::WaterMapPoints()
{
}

WaterMapPoints::~WaterMapPoints()
{
}

void WaterMapPoints::draw(Water2Patches &currentPatch)
{
	GLGlobalState currentState(GLState::TEXTURE_OFF);
	for (int i=0; i<(int) pts_.size(); i++)
	{
		Vector &current = pts_[i];
		Water2Patch::Data *currentData = 
			currentPatch.getPoint(int(current[0]) / 2, int(current[1]) / 2);

		glPushMatrix();
			glTranslatef(current[0], current[1], currentData->z + 0.6f);
			glRotatef(currentData->nx * 90.0f, 1.0f, 0.0f, 0.0f);
			glRotatef(currentData->ny * 90.0f, 0.0f, 1.0f, 0.0f);
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

void WaterMapPoints::generate()
{
	pts_.clear();

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

		pts_.push_back(Vector(arenaX + pos, arenaY));
		pts_.push_back(Vector(arenaX + pos, arenaY + arenaHeight));
	}
	for (i=1; i<=pointsY-1; i++)
	{
		int pos = 32 * i;

		pts_.push_back(Vector(arenaX, arenaY + pos));
		pts_.push_back(Vector(arenaX + arenaWidth, arenaY + pos));
	}
}
