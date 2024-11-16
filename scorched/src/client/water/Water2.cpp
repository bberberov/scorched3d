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

#include <water/Water2.h>
#include <common/Vector.h>
#include <common/Vector4.h>
#include <common/Logger.h>
#include <common/ProgressCounter.h>
#include <common/OptionsTransient.h>
#include <engine/Simulator.h>
#include <client/ScorchedClient.h>
#include <landscapedef/LandscapeTex.h>
#include <landscapedef/LandscapeDefn.h>
#include <landscapemap/LandscapeMaps.h>
#include <graph/OptionsDisplay.h>
#include <GLEXT/GLState.h>
#include <GLEXT/GLStateExtension.h>
#include <image/ImageFactory.h>
#include <lang/LangResource.h>
#include "ocean_wave_generator.h"

#include <water/Water2Constants.h>

Water2::Water2()
{
}

Water2::~Water2()
{
	
}

Water2Patches &Water2::getPatch(float time)
{
	unsigned int index = ((unsigned int)(time)) % generatedPatches_;
	DIALOG_ASSERT(index < (unsigned int) generatedPatches_);
	return patches_[index];
}

static float calculateError(Water2Points &displacement,
	int x1, int x2, int y1, int y2,
	float x1y1, float x2y2, float x1y2, float x2y1)
{
	if (x2 - x1 <= 1) return 0.0f;

	int midx = (x1 + x2) / 2;
	int midy = (y1 + y2) / 2;
	float actualheight = displacement.getPoint(midx, midy)[2];

	float approxheight1 = (x1y1 + x2y2) / 2.0f;
	float approxheight2 = (x1y2 + x2y1) / 2.0f;
	float approxheight3 = (x1y1 + x1y2) / 2.0f;
	float approxheight4 = (x1y1 + x2y1) / 2.0f;
	float approxheight5 = (x1y2 + x2y2) / 2.0f;
	float approxheight6 = (x2y1 + x2y2) / 2.0f;

	float heightdiff1 = fabs(approxheight1 - actualheight);
	float heightdiff2 = fabs(approxheight2 - actualheight);
	
	float errorChild1 = calculateError(displacement,
		x1, midx, y1, midy,
		x1y1, approxheight1, approxheight3, approxheight4);
	float errorChild2 = calculateError(displacement,
		midx, x2, y1, midy,
		approxheight4, approxheight6, approxheight1, x2y1);
	float errorChild3 = calculateError(displacement,
		x1, midx, midy, y2,
		approxheight3, approxheight5, x1y2, approxheight2);
	float errorChild4 = calculateError(displacement,
		midx, x2, midy, y2,
		approxheight2, x2y2, approxheight5, approxheight6);

	float errorChildren = MAX(errorChild1, MAX(errorChild2, MAX(errorChild3, errorChild4)));
	float totalError = MAX(errorChildren, MAX(heightdiff1, heightdiff2));
	return totalError;
}

