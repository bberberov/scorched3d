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

#include <tankgraph/RenderGeoms.h>
#include <target/TargetSpace.h>
#include <target/TargetContainer.h>
#include <target/TargetLife.h>
#include <client/ScorchedClient.h>
#include <graph/OptionsDisplay.h>
#include <engine/ActionController.h>
#include <GLEXT/GLState.h>

RenderGeoms *RenderGeoms::instance()
{
	static RenderGeoms instance;
	return &instance;
}

RenderGeoms::RenderGeoms()
{
}

RenderGeoms::~RenderGeoms()
{
}

void RenderGeoms::draw(const unsigned state)
{
	if (OptionsDisplay::instance()->getDrawCollisionGeoms())
	{
		drawCollisionGeoms();
		drawCollisionBounds();
	}

	if (OptionsDisplay::instance()->getDrawCollisionSpace())
	{
		drawTargetSpace();
	}
}

void RenderGeoms::drawTargetSpace()
{
	ScorchedClient::instance()->getContext().getTargetSpace().draw();
}

void RenderGeoms::drawCollisionBounds()
{
	GLState glState(GLState::TEXTURE_OFF);

	glColor3f(0.0f, 0.0f, 1.0f);
	std::map<unsigned int, Target *> &targets = 
		ScorchedClient::instance()->getTargetContainer().getTargets();
	std::map<unsigned int, Target *>::iterator itor;
	for (itor = targets.begin();
		itor != targets.end();
		++itor)
	{
		Target *target = (*itor).second;
		if (!target->getAlive())
		{
			continue;
		}

		Vector position = target->getLife().getFloatPosition();
		Vector size = target->getLife().getAabbSize().asVector();
		position[2] += size[2] / 2.0f;

		double wid = size[0] + 0.1f;
		double hgt = size[1] + 0.1f;
		double dep = size[2] + 0.1f;

		glPushMatrix();
			glTranslated(position[0], position[1], position[2]);
			glBegin(GL_LINE_LOOP);
				// Top
				glNormal3d(0,1,0);
				glVertex3d(-wid/2,hgt/2,dep/2);
				glVertex3d(wid/2,hgt/2,dep/2);
				glVertex3d(wid/2,hgt/2,-dep/2);
				glVertex3d(-wid/2,hgt/2,-dep/2);
			glEnd();
			glBegin(GL_LINE_LOOP);
				// Back
				glNormal3d(0,0,1);
				glVertex3d(-wid/2,hgt/2,dep/2);
				glVertex3d(-wid/2,-hgt/2,dep/2);
				glVertex3d(wid/2,-hgt/2,dep/2);
				glVertex3d(wid/2,hgt/2,dep/2);
			glEnd();
			glBegin(GL_LINE_LOOP);
				// Front
				glNormal3d(0,0,-1);
				glVertex3d(-wid/2,hgt/2,-dep/2);
				glVertex3d(wid/2,hgt/2,-dep/2);
				glVertex3d(wid/2,-hgt/2,-dep/2);
				glVertex3d(-wid/2,-hgt/2,-dep/2);
			glEnd();
			glBegin(GL_LINE_LOOP);
				// Left
				glNormal3d(1,0,0);
				glVertex3d(wid/2,hgt/2,-dep/2);
				glVertex3d(wid/2,hgt/2,dep/2);
				glVertex3d(wid/2,-hgt/2,dep/2);
				glVertex3d(wid/2,-hgt/2,-dep/2);
			glEnd();
			glBegin(GL_LINE_LOOP);
				// Right
				glNormal3d(-1,0,0);
				glVertex3d(-wid/2,hgt/2,dep/2);
				glVertex3d(-wid/2,hgt/2,-dep/2);
				glVertex3d(-wid/2,-hgt/2,-dep/2);
				glVertex3d(-wid/2,-hgt/2,dep/2);
			glEnd();
			glBegin(GL_LINE_LOOP);
				// Bottom
				glNormal3d(0,-1,0);
				glVertex3d(-wid/2,-hgt/2,dep/2);
				glVertex3d(-wid/2,-hgt/2,-dep/2);
				glVertex3d(wid/2,-hgt/2,-dep/2);
				glVertex3d(wid/2,-hgt/2,dep/2);
			glEnd();
		glPopMatrix();
	}
}

