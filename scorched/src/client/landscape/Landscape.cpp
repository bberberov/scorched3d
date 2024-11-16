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

#include <landscape/Landscape.h>
#include <landscape/LandscapePoints.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscape/LandscapeSoundManager.h>
#include <landscape/LandscapeMusicManager.h>
#include <landscape/Smoke.h>
#include <landscape/Wall.h>
#include <landscape/ShadowMap.h>
#include <landscape/InfoMap.h>
#include <landscape/GraphicalLandscapeMap.h>
#include <landscapedef/LandscapeTex.h>
#include <landscapedef/LandscapeDefn.h>
#include <landscapedef/LandscapeDefinition.h>
#include <landscapedef/LandscapeDefinitions.h>
#include <lang/LangResource.h>
#include <sky/Sky.h>
#include <water/Water.h>
#include <land/VisibilityPatchGrid.h>
#include <movement/TargetMovement.h>
#include <image/ImageFactory.h>
#include <GLEXT/GLImageModifier.h>
#include <GLEXT/GLStateExtension.h>
#include <GLEXT/GLCameraFrustum.h>
#include <console/ConsoleRuleMethodIAdapter.h>
#include <GLSL/GLSLShaderSetup.h>
#include <common/OptionsTransient.h>
#include <common/Defines.h>
#include <graph/OptionsDisplay.h>
#include <graph/MainCamera.h>
#include <engine/Simulator.h>
#include <sound/Sound.h>
#include <client/ScorchedClient.h>
#include <dialogs/CameraDialog.h>
#include <engine/ActionController.h>
#include <tankgraph/RenderTargets.h>
#include <time.h>

Landscape *Landscape::instance_ = 0;

Landscape *Landscape::instance()
{
	if (!instance_)
	{
		instance_ = new Landscape;
	}
	return instance_;
}

Landscape::Landscape() : 
	resetLandscape_(false), resetLandscapeTimer_(0.0f), 
	resetRoof_(false), resetRoofTimer_(0.0f),
	textureType_(eDefault),
	changeCount_(1),
	landShader_(0)
{
	water_ = new Water();
	points_ = new LandscapePoints();
	sky_ = new Sky();
	smoke_ = new Smoke();
	wall_ = new Wall();

	new ConsoleRuleMethodIAdapter<Landscape>(
		this, &Landscape::savePlan, "SavePlan");
}

Landscape::~Landscape()
{
}

void Landscape::simulate(float frameTime)
{
	if (resetLandscape_)
	{
		resetLandscapeTimer_ -= frameTime;
		if (resetLandscapeTimer_ < 0.0f)
		{
			// Update the plan texture
			updatePlanATexture();
			updatePlanTexture();

			// Update the landscape
			GraphicalLandscapeMap *landscapeMap = (GraphicalLandscapeMap *)
				ScorchedClient::instance()->getLandscapeMaps().
					getGroundMaps().getHeightMap().getGraphicalMap();
			landscapeMap->updateWholeBuffer();

			// Re-calculate the landsacpe on the wind indicator
			changeCount_++;
			resetLandscape_ = false;
		}
	}
	if (resetRoof_)
	{
		resetRoofTimer_ -= frameTime;
		if (resetRoofTimer_ < 0.0f)
		{
			if (ScorchedClient::instance()->getLandscapeMaps().getRoofMaps().getRoofOn())
			{
				// Update the landscape
				GraphicalLandscapeMap *roofMap = (GraphicalLandscapeMap *)
					ScorchedClient::instance()->getLandscapeMaps().
						getRoofMaps().getRoofMap().getGraphicalMap();
				roofMap->updateWholeBuffer();

				// Re-calculate the landsacpe on the wind indicator
				changeCount_++;
			}

			resetRoof_ = false;
		}
	}

	float speedMult = ScorchedClient::instance()->
		getSimulator().getFast().asFloat();
	water_->simulate(frameTime * speedMult);
	sky_->simulate(frameTime * speedMult);
	wall_->simulate(frameTime * speedMult);
	LandscapeSoundManager::instance()->simulate(frameTime * speedMult);
}

void Landscape::recalculateLandscape()
{
	if (!resetLandscape_)
	{
		resetLandscape_ = true;
		float recalcTime = float(OptionsDisplay::instance()->getDeformRecalculationTime()) / 1000.0f;
		resetLandscapeTimer_ = recalcTime; // Recalculate the landscape in x seconds
	}
}

