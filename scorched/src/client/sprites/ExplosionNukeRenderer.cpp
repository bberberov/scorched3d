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

#include <sprites/ExplosionNukeRenderer.h>
#include <sprites/ExplosionTextures.h>
#include <client/ScorchedClient.h>
#include <common/Defines.h>
#include <graph/OptionsDisplay.h>
#include <landscape/Landscape.h>
#include <landscapemap/LandscapeMaps.h>
#include <math.h>

static const int AlphaSteps = int(ExplosionNukeRenderer_STEPS * 0.9f);

ExplosionNukeRendererEntry::ExplosionNukeRendererEntry(
	Vector &position, float size) :
	totalTime_(0.0f), size_(size),
	startPosition_(position), cloudRotation_(0.0f)
{
	float rot = RAND * 3.14f * 2.0f;
	float width = RAND * 0.5f + 1.0f;
    
	cloudRotation_ = RAND * 360.0f;
	rotation_[0] = getFastSin(rot) * width;
	rotation_[1] = getFastCos(rot) * width;
}

ExplosionNukeRendererEntry::~ExplosionNukeRendererEntry()
{
}

void ExplosionNukeRendererEntry::simulate(Particle *particle, float time)
{
	totalTime_ += time;
	cloudRotation_ += time * 5.0f;

	int position = int((totalTime_ / (16.0f / 3.0f)) * 
		float(ExplosionNukeRenderer_STEPS));
	if (position >= ExplosionNukeRenderer_STEPS)
		position = ExplosionNukeRenderer_STEPS - 1;

	float z = ExplosionNukeRenderer::positions_[position][2] * (10.0f + size_) / 30.0f;
	float w = ExplosionNukeRenderer::positions_[position][0] + (size_ / 2.0f);
	particle->position_[0] = startPosition_[0] + rotation_[0] * w;
	particle->position_[1] = startPosition_[1] + rotation_[1] * w;
	particle->position_[2] = startPosition_[2] + z;
}

Vector *ExplosionNukeRenderer::positions_ = 0;

ExplosionNukeRenderer::ExplosionNukeRenderer(Vector &position, float size,
	GLTextureSet *set,
	bool animate) 
	: set_(set), animate_(animate), totalTime_(0.0f), time_(0.0f), position_(position), size_(size)
{
	position_[2] -= size_;
	float height = ScorchedClient::instance()->getLandscapeMaps().
		getGroundMaps().getHeight((int) position_[0], (int) position_[1]).asFloat();
	if (position_[2] < height) position_[2] = height;

	if (!positions_)
	{
		positions_ = new Vector[ExplosionNukeRenderer_STEPS];

		float zPos = 0.0f;
		float width = 0.0f;

		for (int i=0; i<ExplosionNukeRenderer_STEPS; i++)
		{
			Vector pos;
			pos[0] = width;
			pos[1] = 0.0f;
			pos[2] = zPos;

			if (i > (ExplosionNukeRenderer_STEPS * (0.3f))){
				width += 0.5f;
				
			}else{ 
				
				zPos += 2.0f;
			}

			positions_[i] = pos;
		}
	}

	emitter_.setAttributes(
		5.0f, 5.0f, // Life
		0.2f, 0.5f, // Mass
		0.01f, 0.02f, // Friction
		Vector(0.0f, 0.0f, 0.0f), Vector(0.0f, 0.0f, 0.0f), // Velocity
		Vector(1.0f, 1.0f, 1.0f), 0.3f, // StartColor1
		Vector(1.0f, 1.0f, 1.0f), 0.2f, // StartColor2
		Vector(1.0f, 1.0f, 1.0f), 0.0f, // EndColor1
		Vector(1.0f, 1.0f, 1.0f), 0.0f, // EndColor2
		2.0f, 2.0f, 3.0f, 3.0f, // Start Size
		4.0f, 4.0f, 6.0f, 6.0f, // EndSize
		Vector(0.0f, 0.0f, 100.0f), // Gravity
		false,
		false);
}

ExplosionNukeRenderer::~ExplosionNukeRenderer()
{
}

void ExplosionNukeRenderer::draw(Action *action)
{
}

void ExplosionNukeRenderer::simulate(Action *action, float frameTime, bool &remove)
{
	float AddSmokeTime = 0.08f;
	int SmokesPerTime = 14;
	if (OptionsDisplay::instance()->getEffectsDetail() == 0) 
	{
		SmokesPerTime = 8;
	}
	else if (OptionsDisplay::instance()->getEffectsDetail() == 2) 
	{
		SmokesPerTime = 18;
	}

	totalTime_ += frameTime;
	time_ += frameTime;
	
	while (time_ > AddSmokeTime)
	{
		time_ -= AddSmokeTime;

		// Add any new entries
		if (totalTime_ > 1.25f)
		{
			if (totalTime_ < 2.25f)
			{
				emitter_.emitMushroom(
					position_,
					ScorchedClient::instance()->getParticleEngine(),
					SmokesPerTime,
					size_,
					set_,
					animate_);
			}
			else remove = true;
		}
	}
}
