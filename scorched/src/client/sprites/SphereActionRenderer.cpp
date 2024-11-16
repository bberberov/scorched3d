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

#include <sprites/SphereActionRenderer.h>
#include <GLEXT/GLCameraFrustum.h>
#include <GLEXT/GLState.h>

SphereActionRenderer::SphereActionRenderer(Vector &position, float size) : 
	frameTime_(0.0f), position_(position), size_(size)
{
}

SphereActionRenderer::~SphereActionRenderer()
{
}

void SphereActionRenderer::draw(Action *action)
{
	GLState state(GLState::TEXTURE_OFF);
	glColor3f(1.0f, 0.0f, 0.0f);

	static GLUquadric *obj = 0;
	if (obj == 0)
	{
		obj = gluNewQuadric();
		gluQuadricTexture(obj, GL_TRUE);
	}

	glPushMatrix();
		glTranslatef(position_[0], position_[1], position_[2]);
		gluSphere(obj, size_, 8, 8);
	glPopMatrix();
}

void SphereActionRenderer::simulate(Action *action, float timepassed, bool &remove)
{
	frameTime_ += timepassed;
	remove = (frameTime_ > 8.0f);
}

