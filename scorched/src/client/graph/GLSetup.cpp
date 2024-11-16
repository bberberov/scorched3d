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

#include <graph/GLSetup.h>
#include <GLEXT/GLStateExtension.h>
#include <console/Console.h>

void GLSetup::setup()
{
	glPolygonMode(GL_FRONT, GL_FILL);

	glDisable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	GLfloat fogColor[4]= {0.8f, 0.8f, 0.8f, 1.0f};

	glFogi(GL_FOG_MODE, GL_EXP2);	
	glFogfv(GL_FOG_COLOR, fogColor);
	glFogf(GL_FOG_DENSITY, 0.001f);	
	//glHint(GL_FOG_HINT, GL_DONT_CARE);
	//glFogf(GL_FOG_START, 1.0f);	
	//glFogf(GL_FOG_END, 2.0f);
	//glFogf(GL_FOG_DISTANCE_MODE_NV, GL_EYE_RADIAL_NV);

	glAlphaFunc(GL_GREATER, 0.00f);

	Console::instance(); // Make sure console is inited
	GLState::setBaseState(GLState::TEXTURE_ON | GLState::BLEND_OFF | GLState::DEPTH_ON);
	GLStateExtension::setup();
}
