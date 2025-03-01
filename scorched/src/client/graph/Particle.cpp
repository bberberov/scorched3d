////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011, 2025
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

#include <graph/Particle.h>

Particle::Particle() :
	life_(-1.0f),
	windAffect_(true),
	engine_(nullptr),
	type_(0),
	texture_(nullptr),
	textureSet_(nullptr),
	textureCoord_(0),
	shadow_(false),
	simulated_(false),
	distance_(0.0f),
	userData_(nullptr),
	renderer_(nullptr)
{}

Particle::~Particle()
{}

void Particle::setParticle(
	float life,
	float mass,
	float friction,
	Vector &velocity,
	Vector &gravity,
	Vector &color,
	Vector &colorCounter,
	Vector &size,
	Vector &sizeCounter,
	float alpha,
	float alphaCounter,
	bool additiveTexture,
	bool windAffect
)
{
	life_ = life;
	mass_ = mass;
	friction_ = friction;

	percent_ = 0.0f;
	percentCounter_ = 1.0f / life_;

	velocity_ = velocity;
	gravity_ = gravity;
	color_ = color;
	colorCounter_ = colorCounter;
	size_ = size;
	sizeCounter_ = sizeCounter;
	alpha_ = alpha;
	alphaCounter_ = alphaCounter;
	additiveTexture_ = additiveTexture;
	windAffect_ = windAffect;
}

void Particle::unsetParticle()
{
	delete userData_;
	userData_ = nullptr;
	texture_ = nullptr;
	textureSet_ = nullptr;
	renderer_ = nullptr;
	type_ = 0;
	shadow_ = false;
	simulated_ = false;
}
