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

#include <graph/SoftwareMouse.h>
#include <client/ScorchedClient.h>
#include <engine/GameState.h>
#include <image/ImageFactory.h>
#include <GLEXT/GLState.h>
#include <common/Defines.h>
#include <graph/OptionsDisplay.h>
#include <SDL/SDL.h>

SoftwareMouse *SoftwareMouse::instance_ = 0;

SoftwareMouse *SoftwareMouse::instance()
{
	if (!instance_)
	{
		instance_ = new SoftwareMouse;
	}
	return instance_;
}

SoftwareMouse::SoftwareMouse() :
	GameStateI("SoftwareMouse")
{
}

SoftwareMouse::~SoftwareMouse()
{
}

void SoftwareMouse::draw(const unsigned currentstate)
{
	if (!OptionsDisplay::instance()->getSoftwareMouse()) return;

	static bool createdTexture = false;
	if (!createdTexture)
	{
		createdTexture = true;
		Image map = ImageFactory::loadImage(
			S3D::eDataLocation,
			"data/images/pointer.bmp",
			"data/images/pointera.bmp");
		mouseTex_.create(map, false);

		SDL_ShowCursor(SDL_DISABLE);
	}

	GLState state(GLState::BLEND_ON | GLState::TEXTURE_ON | GLState::DEPTH_OFF); 

	int mouseX = ScorchedClient::instance()->getGameState().getMouseX();
	int mouseY = ScorchedClient::instance()->getGameState().getMouseY();

	mouseTex_.draw();	
	glPushMatrix();
		glTranslatef(float(mouseX), float(mouseY), 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		
		glScalef(2.0f, 2.0f, 2.0f); // Size of the mouse pointer

		glTranslatef(0.0f, -16.0f, 0.0f);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex2f(0.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex2f(16.0f, 0.0f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex2f(16.0f, 16.0f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex2f(0.0f, 16.0f);
		glEnd();
	glPopMatrix();
}

