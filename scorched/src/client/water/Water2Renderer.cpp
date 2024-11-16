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

#include <water/Water2Renderer.h>
#include <water/Water2.h>
#include <water/WaterMapPoints.h>
#include <water/WaterWaves.h>
#include <common/Vector4.h>
#include <image/ImageFactory.h>
#include <image/ImageFactory.h>
#include <engine/Simulator.h>
#include <GLEXT/GLStateExtension.h>
#include <GLEXT/GLCamera.h>
#include <GLEXT/GLTextureCubeMap.h>
#include <client/ScorchedClient.h>
#include <sky/Sky.h>
#include <land/VisibilityPatchGrid.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscapedef/LandscapeTex.h>
#include <landscapedef/LandscapeDefn.h>
#include <landscape/Landscape.h>
#include <graph/MainCamera.h>
#include <graph/OptionsDisplay.h>

#include <water/Water2Constants.h>

Water2Renderer::Water2Renderer() : 
	waterShader_(0),
	currentPatch_(0), totalTime_(0.0f),
	noShaderWaterTexture_(0)
{
}

Water2Renderer::~Water2Renderer()
{
	delete waterShader_;
}

void Water2Renderer::simulate(float frameTime)
{
	totalTime_ += frameTime * 24.0f;
}

void Water2Renderer::draw(Water2 &water2, WaterMapPoints &points, 
	WaterWaves &waves, float transparency)
{
	GAMESTATE_PERF_COUNTER_START(ScorchedClient::instance()->getGameState(), "WATER_PATCHSETUP");
	// Choose correct water frame
	Water2Patches &currentPatch = water2.getPatch(totalTime_);
	if (&currentPatch != currentPatch_)
	{
		currentPatch_ = &currentPatch;

		// Set the normal map for the current water frame
		if (GLStateExtension::hasShaders() &&
			!OptionsDisplay::instance()->getNoWaterMovement() &&
			!GLStateExtension::useSimpleShaders())
		{
			normalTexture_.replace(currentPatch_->getNormalMap(), 
				GLStateExtension::hasHardwareMipmaps());
		}
	}
	GAMESTATE_PERF_COUNTER_END(ScorchedClient::instance()->getGameState(), "WATER_PATCHSETUP");

	// Draw waves
	GAMESTATE_PERF_COUNTER_START(ScorchedClient::instance()->getGameState(), "WATER_WAVES");
	waves.draw(*currentPatch_);
	GAMESTATE_PERF_COUNTER_END(ScorchedClient::instance()->getGameState(), "WATER_WAVES");

	// Draw Water
	if (GLStateExtension::hasShaders() &&
		OptionsDisplay::instance()->getUseWaterTexture())
	{
		drawWaterShaders(water2, transparency);
	}
	else
	{
		drawWaterNoShaders(water2, transparency);
	}

	GAMESTATE_PERF_COUNTER_START(ScorchedClient::instance()->getGameState(), "WATER_DRAWPOINTS");
	drawPoints(points);
	GAMESTATE_PERF_COUNTER_END(ScorchedClient::instance()->getGameState(), "WATER_DRAWPOINTS");
}

void Water2Renderer::drawPoints(WaterMapPoints &points)
{
	if (!currentPatch_) return;

	// Draw Points
	points.draw(*currentPatch_);
}

