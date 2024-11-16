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

#include <dialogs/ProfileDialog.h>
#include <GLEXT/GLViewPort.h>
#include <GLEXT/GLState.h>
#include <GLW/GLWFont.h>
#include <client/ScorchedClient.h>
#include <client/ClientState.h>
#include <graph/Main2DCamera.h>
#include <graph/OptionsDisplay.h>
#include <graph/ModelRendererSimulator.h>
#include <target/TargetContainer.h>
#include <tanket/TanketShotInfo.h>
#include <target/TargetLife.h>
#include <landscapemap/LandscapeMaps.h>
#include <tankgraph/TargetRendererImplTank.h>
#include <sprites/ExplosionTextures.h>
#include <XML/XMLNode.h>

const int INSET = 8;

ProfileDialog *ProfileDialog::instance_ = 0;

ProfileDialog *ProfileDialog::instance()
{
	if (!instance_)
	{
		instance_ = new ProfileDialog;
	}

	return instance_;
}

ProfileDialog::ProfileDialog() : 
	GLWWindow("Profile", 200, 300, 640, 200, eTransparent | 
	eResizeable | eSmallTitle | eSavePosition,
	"Shows a 2D side view of the landscape and tanks"),
	profileZoom_(1.0f),
	zooming_(false)
{
}

ProfileDialog::~ProfileDialog()
{
}

void ProfileDialog::saveSettings(XMLNode *node)
{
	GLWWindow::saveSettings(node);

	node->addChild(new XMLNode("w", int(getW())));
	node->addChild(new XMLNode("h", int(getH())));
}

void ProfileDialog::loadSettings(XMLNode *node, bool resetPositions)
{
	GLWWindow::loadSettings(node, resetPositions);

	if (!resetPositions)
	{
		int w, h;
		if (node->getNamedChild("w", w, false)) setW(float(w));
		if (node->getNamedChild("h", h, false)) setH(float(h));
	}
}

void ProfileDialog::draw()
{
	GLWWindow::draw();
	
	// Set the new viewport
	int windowWidth = int(float(w_ - INSET - INSET) / GLViewPort::getWidthMult());
	int windowHeight = int(float(h_ - INSET - INSET) / GLViewPort::getHeightMult()); 
	int windowX = int(float(x_ + INSET) / GLViewPort::getWidthMult()); 
	int windowY = int(float(y_ + INSET) / GLViewPort::getHeightMult());
	
	ox_ = 0.0f;
	oy_ = 0.0f;
	ow_ = w_ / 2.0f;
	oh_ = h_ / 2.0f;

	if (profileZoom_ > 1.0f)
	{
		float xper = (zoomX_ - (x_ + INSET)) / (w_ - INSET - INSET);
		float yper = (zoomY_ - (y_ + INSET)) / (h_ - INSET - INSET);
		float x = xper * ow_;
		float y = yper * oh_;
		ox_ = (ox_ - x) / profileZoom_ + x;
		oy_ = (oy_ - y) / profileZoom_ + y;
		ow_ = (ow_ - x) / profileZoom_ + x;
		oh_ = (oh_ - y) / profileZoom_ + y;
	}

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	
	glViewport(windowX, windowY, windowWidth, windowHeight);
	glOrtho(ox_, ow_, oy_, oh_, -100.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	// Draw the stuff we want to see
	Tank *currentTank =
		ScorchedClient::instance()->getTargetContainer().getCurrentTank();
	if (currentTank && currentTank->getAlive())
	{
		drawAIM(currentTank);
		drawAiming(currentTank);
		drawLandscape(currentTank);
		drawTanks(currentTank);
	}
		
	// Return the viewport to the original
	Main2DCamera::instance()->draw(0);
}

void ProfileDialog::simulate(float frameTime)
{
	if (zooming_) profileZoom_ += frameTime * 4.0f;
	else profileZoom_ -= frameTime * 4.0f;

	if (profileZoom_ < 1.0f) profileZoom_ = 1.0f;
	else if (profileZoom_ > 2.0f) profileZoom_ = 2.0f;
}

void ProfileDialog::drawLandscape(Tank *currentTank)
{
	// Get tank attributes
	fixed tankRotation = -currentTank->getShotInfo().getRotationGunXY() / 180 * fixed::XPI;
	FixedVector tankDirection(tankRotation.sin(), tankRotation.cos(), 0);
	FixedVector startPosition = currentTank->getLife().getTargetPosition() - tankDirection * 25;

	// Draw the landscape
	GLState state1(GLState::TEXTURE_OFF | GLState::BLEND_ON | GLState::DEPTH_OFF);
	glColor4f(0.0f, 0.5f, 0.0f, 1.0f);

	glBegin(GL_QUAD_STRIP);
	FixedVector position = startPosition;
	float x = 0.0f;
	for (int i=0; i<=400; i++)
	{
		float height =
			ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getInterpHeight(
			position[0], position[1]).asFloat();
		if (height < 1.0f) height = 1.0f;

		glVertex2f(x, height);
		glVertex2f(x, 0.0f);

		x += 1.0f;
		position += tankDirection;
	}
	glEnd();
}

void ProfileDialog::drawAIM(Tank *currentTank)
{
	GLState state1(GLState::TEXTURE_OFF | GLState::DEPTH_OFF);

	if (TargetRendererImplTankAIM::drawAim())
	{
		FixedVector tankPosition = currentTank->getLife().getTargetPosition();
		float distance = (tankPosition.asVector() - 
			TargetRendererImplTankAIM::getAimPosition()).Magnitude2d();

		glPushMatrix();
			glTranslatef(distance + 25.0f, 
				TargetRendererImplTankAIM::getAimPosition()[2],
				0.0f);

			glColor3f(0.7f, 0.7f, 0.0f);
			glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 1.0f);
				glVertex3f(-0.8f, +15.0f, 0.0f);
				glTexCoord2f(0.0f, 0.0f);
				glVertex3f(-0.8f, -10.0f, 0.0f);
				glTexCoord2f(1.0f, 0.0f);
				glVertex3f(+0.8f, -10.0f, 0.0f);
				glTexCoord2f(1.0f, 1.0f);
				glVertex3f(+0.8f, +15.0f, 0.0f);
			glEnd();
		glPopMatrix();
	}
}

