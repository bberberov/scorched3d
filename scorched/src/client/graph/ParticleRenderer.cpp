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

#include <graph/ParticleRenderer.h>
#include <graph/Particle.h>
#include <sprites/ExplosionTextures.h>
#include <sprites/DebrisActionRenderer.h>
#include <sprites/SmokeActionRenderer.h>
#include <sprites/NapalmRenderer.h>
#include <sprites/ExplosionNukeRenderer.h>
#include <sprites/WallActionRenderer.h>
#include <landscape/Landscape.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscape/ShadowMap.h>
#include <client/ScorchedClient.h>
#include <GLEXT/GLState.h>
#include <GLEXT/GLCamera.h>
#include <GLEXT/GLCameraFrustum.h>
#include <GLEXT/GLInfo.h>
#include <common/Defines.h>
#include <math.h>

ParticleRendererPoints *ParticleRendererPoints::getInstance()
{
	static ParticleRendererPoints instance_;
	return &instance_;
}

void ParticleRendererPoints::simulateParticle(Particle &particle, float time)
{
}

void ParticleRendererPoints::renderParticle(Particle &particle)
{
	GLState state(GLState::TEXTURE_OFF);
	glPointSize(4.0f);
	glColor4f(
		particle.color_[0],
		particle.color_[1],
		particle.color_[2], 
		particle.alpha_);
	glBegin(GL_POINTS);
		glVertex3fv(particle.position_);
	glEnd();
	glPointSize(1.0f);
}

ParticleRendererQuads *ParticleRendererQuads::getInstance()
{
	static ParticleRendererQuads instance_;
	return &instance_;
}

void ParticleRendererQuads::simulateParticle(Particle &particle, float time)
{
}

void ParticleRendererQuads::renderParticle(Particle &particle)
{
	if (!particle.texture_ && !particle.textureSet_) return;

	if (particle.textureSet_)
	{
		int tex = int((particle.textureSet_->getNoTextures() - 1) * particle.percent_);
		tex = MIN(MAX(tex, 0), particle.textureSet_->getNoTextures() - 1);
		GLTexture *texture = particle.textureSet_->getTexture(tex);
		texture->draw();
	}
	else
	{
		particle.texture_->draw();
	}

	GLCameraFrustum::instance()->drawBilboard(
		particle.position_, particle.color_, particle.alpha_,
		particle.size_[0], particle.size_[1], particle.additiveTexture_, 
		particle.textureCoord_);

	// Add a shadow of the smoke on the ground
	float posX = particle.position_[0];
	float posY = particle.position_[1];
	float posZ = particle.position_[2];
	if (particle.shadow_)
	{
		float aboveGround =
			posZ - ScorchedClient::instance()->getLandscapeMaps().
			getGroundMaps().getHeight(
			int(posX), int(posY)).asFloat();
		float smokeAlpha = particle.alpha_ + .2f; 
		if (smokeAlpha > 1.0f) smokeAlpha = 1.0f;
		Landscape::instance()->getShadowMap().
			addCircle(posX, posY, 
			(particle.size_[0] * aboveGround) / 10.0f, smokeAlpha);
	}

	GLInfo::addNoTriangles(2);
}

ParticleRendererDebris *ParticleRendererDebris::getInstance()
{
	static ParticleRendererDebris instance_;
	return &instance_;
}

void ParticleRendererDebris::renderParticle(Particle &particle)
{
	DebrisActionRenderer *renderer = (DebrisActionRenderer *)
		particle.userData_;
	renderer->draw(particle.position_);
}

void ParticleRendererDebris::simulateParticle(Particle &particle, float time)
{
	DebrisActionRenderer *renderer = (DebrisActionRenderer *)
		particle.userData_;
	renderer->simulate(time);	
}

ParticleRendererSmoke *ParticleRendererSmoke::getInstance()
{
	static ParticleRendererSmoke instance_;
	return &instance_;
}

void ParticleRendererSmoke::renderParticle(Particle &particle)
{
}

void ParticleRendererSmoke::simulateParticle(Particle &particle, float time)
{

}

ParticleRendererNapalm *ParticleRendererNapalm::getInstance()
{
	static ParticleRendererNapalm instance_;
	return &instance_;
}

void ParticleRendererNapalm::renderParticle(Particle &particle)
{
	NapalmRenderer *renderer = (NapalmRenderer *)
		particle.userData_;
	renderer->draw(&particle);
	ParticleRendererQuads::getInstance()->renderParticle(particle);
}

