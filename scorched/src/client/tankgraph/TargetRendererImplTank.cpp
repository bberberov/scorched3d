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

#include <tankgraph/TargetRendererImplTank.h>
#include <sprites/ExplosionTextures.h>
#include <tank/TankLib.h>
#include <target/TargetContainer.h>
#include <tank/TankState.h>
#include <tank/TankAvatar.h>
#include <tank/TankModelContainer.h>
#include <tanket/TanketShotInfo.h>
#include <target/TargetLife.h>
#include <target/TargetShield.h>
#include <target/TargetState.h>
#include <tankgraph/ModelRendererTankStore.h>
#include <landscape/Landscape.h>
#include <landscape/ShadowMap.h>
#include <landscape/Smoke.h>
#include <landscapemap/LandscapeMaps.h>
#include <actions/TargetFalling.h>
#include <engine/ActionController.h>
#include <client/ScorchedClient.h>
#include <client/ClientState.h>
#include <graph/OptionsDisplay.h>
#include <graph/ModelRenderer.h>
#include <graph/ModelRendererStore.h>
#include <graph/ModelRendererSimulator.h>
#include <graph/MainCamera.h>
#include <common/Defines.h>
#include <weapons/Shield.h>
#include <weapons/Accessory.h>
#include <dialogs/TutorialDialog.h>
#include <image/ImageFactory.h>
#include <GLEXT/GLCameraFrustum.h>
#include <GLEXT/GLTexture.h>
#include <GLW/GLWFont.h>

float TargetRendererImplTankHUD::timeLeft_ = -1.0f;
float TargetRendererImplTankHUD::percentage_ = -1.0f;
std::string TargetRendererImplTankHUD::textA_ = "";
std::string TargetRendererImplTankHUD::textB_ = "";

Vector TargetRendererImplTankAIM::aimPosition_;
float TargetRendererImplTankAIM::timeLeft_ = -1.0f;

ModelRendererSimulator *TargetRendererImplTankAIM::getAutoAimModel()
{
	static ModelRendererSimulator *modelRenderer = 0;
	if (!modelRenderer)
	{
		ModelID id;
		id.initFromString("ase", "data/meshes/autoaim.ase", "none");
		modelRenderer = new ModelRendererSimulator(
			ModelRendererStore::instance()->loadModel(id));
	}
	return modelRenderer;
}

TargetRendererImplTank::TargetRendererImplTank(Tank *tank) :
	TargetRendererImpl(tank),
	tank_(tank), tankTips_(tank),
	mesh_(0), 
	smokeTime_(0.0f), smokeWaitForTime_(0.0f),
	fireOffSet_(0.0f), shieldHit_(0.0f),
	totalTime_(0.0f)
{
	frame_ = (float) rand();
}

TargetRendererImplTank::~TargetRendererImplTank()
{
}

ModelRendererTank *TargetRendererImplTank::getMesh()
{
	if (!mesh_)
	{
		TankModel *model = tank_->getModelContainer().getTankModel();
		mesh_ = ModelRendererTankStore::instance()->getMesh(model->getTankModelID());
	}
	return mesh_;
}

void TargetRendererImplTank::render(float distance)
{
	createParticle();
	storeTarget2DPos();
	if (tank_->getState().getState() != TankState::sNormal) return;

	if (TargetRendererImplTankAIM::drawAim())
	{
		GLState texState(GLState::TEXTURE_OFF);
		Vector &pos = TargetRendererImplTankAIM::getAimPosition();

		glPushMatrix();
			glTranslatef(pos[0], pos[1], pos[2]);
			TargetRendererImplTankAIM::getAutoAimModel()->draw();
		glPopMatrix();
	}

	float size = getTargetSize();
	float fade = getTargetFade(distance, 
		size * 2.5f * float(OptionsDisplay::instance()->getTankModelSize()) / 100.0f);

	bool currentTank = 
		(tank_ == ScorchedClient::instance()->getTargetContainer().getCurrentTank() &&
		ScorchedClient::instance()->getGameState().getState() == ClientState::StatePlaying);
	if (fade > 0.0f)
	{
		// Add the tank shadow
		if (tank_->getTargetState().getDisplayShadow() &&
			Landscape::instance()->getShadowMap().shouldAddShadow())
		{
			float modelSize = float(OptionsDisplay::instance()->getTankModelSize()) / 100.0f;
			Landscape::instance()->getShadowMap().addCircle(
				tank_->getLife().getFloatPosition()[0], 
				tank_->getLife().getFloatPosition()[1], 
				(tank_->getLife().getSize().Max().asFloat() + 2.0f) * modelSize, 
				fade);
		}

		// Draw the tank model
		ModelRendererTank *mesh = getMesh();
		if (mesh)
		{
			float modelSize = float(OptionsDisplay::instance()->getTankModelSize()) / 100.0f;
			mesh->draw(frame_,
				tank_->getLife().getFloatRotMatrix(),
				tank_->getLife().getFloatPosition(), 
				fireOffSet_, 
				tank_->getShotInfo().getRotationGunXY().asFloat(), 
				tank_->getShotInfo().getRotationGunYZ().asFloat(),
				false, modelSize, fade, true);
		}
	}
}