void Water2Renderer::drawWaterShaders(Water2 &water2, float transparency)
{
	GLState state(GLState::LIGHTING_OFF | GLState::TEXTURE_ON | GLState::BLEND_ON);

	glPushAttrib(GL_ALL_ATTRIB_BITS);

	Vector cameraPos = GLCamera::getCurrentCamera()->getCurrentPos();
	cameraPos[2] = -waterHeight_;
	waterShader_->use();
	waterShader_->set_uniform("viewpos", cameraPos);
	waterShader_->set_uniform("transparency", transparency);

	// Tex 3
	if (!GLStateExtension::useSimpleShaders())
	{
		waterShader_->set_gl_texture(currentPatch_->getAOF(), "tex_foamamount", 3);
	}

	// Tex 2
	if (Landscape::instance()->getShadowFrameBuffer().bufferValid())
	{
		glActiveTextureARB(GL_TEXTURE2_ARB);
		glEnable(GL_TEXTURE_2D);
		waterShader_->set_gl_texture(Landscape::instance()->getShadowFrameBuffer(), 
			"tex_shadow", 2);
		glMatrixMode(GL_TEXTURE);
		glLoadMatrixf(Landscape::instance()->getShadowTextureMatrix());
		glMatrixMode(GL_MODELVIEW);
	}

	// texture units / coordinates:
	// tex0: noise map (color normals) / matching texcoords
	// tex1: reflection map / matching texcoords
	// tex2: foam
	// tex3: amount of foam

	// set up scale/translation of foam and noise maps
	// we do not use the texture matrix here, as this would be overkill
	// and would be clumsy

	// need to divide by noise tile size here too (tex coordinates are in [0...1], not meters)
	Vector D_0 = windDir1_ * (windSpeed1_ / (-64.0f * 6.0f)); // noise tile is 256/8=32m long
	Vector D_1 = windDir2_ * (windSpeed2_ / (-16.0f * 6.0f));  // noise tile is 256/32=8m long

	float mytime = totalTime_ / 24.0f;
	Vector noise_0_pos = D_0 * mytime;
	Vector noise_1_pos = D_1 * mytime;
	noise_0_pos[2] = wavetile_length_rcp * 8.0f;
	noise_1_pos[2] = wavetile_length_rcp * 32.0f;

	//fixme: do we have to treat the viewer offset here, like with tex matrix
	//       setup below?!

	// Tex 0
	glActiveTextureARB(GL_TEXTURE0);
	if (!GLStateExtension::useSimpleShaders())
	{
		waterShader_->set_uniform("noise_xform_0", noise_0_pos);
		waterShader_->set_uniform("noise_xform_1", noise_1_pos);
		waterShader_->set_gl_texture(normalTexture_, "tex_normal", 0);
	}

	const float noisetilescale = 1.0f/32.0f;//meters (128/16=8, 8tex/m).
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glScalef(noisetilescale, noisetilescale, 1.0f);	// fixme adjust scale
	glMatrixMode(GL_MODELVIEW);

	// Tex 1
	glActiveTextureARB(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	waterShader_->set_gl_texture(reflectionTexture_, "tex_reflection", 1);

	// Draw Water
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	drawWater(water2, waterShader_);

	// Cleanup	
	waterShader_->use_fixed();

	glActiveTextureARB(GL_TEXTURE2);
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE1);
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE0);
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);

	glPopAttrib();
}