void Landscape::recalculateRoof()
{
	if (!resetRoof_)
	{
		resetRoof_ = true;
		float recalcTime = float(OptionsDisplay::instance()->getDeformRecalculationTime()) / 1000.0f;
		resetRoofTimer_ = recalcTime; // Recalculate the water in x seconds
	}
}

void Landscape::drawShadows()
{	
	if (!GLStateExtension::hasHardwareShadows()) return;

	GAMESTATE_PERF_COUNTER_START(ScorchedClient::instance()->getGameState(), "LANDSCAPE_SHADOWS_PRE");

	// Turn off texturing
	GLState glstate(GLState::TEXTURE_OFF | GLState::DEPTH_ON);

	float landWidth = ScorchedClient::instance()->getLandscapeMaps().
		getGroundMaps().getLandscapeWidth() / 2.0f;
	float landHeight = ScorchedClient::instance()->getLandscapeMaps().
		getGroundMaps().getLandscapeHeight() / 2.0f;
	float maxWidth = MAX(landWidth, landHeight);

	// Get the sun's position and landscape dimensions
	Vector sunPosition = Landscape::instance()->getSky().getSun().getPosition();
	sunPosition *= 0.5f + (maxWidth - 128.0f) / 256.0f; 

	Vector relativePosition = sunPosition;
	relativePosition[0] -= landWidth;
	relativePosition[1] -= landHeight;
	float magnitude = relativePosition.Magnitude();

	// Bind the frame buffer so we can render into it
	shadowFrameBuffer_.bind();
	glViewport(0, 0, shadowFrameBuffer_.getWidth(), shadowFrameBuffer_.getHeight());

	// Setup the view from the sun
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	
	gluPerspective(60.0f, 1.0f, magnitude - (maxWidth * 1.5f), magnitude + (maxWidth * 1.5f));
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(
		sunPosition[0], sunPosition[1], sunPosition[2], 
		landWidth, landHeight, 0.0f ,
		0.0f, 0.0f, 1.0f);

	GLCameraFrustum::instance()->draw(0);

	// Save the matrixs used for the sun
	glGetFloatv(GL_MODELVIEW_MATRIX, lightModelMatrix_);
	glGetFloatv(GL_PROJECTION_MATRIX, lightProjMatrix_);

	// Clear and setup the offset
    glClear(GL_DEPTH_BUFFER_BIT);

	// Set poly offset so that the shadows dont get precision artifacts
    glPolygonOffset(10.0f, 10.0f);
    glEnable(GL_POLYGON_OFFSET_FILL);
	glCullFace(GL_FRONT);

	//Disable color writes, and use flat shading for speed
    glColorMask(0, 0, 0, 0); 

	// Draw items that cast shadows
	GAMESTATE_PERF_COUNTER_END(ScorchedClient::instance()->getGameState(), "LANDSCAPE_SHADOWS_PRE");

	GAMESTATE_PERF_COUNTER_START(ScorchedClient::instance()->getGameState(), "LANDSCAPE_SHADOWS_DRAW_LAND");
	VisibilityPatchGrid::instance()->drawLand(0, true, true);
	GAMESTATE_PERF_COUNTER_END(ScorchedClient::instance()->getGameState(), "LANDSCAPE_SHADOWS_DRAW_LAND");

	if (!OptionsDisplay::instance()->getNoGLObjectShadows())
	{
		RenderTargets::instance()->shadowDraw();
	}

	GAMESTATE_PERF_COUNTER_START(ScorchedClient::instance()->getGameState(), "LANDSCAPE_SHADOWS_POST");

	static bool createdMap = false;
	if (OptionsDisplay::instance()->getDrawGraphicalShadowMap())
	{
		if (!createdMap)
		{
			createdMap = true;

			static float *depthResult = 
				new float[shadowFrameBuffer_.getWidth() * shadowFrameBuffer_.getHeight()];
			static Image depthImage(
				shadowFrameBuffer_.getWidth(),
				shadowFrameBuffer_.getHeight());

			glReadPixels(0, 0, 
				shadowFrameBuffer_.getWidth(), shadowFrameBuffer_.getHeight(),
				GL_DEPTH_COMPONENT, GL_FLOAT, depthResult);

			float min = 1.0, max = 0.0;
			float *src = depthResult;
			unsigned char *dest = depthImage.getBits();
			for (int i=0; i<shadowFrameBuffer_.getWidth() * shadowFrameBuffer_.getHeight(); i++, src++, dest+=3)
			{
				if (*src != 1.0f)
				{
					if (*src != 0.0f)
					{
						min = MIN(min, *src);
						max = MAX(max, *src);
					}

					//*src = 0.0f; // Black and white
					dest[0] = (unsigned char) (*src * 255.0f);
					dest[1] = (unsigned char) (*src * 255.0f);
					dest[2] = (unsigned char) (*src * 255.0f);
				}
			}

			colorDepthMap_.replace(depthImage);
		}
	}
	else
	{
		createdMap = false;
	}

	//restore states
    glColorMask(1, 1, 1, 1); 

	// Reset offset
	glDisable(GL_POLYGON_OFFSET_FILL);
	glCullFace(GL_BACK);

	// Stop drawing to frame buffer
	shadowFrameBuffer_.unBind();

	// Reset camera
	MainCamera::instance()->getCamera().draw();
	GLCameraFrustum::instance()->draw(0);

	GAMESTATE_PERF_COUNTER_END(ScorchedClient::instance()->getGameState(), "LANDSCAPE_SHADOWS_POST");
}