void ProfileDialog::drawTanks(Tank *currentTank)
{
	// Get tank attributes
	FixedVector tankPosition = currentTank->getLife().getTargetPosition();
	fixed tankRotation = -currentTank->getShotInfo().getRotationGunXY() / 180 * fixed::XPI;
	FixedVector tankDirection(tankRotation.sin(), tankRotation.cos(), 0);
	FixedVector startPosition = tankPosition - tankDirection * 25;

	GLState state2(GLState::TEXTURE_ON | GLState::BLEND_ON | GLState::ALPHATEST_ON | GLState::DEPTH_ON);

	bool showingToolTip = false;
	std::map<unsigned int, Tank *> &tanks =
		ScorchedClient::instance()->getTargetContainer().getTanks();
	std::map<unsigned int, Tank *>::iterator itor;
	for (itor = tanks.begin(); 
		itor != tanks.end();
		++itor)
	{
		Tank *tank = itor->second;
		if (!tank->getAlive()) continue;

		if (tank != currentTank)
		{
			FixedVector thisTankDir = (tank->getLife().getTargetPosition() - tankPosition);
			thisTankDir[2] = 0;
			thisTankDir.StoreNormalize();
			fixed dotP = thisTankDir.dotP2D(tankDirection);
			if (dotP < fixed(true, 8000)) continue;
		}

		FixedVector AP = tank->getLife().getTargetPosition() - startPosition;
		FixedVector AB = tankDirection * 400;

		fixed ab2 = AB[0]*AB[0] + AB[1]*AB[1];
		fixed ap_ab = AP[0]*AB[0] + AP[1]*AB[1];
		fixed t = ap_ab / ab2;
		if (t>=0 && t<1)
		{
			FixedVector point = startPosition + AB * t;
			fixed landheight =
				ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getInterpHeight(
				point[0], point[1]);
			fixed x = t * 400;
			fixed tankheight = tank->getLife().getTargetPosition()[2];

			if (landheight < tankheight + 10)
			{
				landheight = tankheight + 10;
			}

			bool toolTipAdded = false;
			TargetRendererImplTank *renderer = (TargetRendererImplTank *) 
				tank->getRenderer();
			if (renderer)
			{
				float xf = x.asFloat();
				float yf = tankheight.asFloat();
				if (!showingToolTip &&
					xf > ox_ && yf > oy_ &&
					xf < ow_ && yf < oh_)
				{
					float posX = (xf - ox_) / (ow_ - ox_) * (w_ - INSET - INSET) + x_ + INSET;
					float posY = (yf - oy_) / (oh_ - oy_) * (h_ - INSET - INSET) + y_ + INSET;

					// Add the tooltip that displays the tank info
					toolTipAdded = GLWToolTip::instance()->addToolTip(&renderer->getTips()->tankTip,
						float(posX) - 10.0f, float(posY) - 10.0f, 20.0f, 20.0f);
					showingToolTip = toolTipAdded;
				}

				// Draw the tank
				glPushMatrix();
					// Set the tank angle
					glTranslatef(xf, yf, 0.0f);
					glScalef(10.0f, 10.0f, 10.0f);

					Vector position;
					ModelRendererTank *mesh = renderer->getMesh();
					if (mesh)
					{
						glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
						Vector4 rotation;
						Vector axis(1.0f, 0.0f, 0.0f);
						rotation.setQuatFromAxisAndAngle(axis, 0.0f);
						glScalef(0.35f, 0.35f, 0.35f);

						float matrix[16];
						rotation.getOpenGLRotationMatrix(matrix);

						mesh->draw(
							0.0f,
							matrix, position, 0.0f,
							tank->getShotInfo().getRotationGunXY().asFloat(),
							tank->getShotInfo().getRotationGunYZ().asFloat(),
							false);
					}
				glPopMatrix();
			}

			bool bringToFront = (tank == currentTank || toolTipAdded);
			float depth = bringToFront?90.0f:0.0f;

			if (OptionsDisplay::instance()->getDrawPlayerColor())
			{			
				ExplosionTextures::instance()->arrowTexture.draw();
				glColor3fv(tank->getColor());
				glBegin(GL_QUADS);
					glTexCoord2f(0.0f, 1.0f);
					glVertex3f(x.asFloat() - 2.5f, landheight.asFloat() + 10.0f, depth);
					glTexCoord2f(0.0f, 0.0f);
					glVertex3f(x.asFloat() - 2.5f, landheight.asFloat() + 0.0f, depth);
					glTexCoord2f(1.0f, 0.0f);
					glVertex3f(x.asFloat() + 2.5f, landheight.asFloat() + 0.0f, depth);
					glTexCoord2f(1.0f, 1.0f);
					glVertex3f(x.asFloat() + 2.5f, landheight.asFloat() + 10.0f, depth);
				glEnd();
			}

			if (OptionsDisplay::instance()->getDrawPlayerNames() &&
				bringToFront)
			{
				float textWidth =
					GLWFont::instance()->getGameFont()->getWidth(5.0f,
						tank->getTargetName());
				GLWFont::instance()->getGameFont()->draw(
					tank->getColor(), 5.0f, 
					x.asFloat() - textWidth / 2.0f, 
					landheight.asFloat() + 12.0f, 
					depth,
					tank->getTargetName());
			}
		}
	}
}