void TargetRendererImplTank::renderReflection(float distance)
{
	if (tank_->getState().getState() != TankState::sNormal) return;

	float size = getTargetSize();
	float fade = getTargetFade(distance, 
		size * 2.5f * float(OptionsDisplay::instance()->getTankModelSize()) / 100.0f);

	if (fade > 0.0f)
	{
		// Draw the tank model
		ModelRendererTank *mesh = getMesh();
		if (mesh)
		{
			float modelSize = float(OptionsDisplay::instance()->getTankModelSize()) / 100.0f;
			mesh->draw(frame_,
				tank_->getLife().getFloatRotMatrix(),
				tank_->getLife().getFloatPosition(), 
				fireOffSet_, 
				tank_->getShotInfo().getRotationGunXY().asFloat(), 
				tank_->getShotInfo().getRotationGunYZ().asFloat(),
				false, modelSize, fade, true);
		}
	}
}

void TargetRendererImplTank::renderShadow(float distance)
{
	if (tank_->getState().getState() != TankState::sNormal) return;

	ModelRendererTank *mesh = getMesh();
	if (mesh)
	{
		float modelSize = float(OptionsDisplay::instance()->getTankModelSize()) / 100.0f;
		mesh->draw(frame_,
			tank_->getLife().getFloatRotMatrix(),
			tank_->getLife().getFloatPosition(), 
			fireOffSet_, 
			tank_->getShotInfo().getRotationGunXY().asFloat(), 
			tank_->getShotInfo().getRotationGunYZ().asFloat(),
			false, modelSize, 1.0f, false);
	}
}

void TargetRendererImplTank::drawParticle(float distance)
{
	if (!getVisible()) return;
	
	if (tank_->getState().getState() != TankState::sNormal) 
	{
		drawArrow();
		drawNames();
		return;
	}

	drawParachute();
	drawShield(shieldHit_, totalTime_);

	bool currentTank = 
		(tank_ == ScorchedClient::instance()->getTargetContainer().getCurrentTank() &&
		ScorchedClient::instance()->getGameState().getState() == ClientState::StatePlaying);

	// Draw the tank sight
	if (currentTank &&
		OptionsDisplay::instance()->getDrawPlayerSight())
	{
		if (OptionsDisplay::instance()->getLargeSight())
		{
			drawSight();
		}
		else
		{
			drawOldSight();
		}
	}

	if (TargetCamera::getCurrentTargetCamera())
	{
		TargetCamera::CamType camType = TargetCamera::getCurrentTargetCamera()->getCameraType();
		if (!currentTank ||
			(camType != TargetCamera::CamAim &&
			camType != TargetCamera::CamShot &&
			camType != TargetCamera::CamTank &&
			camType != TargetCamera::CamAction &&
			camType != TargetCamera::CamExplosion))
		{	
			drawArrow();
			drawLife();
			drawNames();
		}
	}
}