void Landscape::calculateVisibility()
{
	GAMESTATE_PERF_COUNTER_START(ScorchedClient::instance()->getGameState(), "LANDSCAPE_LOD");
	VisibilityPatchGrid::instance()->calculateVisibility();
	GAMESTATE_PERF_COUNTER_END(ScorchedClient::instance()->getGameState(), "LANDSCAPE_LOD");
}

void Landscape::drawSetup()
{
	if (OptionsDisplay::instance()->getDrawLines()) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// NOTE: The following code is drawn with fog on
	// Be carefull as this we "dull" bilboard textures
	if (!OptionsDisplay::instance()->getNoFog())
	{
		glEnable(GL_FOG); // NOTE: Fog on
	}
}

void Landscape::drawTearDown()
{
	glDisable(GL_FOG); // NOTE: Fog off
	if (OptionsDisplay::instance()->getDrawLines()) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Landscape::drawLand()
{
	drawSetup();

	GAMESTATE_PERF_COUNTER_START(ScorchedClient::instance()->getGameState(), "LANDSCAPE_SKY");
	sky_->drawBackdrop(false);
	GAMESTATE_PERF_COUNTER_END(ScorchedClient::instance()->getGameState(), "LANDSCAPE_SKY");

	if (OptionsDisplay::instance()->getDrawLandscape())
	{
		if (GLStateExtension::hasHardwareShadows() &&
			OptionsDisplay::instance()->getUseLandscapeTexture())
		{
			actualDrawLandShader();
		}
		else
		{
			actualDrawLandTextured();
		}
	}

	if (OptionsDisplay::instance()->getDrawGraphicalShadowMap())
	{
		drawGraphicalTextureMap(colorDepthMap_);
	}
	if (OptionsDisplay::instance()->getDrawGraphicalReflectionMap())
	{
		drawGraphicalTextureMap(water_->getReflectionTexture());
	}

	GAMESTATE_PERF_COUNTER_START(ScorchedClient::instance()->getGameState(), "LANDSCAPE_POINTS");
	points_->draw();
	GAMESTATE_PERF_COUNTER_END(ScorchedClient::instance()->getGameState(), "LANDSCAPE_POINTS");

	if (OptionsDisplay::instance()->getDrawMovement())
	{
		ScorchedClient::instance()->getTargetMovement().draw();
	}

	drawTearDown();
}

void Landscape::drawWater()
{
	if (!water_->getWaterOn()) return;

	if (GLStateExtension::hasFBO() &&
		GLStateExtension::hasShaders() &&
		!OptionsDisplay::instance()->getNoWaterReflections() &&
		OptionsDisplay::instance()->getDrawWater() &&
		water_->getWaterOn())
	{
		water_->bindWaterReflection();

		glClearColor(0, 1.0f/16.0f, 1.0f/8.0f, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glPushMatrix();

		glTranslatef(0.0f, 0.0f, water_->getWaterHeight() * 2.0f);

		// flip geometry at z=0 plane
		glScalef(1.0f, 1.0f, -1.0f);
		glCullFace(GL_FRONT);

		drawSetup();
		GAMESTATE_PERF_COUNTER_START(ScorchedClient::instance()->getGameState(), "LAND_REFLECTIONS");
		sky_->drawBackdrop(true);
		sky_->drawLayers();
		actualDrawLandReflection();
		GAMESTATE_PERF_COUNTER_END(ScorchedClient::instance()->getGameState(), "LAND_REFLECTIONS");
		if (!OptionsDisplay::instance()->getNoObjectReflections())
		{
			GAMESTATE_PERF_COUNTER_START(ScorchedClient::instance()->getGameState(), "TARGET_REFLECTIONS");
			RenderTargets::instance()->draw(true);
			GAMESTATE_PERF_COUNTER_END(ScorchedClient::instance()->getGameState(), "TARGET_REFLECTIONS");
		}
		if (!OptionsDisplay::instance()->getNoParticleReflections())
		{
			GAMESTATE_PERF_COUNTER_START(ScorchedClient::instance()->getGameState(), "PARTICLE_REFLECTIONS");
			ScorchedClient::instance()->getParticleEngine().draw(0);
			GAMESTATE_PERF_COUNTER_END(ScorchedClient::instance()->getGameState(), "PARTICLE_REFLECTIONS");
		}
		drawTearDown();

		//water_->drawPoints(); // Bad reflections in large wind

		glCullFace(GL_BACK);
		glPopMatrix();

		water_->unBindWaterReflection();
	}

	drawSetup();

	water_->draw();

	drawTearDown();
}

void Landscape::drawObjects()
{
	drawSetup();

	sky_->drawLayers();
	wall_->draw();

	drawTearDown();
}

int Landscape::getPlanTexSize()
{
	switch (OptionsDisplay::instance()->getTexSize())
	{
	case 0:
		return 64;
		break;
	case 2:
		return 256;
		break;
	default:
		return 128;
		break;
	}
	return 128;
}

int Landscape::getMapTexSize()
{
	switch (OptionsDisplay::instance()->getTexSize())
	{
	case 0:
		return 256;
		break;
	case 2:
		return 2048;
		break;
	default:
		return 1024;
		break;
	}
	return 1024;
}

void Landscape::generate(ProgressCounter *counter)
{
	GraphicalLandscapeMap *landscapeMap = (GraphicalLandscapeMap *)
		ScorchedClient::instance()->getLandscapeMaps().
			getGroundMaps().getHeightMap().getGraphicalMap();
	landscapeMap->updateWholeBuffer();

	if (ScorchedClient::instance()->getLandscapeMaps().getRoofMaps().getRoofOn())
	{
		GraphicalLandscapeMap *roofMap = (GraphicalLandscapeMap *)
			ScorchedClient::instance()->getLandscapeMaps().
				getRoofMaps().getRoofMap().getGraphicalMap();
		roofMap->updateWholeBuffer();
	}

	textureType_ = eDefault;
	InfoMap::instance()->addAdapters();

	// Choose the correct sizes for the current LOD
	int mapTexSize = getMapTexSize();
	int planTexSize = getPlanTexSize();

	// Generate the texture used to map onto the landscape
	if (!mainMap_.getBits())
	{
		mainMap_ = Image(mapTexSize, mapTexSize);
		bitmapPlanAlpha_ = Image(planTexSize, planTexSize, true);
		bitmapPlan_ = Image(planTexSize, planTexSize);
		bitmapPlanAlphaAlpha_ = Image(planTexSize, planTexSize, 3, 0);
	}

	if (GLStateExtension::hasHardwareShadows())
	{
		if (!shadowFrameBuffer_.bufferValid())
		{
			// Create the frame buffer
			if (!shadowFrameBuffer_.create(2048, 2048))
			{
				S3D::dialogExit("Scorched3D", "Failed to create shadow frame buffer");
			}
		}
	}

	// Removed for now as plan is square
	// If (when) re-instated need to scale alpha map by playable arena, not full map size
	/*Image plana = ImageFactory::loadImage(S3D::getModFile("data/windows/planaa.bmp"));
	ImageModifier::scalePlanBitmap(bitmapPlanAlphaAlpha_, plana,
		ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getLandscapeWidth(),
	ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getLandscapeHeight());*/

	// Load the texture bitmaps from resources 
	LandscapeTex *tex = 
			ScorchedClient::instance()->getLandscapeMaps().getDefinitions().getTex();
	if (tex->texture->getType() == LandscapeTexType::eTextureGenerate)
	{
		LandscapeTexTextureGenerate *generate = 
			(LandscapeTexTextureGenerate *) tex->texture;

		Image texture0 = 
			ImageFactory::loadImage(S3D::eModLocation, generate->texture0.c_str());
		Image texture1 = 
			ImageFactory::loadImage(S3D::eModLocation, generate->texture1.c_str());
		Image texture2 = 
			ImageFactory::loadImage(S3D::eModLocation, generate->texture2.c_str());
		Image texture3 = 
			ImageFactory::loadImage(S3D::eModLocation, generate->texture3.c_str());
		Image bitmapShore = 
			ImageFactory::loadImage(S3D::eModLocation, generate->shore.c_str());
		Image bitmapRock = 
			ImageFactory::loadImage(S3D::eModLocation, generate->rockside.c_str());
		Image bitmapRoof = 
			ImageFactory::loadImage(S3D::eModLocation, generate->roof.c_str());
		Image *bitmaps[4];
		bitmaps[0] = &texture0;
		bitmaps[1] = &texture1;
		bitmaps[2] = &texture2;
		bitmaps[3] = &texture3;

		// Generate the new landscape
		if (counter) counter->setNewOp(LANG_RESOURCE("LANDSCAPE_MAP", "Generate Landscape Map"));
		ImageModifier::addHeightToBitmap(
			ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getHeightMap(),
			mainMap_, 
			bitmapRock, bitmapShore, bitmaps, 4, 1024, counter);

		//mainMap_.writeToFile("i:\\plan.bmp");

		// Set the general surround and roof texture
		groundTexture_.replace(texture0, false);
		roofTexture_.replace(bitmapRoof, true);
	}
	else if (tex->texture->getType() == LandscapeTexType::eTextureFile)
	{
		LandscapeTexTextureFile *generate = 
			(LandscapeTexTextureFile *) tex->texture;

		if (counter) counter->setNewOp(LANG_RESOURCE("LANDSCAPE_MAP", "Load Landscape Map"));
		Image texture = 
			ImageFactory::loadImage(S3D::eModLocation, generate->texture.c_str());
		mainMap_ = texture.createResize(1024, 1024);

		// Set the general surround and roof texture
		if (generate->surroundTexture.empty())
		{
			groundTexture_.replace(texture, false);
			roofTexture_.replace(texture, true);
		}
		else
		{
			Image surroundTexture = 
				ImageFactory::loadImage(S3D::eModLocation, generate->surroundTexture.c_str());
			groundTexture_.replace(surroundTexture, false);
			roofTexture_.replace(surroundTexture, true);
		}
	}
	else
	{
		S3D::dialogExit("Landscape", S3D::formatStringBuffer(
			"Failed to find heightmap type %i",
			tex->texture->getType()));
	}

	points_->generate();

	// Add lighting to the landscape texture
	sky_->getSun().setPosition(tex->skysunxy, tex->skysunyz);
	if (!GLStateExtension::hasHardwareShadows())
	{
		Vector sunPos = sky_->getSun().getPosition();
		ImageModifier::addLightMapToBitmap(mainMap_,
			ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getHeightMap(),
			sunPos,  // Match with shadows
			tex->skyambience, tex->skydiffuse, counter);
	}
	else
	{
		GLSLShader::defines_list dl;
		if (getShadowFrameBuffer().bufferValid())
		{
			dl.push_back("USE_SHADOWS");
		}

		// Load shader
		if (!landShader_) 
		{
			landShader_ = new GLSLShaderSetup(
				S3D::getDataFile("data/shaders/land.vshader"),
				S3D::getDataFile("data/shaders/land.fshader"),
				dl);
		}
	}

	// Add shadows to the mainmap
	std::list<PlacementShadowDefinition::Entry> &shadows = 
		ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getShadows();
	std::list<PlacementShadowDefinition::Entry>::iterator itor;
	for (itor = shadows.begin();
		itor != shadows.end();
		++itor)
	{
		PlacementShadowDefinition::Entry &entry = (*itor);

		entry.definition_->updateLandscapeTexture(
			!GLStateExtension::hasHardwareShadows() ||
			OptionsDisplay::instance()->getNoGLObjectShadows(),
			ScorchedClient::instance()->getContext(),
			entry.position_, entry.size_);
	}

	// Create the main landscape texture
	DIALOG_ASSERT(texture_.replace(mainMap_, false));

    // Create the landscape texture used for the small plan window
	gluScaleImage(
		GL_RGB,
		mainMap_.getWidth(), mainMap_.getHeight(),
		GL_UNSIGNED_BYTE, mainMap_.getBits(),
		bitmapPlan_.getWidth(), bitmapPlan_.getHeight(),
		GL_UNSIGNED_BYTE, bitmapPlan_.getBits());

	// Generate the scorch map for the landscape
	Image sprayMaskBitmap = 
		ImageFactory::loadImage(
			S3D::eModLocation, 
			"data/textures/smoke01.bmp", 
			"data/textures/smoke01.bmp", 
			false);
	scorchMap_ = 
		ImageFactory::loadImage(
			S3D::eModLocation, 
			tex->scorch);
	Image scorchMap = scorchMap_.createResize(
		sprayMaskBitmap.getWidth(), sprayMaskBitmap.getHeight());
	Image texture1New(sprayMaskBitmap.getWidth(), sprayMaskBitmap.getHeight(), true);
	ImageModifier::makeBitmapTransparent(texture1New, scorchMap, sprayMaskBitmap);
	landTex1_.replace(texture1New);

	// Arena
	Image arenaBitmap = ImageModifier::makeArenaBitmap();
	DIALOG_ASSERT(arenaMainTexture_.replace(arenaBitmap));
	arenaMainTexture_.draw();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	Image arenaSurroundBitmap = ImageModifier::makeArenaSurroundBitmap();
	DIALOG_ASSERT(arenaSurroundTexture_.replace(arenaSurroundBitmap));

	// Magma
	Image bitmapMagma = 
		ImageFactory::loadImage(
			S3D::eModLocation, 
			tex->magmasmall);
	DIALOG_ASSERT(magTexture_.replace(bitmapMagma));

	// Detail
	Image bitmapDetail = 
		ImageFactory::loadImage(
			S3D::eModLocation, 
			tex->detail);
	DIALOG_ASSERT(detailTexture_.replace(bitmapDetail, true));

	// Set the fog color
	GLfloat fogColorF[4];
	fogColorF[0] = tex->fog[0];
	fogColorF[1] = tex->fog[1];
	fogColorF[2] = tex->fog[2];
	fogColorF[3] = 1.0f;
	glFogfv(GL_FOG_COLOR, fogColorF);
	GLfloat fogDensity = tex->fogdensity;
	glFogf(GL_FOG_DENSITY, fogDensity);	
	
	// Load the sky
	sky_->generate();

	// Load water
	water_->generate(counter);

	// Create the plan textures (for the plan and wind dialogs)
	updatePlanTexture();
	updatePlanATexture();

	// Add any ambientsounds
	LandscapeSoundManager::instance()->addSounds();
	LandscapeMusicManager::instance()->addMusics();
}

void Landscape::updatePlanTexture()
{
	if (water_->getWaterOn())
	{
		ImageModifier::addWaterToBitmap(
			ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getHeightMap(), 
			bitmapPlan_, water_->getWaterBitmap(), water_->getWaterHeight());
	}

	DIALOG_ASSERT(planTexture_.replace(bitmapPlan_, false));
}

void Landscape::actualDrawLandTextured()
{
	unsigned int state = 0;
	if (!OptionsDisplay::instance()->getUseLandscapeTexture()) state |= GLState::TEXTURE_OFF;

	GLState glState(state);

	bool showArenaArea = MainCamera::instance()->getShowArena();

	bool useDetail = 
		GLStateExtension::getTextureUnits() > 2 &&
		OptionsDisplay::instance()->getDetailTexture() &&
		GLStateExtension::hasEnvCombine();

	if (OptionsDisplay::instance()->getUseLandscapeTexture())
	{
		if (GLStateExtension::hasMultiTex())
		{
			if (useDetail)
			{
				glActiveTextureARB(GL_TEXTURE2_ARB);
				glEnable(GL_TEXTURE_2D);
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
				glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE_ARB, 2);

				detailTexture_.draw(true);
			}

			glActiveTextureARB(GL_TEXTURE1_ARB);
			glEnable(GL_TEXTURE_2D);
			if (showArenaArea) 
			{
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
				arenaMainTexture_.draw();
			}
			else getShadowMap().setTexture();

			glActiveTextureARB(GL_TEXTURE0_ARB);
		}

		texture_.draw(true);
	}
	
	glColor3f(1.0f, 1.0f, 1.0f);
	GAMESTATE_PERF_COUNTER_START(ScorchedClient::instance()->getGameState(), "LANDSCAPE_LAND");
	VisibilityPatchGrid::instance()->drawLand(0, false, false);
	GAMESTATE_PERF_COUNTER_END(ScorchedClient::instance()->getGameState(), "LANDSCAPE_LAND");

	if (OptionsDisplay::instance()->getDrawLandLOD())
	{
		VisibilityPatchGrid::instance()->drawLandLODLevels();
	}

	if (OptionsDisplay::instance()->getUseLandscapeTexture())
	{
		if (GLStateExtension::hasMultiTex())
		{
			glActiveTextureARB(GL_TEXTURE1_ARB);
			if (showArenaArea) arenaSurroundTexture_.draw();
			else glEnable(GL_TEXTURE_2D);

			glActiveTextureARB(GL_TEXTURE0_ARB);
		}

		groundTexture_.draw(true);
	}

	if (OptionsDisplay::instance()->getDrawSurround())
	{
		GAMESTATE_PERF_COUNTER_START(ScorchedClient::instance()->getGameState(), "LANDSCAPE_SURROUND");
		VisibilityPatchGrid::instance()->drawSurround();
		GAMESTATE_PERF_COUNTER_END(ScorchedClient::instance()->getGameState(), "LANDSCAPE_SURROUND");
	}

	if (OptionsDisplay::instance()->getUseLandscapeTexture())
	{
		if (GLStateExtension::hasMultiTex())
		{
			if (useDetail)
			{
				glActiveTextureARB(GL_TEXTURE2_ARB);
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
				glDisable(GL_TEXTURE_2D);
			}

			glActiveTextureARB(GL_TEXTURE1_ARB);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glDisable(GL_TEXTURE_2D);
			glActiveTextureARB(GL_TEXTURE0_ARB);
		}
	}
}

void Landscape::actualDrawLandReflection()
{
	unsigned int state = GLState::BLEND_ON | GLState::LIGHTING_ON | GLState::LIGHT1_ON;
	if (!OptionsDisplay::instance()->getUseLandscapeTexture()) state |= GLState::TEXTURE_OFF;
	GLState glState(state);

	Vector4 ambientColor(1.0f, 1.0f, 1.0f, 1.0f);
	Vector4 diffuseColor(0.0f, 0.0f, 0.0f, 1.0f);
	Vector4 specularColor(1.0f, 1.0f, 1.0f, 1.0f);
	Vector4 emissiveColor(0.0f, 0.0f, 0.0f, 1.0f);
	float shininess = 1.0f;
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientColor);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseColor);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularColor);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissiveColor);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

	Landscape::instance()->getSky().getSun().setLightPosition();

	if (OptionsDisplay::instance()->getUseLandscapeTexture())
	{
		planAlphaTexture_.draw(true);
	}
	
	glColor3f(1.0f, 1.0f, 1.0f);
	VisibilityPatchGrid::instance()->drawLand(2, false, false);
}