void RenderGeoms::drawCollisionGeoms()
{
	GLState glState(GLState::TEXTURE_OFF);

	glColor3f(1.0f, 1.0f, 1.0f);
	std::map<unsigned int, Target *> &targets = 
		ScorchedClient::instance()->getTargetContainer().getTargets();
	std::map<unsigned int, Target *>::iterator itor;
	for (itor = targets.begin();
		itor != targets.end();
		++itor)
	{
		Target *target = (*itor).second;
		if (!target->getAlive())
		{
			continue;
		}

		if (target->getLife().getBoundingSphere())
		{
			Vector position = target->getLife().getFloatPosition();
			Vector &size = target->getLife().getSize().asVector();
			position[2] += size[2] / 2.0f;
			float radius = MAX(MAX(size[0], size[1]), size[2]) / 2.0f;

			static GLUquadric *obj = 0;
			if (!obj)
			{
				obj = gluNewQuadric();
				gluQuadricDrawStyle(obj, GLU_LINE);
			}

			glPushMatrix();
				glTranslated(position[0], position[1], position[2]);
				gluSphere(obj, radius, 6, 6);
			glPopMatrix();
		}
		else
		{
			Vector position = target->getLife().getFloatPosition();
			Vector &size = target->getLife().getSize().asVector();
			position[2] += size[2] / 2.0f;

			double wid = size[0];
			double hgt = size[1];
			double dep = size[2];

			static float rotMatrix[16];
			target->getLife().getQuaternion().getOpenGLRotationMatrix(rotMatrix);

			glPushMatrix();
				glTranslated(position[0], position[1], position[2]);
				glMultMatrixf(rotMatrix);
				glBegin(GL_LINE_LOOP);
					// Top
					glNormal3d(0,1,0);
					glVertex3d(-wid/2,hgt/2,dep/2);
					glVertex3d(wid/2,hgt/2,dep/2);
					glVertex3d(wid/2,hgt/2,-dep/2);
					glVertex3d(-wid/2,hgt/2,-dep/2);
				glEnd();
				glBegin(GL_LINE_LOOP);
					// Back
					glNormal3d(0,0,1);
					glVertex3d(-wid/2,hgt/2,dep/2);
					glVertex3d(-wid/2,-hgt/2,dep/2);
					glVertex3d(wid/2,-hgt/2,dep/2);
					glVertex3d(wid/2,hgt/2,dep/2);
				glEnd();
				glBegin(GL_LINE_LOOP);
					// Front
					glNormal3d(0,0,-1);
					glVertex3d(-wid/2,hgt/2,-dep/2);
					glVertex3d(wid/2,hgt/2,-dep/2);
					glVertex3d(wid/2,-hgt/2,-dep/2);
					glVertex3d(-wid/2,-hgt/2,-dep/2);
				glEnd();
				glBegin(GL_LINE_LOOP);
					// Left
					glNormal3d(1,0,0);
					glVertex3d(wid/2,hgt/2,-dep/2);
					glVertex3d(wid/2,hgt/2,dep/2);
					glVertex3d(wid/2,-hgt/2,dep/2);
					glVertex3d(wid/2,-hgt/2,-dep/2);
				glEnd();
				glBegin(GL_LINE_LOOP);
					// Right
					glNormal3d(-1,0,0);
					glVertex3d(-wid/2,hgt/2,dep/2);
					glVertex3d(-wid/2,hgt/2,-dep/2);
					glVertex3d(-wid/2,-hgt/2,-dep/2);
					glVertex3d(-wid/2,-hgt/2,dep/2);
				glEnd();
				glBegin(GL_LINE_LOOP);
					// Bottom
					glNormal3d(0,-1,0);
					glVertex3d(-wid/2,-hgt/2,dep/2);
					glVertex3d(-wid/2,-hgt/2,-dep/2);
					glVertex3d(wid/2,-hgt/2,-dep/2);
					glVertex3d(wid/2,-hgt/2,dep/2);
				glEnd();
			glPopMatrix();
		}
	}
}