void TargetRendererImplTank::drawNames()
{
	Vector &position = tank_->getLife().getTargetPosition().asVector();
	float height = position[2];
	float groundHeight = ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().
		getHeight((int) position[0], (int) position[1]).asFloat();
	if (height < groundHeight)
	{
		height = groundHeight;
	}

	// Draw the names above all the tanks
	if (OptionsDisplay::instance()->getDrawPlayerNames())
	{
		Vector &bilX = GLCameraFrustum::instance()->getBilboardVectorX(); 
		bilX *= 0.5f * GLWFont::instance()->getGameFont()->getWidth(
			1, tank_->getTargetName());

		glDepthMask(GL_FALSE);
		GLWFont::instance()->getGameFont()->drawBilboard(
			tank_->getColor(), 1.0f, 1,
			(float) tank_->getLife().getFloatPosition()[0] - bilX[0], 
			(float) tank_->getLife().getFloatPosition()[1] - bilX[1], 
			(float) height + 8.0f,
			tank_->getTargetName());
		glDepthMask(GL_TRUE);
	}

	if (OptionsDisplay::instance()->getDrawPlayerIcons())
	{
		Vector &bilX = GLCameraFrustum::instance()->getBilboardVectorX(); 
		if (OptionsDisplay::instance()->getDrawPlayerNames())
		{
			bilX *= 0.5f * GLWFont::instance()->getGameFont()->getWidth(
				1, tank_->getTargetName());
			bilX += bilX.Normalize() * 1.0f;
		}
		else
		{
			bilX *= 0.0f;
		}

		Vector position = 
			tank_->getLife().getTargetPosition().asVector() - bilX;
		position[2] = height + 8.5f;

		tank_->getAvatar().getTexture()->draw();
		static Vector white(1.0f, 1.0f, 1.0f);
		GLCameraFrustum::instance()->drawBilboard(
			position, white, 0.8f, 0.5f, 0.5f, false, 0);
	}
}