void ParticleRendererNapalm::simulateParticle(Particle &particle, float time)
{
	NapalmRenderer *renderer = (NapalmRenderer *)
		particle.userData_;
	renderer->simulate(&particle, time);
}

ParticleRendererMushroom *ParticleRendererMushroom::getInstance()
{
	static ParticleRendererMushroom instance_;
	return &instance_;
}

void ParticleRendererMushroom::renderParticle(Particle &particle)
{
	ExplosionNukeRendererEntry *renderer = (ExplosionNukeRendererEntry *)
		particle.userData_;

	Vector oldPosition = particle.position_;
	bool shadow = particle.shadow_;

	particle.shadow_ = false;
	particle.position_[0] = oldPosition[0];
	particle.position_[1] = oldPosition[1] + 
		getFastSin(renderer->getCloudRotation() + 2.0f) * 2.0f;
	particle.position_[2] = oldPosition[2] + 
		getFastCos(renderer->getCloudRotation() + 2.0f) * 2.0f;
	ParticleRendererQuads::getInstance()->renderParticle(particle);

	particle.shadow_ = false;
	particle.position_[0] = oldPosition[0];
	particle.position_[1] = oldPosition[1] + 
		getFastSin(renderer->getCloudRotation() + 4.0f) * 2.0f;
	particle.position_[2] = oldPosition[2] + 
		getFastCos(renderer->getCloudRotation() + 4.0f) * 2.0f;
	ParticleRendererQuads::getInstance()->renderParticle(particle);

	particle.shadow_ = shadow;
	particle.position_[0] = oldPosition[0];
	particle.position_[1] = oldPosition[1] + 
		getFastSin(renderer->getCloudRotation()) * 2.0f;
	particle.position_[2] = oldPosition[2] + 
		getFastCos(renderer->getCloudRotation()) * 2.0f;
	ParticleRendererQuads::getInstance()->renderParticle(particle);

	particle.position_ = oldPosition;
	particle.shadow_ = shadow;
}

void ParticleRendererMushroom::simulateParticle(Particle &particle, float time)
{
	ExplosionNukeRendererEntry *renderer = (ExplosionNukeRendererEntry *)
		particle.userData_;
	renderer->simulate(&particle, time);
}

ParticleRendererRain *ParticleRendererRain::getInstance()
{
	static ParticleRendererRain instance_;
	return &instance_;
}

void ParticleRendererRain::renderParticle(Particle &particle)
{
	ParticleRendererQuads::getInstance()->renderParticle(particle);
}

void ParticleRendererRain::simulateParticle(Particle &particle, float time)
{
	if (particle.position_[2] < 0.0f)
	{
		particle.life_ = 0.0f;
		return;
	}

	Vector &cameraPos = particle.engine_->getCamera()->getCurrentPos();
	Vector &cameraTarget = particle.engine_->getCamera()->getLookAt();
	Vector cameraDirection = (cameraTarget - cameraPos).Normalize();

	// Size
	particle.size_[0] = 0.1f;
	particle.size_[1] = 1.0f - fabsf(cameraDirection[2]) + 0.1f;

	// Alpha
	const float MaxDist = 200.0f * 200.0f;
	float alpha = 0.0f;
	if (particle.distance_ < MaxDist)
	{
		alpha = 0.7f * (1.0f - (particle.distance_ / MaxDist));
	}
	particle.alpha_ = alpha;
}

ParticleRendererSnow *ParticleRendererSnow::getInstance()
{
	static ParticleRendererSnow instance_;
	return &instance_;
}

void ParticleRendererSnow::renderParticle(Particle &particle)
{
	ParticleRendererQuads::getInstance()->renderParticle(particle);
}

void ParticleRendererSnow::simulateParticle(Particle &particle, float time)
{
	if (particle.position_[2] < 0.0f)
	{
		particle.life_ = 0.0f;
		return;
	}

	// Alpha
	const float MaxDist = 200.0f * 200.0f;
	float alpha = 0.0f;
	if (particle.distance_ < MaxDist)
	{
		alpha = 0.7f * (1.0f - (particle.distance_ / MaxDist));
	}
	particle.alpha_ = alpha;
}

ParticleRendererWall *ParticleRendererWall::getInstance()
{
	static ParticleRendererWall instance_;
	return &instance_;
}

void ParticleRendererWall::renderParticle(Particle &particle)
{
	WallActionRenderer *renderer = (WallActionRenderer *)
		particle.userData_;
	renderer->draw();
}

void ParticleRendererWall::simulateParticle(Particle &particle, float time)
{
	WallActionRenderer *renderer = (WallActionRenderer *)
		particle.userData_;
	renderer->simulate(time);
}