void Landscape::createShadowMatrix()
{
	if (!GLStateExtension::hasHardwareShadows()) return;

	// Create texture matrix
	static const float mNormalizeMatrix[] = 
	{ 
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f, 
		0.0f, 0.0f, 0.5f, 0.0f, 
		0.5f, 0.5f, 0.5f, 1.0f
	};
	static GLfloat modelMatrix[16], modelMatrixI[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, modelMatrix);

	// Copy and transpose
	modelMatrixI[ 0] = modelMatrix[ 0];
	modelMatrixI[ 4] = modelMatrix[ 1];
	modelMatrixI[ 8] = modelMatrix[ 2];
	modelMatrixI[ 1] = modelMatrix[ 4];
	modelMatrixI[ 5] = modelMatrix[ 5];
	modelMatrixI[ 9] = modelMatrix[ 6];
	modelMatrixI[ 2] = modelMatrix[ 8];
	modelMatrixI[ 6] = modelMatrix[ 9];
	modelMatrixI[10] = modelMatrix[10];
	modelMatrixI[ 3] = 0;
	modelMatrixI[ 7] = 0;
	modelMatrixI[11] = 0;
	modelMatrixI[15] = modelMatrix[15];
	
	// Apply the inverse transformation
	modelMatrixI[12] = modelMatrixI[0] * -modelMatrix[12] + modelMatrixI[4] * -modelMatrix[13] + modelMatrixI[ 8] * -modelMatrix[14];
	modelMatrixI[13] = modelMatrixI[1] * -modelMatrix[12] + modelMatrixI[5] * -modelMatrix[13] + modelMatrixI[ 9] * -modelMatrix[14];
	modelMatrixI[14] = modelMatrixI[2] * -modelMatrix[12] + modelMatrixI[6] * -modelMatrix[13] + modelMatrixI[10] * -modelMatrix[14];

	// Create and save texture matrix
	glMatrixMode(GL_TEXTURE);
	glLoadMatrixf(mNormalizeMatrix);
	glMultMatrixf(lightProjMatrix_);
	glMultMatrixf(lightModelMatrix_);
	glMultMatrixf(modelMatrixI);
	glGetFloatv(GL_TEXTURE_MATRIX, shadowTextureMatrix_);
	glMatrixMode(GL_MODELVIEW);
}

