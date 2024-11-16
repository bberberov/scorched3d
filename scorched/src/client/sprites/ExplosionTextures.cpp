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

#include <image/ImageFactory.h>
#include <XML/XMLFile.h>
#include <GLEXT/GLTextureStore.h>
#include <sprites/ExplosionTextures.h>
#include <landscape/Landscape.h>
#include <lang/LangResource.h>
#include <common/Defines.h>
#include <stdio.h>

ExplosionTextures *ExplosionTextures::instance_ = 0;

ExplosionTextures *ExplosionTextures::instance()
{
	if (!instance_)
	{
		instance_ = new ExplosionTextures;
	}

	return instance_;
}

ExplosionTextures::ExplosionTextures()
{

}

ExplosionTextures::~ExplosionTextures()
{

}

bool ExplosionTextures::createTextures(ProgressCounter *counter)
{
	// Tidy
	{
		std::map<std::string, GLTextureSet*>::iterator itor;
		for (itor = textureSets.begin();
			itor != textureSets.end();
			++itor)
		{
			delete itor->second;
		}
		textureSets.clear();
	}

	// Tidy
	{
		std::map<std::string, Image*>::iterator itor;
		for (itor = scorchedBitmaps.begin();
			itor != scorchedBitmaps.end();
			++itor)
		{
			delete itor->second;
		}
		scorchedBitmaps.clear();
	}

	// Tidy
	GLTextureStore::instance()->resetModFiles();

	if (counter) counter->setNewOp(LANG_RESOURCE("EXPLOSION_TEXTURES", "Explosion Textures"));

	{
		Image bitmap = 
			ImageFactory::loadImage(
				S3D::eDataLocation, 
				"data/images/arrow.bmp",
				"data/images/arrowi.bmp",
				true);
		arrowTexture.create(bitmap);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
	}

	Image bitmap = ImageFactory::loadImage(
		S3D::eModLocation, 
		"data/textures/smoke01.bmp",
		"data/textures/smoke01.bmp",
		false);
	smokeTexture.create(bitmap);
	DIALOG_ASSERT(smokeTexture.textureValid());

	Image bitmap2 = ImageFactory::loadImage(
		S3D::eModLocation,
		"data/textures/smoke02.bmp",
		"data/textures/smoke02.bmp", 
		false);
	smokeTexture2.create(bitmap2);
	DIALOG_ASSERT(smokeTexture2.textureValid());

	Image bitmap3 = ImageFactory::loadImage(
		S3D::eModLocation,
		"data/textures/particle.bmp",
		"data/textures/particle.bmp",
		false);
	particleTexture.create(bitmap3);
	DIALOG_ASSERT(particleTexture.textureValid());

	Image talkBitmap = ImageFactory::loadAlphaImage(
		S3D::eModLocation,
		"data/textures/talk.bmp");
	talkTexture.create(talkBitmap);
	DIALOG_ASSERT(talkTexture.textureValid());

	Image bitmap5 = ImageFactory::loadImage(
		S3D::eModLocation, 
		"data/textures/rainm.bmp",
		"data/textures/rain.bmp",
		false);
	rainTexture.create(bitmap5);
	DIALOG_ASSERT(rainTexture.textureValid());

	Image bitmap6 = ImageFactory::loadImage(
		S3D::eModLocation,
		"data/textures/snow.bmp",
		"data/textures/snowm.bmp",
		false);
	snowTexture.create(bitmap6);
	DIALOG_ASSERT(snowTexture.textureValid());

	XMLFile file;
	if (!file.readFile(S3D::getModFile("data/textureset.xml")))
	{
		S3D::dialogMessage("ExplosionTextures", S3D::formatStringBuffer(
					  "Failed to parse \"%s\"\n%s", 
					  "data/textureset.xml",
					  file.getParserError()));
		return false;
	}
	if (!file.getRootNode())
	{
		S3D::dialogMessage("ExplosionTextures", S3D::formatStringBuffer(
					  "Failed to find explosion textures definition file \"%s\"",
					  "data/textureset.xml"));
		return false;		
	}

    std::list<XMLNode *>::iterator childrenItor;
	std::list<XMLNode *> &children = file.getRootNode()->getChildren();
    for (childrenItor = children.begin();
		 childrenItor != children.end();
		 ++childrenItor)
    {
		// Check if it is a texture set
        XMLNode *currentNode = (*childrenItor);
		if (strcmp(currentNode->getName(), "textureset"))
		{
			S3D::dialogMessage("ExplosionTextures",
						  "Failed to find textureset node");
			return false;
		}
		XMLNode *name = 0;
		if (!currentNode->getNamedParameter("name", name)) return false;
		const char *setName = name->getContent();

		// Create and store the new texture set
		GLTextureSet *set = new GLTextureSet();
		textureSets[setName] = set;

		// Load all the textures
		std::list<XMLNode *>::iterator textureItor;
		std::list<XMLNode *> &textures = currentNode->getChildren();
		for (textureItor = textures.begin();
			textureItor != textures.end();
			++textureItor)
		{
			// Check if it is a texture
			XMLNode *currentTexture = (*textureItor);
			if (strcmp(currentTexture->getName(), "texture"))
			{
				S3D::dialogMessage("ExplosionTextures",
							"Failed to find texture sub-node");
				return false;
			}

			// Load texture
			std::string texFile = 
				S3D::formatStringBuffer("data/%s", currentTexture->getContent());
			Image bitmap =
				ImageFactory::loadImage(
					S3D::eModLocation,
					texFile, 
					texFile, 
					false);
			GLTexture *texture = new GLTexture;
			if (!texture->create(bitmap)) 
			{
				S3D::dialogMessage("ExplosionTextures", S3D::formatStringBuffer(
					"Failed to load texture %s",
					texFile.c_str()));
				return false;
			}
			set->addTexture(texture);
		}
	}

	return true;
}

GLTextureSet *ExplosionTextures::getTextureSetByName(const std::string &name)
{
	DIALOG_ASSERT(!textureSets.empty());

	GLTextureSet *result = (*textureSets.begin()).second;
	std::map<std::string, GLTextureSet*>::iterator itor =
		textureSets.find(name);
	if (itor != textureSets.end())
	{
		result = (*itor).second;
	}
	return result;
}

Image &ExplosionTextures::getScorchBitmap(const std::string &name)
{
	if (!name.empty())
	{
		std::map<std::string, Image*>::iterator findItor =
			scorchedBitmaps.find(name);
		if (findItor != scorchedBitmaps.end())
		{
			return *(*findItor).second;
		}

		if (S3D::fileExists(S3D::getModFile(name)))
		{
			Image *map = new Image(ImageFactory::loadImage(S3D::eModLocation, name));
			scorchedBitmaps[name] = map;
			return *map;
		}
	}
	return Landscape::instance()->getScorchMap();
}