void Water2Renderer::drawWaterNoShaders(Water2 &water2, float transparency)
{
	glPushAttrib(GL_TEXTURE_BIT);

	// Set up texture coordinate generation for reflections
	static float PlaneS[] = { 0.0f, 1.0f / 20.0f, 0.0f, 0.0f };
	static float PlaneT[] = { 1.0f / 20.0f, 0.0f, 0.0f, 0.0f };

	if (GLStateExtension::hasMultiTex() &&
		OptionsDisplay::instance()->getUseWaterTexture())
	{
		// Set up texture coordinate generation for base texture
		glActiveTextureARB(GL_TEXTURE1);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_TEXTURE_GEN_S); 
		glEnable(GL_TEXTURE_GEN_T);
		glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
		glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
		glTexGenfv(GL_S, GL_OBJECT_PLANE, PlaneS);
		glTexGenfv(GL_T, GL_OBJECT_PLANE, PlaneT);
		reflectionTexture_.draw(true);

		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
		glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE_ARB, 2);

		glActiveTextureARB(GL_TEXTURE0);


		glColor4f(0.3f, 0.3f, 0.3f, 0.8f * transparency);
	}
	else
	{
		glColor4f(0.7f, 0.7f, 0.7f, 0.8f * transparency);
	}

	// Turn lighting on (if enabled)
	unsigned int state = 0;
	if (!OptionsDisplay::instance()->getNoModelLighting())
	{
		state = 
			GLState::LIGHTING_ON | 
			GLState::LIGHT1_ON;

		Vector4 ambientColor(0.2f, 0.2f, 0.2f, 0.8f * transparency);
		Vector4 diffuseColor(0.8f, 0.8f, 0.8f, 0.8f * transparency);
		Vector4 specularColor(1.0f, 1.0f, 1.0f, 0.8f * transparency);
		Vector4 emissiveColor(0.0f, 0.0f, 0.0f, 0.8f * transparency);
		float shininess = 100.0f;
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientColor);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseColor);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularColor);
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissiveColor);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

		Landscape::instance()->getSky().getSun().setLightPosition();
	}

	if (!OptionsDisplay::instance()->getUseWaterTexture())
	{
		GLState currentState(GLState::LIGHTING_OFF | GLState::TEXTURE_OFF);
		glColor3f(0.0f, 0.3f, 1.0f);
		drawWater(water2, 0);
	}
	else if (GLStateExtension::hasCubeMap())
	{
		GLState currentState(state | GLState::TEXTURE_OFF | GLState::BLEND_ON | GLState::CUBEMAP_ON);

		// Set up texture coordinate generation for cubemap reflections
		glEnable(GL_TEXTURE_GEN_S); 
		glEnable(GL_TEXTURE_GEN_T);
		glEnable(GL_TEXTURE_GEN_R);
		glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_EXT);
		glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_EXT);
		glTexGenf(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_EXT);

		drawWater(water2, 0);
	}
	else if (GLStateExtension::hasSphereMap())
	{
		GLState currentState(state | GLState::TEXTURE_ON | GLState::BLEND_ON);
		noShaderWaterTexture_->draw();

		// Set up texture coordinate generation for spheremap reflections
		glEnable(GL_TEXTURE_GEN_S); 
		glEnable(GL_TEXTURE_GEN_T);
		glEnable(GL_TEXTURE_GEN_R);
		glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glTexGenf(GL_R, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

		drawWater(water2, 0);
	}
	else
	{
		GLState currentState(state | GLState::TEXTURE_ON | GLState::BLEND_ON);
		noShaderWaterTexture_->draw();

		// Set up texture coordinate generation for linear reflections
		glEnable(GL_TEXTURE_GEN_S); 
		glEnable(GL_TEXTURE_GEN_T);
		glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
		glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
		glTexGenfv(GL_S, GL_OBJECT_PLANE, PlaneS);
		glTexGenfv(GL_T, GL_OBJECT_PLANE, PlaneT);

		drawWater(water2, 0);
	}

	if (GLStateExtension::hasMultiTex())
	{
		glActiveTextureARB(GL_TEXTURE1);
		glDisable(GL_TEXTURE_GEN_S); 
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_R);
		glDisable(GL_TEXTURE_2D);

		glActiveTextureARB(GL_TEXTURE0);
		glDisable(GL_TEXTURE_GEN_S); 
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_R);
	}

	glPopAttrib();
}

void Water2Renderer::drawWater(Water2 &water2, GLSLShaderSetup *waterShader)
{
	// Draw Water
	Vector &cameraPos = GLCamera::getCurrentCamera()->getCurrentPos();
	VisibilityPatchGrid::instance()->drawWater(
		*currentPatch_, water2.getIndexs(), cameraPos, landscapeSize_, waterShader);
}

