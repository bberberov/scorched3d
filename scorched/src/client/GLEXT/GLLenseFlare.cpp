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

#include <math.h>
#include <GLEXT/GLState.h>
#include <GLEXT/GLCamera.h>
#include <image/ImageLuminanceFactory.h>
#include <GLEXT/GLLenseFlare.h>
#include <GLEXT/GLCameraFrustum.h>
#include <common/Defines.h>
#include <lang/LangResource.h>

GLLenseFlare *GLLenseFlare::instance_ = 0;

GLLenseFlare *GLLenseFlare::instance()
{
	if (!instance_)
	{
		instance_ = new GLLenseFlare;
	}

	return instance_;
}

GLLenseFlare::GLLenseFlare() : shineTic_(0)
{

}

GLLenseFlare::~GLLenseFlare()
{

}

void GLLenseFlare::setFlare(int index, int type, float scale, float loc, 
							Vector &color1, Vector &color2, Vector &color3, Vector &color4,
							float colorScale)
{
	if (index > 2) loc *= 500;

	flare_[index].type = type;
	flare_[index].loc = loc;
	flare_[index].scale = scale;
	flare_[index].color1 = color1 * colorScale;
	flare_[index].color2 = color2 * colorScale;
	flare_[index].color3 = color3 * colorScale;
	flare_[index].color4 = color4 * colorScale;
}

void GLLenseFlare::init(ProgressCounter *counter)
{
	if (counter) counter->setNewOp(LANG_RESOURCE("LENS_FLARES", "Lens Flares"));

	Vector red1(1.0f, 0.0f, 0.0f);
	Vector green1(0.0f, 1.0f, 0.0f);
	Vector blue1(0.0f, 0.0f, 1.0f);

	Vector red2(1.0f, 0.0f, 0.0f);
	Vector green2(0.5f, 1.0f, 0.0f);
	Vector blue2(0.5f, 0.0f, 1.0f);

	Vector red3(1.0f, 0.5f, 0.0f);
	Vector green3(0.0f, 1.0f, 0.0f);
	Vector blue3(0.0f, 0.5f, 1.0f);

	Vector red4(1.0f, 0.0f, 0.5f);
	Vector green4(0.0f, 1.0f, 0.5f);
	Vector blue4(0.0f, 0.0f, 1.0f);

	setFlare(0, -1, 1.0f, 0.3f, blue1, blue2, blue3, blue4, 1.0f);
	setFlare(1, -1, 1.0f, 0.2f, green1, green2, green3, green4, 1.f);
	setFlare(2, -1, 1.0f, 0.25f, red1, red2, red3, red4, 1.0f);

	/* Flares, ordered to eliminate redundant texture binds */
	setFlare(3, 1, 0.5f, 0.2f, red1, red2, red3, red4, 0.3f);
	setFlare(4, 2, 1.3f, 0.04f, red1, red2, red3, red4, 0.6f);
	setFlare(5, 3, 1.0f, 0.1f, red1, red2, red3, red4, 0.4f);
	setFlare(6, 3, 0.2f, 0.05f, red1, red2, red3, red4, 0.3f);
	setFlare(7, 0, 0.0f, 0.04f, red1, red2, red3, red4, 0.3f);
	setFlare(8, 5, -0.25f, 0.07f, red1, red2, red3, red4, 0.5f);
	setFlare(9, 5, -0.4f, 0.02f, red1, red2, red3, red4, 0.6f);
	setFlare(10, 5, -0.6f, 0.04f, red1, red2, red3, red4, 0.4f);
	setFlare(11, 5, -1.0f, 0.03f, red1, red2, red3, red4, 0.2f);

	int i;
	for (i = 0; i < 10; i++) 
	{
		if (counter) counter->setNewPercentage(float (i) / 16.0f * 100.0f);

		Image bitmap = ImageLuminanceFactory::loadFromFile(S3D::getModFile(S3D::formatStringBuffer("data/textures/lensflare/shine%d.bw", i)));
		shines_[i].create(bitmap);
	}

	for (i = 0; i < 6; i++) 
	{
		if (counter) counter->setNewPercentage(float (i+10) / 16.0f * 100.0f);

		Image bitmap = ImageLuminanceFactory::loadFromFile(S3D::getModFile(S3D::formatStringBuffer("data/textures/lensflare/flare%d.bw", i)));
		flares_[i].create(bitmap);
	}
}

