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
#include <GLW/GLWFlag.h>

REGISTER_CLASS_SOURCE(GLWFlag);

GLWFlag::GLWFlag(Vector &color, float x, float y, float w) : 
	color_(color), GLWidget(x, y, w, 20.0f), offset_(0.0f)
{

}

GLWFlag::~GLWFlag()
{

}

void GLWFlag::simulate(float frameTime)
{
	offset_ -= 10.0f * frameTime;
}

void GLWFlag::draw()
{
	GLState currentState(GLState::TEXTURE_OFF);

	const float diff = 1.0f;
	const float rad = (3.14f * 4.0f) / w_;

	glBegin(GL_QUADS);
		for (float a=0.0f; a<=w_-diff; a+=diff)
		{
			float mult = float (sin((a * rad) + offset_));
			float cmult = (mult * 0.25f) + 0.75f;
			float hmult = mult * 2.0f;

			glColor3f(color_[0] * cmult, color_[1] * cmult, color_[2] * cmult);
			glVertex2f(x_ + a, y_  + hmult);
			glVertex2f(x_ + a + diff, y_ + hmult);
			glVertex2f(x_ + a + diff, y_ + h_ + hmult);
			glVertex2f(x_ + a, y_ + h_ + hmult);
		}
	glEnd();
}
