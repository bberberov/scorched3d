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

#include <GLW/GLWWeaponModel.h>
#include <client/ScorchedClient.h>
#include <client/ClientState.h>
#include <tankgraph/TargetRendererImplTank.h>
#include <tankgraph/MissileMesh.h>
#include <target/TargetContainer.h>
#include <tanket/TanketAccessories.h>
#include <tank/TankState.h>
#include <GLW/GLWTranslate.h>
#include <weapons/AccessoryStore.h>
#include <weapons/Accessory.h>
#include <landscape/Landscape.h>
#include <sky/Sky.h>
#include <common/Vector4.h>
#include <common/Defines.h>

REGISTER_CLASS_SOURCE(GLWWeaponModel);

GLWWeaponModel::GLWWeaponModel(float x, float y, float w, float h) :
	GLWidget(x, y, w, h),
	totalTime_(0.0f)
{
}

GLWWeaponModel::~GLWWeaponModel()
{
}

void GLWWeaponModel::simulate(float frameTime)
{
	GLWidget::simulate(frameTime);
	totalTime_+=frameTime;
}

void GLWWeaponModel::draw()
{
	GLWidget::draw();

	Tank *current = 
		ScorchedClient::instance()->getTargetContainer().getCurrentTank();
	if (!current ||
		current->getState().getState() != TankState::sNormal)
	{
		return;
	}

	TargetRendererImplTank *renderer = (TargetRendererImplTank *) 
		current->getRenderer();
	if (!renderer) return;

	Accessory *weapon = current->getAccessories().getWeapons().getCurrent();
	if (!weapon) return;

	Vector4 sunPosition(-100.0f, 100.0f, 400.0f, 1.0f);
	Vector4 sunDiffuse(0.9f, 0.9f, 0.9f, 1.0f);
	Vector4 sunAmbient(0.4f, 0.4f, 0.4f, 1.0f);
	glLightfv(GL_LIGHT1, GL_AMBIENT, sunAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, sunDiffuse);

	// Cache some stuff we should only fetch when tank
	// or tank's weapon changes
	static Accessory *storedWeapon = 0;
	static Tank *storedTank = 0;
	static MissileMesh *storedMesh = 0;
	if (weapon != storedWeapon || current != storedTank)
	{
		storedWeapon = weapon;
		storedTank = current;
		storedMesh = Accessory::getWeaponMesh(storedWeapon->getModel(), current);
	}

	GLState tankState(GLState::TEXTURE_ON | GLState::DEPTH_ON);
	setToolTip(&weapon->getToolTip());

	// Draw the current weapon
	glPushMatrix();
		glTranslatef(x_ + w_ / 2.0f, y_ + w_ / 2.0f, 0.0f);

		glLightfv(GL_LIGHT1, GL_POSITION, sunPosition);

		glRotatef(totalTime_ * 45.0f, 0.0f, 0.0f, 1.0f);
		Vector position;
		Vector direction(0.3f, 1.0f, 1.0f);
		float scale = MIN(storedWeapon->getModelScale().asFloat(), 1.0f);
		storedMesh->setScale(w_ / 3.0f * scale);
		Vector rotationAxis(0.0f, 0.0f, 1.0f);
		storedMesh->draw(position, direction, 0, totalTime_ * 45.0f, rotationAxis, totalTime_ * 20.0f);
	glPopMatrix();

	Landscape::instance()->getSky().getSun().setLightPosition(); // Reset light
}

void GLWWeaponModel::mouseDown(int button, float x, float y, bool &skipRest)
{
	Tank *current = 
 		ScorchedClient::instance()->getTargetContainer().getCurrentTank();
	if (!current ||
		current->getState().getState() != TankState::sNormal)
  	{
  		return;
	}
	TargetRendererImplTank *renderer = (TargetRendererImplTank *) 
		current->getRenderer();
	if (!renderer) return;
	GLWTankTips *tankTips = renderer->getTips();

	if (inBox(x, y, x_, y_, w_, h_))
	{
		skipRest = true;

		tankTips->weaponTip.showItems(GLWTranslate::getPosX() + x, 
			GLWTranslate::getPosY() + y);
	}
}