void Landscape::actualDrawLandShader()
{
	GLState glState(GLState::TEXTURE_ON | GLState::DEPTH_ON);

	getSky().getSun().setLightPosition(false);

	landShader_->use();
	landShader_->set_gl_texture(texture_, "mainmap", 0);
	landShader_->set_gl_texture(detailTexture_, "detailmap", 1);
	landShader_->set_gl_texture(arenaMainTexture_, "arenamap", 3);

	bool showArenaArea = MainCamera::instance()->getShowArena();
	landShader_->set_uniform("showarena", showArenaArea?1.0f:0.0f);

	if (getShadowFrameBuffer().bufferValid())
	{
		glActiveTextureARB(GL_TEXTURE2_ARB);
		glEnable(GL_TEXTURE_2D);
		landShader_->set_gl_texture(shadowFrameBuffer_, "shadow", 2);
		glMatrixMode(GL_TEXTURE);
		createShadowMatrix();
		glMatrixMode(GL_MODELVIEW);
	}

	glActiveTextureARB(GL_TEXTURE0_ARB);
	glColor3f(1.0f, 1.0f, 1.0f);

	// Draw Land
	GAMESTATE_PERF_COUNTER_START(ScorchedClient::instance()->getGameState(), "LANDSCAPE_LAND");
	VisibilityPatchGrid::instance()->drawLand(0, false, false);
	GAMESTATE_PERF_COUNTER_END(ScorchedClient::instance()->getGameState(), "LANDSCAPE_LAND");

	// Draw Surround
	if (OptionsDisplay::instance()->getDrawSurround())
	{
		GAMESTATE_PERF_COUNTER_START(ScorchedClient::instance()->getGameState(), "LANDSCAPE_SURROUND");
		if (OptionsDisplay::instance()->getDrawWater() &&
			Landscape::instance()->getWater().getTransparency() == 1.0f)
		{
			// Disable Tex
			glActiveTextureARB(GL_TEXTURE2_ARB);
			glMatrixMode(GL_TEXTURE);
			glLoadIdentity();
			glMatrixMode(GL_MODELVIEW);
			glActiveTextureARB(GL_TEXTURE0_ARB);

			landShader_->use_fixed();

			groundTexture_.draw(true);
		}
		else
		{
			landShader_->set_gl_texture(groundTexture_, "mainmap", 0);
			landShader_->set_gl_texture(arenaSurroundTexture_, "arenamap", 3);
		}

		VisibilityPatchGrid::instance()->drawSurround();
		GAMESTATE_PERF_COUNTER_END(ScorchedClient::instance()->getGameState(), "LANDSCAPE_SURROUND");
	}

	// Disable Tex
	glActiveTextureARB(GL_TEXTURE2_ARB);
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glActiveTextureARB(GL_TEXTURE0_ARB);

	landShader_->use_fixed();
}

