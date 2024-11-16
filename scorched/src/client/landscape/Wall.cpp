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

#include <client/ScorchedClient.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscape/Wall.h>
#include <sound/SoundUtils.h>
#include <image/ImageFactory.h>
#include <common/Defines.h>

Wall::Wall() : texture_(ImageID(S3D::eModLocation,
	"data/textures/bordershield/grid.bmp",
	"data/textures/bordershield/grid.bmp",
	false))
{
	for (int i=0; i<4; i++) fadeTime_[i] = 0.0f;
}

Wall::~Wall()
{
}

void Wall::draw()
{
	float arenaX = (float) ScorchedClient::instance()->getLandscapeMaps().
			getGroundMaps().getArenaX();
	float arenaY = (float) ScorchedClient::instance()->getLandscapeMaps().
			getGroundMaps().getArenaY();
	float arenaWidth = (float) ScorchedClient::instance()->getLandscapeMaps().
			getGroundMaps().getArenaWidth();
	float arenaHeight = (float) ScorchedClient::instance()->getLandscapeMaps().
			getGroundMaps().getArenaHeight();

	Vector botA(arenaX, arenaY, 0.0f);
	Vector botB(arenaX + arenaWidth, arenaY, 0.0f);
	Vector botC(arenaX + arenaWidth, arenaY + arenaHeight, 0.0f);
	Vector botD(arenaX, arenaY + arenaHeight, 0.0f);
	Vector topA(arenaX, arenaY, 256.0f);
	Vector topB(arenaX + arenaWidth, arenaY, 256.0f);
	Vector topC(arenaX + arenaWidth, arenaY + arenaHeight, 256.0f);
	Vector topD(arenaX, arenaY + arenaHeight, 256.0f);

	if (fadeTime_[OptionsTransient::LeftSide] > 0.0f ||
		fadeTime_[OptionsTransient::BotSide] > 0.0f ||
		fadeTime_[OptionsTransient::RightSide] > 0.0f ||
		fadeTime_[OptionsTransient::TopSide] > 0.0f)
	{
		GLState currentState(GLState::BLEND_ON | GLState::TEXTURE_ON);

		texture_.draw();
		drawWall(botA, botB, topB, topA, fadeTime_[OptionsTransient::TopSide]);
		drawWall(botB, botC, topC, topB, fadeTime_[OptionsTransient::RightSide]);
		drawWall(botC, botD, topD, topC, fadeTime_[OptionsTransient::BotSide]);
		drawWall(botD, botA, topA, topD, fadeTime_[OptionsTransient::LeftSide]);
	}
}

void Wall::drawWall(Vector &cornerA, Vector &cornerB, 
						 Vector &cornerC, Vector &cornerD,
						 float fade)
{
	if (fade <= 0.0f) return;

	int rot = 0;//int(fade * 75) % 2;
	float pos = float(int(fade * 75) % 2) * 5.0f;

	Vector &wallColor = ScorchedClient::instance()->getOptionsTransient().getWallColor();
	glColor4f(wallColor[0], wallColor[1], wallColor[2], fade);
	glBegin(GL_QUADS);
		// Don't draw the wall if we are behind it
		// as its z-ordering obscures other objects
		/*switch(rot)
		{
		case 0: glTexCoord2f(20.0f + pos, 20.0f + pos); break;
		case 1: glTexCoord2f(0.0f + pos, 20.0f + pos); break;
		}
		glVertex3fv(cornerA);
		switch(rot)
		{
		case 0: glTexCoord2f(0.0f + pos, 20.0f + pos); break;
		case 1: glTexCoord2f(0.0f + pos, 0.0f + pos); break;
		}		
		glVertex3fv(cornerB);
		switch(rot)
		{
		case 0: glTexCoord2f(0.0f, 0.0f); break;
		case 1: glTexCoord2f(20.0f, 0.0f); break;
		}	
		glVertex3fv(cornerC);
		switch(rot)
		{
		case 0: glTexCoord2f(20.0f, 0.0f); break;
		case 1: glTexCoord2f(20.0f, 20.0f); break;
		}	
		glVertex3fv(cornerD);*/

		switch(rot)
		{
		case 0: glTexCoord2f(20.0f + pos, 0.0f + pos); break;
		case 1: glTexCoord2f(20.0f + pos, 20.0f + pos); break;
		}	
		glVertex3fv(cornerD);
		switch(rot)
		{
		case 0: glTexCoord2f(0.0f + pos, 0.0f + pos); break;
		case 1: glTexCoord2f(20.0f + pos, 0.0f + pos); break;
		}	
		glVertex3fv(cornerC);
		switch(rot)
		{
		case 0: glTexCoord2f(0.0f, 20.0f); break;
		case 1: glTexCoord2f(0.0f, 0.0f); break;
		}	
		glVertex3fv(cornerB);
		switch(rot)
		{
		case 0: glTexCoord2f(20.0f, 20.0f); break;
		case 1: glTexCoord2f(0.0f, 20.0f); break;
		}
		glVertex3fv(cornerA);
	glEnd();
}

void Wall::wallHit(Vector &position, OptionsTransient::WallSide side)
{
	fadeTime_[side] = 1.0f;
	CACHE_SOUND(sound, S3D::getModFile("data/wav/shield/hit2.wav"));
	SoundUtils::playAbsoluteSound(VirtualSoundPriority::eAction,
		sound, position);
}

void Wall::simulate(float time)
{
	fadeTime_[OptionsTransient::LeftSide] -= time / 2.0f;
	fadeTime_[OptionsTransient::BotSide] -= time / 2.0f;
	fadeTime_[OptionsTransient::TopSide] -= time / 2.0f;
	fadeTime_[OptionsTransient::RightSide] -= time / 2.0f;
}