void TargetRendererImplTank::drawSight()
{
	static GLTextureReference aimTopTexture(ImageID(S3D::eModLocation, "", "data/windows/aimtop.png"), 
		GLTextureReference::eMipMap | GLTextureReference::eTextureClamped);
	static GLTextureReference aimBotTexture(ImageID(S3D::eModLocation, "", "data/windows/aimbot.png"), 
		GLTextureReference::eMipMap | GLTextureReference::eTextureClamped);
	static GLTextureReference aimSideTexture(ImageID(S3D::eModLocation, "", "data/windows/aimside.png"), 
		GLTextureReference::eMipMap | GLTextureReference::eTextureClamped);
	static GLTextureReference aimRotationTexture(ImageID(S3D::eModLocation, "", "data/windows/aimrotation.png"), 
		GLTextureReference::eMipMap | GLTextureReference::eTextureClamped);

	float tankRotationDeg = tank_->getShotInfo().getRotationGunXY().asFloat();
	float tankElevationDeg = tank_->getShotInfo().getRotationGunYZ().asFloat();

	glPushMatrix();
 		if (OptionsDisplay::instance()->getOldSightPosition())
		{
			FixedVector &turretCenter = getMesh()->getTurretCenter();
			FixedVector &targetPosition = tank_->getLife().getTargetPosition();
			glTranslatef(
				targetPosition[0].asFloat(),
				targetPosition[1].asFloat(),
				targetPosition[2].asFloat() +
				(turretCenter[2].asFloat() - getMesh()->getModel()->getMin()[2].asFloat()) * getMesh()->getScale());
		}
		else
		{
			FixedVector &tankGunPosition = TankLib::getTankGunPosition(
				tank_->getLife().getTankTurretPosition(),
				tank_->getShotInfo().getRotationGunXY(),
				tank_->getShotInfo().getRotationGunYZ());
			glTranslatef(
				tankGunPosition[0].asFloat(),
				tankGunPosition[1].asFloat(),
				tankGunPosition[2].asFloat());
		}
	
		GLState sightState1(GLState::BLEND_ON | GLState::TEXTURE_ON | GLState::LIGHTING_OFF | GLState::ALPHATEST_ON);
		
		// Blue segments round bottom
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		aimRotationTexture.draw();
		glBegin(GL_QUADS);
		{
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(15.0f, 15.0f, -0.02f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-15.0f, 15.0f, -0.02f);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-15.0f, -15.0f, -0.02f);	
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(15.0f, -15.0f, -0.02f);

			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-15.0f, -15.0f, -0.02f);	
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-15.0f, 15.0f, -0.02f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(15.0f, 15.0f, -0.02f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(15.0f, -15.0f, -0.02f);
		}		
		glEnd();

		glRotatef(tankRotationDeg, 0.0f, 0.0f, 1.0f);

		if (OptionsDisplay::instance()->getOldSightPosition())
		{
			FixedVector &gunOffSet = getMesh()->getGunOffSet();
			glTranslatef(
				gunOffSet[0].asFloat() * getMesh()->getScale(), 
				gunOffSet[1].asFloat() * getMesh()->getScale(), 
				gunOffSet[2].asFloat() * getMesh()->getScale());
		}

		// Rotation marker
		aimBotTexture.draw();
		glBegin(GL_QUADS);
		{
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-1.0f, 15.0f, -0.01f);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-1.0f, 3.0f, -0.01f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(1.0f, 3.0f, -0.01f);	
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(1.0f, 15.0f, -0.01f);

			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(1.0f, 3.0f, -0.01f);	
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-1.0f, 3.0f, -0.01f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-1.0f, 15.0f, -0.01f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(1.0f, 15.0f, -0.01f);
		}
		glEnd();

		glRotatef(tankElevationDeg, 1.0f, 0.0f, 0.0f);

		// Elevation segements
		aimRotationTexture.draw();
		glBegin(GL_QUADS);
		{
			glTexCoord2f(0.5f, 0.5f);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glTexCoord2f(0.5f, 0.0f);
			glVertex3f(0.0f, 0.0f, -15.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(0.0f, 15.0f, -15.0f);
			glTexCoord2f(1.0f, 0.5f);
			glVertex3f(0.0f, 15.0f, 0.0f);

			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(0.0f, 15.0f, -15.0f);
			glTexCoord2f(0.5f, 0.0f);
			glVertex3f(0.0f, 0.0f, -15.0f);
			glTexCoord2f(0.5f, 0.5f);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glTexCoord2f(1.0f, 0.5f);
			glVertex3f(0.0f, 15.0f, 0.0f);
		}
		glEnd();

		// Elevation side marker
		aimSideTexture.draw();
		glBegin(GL_QUADS);
		{
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(-0.01f, 15.0f, -1.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(-0.01f, 3.0f, -1.0f);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-0.01f, 3.0f, 0.0f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-0.01f, 15.0f, 0.0f);

			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(0.01f, 3.0f, -1.0f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(0.01f, 15.0f, -1.0f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(0.01f, 15.0f, 0.0f);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(0.01f, 3.0f, 0.0f);
		}
		glEnd();

		// Elevation top marker
		aimTopTexture.draw();
		glBegin(GL_QUADS);
		{
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-1.0f, 15.0f, 0.0f);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-1.0f, 3.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(1.0f, 3.0f, 0.0f);	
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(1.0f, 15.0f, 0.0f);

			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(1.0f, 3.0f, 0.0f);	
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-1.0f, 3.0f, 0.0f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-1.0f, 15.0f, 0.0f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(1.0f, 15.0f, 0.0f);
		}
		glEnd();
	glPopMatrix();
}

void TargetRendererImplTank::drawOldSight()
{
	GLState sightState(GLState::BLEND_OFF | GLState::TEXTURE_OFF | GLState::LIGHTING_OFF);

	/*
	static ModelRendererSimulator *aimModel = 0;
	if (aimModel == 0)
	{
		ModelID id;
		id.initFromString("MilkShape", "data/meshes/aim/aim.txt", "none");
		aimModel = new ModelRendererSimulator(
			ModelRendererStore::instance()->loadModel(id));
	}

	glPushMatrix();
		Model *model = aimModel->getRenderer()->getModel();
		glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
		glTranslatef(-model->getMax()[0].asFloat() / 2.0f, 0.0f, 0.0f);
		glScalef(0.3f, 0.3f, 0.3f);
		aimModel->draw();
	glPopMatrix();
	*/

	static GLuint sightList_ = 0;
	if (!sightList_)
	{
		glNewList(sightList_ = glGenLists(1), GL_COMPILE);
			glBegin(GL_QUAD_STRIP);
				float x;
				for (x=126.0f; x>=90.0f; x-=9.0f)
				{
					const float deg = 3.14f / 180.0f;
					float dx = x * deg;
					float color = 1.0f - fabsf(90.0f - x) / 45.0f;

					glColor3f(1.0f * color, 0.5f * color, 0.5f * color);
					glVertex3f(+0.03f * color, 2.0f * sinf(dx), 2.0f * cosf(dx));
					glVertex3f(+0.03f * color, 10.0f * sinf(dx), 10.0f * cosf(dx));
				}
				for (x=90.0f; x<135.0f; x+=9.0f)
				{
					const float deg = 3.14f / 180.0f;
					float dx = x * deg;
					float color = 1.0f - fabsf(90.0f - x) / 45.0f;

					glColor3f(1.0f * color, 0.5f * color, 0.5f * color);
					glVertex3f(-0.03f * color, 2.0f * sinf(dx), 2.0f * cosf(dx));
					glVertex3f(-0.03f * color, 10.0f * sinf(dx), 10.0f * cosf(dx));
				}
			glEnd();
		glEndList();
	}

	glPushMatrix();
 		if (OptionsDisplay::instance()->getOldSightPosition())
		{		
			FixedVector &turretCenter = getMesh()->getTurretCenter();
			glTranslatef(
				tank_->getLife().getTargetPosition()[0].asFloat(),
				tank_->getLife().getTargetPosition()[1].asFloat(),
				tank_->getLife().getTargetPosition()[2].asFloat() +
				(turretCenter[2].asFloat() - getMesh()->getModel()->getMin()[2].asFloat()) * getMesh()->getScale());
			glRotatef(tank_->getShotInfo().getRotationGunXY().asFloat(), 
				0.0f, 0.0f, 1.0f);
			
			FixedVector &gunOffSet = getMesh()->getGunOffSet();
			glTranslatef(
				gunOffSet[0].asFloat() * getMesh()->getScale(), 
				gunOffSet[1].asFloat() * getMesh()->getScale(), 
				gunOffSet[2].asFloat() * getMesh()->getScale());
		}
		else
		{
			FixedVector &tankGunPosition = TankLib::getTankGunPosition(
				tank_->getLife().getTankTurretPosition(),
				tank_->getShotInfo().getRotationGunXY(),
				tank_->getShotInfo().getRotationGunYZ());

			glTranslatef(
				tankGunPosition[0].asFloat(),
				tankGunPosition[1].asFloat(),
				tankGunPosition[2].asFloat());
			glRotatef(tank_->getShotInfo().getRotationGunXY().asFloat(), 
				0.0f, 0.0f, 1.0f);
		}

		glRotatef(tank_->getShotInfo().getRotationGunYZ().asFloat(), 
			1.0f, 0.0f, 0.0f);
		glCallList(sightList_);
	glPopMatrix();
}

void TargetRendererImplTank::fired()
{
	fireOffSet_ = -0.25f;
}

void TargetRendererImplTank::shieldHit()
{
	shieldHit_ = 0.25f;
}

void TargetRendererImplTank::simulate(float frameTime)
{
	if (tank_->getState().getState() != TankState::sNormal) return;

	totalTime_ += frameTime;
	frame_ += frameTime * 20.0f;

	if (fireOffSet_ < 0.0f)
	{
		fireOffSet_ += frameTime / 25.0f;
		if (fireOffSet_ > 0.0f) fireOffSet_ = 0.0f;
	}
	if (shieldHit_ > 0.0f)
	{
		shieldHit_ -= frameTime / 25.0f;
		if (shieldHit_ < 0.0f) shieldHit_ = 0.0f;
	}
	if (tank_->getLife().getLife() < tank_->getLife().getMaxLife())
	{
		smokeTime_ += frameTime;
		if (smokeTime_ >= smokeWaitForTime_)
		{
			FixedVector &tankTurretPosition = tank_->getLife().getTankTurretPosition();

			const float randOff = 1.0f;
			const float randOffDiv = 0.5f;
			float randX = RAND * randOff - randOffDiv; 
			float randY = RAND * randOff - randOffDiv; 
			Landscape::instance()->getSmoke().addSmoke(
				tankTurretPosition[0].asFloat() + randX, 
				tankTurretPosition[1].asFloat() + randY, 
				tankTurretPosition[2].asFloat());

			smokeWaitForTime_ = (
				(RAND * float(tank_->getLife().getLife().asFloat()) * 10.0f) + 250.0f)
				/ 3000.0f;;
			smokeTime_ = 0.0f;
		}
	}
}

void TargetRendererImplTank::drawArrow()
{
	if (!OptionsDisplay::instance()->getDrawPlayerColor())
	{
		return;
	}

	GLState currentState(GLState::TEXTURE_OFF | GLState::BLEND_OFF);
	Vector &bilX = GLCameraFrustum::instance()->getBilboardVectorX();
	bilX /= 2.0f;

	Vector &position = tank_->getLife().getTargetPosition().asVector();
	float height = position[2];
	float groundHeight = ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().
		getHeight((int) position[0], (int) position[1]).asFloat();
	if (height < groundHeight)
	{
		height = groundHeight;
	}

	// Arrow over tank	
	{
		GLState currentState(GLState::TEXTURE_ON | GLState::BLEND_ON);
		ExplosionTextures::instance()->arrowTexture.draw(true);

		glDepthMask(GL_FALSE);
		glColor3fv(tank_->getColor());
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(position[0] - bilX[0], 
				position[1] - bilX[1], height + 4.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(position[0] + bilX[0], 
				position[1] + bilX[1], height + 4.0f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(position[0] + bilX[0], 
				position[1] + bilX[1], height + 7.0f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(position[0] - bilX[0], 
				position[1] - bilX[1], height + 7.0f);
		glEnd();
		glDepthMask(GL_TRUE);
	}

}

void TargetRendererImplTank::drawLife()
{
	if (!OptionsDisplay::instance()->getDrawPlayerHealth())
	{
		return;
	}

	GLState currentState(GLState::TEXTURE_OFF | GLState::BLEND_OFF);
	Vector &bilX = GLCameraFrustum::instance()->getBilboardVectorX();
	bilX /= 2.0f;

	Vector &position = tank_->getLife().getTargetPosition().asVector();
	float height = position[2];
	float groundHeight = ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().
		getHeight((int) position[0], (int) position[1]).asFloat();
	if (height < groundHeight)
	{
		height = groundHeight;
	}
	{
		float shieldLife = 0.0f;
		Accessory *currentShield =
			tank_->getShield().getGraphicalCurrentShield();
		if (currentShield)
		{	
			Shield *shield =  (Shield *) currentShield->getAction();
			shieldLife = tank_->getShield().getGraphicalShieldPower().asFloat() / 
				shield->getPower().asFloat() * 100.0f;
		}

		drawLifeBar(bilX, 
			tank_->getLife().getLife().asFloat() / 
			tank_->getLife().getMaxLife().asFloat() * 100.0f, 
			height, 3.3f);
		drawLifeBar(bilX, shieldLife, height, 3.7f);
	}
}

void TargetRendererImplTank::drawLifeBar(Vector &bilX, float value, 
									float height, float barheight)
{
	Vector &position = tank_->getLife().getTargetPosition().asVector();
	glBegin(GL_QUADS);
		if (value == 100.0f || value == 0.0f)
		{
			if (value == 100.0f) glColor3f(0.0f, 1.0f, 0.0f);
			else glColor3f(0.0f, 0.0f, 0.0f);

			glVertex3f(position[0] + bilX[0], 
				position[1] + bilX[1], height + barheight);
			glVertex3f(position[0] + bilX[0], 
				position[1] + bilX[1], height + barheight + 0.2f);
			glVertex3f(position[0] - bilX[0], 
				position[1] - bilX[1], height + barheight + 0.2f);
			glVertex3f(position[0] - bilX[0], 
				position[1] - bilX[1], height + barheight);
		}
		else
		{
			float lifePer = value / 50.0f;
			static Vector newBilX;
			newBilX = -bilX;
			newBilX += bilX * lifePer;

			glColor3f(0.0f, 1.0f, 0.0f);
			glVertex3f(position[0] + newBilX[0], 
				position[1] + newBilX[1], height + barheight);
			glVertex3f(position[0] + newBilX[0], 
				position[1] + newBilX[1], height + barheight + 0.2f);
			glVertex3f(position[0] - bilX[0], 
				position[1] - bilX[1], height + barheight + 0.2f);
			glVertex3f(position[0] - bilX[0], 
				position[1] - bilX[1], height + barheight);

			glColor3f(1.0f, 0.0f, 0.0f);

			glVertex3f(position[0] + bilX[0], 
				position[1] + bilX[1], height + barheight);
			glVertex3f(position[0] + bilX[0], 
				position[1] + bilX[1], height + barheight + 0.2f);
			glVertex3f(position[0] + newBilX[0], 
				position[1] + newBilX[1], height + barheight + 0.2f);
			glVertex3f(position[0] + newBilX[0], 
				position[1] + newBilX[1], height + barheight);
		}
	glEnd();
}

void TargetRendererImplTank::render2D(float distance)
{
	// Add the tooltip that displays the tank info
	GLWToolTip::instance()->addToolTip(&tankTips_.tankTip,
		float(posX_) - 10.0f, float(posY_) - 10.0f, 20.0f, 20.0f);

	// Draw the hightlighted ring around the tank
	bool currentTank = 
		(tank_ == ScorchedClient::instance()->getTargetContainer().getCurrentTank() &&
		ScorchedClient::instance()->getGameState().getState() == ClientState::StatePlaying);
	if (currentTank)
	{
		GLState firstState(GLState::DEPTH_OFF);
		if (TargetRendererImplTankHUD::drawText())
		{
			Vector yellow(0.7f, 0.7f, 0.0f);
			GLWFont::instance()->getGameFont()->draw(
				yellow, 10,
				(float) posX_ + 47.0f, (float) posY_ - 4.0f, (float) posZ_,
				TargetRendererImplTankHUD::getTextA());
			GLWFont::instance()->getGameFont()->draw(
				yellow, 10,
				(float) posX_ + 47.0f, (float) posY_ - 15.0f, (float) posZ_,
				TargetRendererImplTankHUD::getTextB());

			if (TargetRendererImplTankHUD::getPercentage() >= 0.0f)
			{
				float totalHeight = 40.0f;
				float halfHeight = totalHeight / 2.0f;
				float height = totalHeight * TargetRendererImplTankHUD::getPercentage() / 100.0f;
				
				GLState state2(GLState::TEXTURE_OFF);
				glBegin(GL_QUADS);
					glColor3f(1.0f, 0.0f, 0.0f);
					glVertex2f((float) posX_ + 42.0f, (float) posY_ -halfHeight);
					glVertex2f((float) posX_ + 45.0f, (float) posY_ -halfHeight);
					glVertex2f((float) posX_ + 45.0f, (float) posY_ -halfHeight + height);
					glVertex2f((float) posX_ + 42.0f, (float) posY_ -halfHeight + height);

					glColor3f(0.0f, 0.0f, 0.0f);
					glVertex2f((float) posX_ + 42.0f, (float) posY_ -halfHeight + height);
					glVertex2f((float) posX_ + 45.0f, (float) posY_ -halfHeight + height);
					glVertex2f((float) posX_ + 45.0f, (float) posY_ -halfHeight + totalHeight);
					glVertex2f((float) posX_ + 42.0f, (float) posY_ -halfHeight + totalHeight);
				glEnd();
				glBegin(GL_LINES);
					glColor3f(1.0f, 0.0f, 0.0f);
					glVertex2f((float) posX_ + 40.0f, (float) posY_ -halfHeight);
					glVertex2f((float) posX_ + 47.0f, (float) posY_ -halfHeight);

					glVertex2f((float) posX_ + 40.0f, (float) posY_ +halfHeight);
					glVertex2f((float) posX_ + 47.0f, (float) posY_ +halfHeight);
				glEnd();
			}
		}

		GLState newState(GLState::TEXTURE_OFF | GLState::BLEND_ON);

		glColor4f(0.9f, 0.9f, 1.0f, 0.5f);
		static GLuint listNo = 0;
		if (!listNo)
		{
			glNewList(listNo = glGenLists(1), GL_COMPILE);
				glBegin(GL_QUADS);
					for (float a=0; a<6.25f ;a+=0.25f)
					{
						const float skip = 0.2f;
						glVertex2d(sin(a + skip) * 40.0, 
							cos(a + skip) * 40.0);
						glVertex2d(sin(a) * 40.0, 
							cos(a) * 40.0);
						glVertex2d(sin(a) * 35.0, 
							cos(a) * 35.0);			
						glVertex2d(sin(a + skip) * 35.0, 
							cos(a + skip) * 35.0);
					}
				glEnd();
			glEndList();
		}

		glPushMatrix();
			glTranslated(posX_, posY_, 0.0);
			glCallList(listNo);
		glPopMatrix();
	}

	// Draw highlight around the tank
	if ((highlightType_ == ePlayerHighlight && tank_->getDestinationId()) ||
		(highlightType_ == eOtherHighlight && !tank_->getDestinationId() ))
	{
		TutorialDialog::instance()->drawHighlight(
			float(posX_) - 10.0f, float(posY_) - 10.0f, 20.0f, 20.0f);
	}
}
