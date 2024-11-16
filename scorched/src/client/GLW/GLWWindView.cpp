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

#include <GLW/GLWWindView.h>
#include <client/ScorchedClient.h>
#include <graph/MainCamera.h>
#include <GLEXT/GLViewPort.h>
#include <common/Defines.h>
#include <engine/Simulator.h>
#include <3dsparse/ModelStore.h>
#include <graph/ModelRenderer.h>
#include <graph/ModelRendererStore.h>
#include <graph/ModelRendererSimulator.h>
#include <landscape/Landscape.h>
#include <landscapemap/LandscapeMaps.h>
#include <lang/LangResource.h>
#include <sky/Sky.h>
#include <math.h>

REGISTER_CLASS_SOURCE(GLWWindView);

WindDialogToolTip::WindDialogToolTip()
{
}

WindDialogToolTip::~WindDialogToolTip()
{
}

void WindDialogToolTip::populate()
{
	LangString wallTypeStr = LANG_RESOURCE("WALLS_NONE", "Currently no walls");
	OptionsTransient::WallType wallType =
		ScorchedClient::instance()->getOptionsTransient().getWallType();
	switch (wallType)
	{
	case OptionsTransient::wallBouncy:
		wallTypeStr = LANG_RESOURCE("WALLS_BOUNCY", "Current Wall Type : Bouncy");
		break;
	case OptionsTransient::wallConcrete:
		wallTypeStr = LANG_RESOURCE("WALLS_CONCRETE", "Current Wall Type : Concrete");
		break;
	case OptionsTransient::wallWrapAround:
		wallTypeStr = LANG_RESOURCE("WALL_WRAP", "Current Wall Type : Wrap Around");
		break;
	}

	if (ScorchedClient::instance()->
		getSimulator().getWind().getWindSpeed() == 0)
	{
		setText(ToolTip::ToolTipHelp, 
			LANG_RESOURCE("WIND", "Wind"), 
			LANG_RESOURCE("WIND_TOOLTIP_NOWIND", 
			"Displays the current wind direction\n"
			"and speed, and the wall type.\n"
			"Currently No Wind.\n") + 
			wallTypeStr);
	}
	else
	{
		setText(ToolTip::ToolTipHelp, LANG_RESOURCE("WIND", "Wind"), 
			LANG_RESOURCE_1("WIND_TOOLTIP_WIND",
			"Displays the current wind direction\n"
			"and speed, and the wall type.\n"
			"Current Wind Force : {0} (out of 5)\n",
			S3D::formatStringBuffer("%i", (int) ScorchedClient::instance()->
			getSimulator().getWind().getWindSpeed().asFloat())) +
			wallTypeStr);
	}
}

GLWWindView::GLWWindView(float x, float y, float w, float h) :
	GLWidget(x, y, w, h),
	listNo_(0), changeCount_(0), windModel_(0)
{
	setToolTip(new WindDialogToolTip());
}

GLWWindView::~GLWWindView()
{
	delete windModel_;
	if (glIsList(listNo_)) glDeleteLists(listNo_, 1);
}

void GLWWindView::draw()
{
	if (!windModel_)
	{
		ModelID id;
		id.initFromString("ase", "data/meshes/wind.ase", "none");
		windModel_ = new ModelRendererSimulator(
			ModelRendererStore::instance()->loadModel(id));
	}

	if (changeCount_ != Landscape::instance()->getChangeCount())
	{
		changeCount_ = Landscape::instance()->getChangeCount();
		if (glIsList(listNo_)) glDeleteLists(listNo_, 1);
		listNo_ = 0;
	}

	GLWidget::draw();
	drawDisplay();
}

void GLWWindView::drawDisplay()
{
	Vector &lookFrom = MainCamera::instance()->getCamera().getCurrentPos();
	Vector &lookAt = MainCamera::instance()->getCamera().getLookAt();
	Vector dir = (lookAt - lookFrom).Normalize();

	float scale2 = MIN(w_, h_) / 90.0f;
	float angXY = atan2f(dir[0], dir[1]) / 3.14f * 180.0f;
	float angYZ = acosf(dir[2]) / 3.14f * 180.0f + 180.0f;
	if (angYZ < 280.0f) angYZ = 280.0f;

	GLState currentState(GLState::DEPTH_ON | GLState::TEXTURE_ON);
	glPushMatrix();
		glTranslatef(x_ + w_ / 2.0f, y_ + h_ / 2.0f, 0.0f);
		glRotatef(angYZ, 1.0f, 0.0f, 0.0f);
		glRotatef(angXY, 0.0f, 0.0f, 1.0f);

		Landscape::instance()->getSky().getSun().setLightPosition(); // Reset light

		// Draw the minature landscape
		glScalef(scale2, scale2, scale2);
		glPushMatrix();
			if (!listNo_)
			{
				glNewList(listNo_ = glGenLists(1), GL_COMPILE);
					drawScene();
				glEndList();
			}
			glCallList(listNo_);
		glPopMatrix();

		// Draw the wind arrow for direction
		GLState texState(GLState::TEXTURE_OFF);
		if (ScorchedClient::instance()->getSimulator().getWind().getWindOn())
		{
			Vector4 sunPosition(-100.0f, 100.0f, 400.0f, 1.0f);
			Vector4 sunDiffuse(0.9f, 0.9f, 0.9f, 1.0f);
			Vector4 sunAmbient(0.4f, 0.4f, 0.4f, 1.0f);
			glLightfv(GL_LIGHT1, GL_AMBIENT, sunAmbient);
			glLightfv(GL_LIGHT1, GL_DIFFUSE, sunDiffuse);
			glLightfv(GL_LIGHT1, GL_POSITION, sunPosition);

			glTranslatef(0.0f, 0.0f, 20.0f);
			glScalef(0.1f, 0.1f, 0.1f);

			glRotatef(-ScorchedClient::instance()->getSimulator().getWind().getWindAngle().asFloat(), 
				0.0f, 0.0f, 1.0f);
			drawArrow();
		}
	glPopMatrix();

	Landscape::instance()->getSky().getSun().setLightPosition(); // Reset light
}