void Water2Renderer::generate(LandscapeTexBorderWater *water, ProgressCounter *counter)
{
	currentPatch_ = 0;
	if (GLStateExtension::hasShaders())
	{
		// Load shaders
		if (!waterShader_) 
		{
			GLSLShader::defines_list dl;
			if (Landscape::instance()->getShadowFrameBuffer().bufferValid())
			{
				dl.push_back("USE_SHADOWS");
			}

			if (GLStateExtension::useSimpleShaders())
			{
				waterShader_ = new GLSLShaderSetup(
					S3D::getDataFile("data/shaders/watersimple.vshader"),
					S3D::getDataFile("data/shaders/watersimple.fshader"),
					dl);
			}
			else
			{
				waterShader_ = new GLSLShaderSetup(
					S3D::getDataFile("data/shaders/water.vshader"),
					S3D::getDataFile("data/shaders/water.fshader"),
					dl);
			}
		}
	}

	// Set the water height
	waterHeight_ = water->height.asFloat();

	// fixme: color depends also on weather. bad weather -> light is less bright
	// so this will be computed implicitly. Environment can also change water color
	// (light blue in tropic waters), water depth also important etc.
	// this depends on sky color...
	// good weather
	//	color wavetop = color(color(10, 10, 10), color(18, 93, 77), light_brightness);
	//	color wavebottom = color(color(10, 10, 20), color(18, 73, 107), light_brightness);
	// rather bad weather
	//fixme: multiply with light color here, not only light brightness.
	//dim yellow light should result in dim yellow-green upwelling color, not dim green.
	Vector4 light_color(water->wavelight, 1.0f);
	Vector4 wavetopA(water->wavetopa, 0.0f);
	Vector4 wavetopB(water->wavetopb, 0.0f);
	Vector4 wavebottomA(water->wavebottoma, 0.0f);
	Vector4 wavebottomB(water->wavebottomb, 0.0f);
	Vector4 wavetop = light_color.lerp(wavetopA, wavetopB);
	Vector4 wavebottom = light_color.lerp(wavebottomA, wavebottomB);

	// Create textures
	if (GLStateExtension::hasFBO() &&
		GLStateExtension::hasShaders() &&
		!OptionsDisplay::instance()->getNoWaterReflections())
	{
		reflectionTexture_.createBufferTexture(512, 512, false);
		reflectionBuffer_.create(reflectionTexture_, true);
	}
	else
	{
		Image loadedBitmapWater = 
			ImageFactory::loadImage(
				S3D::eModLocation, 
				water->texture);
		Image bitmapWater2 = loadedBitmapWater.createResize(128, 128);
		reflectionTexture_.create(bitmapWater2, true); // Not the reflection in this case
	}

	Image map(128, 128, 3, 0);
	normalTexture_.create(map, GLStateExtension::hasHardwareMipmaps());

	LandscapeDefn &defn = *ScorchedClient::instance()->getLandscapeMaps().
		getDefinitions().getDefn();
	landscapeSize_ = Vector(defn.getLandscapeWidth(), defn.getLandscapeHeight());

	if (GLStateExtension::hasShaders())
	{
		Vector upwelltop(wavetop[0], wavetop[1], wavetop[2]);
		Vector upwellbot(wavebottom[0], wavebottom[1], wavebottom[2]);
		Vector upwelltopbot = upwelltop - upwellbot;

		waterShader_->use();
		waterShader_->set_uniform("upwelltop", upwelltop);
		waterShader_->set_uniform("upwellbot", upwellbot);
		waterShader_->set_uniform("upwelltopbot", upwelltopbot);
		if (!GLStateExtension::useSimpleShaders())
		{
			Vector landfoam;
			waterShader_->set_uniform("landfoam", landfoam);
			waterShader_->set_uniform("landscape_size", landscapeSize_);
		}
		waterShader_->use_fixed();
	}
	else
	{
		// Load the water reflection bitmap
		// Create water cubemap texture
		Image loadedBitmapWater = 
			ImageFactory::loadImage(
				S3D::eModLocation, 
				water->reflection);
		Image bitmapWater2 = loadedBitmapWater.createResize(256, 256);
		delete noShaderWaterTexture_;
		if (GLStateExtension::hasCubeMap())
		{
			GLTextureCubeMap *waterCubeMap = new GLTextureCubeMap();
			waterCubeMap->create(bitmapWater2, false);
			noShaderWaterTexture_ = waterCubeMap;
		}
		else 
		{
			GLTexture *waterNormalMap = new GLTexture();
			waterNormalMap->create(bitmapWater2, false);
			noShaderWaterTexture_ = waterNormalMap;
		}
	}

	// Setup wind dir/speed
	windSpeed1_ = ScorchedClient::instance()->
		getSimulator().getWind().getWindSpeed().asFloat() * 2.0f + 3.0f;
	windDir1_ = ScorchedClient::instance()->
		getSimulator().getWind().getWindDirection().asVector();
	windDir1_[2] = 0.0f;
	windSpeed2_ = MAX(0.0f, RAND * 2.0f - 1.0f + windSpeed1_);
	windDir2_ = Vector(windDir1_[0] + RAND * 0.4f - 0.2f,
		windDir1_[1] + RAND * 0.4f - 0.2f);
	windDir1_.StoreNormalize();
	windDir2_.StoreNormalize();
}