void Water2::generate(LandscapeTexBorderWater *water, ProgressCounter *counter)
{
	if (counter) counter->setNewOp(LANG_RESOURCE("WATER_MOTION", "Water Motion"));

	// Calculate water for position n
	float windSpeed = ScorchedClient::instance()->
		getSimulator().getWind().getWindSpeed().asFloat() * 2.0f + 3.0f;
	Vector windDir = ScorchedClient::instance()->
		getSimulator().getWind().getWindDirection().asVector();
	if (windDir == Vector::getNullVector())
	{
		windDir = Vector(0.8f, 0.8f);
	}

	ocean_wave_generator<float> 
		owg(wave_resolution, // Resolution
			windDir, // Wind dir
			windSpeed,  // wind speed m/s
			float(wave_resolution) * (1e-8f), // scale factor for heights
			float(wave_waterwidth), // wavetile_length
			wave_tidecycle_time); // wave_tidecycle_time

	// For each frame get the height data
	generatedPatches_ = 0;
	static Water2Points displacements[256];
	for (unsigned i=0; i<wave_phases; i++) 
	{
		if (counter) counter->setNewPercentage(float(i * 100) / float(wave_phases));

		// Set frame number
		float currentTime = wave_tidecycle_time * float(i) / float(wave_phases);
		float timeMod = myfmod(currentTime, wave_tidecycle_time);
		owg.set_time(timeMod);

		// Calculate Zs
		owg.compute_heights(displacements[i]);

		// Calculate X,Ys
		owg.compute_displacements(-2.0f, displacements[i]);

		// Form a vector with the correct X,Y,Zs
		for (int y=0; y<wave_resolution; y++)
		{
			for (int x=0; x<wave_resolution; x++)	
			{
				Vector &point = displacements[i].getPoint(x, y);
				point[2] += water->height.asFloat();
			}
		}

		// Create the patches
		patches_[i].generate(displacements[i], wave_resolution, 
			wave_patch_width, water->height.asFloat());
		generatedPatches_++;

		// Figure out the error for each LOD level for the water
		// Do this for 1 64x64 patch as they should all be roughly similar
		if (i == 0)
		{
			for (int j=0; j<=6; j++)
			{
				float error = 0.0f;
				if (j>0)
				{
					int skip = 1 << j;
					for (int y1=0; y1<wave_patch_width; y1+=skip)
					{
						for (int x1=0; x1<wave_patch_width; x1+=skip)
						{
							int x2 = x1 + skip;
							int y2 = y1 + skip;

							float x1y1 = displacements[i].getPoint(x1, y1)[2];
							float x2y2 = displacements[i].getPoint(x2, y2)[2];
							float x1y2 = displacements[i].getPoint(x1, y2)[2];
							float x2y1 = displacements[i].getPoint(x2, y1)[2];

							float thisError = calculateError(displacements[i],
								x1, x2, y1, y2,
								x1y1, x2y2, x1y2, x2y1);
							error = MAX(error, thisError);
						}
					}
				}

				indexErrors_[j] = error;
			}
		}

		// If we are not drawing water or no movement generate one patch
		if (OptionsDisplay::instance()->getNoWaterMovement() ||
			!OptionsDisplay::instance()->getDrawWater())
		{
			break;
		}
	}

	if (indexs_.getNoLevels() == 0)
	{
		// Create the indexes
		indexs_.generate(wave_patch_width, wave_patch_width, 2);
	}

	// compute amount of foam per vertex sample
	LandscapeDefn &defn = *ScorchedClient::instance()->getLandscapeMaps().
		getDefinitions().getDefn();
	Image loadedFoam = 
		ImageFactory::loadImage(
			S3D::eModLocation, 
			water->foam);	
	if (loadedFoam.getWidth() != wave_resolution ||
		loadedFoam.getHeight() != wave_resolution)
	{
		S3D::dialogExit("Water2", 
			S3D::formatStringBuffer("Foam image size must be %ix%i", 
				wave_resolution, wave_resolution));
	}

	float aof[wave_resolution*wave_resolution];
	memset(aof, 0, sizeof(float) * wave_resolution * wave_resolution);

	float rndtab[37];
	for (unsigned k = 0; k < 37; ++k) rndtab[k] = RAND;

	// Waves, oaf part 1
	if (GLStateExtension::hasShaders() &&
		!OptionsDisplay::instance()->getNoWaterWaves() &&
		!GLStateExtension::useSimpleShaders())
	{
		counter->setNewOp(LANG_RESOURCE("WATER_WAVES", "Water Waves"));
		for (unsigned k = 0; k < wave_phases; ++k) 
		{
			if (counter) counter->setNewPercentage(float(k * 50) / float(wave_phases));
			Water2Points &wd = displacements[k % wave_phases];
			generateAOF(wd, 0, rndtab, displacements, aof);
			if (generatedPatches_ == 1) break;
		}
	}

	// Waves, oaf part 2
	counter->setNewOp(LANG_RESOURCE("WATER_EFFECTS", "Water Effects"));
	for (unsigned k = 0; k < wave_phases; ++k) 
	{
		if (counter) counter->setNewPercentage(float(k * 50) / float(wave_phases));
		Water2Points &wd = displacements[k % wave_phases];

		Image aofImage(wave_resolution, wave_resolution, 3, 0);
		memcpy(aofImage.getBits(), loadedFoam.getBits(), wave_resolution * wave_resolution * 3);

		// Add waves to AOF image
		if (GLStateExtension::hasShaders() &&
			!OptionsDisplay::instance()->getNoWaterWaves() &&
			!GLStateExtension::useSimpleShaders())
		{
			generateAOF(wd, &aofImage, rndtab, displacements, aof);
		}
		else
		{
			unsigned char *bits = aofImage.getBits();
			for (unsigned y = 0; y<wave_resolution; ++y) 
			{
				for (unsigned x = 0; x<wave_resolution; ++x, bits+=3) 
				{
					bits[0] = 0;
				}
			}
		}

		// Add transparency to AOF image
		generateTransparency(wd, aofImage, defn);

		// Save AOF image
		Water2Patches &patches = patches_[k];
		patches.getAOF().create(aofImage);

		if (generatedPatches_ == 1) break;
	}
}

