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

#include <sprites/MissileActionRenderer.h>
#include <sprites/ExplosionTextures.h>
#include <GLEXT/GLCameraFrustum.h>
#include <weapons/Accessory.h>
#include <actions/ShotProjectile.h>
#include <landscape/Landscape.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscape/ShadowMap.h>
#include <graph/OptionsDisplay.h>
#include <common/Defines.h>
#include <target/TargetContainer.h>
#include <client/ScorchedClient.h>
#include <engine/ScorchedContext.h>
#include <graph/ParticleEngine.h>
#include <sound/Sound.h>
#include <tank/Tank.h>

MissileActionRenderer::MissileActionRenderer(int flareType, float scale, float spinSpeed, const Vector &rotationAxis) : 
	flareType_(flareType), counter_(0.05f, 0.05f), 
	mesh_(0), scale_(scale), rotation_(180.0f),
	flameemitter_(0), smokeemitter_(0), sound_(0),
	rotationAxis_(rotationAxis),
	spinSpeed_(spinSpeed)
{
	frame_ = (float) rand();
}

MissileActionRenderer::~MissileActionRenderer()
{
	delete flameemitter_;
	delete smokeemitter_;
	delete sound_;
}

void MissileActionRenderer::simulate(Action *action, float timepassed, bool &remove)
{
	ShotProjectile *shot = (ShotProjectile *) action;
	if (!flameemitter_)
	{
		flameTextureSet_ = ExplosionTextures::instance()->getTextureSetByName(
			shot->getWeapon()->getFlameTexture());
		flameemitter_ = new ParticleEmitter;
		flameemitter_->setAttributes(
			shot->getWeapon()->getFlameLife() / 2.0f, shot->getWeapon()->getFlameLife(), // Life
			0.5f, 1.0f, // Mass
			0.01f, 0.02f, // Friction
			Vector(-0.05f, -0.1f, 0.3f), Vector(0.05f, 0.1f, 0.9f), // Velocity
			shot->getWeapon()->getFlameStartColor1(), 0.9f, // StartColor1
			shot->getWeapon()->getFlameStartColor2(), 1.0f, // StartColor2
			shot->getWeapon()->getFlameEndColor1(), 0.0f, // EndColor1
			shot->getWeapon()->getFlameEndColor2(), 0.1f, // EndColor2
			shot->getWeapon()->getFlameStartSize() / 2.0f, shot->getWeapon()->getFlameStartSize() / 2.0f, shot->getWeapon()->getFlameStartSize(), shot->getWeapon()->getFlameStartSize(), // Start Size
			shot->getWeapon()->getFlameEndSize() / 2.0f, shot->getWeapon()->getFlameEndSize() / 2.0f, shot->getWeapon()->getFlameEndSize(), shot->getWeapon()->getFlameEndSize(), // EndSize
			Vector(0.0f, 0.0f, 10.0f), // Gravity
			true,
			true);
	}
	if (!smokeemitter_)
	{
		smokeTextureSet_ = ExplosionTextures::instance()->getTextureSetByName(
			shot->getWeapon()->getSmokeTexture());
		smokeemitter_ = new ParticleEmitter;
		smokeemitter_->setAttributes(
			shot->getWeapon()->getSmokeLife() / 2.0f, shot->getWeapon()->getSmokeLife(), // Life
			0.2f, 0.5f, // Mass
			0.01f, 0.02f, // Friction
			Vector(-0.05f, -0.1f, 0.3f), Vector(0.05f, 0.1f, 0.9f), // Velocity
			Vector(0.7f, 0.7f, 0.7f), 0.3f, // StartColor1
			Vector(0.7f, 0.7f, 0.7f), 0.3f, // StartColor2
			Vector(0.7f, 0.7f, 0.7f), 0.0f, // EndColor1
			Vector(0.8f, 0.8f, 0.8f), 0.1f, // EndColor2
			shot->getWeapon()->getSmokeStartSize() / 2.0f, shot->getWeapon()->getSmokeStartSize() / 2.0f, shot->getWeapon()->getSmokeStartSize(), shot->getWeapon()->getSmokeStartSize(), // Start Size
			shot->getWeapon()->getSmokeEndSize() / 2.0f, shot->getWeapon()->getSmokeEndSize() / 2.0f, shot->getWeapon()->getSmokeEndSize(), shot->getWeapon()->getSmokeEndSize(), // EndSize
			Vector(0.0f, 0.0f, 100.0f), // Gravity
			false,
			true);
	}
	if (!sound_)
	{
		const char *engineSound = shot->getWeapon()->getEngineSound();
		if (0 != strcmp("none", engineSound))
		{
			SoundBuffer *rocket = Sound::instance()->fetchOrCreateBuffer(
				S3D::getModFile(engineSound));
			sound_ = new VirtualSoundSource(VirtualSoundPriority::eMissile, true, false);
			sound_->setPosition(shot->getPhysics().getPosition().asVector());
			sound_->setGain(0.25f);
			sound_->play(rocket);
		}
	}
	if (sound_)
	{
		sound_->setPosition(shot->getPhysics().getPosition().asVector());
		sound_->setVelocity(shot->getPhysics().getVelocity().asVector());
	}

	Vector &actualPos = shot->getPhysics().getPosition().asVector();
	Vector actualPos1;
	actualPos1[0] = actualPos[0] - 0.25f;
	actualPos1[1] = actualPos[1] - 0.25f;
	actualPos1[2] = actualPos[2] - 0.25f;
	Vector actualPos2;
	actualPos2[0] = actualPos[0] + 0.25f;
	actualPos2[1] = actualPos[1] + 0.25f;
	actualPos2[2] = actualPos[2] + 0.25f;

	// Rotate the shot
	frame_ += timepassed * 20.0f;
	rotation_ += shot->getPhysics().getVelocity().Magnitude().asFloat() * spinSpeed_;

	// Add flame trail
	if (shot->getWeapon()->getCreateFlame())
	{
		flameemitter_->emitLinear(2, actualPos1, actualPos2, 
			ScorchedClient::instance()->getParticleEngine(), 
			ParticleRendererQuads::getInstance(),
			flameTextureSet_,
			shot->getWeapon()->getAnimateFlameTexture());
	}

	// Add the smoke trail
	if (shot->getWeapon()->getCreateSmoke())
	{
		if (counter_.nextDraw(timepassed))
		{
			Vector vel1 = shot->getPhysics().getVelocity().asVector();
			Vector vel2;
			vel1 *= -0.4f;
			vel2 = vel1 * 0.7f;

			actualPos1 -= shot->getPhysics().getVelocity().asVector() * 0.2f;
			actualPos2 -= shot->getPhysics().getVelocity().asVector() * 0.2f;

			smokeemitter_->setVelocity(vel1, vel2);
			smokeemitter_->emitLinear(3, actualPos1, actualPos2, 
				ScorchedClient::instance()->getParticleEngine(), 
				ParticleRendererQuads::getInstance(),
				smokeTextureSet_,
				shot->getWeapon()->getAnimateSmokeTexture());
		}
	}
}

