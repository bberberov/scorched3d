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

#include <sprites/NapalmRenderer.h>
#include <sprites/ExplosionTextures.h>
#include <client/ScorchedClient.h>
#include <landscapemap/LandscapeMaps.h>
#include <common/Defines.h>

NapalmRenderer::NapalmRenderer(GLTextureSet *set) : 
	textureNo_(0.0f), set_(set)
{
	int noTextures = set_->getNoTextures();
	plus_ = int(RAND * noTextures);
}

NapalmRenderer::~NapalmRenderer()
{
}

void NapalmRenderer::draw(Particle *particle)
{
	particle->position_[2] = 
		ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getHeight(
			(int) particle->position_[0],
			(int) particle->position_[1]).asFloat() + particle->size_[1] * 2.0f;
}

void NapalmRenderer::simulate(Particle *particle, float timepassed)
{
	int noTextures = set_->getNoTextures();
	textureNo_ += 0.1f;
	int no = (int(textureNo_) + plus_) % noTextures;
	particle->texture_ = set_->getTexture(no);
}
