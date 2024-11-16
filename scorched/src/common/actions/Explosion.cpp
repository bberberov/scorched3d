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

#include <target/TargetDamageCalc.h>
#include <common/OptionsScorched.h>
#include <common/Defines.h>
#include <common/Logger.h>
#include <weapons/AccessoryStore.h>
#include <weapons/WeaponExplosion.h>
#include <actions/Explosion.h>
#include <actions/CameraPositionAction.h>
#include <engine/ScorchedContext.h>
#include <engine/ActionController.h>
#ifndef S3D_SERVER
	#include <graph/OptionsDisplay.h>
	#include <graph/ParticleEmitter.h>
	#include <graph/MainCamera.h>
	#include <client/ScorchedClient.h>
	#include <sound/SoundUtils.h>
#endif
#include <landscapemap/DeformLandscape.h>
#include <landscapemap/LandscapeMaps.h>
#ifndef S3D_SERVER
	#include <landscape/Landscape.h>
	#include <water/Water.h>
	#include <landscape/Smoke.h>
	#include <sprites/ExplosionNukeRenderer.h>
	#include <sprites/ExplosionTextures.h>
#endif
#include <math.h>

Explosion::Explosion(FixedVector &position,
	FixedVector &velocity,
	ExplosionParams *params,
	Weapon *weapon, WeaponFireContext &weaponContext) :
	Action(weaponContext.getInternalContext().getReferenced()),
	params_(params),
	firstTime_(true), totalTime_(0),
	weapon_(weapon), weaponContext_(weaponContext), 
	position_(position), velocity_(velocity)
{
}

Explosion::~Explosion()
{
	delete params_;
}

