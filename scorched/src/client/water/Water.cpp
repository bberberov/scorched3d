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

#include <water/Water.h>
#include <water/Water2.h>
#include <water/Water2Renderer.h>
#include <water/WaterMapPoints.h>
#include <water/WaterWaveDistance.h>
#include <water/WaterWaves.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscapedef/LandscapeTex.h>
#include <landscapedef/LandscapeDefn.h>
#include <landscapedef/LandscapeDefinition.h>
#include <graph/OptionsDisplay.h>
#include <common/Defines.h>
#include <common/Clock.h>
#include <common/Logger.h>
#include <sound/SoundUtils.h>
#include <client/ScorchedClient.h>
#include <graph/ParticleEmitter.h>
#include <image/ImageFactory.h>
#include <GLEXT/GLImageModifier.h>

Water::Water() :
	height_(25.0f), transparency_(1.0f), maxTransparency_(1.0f),
	waterOn_(false)
{
	wTex_ = new Water2Renderer();
	wMap_ = new Water2();
	wMapPoints_ = new WaterMapPoints();
	wWaveDistance_ = new WaterWaveDistance();
	waves_ = new WaterWaves();
}

Water::~Water()
{
}

void Water::setTransparency(float transparency)
{
	transparency_ = MIN(maxTransparency_, transparency);
}

void Water::draw()
{
	if (!waterOn_ ||
		!OptionsDisplay::instance()->getDrawWater()) return;

	wTex_->draw(*wMap_, *wMapPoints_, *waves_, transparency_);
}

void Water::simulate(float frameTime)
{
	if (!waterOn_ ||
		!OptionsDisplay::instance()->getDrawWater()) return;

	wTex_->simulate(frameTime);
	waves_->simulate(frameTime);
}

void Water::generate(ProgressCounter *counter)
{
	LandscapeTex &tex = *ScorchedClient::instance()->getLandscapeMaps().
		getDefinitions().getTex();
	LandscapeDefn &defn = *ScorchedClient::instance()->getLandscapeMaps().
		getDefinitions().getDefn();
	if (tex.border->getType() == LandscapeTexType::eNone)
	{
		height_ = 0.0f;
		waterOn_ = false;
		return;
	}

	// Check the border means water
	if (tex.border->getType() != LandscapeTexType::eWater)
	{
		S3D::dialogExit("Landscape", S3D::formatStringBuffer(
			"Failed to find border type %i",
			tex.border->getType()));
	}

	LandscapeTexBorderWater *water = 
		(LandscapeTexBorderWater *) tex.border;

	// Set quick water attributes
	height_ = water->height.asFloat();
	transparency_ = maxTransparency_ = water->waterTransparency;
	waterOn_ = true;

	// General all other water items
	Clock timer;
	wMap_->generate(water, counter);
	float timeDifference = timer.getTimeDifference();
	Logger::log(S3D::formatStringBuffer("Water time %.3f", timeDifference));

	wTex_->generate(water, counter);
	wMapPoints_->generate();
	wWaveDistance_->generate(defn.getLandscapeWidth(), defn.getLandscapeHeight(), height_, counter);

	// Generate the water texture for the spray sprite
	Image sprayMaskBitmap = 
		ImageFactory::loadImage(
			S3D::eModLocation,
			"data/textures/smoke01.bmp",
			"data/textures/smoke01.bmp", 
			false);
	Image loadedBitmapWater = 
		ImageFactory::loadImage(
			S3D::eModLocation, 
			water->reflection);
	bitmapWater_ = ImageFactory::loadImage(
		S3D::eModLocation, 
		water->reflection);

	{
		Image bitmapWater = loadedBitmapWater.createResize(
			sprayMaskBitmap.getWidth(), sprayMaskBitmap.getHeight());
		Image textureWaterNew(
			sprayMaskBitmap.getWidth(), sprayMaskBitmap.getHeight(), true);
		ImageModifier::makeBitmapTransparent(textureWaterNew, 
			bitmapWater, sprayMaskBitmap);
		landTexWater_.replace(textureWaterNew);
	}

	waves_->generateWaves(height_, counter);
}

void Water::bindWaterReflection()
{
	wTex_->bindWaterReflection();
}

void Water::unBindWaterReflection()
{
	wTex_->unBindWaterReflection();
}

void Water::drawPoints()
{
	wTex_->drawPoints(*wMapPoints_);
}

GLTexture &Water::getReflectionTexture()
{
	return wTex_->getReflectionTexture();
}

bool Water::explosion(Vector position, float size)
{
	if (!waterOn_ ||
		!OptionsDisplay::instance()->getDrawWater()) return false;

	if (position[2] < height_)
	{
		CACHE_SOUND(sound, S3D::getModFile("data/wav/misc/splash.wav"));
		SoundUtils::playAbsoluteSound(VirtualSoundPriority::eAction,
			sound, position);

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
		sprayemitter.emitSpray(position, 
			ScorchedClient::instance()->getParticleEngine(),
			size - 2.0f,
			&landTexWater_);
		return true;
	}

	return false;
}

float Water::getWaveDistance(int x, int y)
{
	return wWaveDistance_->getWaveDistance(x, y);
}

float *Water::getIndexErrors()
{ 
	return wMap_->getIndexErrors(); 
}