void Water2::generateAOF(Water2Points &wd, Image *aofImage, float *rndtab, 
						 Water2Points *displacements, float *aof)
{
	// factor to build derivatives correctly
	const float deriv_fac = wavetile_length_rcp * wave_resolution;
	const float lambda = 1.0; // lambda has already been multiplied with x/y displacements...
	const float decay = 4.0/wave_phases;
	const float decay_rnd = 0.25/wave_phases;
	const float foam_spawn_fac = 0.25;//0.125;

	// compute for each sample how much foam is added (spawned)
	for (unsigned y = 0; y < wave_resolution; ++y) {
		unsigned ym1 = (y + wave_resolution - 1) & (wave_resolution-1);
		unsigned yp1 = (y + 1) & (wave_resolution-1);
		for (unsigned x = 0; x < wave_resolution; ++x) {
			unsigned xm1 = (x + wave_resolution - 1) & (wave_resolution-1);
			unsigned xp1 = (x + 1) & (wave_resolution-1);

			Vector &xp1y = wd.getPoint(xp1, y);
			Vector &xm1y = wd.getPoint(xm1, y);
			Vector &xyp1 = wd.getPoint(x, yp1);
			Vector &xym1 = wd.getPoint(x, ym1);
			float dispx_dx = (xp1y[0] - xm1y[0]) * deriv_fac;
			float dispx_dy = (xyp1[0] - xym1[0]) * deriv_fac;
			float dispy_dx = (xp1y[1] - xm1y[1]) * deriv_fac;
			float dispy_dy = (xyp1[1] - xym1[1]) * deriv_fac;
			float Jxx = 1.0f + lambda * dispx_dx;
			float Jyy = 1.0f + lambda * dispy_dy;
			float Jxy = lambda * dispy_dx;
			float Jyx = lambda * dispx_dy;
			float J = Jxx*Jyy - Jxy*Jyx;

			float foam_add = (J < 0.0f) ? ((J < -1.0f) ? 1.0f : -J) : 0.0f;			

			aof[y*wave_resolution+x] += foam_add * foam_spawn_fac;

			// spawn foam also on neighbouring fields
			aof[ym1*wave_resolution+x] += foam_add * foam_spawn_fac * 0.5f;
			aof[yp1*wave_resolution+x] += foam_add * foam_spawn_fac * 0.5f;
			aof[y*wave_resolution+xm1] += foam_add * foam_spawn_fac * 0.5f;
			aof[y*wave_resolution+xp1] += foam_add * foam_spawn_fac * 0.5f;
		}
	}

	// compute decay, depends on time with some randomness
	unsigned ptr = 0;
	for (unsigned y = 0; y < wave_resolution; ++y) 
	{
		for (unsigned x = 0; x < wave_resolution; ++x, ++ptr) 
		{
			float aofVal = std::max(std::min(aof[ptr], 1.0f) - 
				(decay + decay_rnd * rndtab[(3*x + 5*y) % 37]), 0.0f);

			aof[ptr] = aofVal;

			if (aofImage)
			{
				aofImage->getBits()[ptr * 3 + 0] = (unsigned char) (255.0f * aofVal);
			}
		}
	}
}

void Water2::generateTransparency(Water2Points &wd, 
								  Image &aofImage, LandscapeDefn &defn)
{
	unsigned ptr = 0;
	for (unsigned y = 0; y < wave_resolution; ++y) 
	{
		for (unsigned x = 0; x < wave_resolution; ++x, ++ptr) 
		{
			// Water height
			Vector &points = wd.getPoint(x, y);
			float waterHeight = points[2];

			// Not quite right!! but it will do
			int lx = int(float(x) * float(defn.getLandscapeWidth()) / float(wave_resolution));
			int ly = int(float(y) * float(defn.getLandscapeHeight()) / float(wave_resolution));
			float groundHeight = ScorchedClient::instance()->getLandscapeMaps().
				getGroundMaps().getHeight(lx, ly).asFloat();
			
			// Water depth
			float waterDepth = waterHeight - groundHeight;
			if (waterDepth < 0.0f) waterDepth = 0.0f;
			else if (waterDepth > 10.0f) waterDepth = 10.0f;

			// Store
			unsigned char result = (unsigned char) (waterDepth * 25.0f);
			aofImage.getBits()[ptr * 3 + 1] = result;
		}
	}
}