void Explosion::init()
{
	fixed multiplier = fixed(((int) context_->getOptionsGame().getWeapScale()) - 
							 OptionsGame::ScaleMedium);
	multiplier *= fixed(true, 5000);
	multiplier += 1;
	fixed explosionSize = params_->getSize() * multiplier;	

#ifndef S3D_SERVER
	if (!context_->getServerMode()) 
	{
		float height = context_->getLandscapeMaps().getGroundMaps().getInterpHeight(
			position_[0], position_[1]).asFloat();
		float aboveGround = position_[2].asFloat() - height;

		// If there is a weapon play a splash sound when in water
		bool waterSplash = false;
		if (params_->getCreateSplash())
		{
			waterSplash = 
				Landscape::instance()->getWater().explosion(
					position_.asVector(), params_->getSize().asFloat());
		}

		// Create particles from the center of the explosion
		// These particles will render smoke trails or bits of debris
		if (params_->getCreateDebris())
		{
			// If we are below the ground create the spray of dirt (or water)
			if (aboveGround < 1.0f && !waterSplash)
			{
				ParticleEmitter sprayemitter;
				sprayemitter.setAttributes(
					3.0f, 4.0f, // Life
					0.5f, 1.0f, // Mass
					0.01f, 0.02f, // Friction
					Vector(0.0f, 0.0f, 0.0f), Vector(0.0f, 0.0f, 0.0f), // Velocity
					Vector(0.9f, 0.9f, 0.9f), 0.5f, // StartColor1
					Vector(1.0f, 1.0f, 1.0f), 0.7f, // StartColor2
					Vector(0.9f, 0.9f, 0.9f), 0.0f, // EndColor1
					Vector(1.0f, 1.0f, 1.0f), 0.1f, // EndColor2
					3.0f, 3.0f, 4.0f, 4.0f, // Start Size
					3.0f, 3.0f, 4.0f, 4.0f, // EndSize
					Vector(0.0f, 0.0f, -800.0f), // Gravity
					false,
					true);

				sprayemitter.emitSpray(position_.asVector(), 
					ScorchedClient::instance()->getParticleEngine(),
					params_->getSize().asFloat() - 2.0f,
					&Landscape::instance()->getLandscapeTexture1());
			}

			ParticleEmitter emitter;
			emitter.setAttributes(
				2.5f, 4.0f, // Life
				0.2f, 0.5f, // Mass
				0.01f, 0.02f, // Friction
				Vector(-0.05f, -0.1f, 0.3f), Vector(0.05f, 0.1f, 0.9f), // Velocity
				Vector(0.7f, 0.7f, 0.7f), 0.3f, // StartColor1
				Vector(0.7f, 0.7f, 0.7f), 0.3f, // StartColor2
				Vector(0.7f, 0.7f, 0.7f), 0.0f, // EndColor1
				Vector(0.8f, 0.8f, 0.8f), 0.1f, // EndColor2
				0.2f, 0.2f, 0.5f, 0.5f, // Start Size
				2.2f, 2.2f, 4.0f, 4.0f, // EndSize
				Vector(0.0f, 0.0f, -800.0f), // Gravity
				false,
				true);

			// Create debris
			float mult = float(
				OptionsDisplay::instance()->getExplosionParticlesMult()) / 40.0f;
			int debris = 5 + int(params_->getSize().asFloat() * mult);
			emitter.emitDebris(debris,
				position_.asVector(), 
				ScorchedClient::instance()->getParticleEngine());
		}

		if (0 == strcmp(weapon_->getAccessoryTypeName(), "WeaponMuzzle"))
		{
			// Add initial smoke clouds
			for (int a=0; a<OptionsDisplay::instance()->getNumberExplosionSubParts() * 2; a++)
			{
				float posXY = (RAND * 4.0f) - 2.0f;
				float posYZ = (RAND * 4.0f) - 2.0f;

				Landscape::instance()->getSmoke().addSmoke(
					position_[0].asFloat() + posXY, 
					position_[1].asFloat() + posYZ, 
					position_[2].asFloat() + 2.0f);
			}
		}
		else
		{
			if (!params_->getNoCameraTrack())
			{
				TankViewPointProvider *vPoint = new TankViewPointProvider();
				vPoint->setValues(position_, TankViewPointProvider::defaultLookFrom, explosionSize);
				CameraPositionAction *pos = new CameraPositionAction(
					weaponContext_.getPlayerId(),
					vPoint,
					4, 10, true);
				context_->getActionController().addAction(pos);
			}
		}

		{
			GLTextureSet *texture = 0;
			if (0 != strcmp(params_->getExplosionTexture(), "none"))
			{
				texture = ExplosionTextures::instance()->getTextureSetByName(
					params_->getExplosionTexture());
			}

			if (texture)
			{
				Vector expColor = params_->getExplosionColor().asVector();

				if (params_->getExplosionType() == ExplosionParams::ExplosionNormal)
				{
					ParticleEmitter exploemitter;
					exploemitter.setAttributes(
						params_->getMinLife().asFloat(), params_->getMaxLife().asFloat(), // Life
						0.2f, 0.5f, // Mass
						0.01f, 0.02f, // Friction
						Vector(0.0f, 0.0f, 0.0f), Vector(0.0f, 0.0f, 0.0f), // Velocity
						expColor, 0.8f, // StartColor1
						expColor, 0.9f, // StartColor2
						expColor, 0.0f, // EndColor1
						expColor, 0.1f, // EndColor2
						0.2f, 0.2f, 0.5f, 0.5f, // Start Size
						2.2f, 2.2f, 4.0f, 4.0f, // EndSize
						Vector(0.0f, 0.0f, 0.0f), // Gravity
						params_->getLuminance(),
						params_->getWindAffected());
					exploemitter.emitExplosion(position_.asVector(),
						ScorchedClient::instance()->getParticleEngine(),
						explosionSize.asFloat(),
						texture,
						params_->getAnimate());
				}
				else
				{
					// Emmit explosion ring
					ParticleEmitter emmiter;
					emmiter.setAttributes(
						params_->getMinLife().asFloat(), params_->getMaxLife().asFloat(), // Life
						0.5f, 0.5f, // Mass
						0.0f, 0.0f, // Friction
						Vector(), Vector(), // Velocity
						Vector(0.0f, 0.0f, 0.8f), 0.9f, // StartColor1
						Vector(0.2f, 0.2f, 0.9f), 1.0f, // StartColor2
						Vector(0.6f, 0.6f, 0.95f), 0.0f, // EndColor1
						Vector(0.8f, 0.8f, 1.0f), 0.1f, // EndColor2
						0.2f, 0.2f, 0.5f, 0.5f, // Start Size
						1.5f, 1.5f, 3.0f, 3.0f, // EndSize
						Vector(0.0f, 0.0f, 0.0f), // Gravity
						params_->getLuminance(),
						params_->getWindAffected());

					Vector axis(0.0f, 0.0f, 1.0f);
					if (params_->getExplosionType() == ExplosionParams::ExplosionRingDirectional)
					{
						axis = velocity_.asVector();
						if (axis.MagnitudeSquared() == 0.0f)
						{
							axis[2] = 1.0f;
						}
					}
					emmiter.emitExplosionRing(400, 
						position_.asVector(),
						axis,
						ScorchedClient::instance()->getParticleEngine(),
						explosionSize.asFloat(),
						texture,
						params_->getAnimate());
				}
			}
		}

		if (params_->getCreateMushroomAmount() > 0)
		{
			if (RAND <= params_->getCreateMushroomAmount().asFloat())
			{
				GLTextureSet *texture = ExplosionTextures::instance()->getTextureSetByName(
					params_->getMushroomTexture());
				if (texture)
				{
					context_->getActionController().addAction(
						new SpriteAction(
						new ExplosionNukeRenderer(position_.asVector(), 
							params_->getSize().asFloat() - 2.0f,
							texture,
							false)));	
				}
			}
		}

		// Make the camera shake
		MainCamera::instance()->getCamera().addShake(params_->getShake().asFloat());
	}
#endif // #ifndef S3D_SERVER
}