void GLLenseFlare::draw(Vector &flarePos, 
	bool fullFlare, int colorNo, 
	float size, float alpha)
{
	if (GLCameraFrustum::instance()->sphereInFrustum(flarePos, 5))
	{
		Vector &cameraPos = GLCamera::getCurrentCamera()->getCurrentPos();
		Vector &cameraAt = GLCamera::getCurrentCamera()->getLookAt();

		Vector view_dir = flarePos - cameraPos;
		Vector centre = cameraPos + view_dir.Normalize() * 20.0f;
		Vector axis = (cameraAt - flarePos).Normalize();

		Vector dx = axis.Normalize();
		Vector dy = (dx * view_dir).Normalize();
		dx = -(dy * view_dir).Normalize();

		dx *= 2.0f * size;
		dy *= 2.0f * size;

		glDepthMask(GL_FALSE);
		GLState currentState(GLState::BLEND_ON);
		glBlendFunc(GL_ONE, GL_ONE);

		GLState *afterThreeState = 0;
		int endTexture = 12;
		if (!fullFlare) endTexture = 3;
		for (int i=0; i<endTexture; i++) 
		{
			Vector sx = dx * flare_[i].scale;
			Vector sy = dy * flare_[i].scale;
			static Vector position;

			switch(colorNo)
			{
				case 0: glColor4f(flare_[i].color1[0], flare_[i].color1[1], flare_[i].color1[2], alpha); break;
				case 1: glColor4f(flare_[i].color2[0], flare_[i].color2[1], flare_[i].color2[2], alpha); break;
				case 2: glColor4f(flare_[i].color3[0], flare_[i].color3[1], flare_[i].color3[2], alpha); break;
				case 3: glColor4f(flare_[i].color4[0], flare_[i].color4[1], flare_[i].color4[2], alpha); break;
			}
			if (flare_[i].type < 0) 
			{
				shines_[shineTic_].draw();
				shineTic_ = (shineTic_ + 1) % 10;

				position = flarePos + (axis * flare_[i].loc);
			} 
			else 
			{
				flares_[flare_[i].type].draw();

				position = flarePos + (axis * flare_[i].loc);
			}

			if (i==3) afterThreeState = new GLState(GLState::DEPTH_OFF);

			glBegin(GL_QUADS);

			static Vector tmp;

			tmp = position + sx + sy;
			glTexCoord2f(0.0, 0.0);
			glVertex3fv(tmp);

			tmp = (position - sx) + sy;
			glTexCoord2f(1.0, 0.0);
			glVertex3fv(tmp);

			tmp = (position - sx) - sy;
			glTexCoord2f(1.0, 1.0);
			glVertex3fv(tmp);

			tmp = (position + sx) - sy;
			glTexCoord2f(0.0, 1.0);
			glVertex3fv(tmp);

			glEnd();
		}

		glDepthMask(GL_TRUE);
		delete afterThreeState;
	}
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void GLLenseFlare::draw(Vector &flarePos, Vector &flareDir, int colorNo)
{
	if (!GLCameraFrustum::instance()->sphereInFrustum(flarePos, 5))
	{
		return;
	}

	Vector &cameraPos = GLCamera::getCurrentCamera()->getCurrentPos();
	Vector &cameraAt = GLCamera::getCurrentCamera()->getLookAt();

	Vector flare_dir = flareDir;// .Normalize(); // Should already be normalized
	Vector view_dir = (flarePos - cameraPos).Normalize();

	float dotP = (float) flare_dir.dotP(view_dir);
	if (dotP <= 0.2f) return;

	Vector centre = cameraPos + view_dir * 20.0f;
	Vector axis = (cameraAt - flarePos).Normalize();

	Vector dx = axis.Normalize();
	Vector dy = (dx * view_dir).Normalize();
	dx = -(dy * view_dir).Normalize();

	dx *= 2.0f * (dotP - 0.2f);
	dy *= 2.0f * (dotP - 0.2f);

	glDepthMask(GL_FALSE);
	GLState currentState(GLState::BLEND_ON | GLState::TEXTURE_ON);
	glBlendFunc(GL_ONE, GL_ONE);

	for (int i=0; i<3; i++) 
	{
		switch(colorNo)
		{
			case 0: glColor3fv(flare_[i].color1); break;
			case 1: glColor3fv(flare_[i].color2); break;
			case 2: glColor3fv(flare_[i].color3); break;
			case 3: glColor3fv(flare_[i].color4); break;
		}
	
		shines_[shineTic_].draw();
		shineTic_ = (shineTic_ + 1) % 10;

		Vector position = flarePos + (axis * flare_[i].loc);
		Vector sx = dx * flare_[i].scale;
		Vector sy = dy * flare_[i].scale;

		glBegin(GL_QUADS);
			static Vector tmp;
			tmp = position + sx + sy;
			glTexCoord2f(0.0, 0.0);
			glVertex3fv(tmp);

			tmp = (position - sx) + sy;
			glTexCoord2f(1.0, 0.0);
			glVertex3fv(tmp);

			tmp = (position - sx) - sy;
			glTexCoord2f(1.0, 1.0);
			glVertex3fv(tmp);

			tmp = (position + sx) - sy;
			glTexCoord2f(0.0, 1.0);
			glVertex3fv(tmp);

		glEnd();
	}
	glDepthMask(GL_TRUE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
