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

#include <sprites/DebrisActionRenderer.h>
#include <3dsparse/ModelStore.h>
#include <common/Defines.h>
#include <GLEXT/GLState.h>
#include <graph/ModelRendererStore.h>
#include <stdlib.h>

DebrisActionRenderer::DebrisActionRenderer() :
	rotationAng_(0)
{
	rotationX_ = RAND;
	rotationY_ = RAND;
	rotationZ_ = RAND;
	rotationSpeed_ = 90.0f + 180.0f * RAND;

	if (RAND > 0.5f)
	{
		ModelID id;
		id.initFromString("ase", "data/meshes/rock1.ase", 
			"none");
		debris_ = new ModelRendererSimulator(
			ModelRendererStore::instance()->loadModel(id));
	}
	else
	{
		ModelID id;
		id.initFromString("ase", "data/meshes/rock2.ase", 
			"none");
		debris_ = new ModelRendererSimulator(
			ModelRendererStore::instance()->loadModel(id));
	}
	DIALOG_ASSERT(debris_);
}

DebrisActionRenderer::~DebrisActionRenderer()
{
	delete debris_;
	debris_ = 0;
}

void DebrisActionRenderer::simulate(float timepassed)
{
	rotationAng_ += timepassed * rotationSpeed_;
}

void DebrisActionRenderer::draw(Vector &actualPos)
{
	GLState state(GLState::TEXTURE_OFF | GLState::BLEND_OFF);

	glColor3f(0.3f, 0.4f, 0.3f);
	glPushMatrix();
		glTranslatef(actualPos[0], actualPos[1], actualPos[2]);
		glRotatef(rotationAng_, rotationX_, rotationY_, rotationZ_);
		debris_->draw();
	glPopMatrix();
}