std::string Explosion::getActionDetails()
{
	return S3D::formatStringBuffer("%s %s", 
		position_.asQuickString(), 
		weapon_->getParent()->getName());
}

void Explosion::simulate(fixed frameTime, bool &remove)
{
	totalTime_ += frameTime;
	if (firstTime_)
	{
		firstTime_ = false;
#ifndef S3D_SERVER
		if (!context_->getServerMode()) 
		{
			if (params_->getExplosionSound() &&
				0 != strcmp("none", params_->getExplosionSound()))
			{
				SoundBuffer *expSound = 
					Sound::instance()->fetchOrCreateBuffer(
						S3D::getModFile(S3D::formatStringBuffer("data/wav/%s", params_->getExplosionSound())));
				SoundUtils::playAbsoluteSound(VirtualSoundPriority::eAction,
					expSound, position_.asVector());
			}
		}
#endif // #ifndef S3D_SERVER

		// Get the land height at the explosion
		fixed landHeight = context_->getLandscapeMaps().getGroundMaps().
				getInterpHeight(position_[0], position_[1]);

		// Dirt should only form along the ground
		FixedVector newPosition = position_;
		if (params_->getDeformType() == ExplosionParams::DeformUp)
		{
			newPosition[2] = landHeight;
		}

		if (params_->getDeformType() != ExplosionParams::DeformNone)
		{
			// Get the actual explosion size
			fixed multiplier = 
				fixed(((int) context_->getOptionsGame().getWeapScale()) - 
					OptionsGame::ScaleMedium);
			multiplier *= fixed(true, 5000);
			multiplier += 1;
			fixed deformExplosionSize = params_->getDeformSize() * multiplier;	

			// Check if we are allowed to explode underground
			if (!params_->getExplodeUnderGround())
			{
				if (position_[2] - landHeight < -1)
				{
					deformExplosionSize = 0;
				}
			}

			// Remove areas from the height map
			if (deformExplosionSize > 0)
			{
				DeformLandscape::deformLandscape(
					*context_,
					newPosition, deformExplosionSize, 
					(params_->getDeformType() == ExplosionParams::DeformDown), 1,
					params_->getDeformTexture());
			}
		}

		if (params_->getHurtAmount() != 0 ||
			params_->getDeformType() != ExplosionParams::DeformNone)
		{
			// Check the tanks for damage
			TargetDamageCalc::explosion(
				*context_,
				weapon_, weaponContext_, 
				newPosition, 
				params_->getSize() , 
				params_->getHurtAmount(),
				(params_->getDeformType() != ExplosionParams::DeformNone),
				params_->getOnlyHurtShield());
		}
	}

	remove = true;
	Action::simulate(frameTime, remove);
}
