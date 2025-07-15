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

#include <GLW/GLWTankModel.h>
#include <client/ScorchedClient.h>
#include <graph/MainCamera.h>
#include <client/ClientState.h>
#include <tankgraph/TargetRendererImplTank.h>
#include <target/TargetContainer.h>
#include <tanket/TanketShotInfo.h>
#include <common/Vector4.h>
#include <landscape/Landscape.h>
#include <sky/Sky.h>
#include <GLW/GLWTranslate.h>

REGISTER_CLASS_SOURCE(GLWTankModel);

GLWTankModel::GLWTankModel(float x, float y, float w, float h) :
	GLWidget(x, y, w, h), 
	totalTime_(0.0f)
{
}

GLWTankModel::~GLWTankModel()
{
}

void GLWTankModel::draw()
{
	Tank *current = 
		ScorchedClient::instance()->getTargetContainer().getCurrentTank();
	if (!current) return;

	TargetRendererImplTank *renderer = (TargetRendererImplTank *) 
		current->getRenderer();
	if (!renderer) return;

	Vector4 sunPosition(-100.0f, 100.0f, 400.0f, 1.0f);
	Vector4 sunDiffuse(0.9f, 0.9f, 0.9f, 1.0f);
	Vector4 sunAmbient(0.4f, 0.4f, 0.4f, 1.0f);
	glLightfv(GL_LIGHT1, GL_AMBIENT, sunAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, sunDiffuse);

	// Add the tooltip for the model name+attributes
	GLWToolTip::instance()->addToolTip(&renderer->getTips()->tankTip, 
		GLWTranslate::getPosX() + x_ + 20.0f, 
		GLWTranslate::getPosY() + y_ + 20.0f, 80.0f, 80.0f);

	// Find the angles to rotate so the tank is at the
	// same angle as the "real" tank on the landscape
	Vector &lookFrom = MainCamera::instance()->getCamera().getCurrentPos();
	Vector &lookAt = MainCamera::instance()->getCamera().getLookAt();
	Vector dir = (lookAt - lookFrom).Normalize();
	float angXY = atan2f(dir[0], dir[1]) / 3.14f * 180.0f;
	float angYZ = acosf(dir[2]) / 3.14f * 180.0f + 180.0f;
	if (angYZ < 280.0f) angYZ = 280.0f;

	// Draw the tank
	glPushMatrix();
		// Set the tank angle
		glTranslatef(x_ + w_ / 2.0f, y_ + w_ / 2.0f, 0.0f);

		glLightfv(GL_LIGHT1, GL_POSITION, sunPosition);

		glRotatef(angYZ, 1.0f, 0.0f, 0.0f);
		glRotatef(angXY, 0.0f, 0.0f, 1.0f);

		// Draw the tank model
		glScalef(w_ / 4.0f, w_ / 4.0f, w_ / 4.0f);
		GLState tankState(GLState::TEXTURE_OFF | GLState::DEPTH_ON); // For no tank skins
		Vector position;
		ModelRendererTank *mesh = renderer->getMesh();
		if (mesh)
		{
			Vector4 rotation(1.0f, 0.0f, 0.0f, 0.0f);
			float matrix[16];
			rotation.getOpenGLRotationMatrix(matrix);

			mesh->draw(
				totalTime_ * 20.0f,
				matrix, position, 0.0f,
				current->getShotInfo().getRotationGunXY().asFloat(),
				current->getShotInfo().getRotationGunYZ().asFloat(),
				true);
		}
	glPopMatrix();

	Landscape::instance()->getSky().getSun().setLightPosition(); // Reset light
}

void GLWTankModel::mouseDown(int button, float x, float y, bool &skipRest)
{
	if (inBox(x, y, x_, y_, w_, h_))
	{
		skipRest = true;
		MainCamera::instance()->getTarget().setCameraType(TargetCamera::CamBehind);
	}
}

void GLWTankModel::simulate(float frameTime)
{
	totalTime_ += frameTime;
}