void GLWWindView::drawScene()
{
	Landscape::instance()->getPlanTexture().draw(true);
	glColor3f(1.0f, 1.0f, 1.0f);

	HeightMap &hmp = ScorchedClient::instance()->getLandscapeMaps().
		getGroundMaps().getHeightMap();

	int arenaX = ScorchedClient::instance()->getLandscapeMaps().
		getGroundMaps().getArenaX();
	int arenaY = ScorchedClient::instance()->getLandscapeMaps().
		getGroundMaps().getArenaY();
	int arenaWidth = ScorchedClient::instance()->getLandscapeMaps().
		getGroundMaps().getArenaWidth();
	int arenaHeight = ScorchedClient::instance()->getLandscapeMaps().
		getGroundMaps().getArenaHeight();
	int landscapeWidth = ScorchedClient::instance()->getLandscapeMaps().
		getGroundMaps().getLandscapeWidth();
	int landscapeHeight = ScorchedClient::instance()->getLandscapeMaps().
		getGroundMaps().getLandscapeHeight();

	int maxSize = MAX(arenaWidth, arenaHeight);
	float scale = 60.0f / maxSize;
	glScalef(scale, scale, scale);
	glTranslatef(arenaWidth/-2.0f-arenaX, arenaHeight/-2.0f-arenaY, 0.0f);

	int sqSizeW = arenaWidth / 16;
	int sqSizeH = arenaHeight / 16;
	for (int y=arenaY; y<=arenaY+arenaHeight-sqSizeH; y+=sqSizeH)
	{
		glBegin(GL_QUAD_STRIP);
		for (int x=arenaX; x<=arenaX+arenaWidth; x+=sqSizeW)
		{
			float xPer = float(x) / float(landscapeWidth);
			float yPer = float(y) / float(landscapeHeight);
			float yPer2 = float(y + sqSizeH) / float(landscapeHeight);

			glTexCoord2f(xPer, yPer2);
			glVertex3f(float(x), float(y + sqSizeH), hmp.getHeight(x, y+sqSizeH).asFloat());
			glTexCoord2f(xPer, yPer);
			glVertex3f(float(x), float(y), hmp.getHeight(x, y).asFloat());
		}
		glEnd();
	}

	Landscape::instance()->getMagTexture().draw();
	glBegin(GL_QUAD_STRIP);
	for (int y=arenaY; y<=arenaY+arenaHeight; y+=sqSizeH)
	{
		float per = float(y - arenaY) / float(arenaHeight);

		glTexCoord2f(per, 0.0f);
		glVertex3f(float(arenaX), float(y), -15.0f);
		glTexCoord2f(per, 1.0f);
		glVertex3f(float(arenaX), float(y), hmp.getHeight(arenaX, y).asFloat());		
	}
	for (int x=arenaX; x<=arenaX+arenaWidth; x+=sqSizeW)
	{
		float per = float(x - arenaX) / float(arenaWidth);

		glTexCoord2f(per, 0.0f);
		glVertex3f(float(x), float(arenaY + arenaHeight), -15.0f);
		glTexCoord2f(per, 1.0f);
		glVertex3f(float(x), float(arenaY + arenaHeight), hmp.getHeight(x, arenaY + arenaHeight).asFloat());		
	}
	for (int y=arenaY+arenaHeight; y>=arenaY; y-=sqSizeH)
	{
		float per = float(y - arenaY) / float(arenaHeight);

		glTexCoord2f(per, 0.0f);
		glVertex3f(float(arenaX + arenaWidth), float(y), -15.0f);
		glTexCoord2f(per, 1.0f);
		glVertex3f(float(arenaX + arenaWidth), float(y), hmp.getHeight(arenaX + arenaWidth, y).asFloat());		
	}
	for (int x=arenaX+arenaWidth; x>=arenaX; x-=sqSizeW)
	{
		float per = float(x - arenaX) / float(arenaWidth);

		glTexCoord2f(per, 0.0f);
		glVertex3f(float(x), float(arenaY), -15.0f);
		glTexCoord2f(per, 1.0f);
		glVertex3f(float(x), float(arenaY), hmp.getHeight(x, arenaY).asFloat());		
	}
	glEnd();
}

void GLWWindView::drawArrow()
{
	windModel_->draw();
}

void GLWWindView::mouseDown(int button, float x, float y, bool &skipRest)
{
	if (inBox(x, y, x_, y_, w_, h_))
	{
		skipRest = true;

		unsigned int type =
			MainCamera::instance()->getTarget().getCameraType();
		type++;
		if (type >= TargetCamera::CamFree) type = 0;

		MainCamera::instance()->getTarget().setCameraType(
			(TargetCamera::CamType) type);
	}
}
