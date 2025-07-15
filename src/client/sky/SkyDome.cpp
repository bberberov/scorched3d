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

#include <sky/SkyDome.h>
#include <sky/Sky.h>
#include <sky/Hemisphere.h>
#include <engine/Simulator.h>
#include <landscape/Landscape.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscapedef/LandscapeTex.h>
#include <landscapedef/LandscapeDefinition.h>
#include <GLEXT/GLState.h>
#include <GLEXT/GLCamera.h>
#include <image/ImageFactory.h>
#include <client/ScorchedClient.h>
#include <graph/OptionsDisplay.h>
#include <common/Defines.h>

SkyDome::SkyDome() : 
	xy_(0.0f), 
	cloudSpeed_(500.0f), 
	flashTime_(0.0f)
{
}

SkyDome::~SkyDome()
{
}

void SkyDome::flash()
{
	flashTime_ = 0.25f;
}

void SkyDome::generate()
{
	LandscapeTex *tex = ScorchedClient::instance()->
		getLandscapeMaps().getDefinitions().getTex();	

	// Sky
	Image bitmapCloud = ImageFactory::loadImage(
		S3D::eModLocation,
		tex->skytexture, 
		tex->skytexturemask,
		false);
	DIALOG_ASSERT(cloudTexture_.replace(bitmapCloud));
	skyColorsMap_ = ImageFactory::loadImage(
		S3D::eModLocation, 
		tex->skycolormap);

	// Stars
	useStarTexture_ = false;
	if (!tex->skytexturestatic.empty())
	{
		useStarTexture_ = true;
		Image bitmapStars = ImageFactory::loadImage(
			S3D::eModLocation,
			tex->skytexturestatic,
			tex->skytexturestatic,
			false);
		DIALOG_ASSERT(starTexture_.replace(bitmapStars));
	}
	noSunFog_ = tex->nosunfog;
	horizonGlow_ = !tex->nohorizonglow;

	// Skyline
	skyLine1_.clear();
	useSkyLine_ = false;
	if (!tex->skyline.empty())
	{
		useSkyLine_ = true;
		Image bitmapSkyLine = ImageFactory::loadImage(
			S3D::eModLocation,
			tex->skyline,
			tex->skylinemask);
		DIALOG_ASSERT(skyLineTexture_.replace(bitmapSkyLine));
	}

	// Force refresh of colors
	colors_.clear();
}

void SkyDome::simulate(float frameTime)
{
	if (OptionsDisplay::instance()->getNoSkyMovement()) return;

	float fastSpeed = 100;
	float slowSpeed = 500;
	float currentSpeed = ScorchedClient::instance()->getSimulator().getWind().getWindSpeed().asFloat();
	float wantedSpeed = (((5.0f - currentSpeed) / 5.0f) * (slowSpeed - fastSpeed)) + fastSpeed;

	// Move the cloud layer
	cloudSpeed_ = wantedSpeed;
	cloudDirection_ = -ScorchedClient::instance()->getSimulator().getWind().
		getWindStartingDirection().asVector();
	xy_ += frameTime / cloudSpeed_;

	// The sky flash
	flashTime_ -= frameTime;
}

void SkyDome::drawBackdrop()
{
	Vector &pos = GLCamera::getCurrentCamera()->getCurrentPos();
	Vector sunDir = 
		Landscape::instance()->getSky().getSun().getPosition().Normalize();
	LandscapeTex &tex =
		*ScorchedClient::instance()->getLandscapeMaps().getDefinitions().getTex();

	glPushMatrix();
		// Translate scene so it is in the middle of the camera
		glTranslatef(pos[0], pos[1], -15.0f);

		// Draw sky color
		GLState mainState2(GLState::TEXTURE_OFF | GLState::BLEND_OFF);
		colors_.drawColored(2000, 225, skyColorsMap_, sunDir, tex.skytimeofday, horizonGlow_);

		// Draw stars
		if (useStarTexture_)
		{
			glDisable(GL_FOG); 

			glColor4f(1.0f, 1.0f, 1.0f, 0.7f);
			GLState currentState(GLState::TEXTURE_ON | GLState::BLEND_ON);
			starTexture_.draw();

			glBlendFunc(GL_SRC_ALPHA, GL_ONE);

			glMatrixMode(GL_TEXTURE);
			glLoadIdentity();
			glScalef(9.0f, 9.0f, 0.0f);
			glMatrixMode(GL_MODELVIEW);

			stars_.draw(1990, 215, Hemisphere::eWidthTexture);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glMatrixMode(GL_TEXTURE);
			glLoadIdentity();
			glMatrixMode(GL_MODELVIEW);
		}

		if (flashTime_ > 0.0f) 
		{
			glDisable(GL_FOG); 

			GLState currentState(GLState::TEXTURE_OFF | GLState::BLEND_ON);

			glColor4f(1.0f, 1.0f, 1.0f, flashTime_);
			flash_.draw(1985, 220, Hemisphere::eWidthTexture);
		}

	glPopMatrix();

	if (!OptionsDisplay::instance()->getNoFog())
	{
		glEnable(GL_FOG);
	}

	// Draw Sun Glow
	if (noSunFog_) glDisable(GL_FOG); 
	Landscape::instance()->getSky().getSun().draw();

	if (!OptionsDisplay::instance()->getNoFog())
	{
		glEnable(GL_FOG);
	}
}

void SkyDome::drawLayers()
{
	Vector &pos = GLCamera::getCurrentCamera()->getCurrentPos();

	glPushMatrix();
		// Translate scene so it is in the middle of the land
		glTranslatef(pos[0], pos[1], -15.0f);

		// Skyline
		if (useSkyLine_)
		{
			//glDisable(GL_FOG);

			skyLineTexture_.draw();
			skyLine1_.draw(1000.0f, 1000.0f, 200.0f);

			//glEnable(GL_FOG);
		}

		// Layer 2
		if (!OptionsDisplay::instance()->getNoSkyLayers())
		{
			// Rotate the scene so clouds blow the correct way
			float slowXY = (xy_ + 45.5f) / 1.5f;
			GLState currentState(GLState::TEXTURE_ON | GLState::BLEND_ON);
			glColor4f(1.0f, 1.0f, 1.0f, 0.7f);

			// Cloud texture
			cloudTexture_.draw();

			// Cloud Layer 1
			glMatrixMode(GL_TEXTURE);
			glLoadIdentity();
			glTranslatef(xy_ * cloudDirection_[0], 
				xy_ * cloudDirection_[1], 0.0f);
			glMatrixMode(GL_MODELVIEW);

			clouds1_.draw(1980, 210, Hemisphere::eWidthTexture);

			// Cloud Layer 2
			glMatrixMode(GL_TEXTURE);
			glLoadIdentity();
			glTranslatef((slowXY) * (cloudDirection_[0] + cloudDirection_[1] * 0.3f), 
				(slowXY) * (cloudDirection_[1] - cloudDirection_[0] * 0.3f), 0.0f);
			glMatrixMode(GL_MODELVIEW);

			clouds2_.draw(1980, 170, Hemisphere::eWidthTexture);

			// Reset tex matrix
			glMatrixMode(GL_TEXTURE);
			glLoadIdentity();
			glMatrixMode(GL_MODELVIEW);
		}

	glPopMatrix();
}