void MissileActionRenderer::draw(Action *action)
{
	ShotProjectile *shot = (ShotProjectile *) action;
	Vector &actualPos = shot->getPhysics().getPosition().asVector();
	Vector &actualdir = shot->getPhysics().getVelocity().asVector();

	if (shot->getWeapon()->getShowShotPath())
	{
		Tank *current = 
			action->getScorchedContext()->getTargetContainer().
			getTankById(shot->getPlayerId());
		if (current)
		{
			glColor3fv(current->getColor());
			RenderTracer::instance()->drawSmokeTracer(
				shot->getPositions());
		}
	}

	// Check we can see the missile
	if (!GLCameraFrustum::instance()->sphereInFrustum(actualPos, 1.0f))
	{
		return;
	}

	// Do we have a loaded mesh
	if (!mesh_)
	{
		Tank *currentPlayer = action->
			getScorchedContext()->getTargetContainer().getTankById(
			shot->getPlayerId());
		mesh_ = Accessory::getWeaponMesh(
			shot->getWeapon()->getModelID(), currentPlayer);
	}

	// Draw the missile
	mesh_->setScale(scale_);
	mesh_->draw(actualPos, actualdir, flareType_, rotation_, rotationAxis_, frame_);

	// Draw the missile shadow
	float aboveGround =
		actualPos[2] - ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().
		getHeight((int) actualPos[0], (int) actualPos[1]).asFloat();
	Landscape::instance()->getShadowMap().
		addCircle(actualPos[0], actualPos[1], aboveGround / 10.0f);
}