void ProfileDialog::drawAiming(Tank *currentTank)
{
	GLState state3(GLState::TEXTURE_OFF | GLState::BLEND_ON);

	FixedVector tankPosition = currentTank->getLife().getTankTurretPosition();
	FixedVector tankWindowPosition(25, tankPosition[2], 0);

	const fixed diff = fixed(5) / 180 * fixed::XPI;
	fixed tankElevation1 = (fixed(90) - currentTank->getShotInfo().getRotationGunYZ()) / 180 * fixed::XPI;
	fixed tankElevation2 = tankElevation1 + diff;
	FixedVector tankElevationDirection1(tankElevation1.sin(), tankElevation1.cos(), 0);
	FixedVector tankElevationDirection2(tankElevation2.sin(), tankElevation2.cos(), 0);

	FixedVector aimPos1 = tankWindowPosition + tankElevationDirection1  * 5;
	FixedVector aimPos2 = tankWindowPosition + tankElevationDirection1  * 30;
	FixedVector aimPos3 = tankWindowPosition + tankElevationDirection2  * 30;

	glColor3f(1.0f, 0.3f, 0.0f);

	glBegin(GL_QUADS);
		glVertex2f(aimPos2[0].asFloat(), aimPos2[1].asFloat());
		glVertex2f(aimPos1[0].asFloat(), aimPos1[1].asFloat());
		glVertex2f(aimPos1[0].asFloat(), aimPos1[1].asFloat());
		glVertex2f(aimPos3[0].asFloat(), aimPos3[1].asFloat());
	glEnd();
}

void ProfileDialog::mouseDown(int button, float x, float y, bool &skipRest)
{
	GLWWindow::mouseDown(button, x, y, skipRest);
	if (!inBox(x, y, x_ + w_ - 12.0f, y_, 12.0f, 12.0f))
	{
		if (x > x_ && x < x_ + w_ && y > y_ && y < y_ + h_)
		{
			zooming_ = !zooming_;

			if (profileZoom_ == 1.0f)
			{
				zoomX_ = x;
				zoomY_ = y;
			}
		}
	}
}

void ProfileDialog::mouseUp(int button, float x, float y, bool &skipRest)
{
	GLWWindow::mouseUp(button, x, y, skipRest);
	if (!inBox(x, y, x_ + w_ - 12.0f, y_, 12.0f, 12.0f))
	{
		if (x > x_ && x < x_ + w_ && y > y_ && y < y_ + h_)
		{
			//zooming_ = false;
		}
	}
}