void Landscape::drawGraphicalTextureMap(GLTexture &texture)
{
	GLState state(GLState::TEXTURE_ON);

	glColor3f( 1.f, 1.f, 1.f );
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, 800, 0, 600);
	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	texture.draw(true);
	glBegin(GL_QUADS);
		glTexCoord2f(0.f,   0.f);
		glVertex2i  (0,   0);
		glTexCoord2f(1.f, 0.f);
		glVertex2i  (300, 0);
		glTexCoord2f(1.f, 1.f);
		glVertex2i  (300, 300);
		glTexCoord2f(0.f,   1.f);
		glVertex2i  (0,   300);
	glEnd();  
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode( GL_TEXTURE );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();
}

void Landscape::updatePlanATexture()
{
	ImageModifier::removeWaterFromBitmap(
		ScorchedClient::instance()->getLandscapeMaps().getGroundMaps().getHeightMap(), 
		bitmapPlan_, bitmapPlanAlpha_, bitmapPlanAlphaAlpha_, 
		(water_->getWaterOn()?water_->getWaterHeight():-50.0f));
	DIALOG_ASSERT(planAlphaTexture_.replace(bitmapPlanAlpha_, false));
	planAlphaTexture_.draw();
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
}

void Landscape::restoreLandscapeTexture()
{
	if (textureType_ == eDefault) return;

	DIALOG_ASSERT(texture_.replace(mainMap_, false));
	textureType_ = eDefault;
}

void Landscape::savePlan()
{
	static unsigned counter = 0;
	time_t currentTime = time(0);
	bitmapPlan_.writeToFile(
		S3D::getHomeFile(
			S3D::formatStringBuffer("PlanShot-%i-%i.bmp", currentTime, counter++)));
}

Landscape::CameraContext::CameraContext()
{
	shadowMap_ = new ShadowMap();
}

ShadowMap &Landscape::getShadowMap()
{
	if (GLCamera::getCurrentCamera() ==
		&MainCamera::instance()->getCamera())
	{
		return *cameraContexts_[0].shadowMap_;
	}
	else
	{
		return *cameraContexts_[1].shadowMap_;
	}
}

