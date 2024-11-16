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

#include <landscapedef/LandscapeTex.h>
#include <landscapedef/LandscapeDefinitions.h>
#include <engine/ScorchedContext.h>
#include <weapons/AccessoryStore.h>
#include <common/Defines.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>

static LandscapeTexType *fetchBorderTexType(const char *type)
{
	if (0 == strcmp(type, "water")) return new LandscapeTexBorderWater;
	if (0 == strcmp(type, "none")) return new LandscapeTexTypeNone;
	S3D::dialogMessage("LandscapeTexType", S3D::formatStringBuffer("Unknown border type %s", type));
	return 0;
}

static LandscapeTexType *fetchTextureTexType(const char *type)
{
	if (0 == strcmp(type, "generate")) return new LandscapeTexTextureGenerate;
	if (0 == strcmp(type, "file")) return new LandscapeTexTextureFile;
	S3D::dialogMessage("LandscapeTexType", S3D::formatStringBuffer("Unknown texture type %s", type));
	return 0;
}

static LandscapeTexType *fetchPrecipitationTexType(const char *type)
{
	if (0 == strcmp(type, "none")) return new LandscapeTexTypeNone;
	if (0 == strcmp(type, "rain")) return new LandscapeTexPrecipitationRain;
	if (0 == strcmp(type, "snow")) return new LandscapeTexPrecipitationSnow;
	S3D::dialogMessage("LandscapeTexType", S3D::formatStringBuffer("Unknown precipitation type %s", type));
	return 0;
}

bool LandscapeTexTypeNone::readXML(XMLNode *node)
{
	return node->failChildren();
}

// LandscapeTexPrecipitation
bool LandscapeTexPrecipitation::readXML(XMLNode *node)
{
	if (!node->getNamedChild("particles", particles)) return false;
	return node->failChildren();
}

// LandscapeTexBorderWater 
bool LandscapeTexBorderWater::readXML(XMLNode *node)
{
	if (!node->getNamedChild("reflection", reflection)) return false;
	if (!node->getNamedChild("texture", texture)) return false;
	if (!node->getNamedChild("foam", foam)) return false;
	if (!S3D::checkDataFile(reflection.c_str())) return false;
	if (!S3D::checkDataFile(texture.c_str())) return false;
	if (!S3D::checkDataFile(foam.c_str())) return false;

	if (!node->getNamedChild("height", height)) return false;
	if (!node->getNamedChild("wavetopa", wavetopa)) return false;
	if (!node->getNamedChild("wavetopb", wavetopb)) return false;
	if (!node->getNamedChild("wavebottoma", wavebottoma)) return false;
	if (!node->getNamedChild("wavebottomb", wavebottomb)) return false;
	if (!node->getNamedChild("wavelight", wavelight)) return false;

	waterTransparency = 1.0f;
	node->getNamedChild("watertransparency", waterTransparency, false);	

	return node->failChildren();
}

// LandscapeTexTextureGenerate
bool LandscapeTexTextureGenerate::readXML(XMLNode *node)
{
	if (!node->getNamedChild("roof", roof)) return false;
	if (!node->getNamedChild("rockside", rockside)) return false;
	if (!node->getNamedChild("shore", shore)) return false;
	if (!node->getNamedChild("texture0", texture0)) return false;
	if (!node->getNamedChild("texture1", texture1)) return false;
	if (!node->getNamedChild("texture2", texture2)) return false;
	if (!node->getNamedChild("texture3", texture3)) return false;
	if (!S3D::checkDataFile(roof.c_str())) return false;
	if (!S3D::checkDataFile(rockside.c_str())) return false;
	if (!S3D::checkDataFile(shore.c_str())) return false;
	if (!S3D::checkDataFile(texture0.c_str())) return false;
	if (!S3D::checkDataFile(texture1.c_str())) return false;
	if (!S3D::checkDataFile(texture2.c_str())) return false;
	if (!S3D::checkDataFile(texture3.c_str())) return false;
	return node->failChildren();
}

// LandscapeTexTextureFile
bool LandscapeTexTextureFile::readXML(XMLNode *node)
{
	if (!node->getNamedChild("texture", texture)) return false;
	if (!S3D::checkDataFile(texture.c_str())) return false;
	if (node->getNamedChild("surroundtexture", surroundTexture, false))
	{
		if (!S3D::checkDataFile(surroundTexture.c_str())) return false;
	}
	return node->failChildren();
}

// LandscapeTex
LandscapeTex::LandscapeTex() :
	border(0), texture(0), precipitation(0)
{
}

LandscapeTex::~LandscapeTex()
{
	delete border;
	delete texture;
	delete precipitation;
}

bool LandscapeTex::readXML(LandscapeDefinitions *definitions, XMLNode *node)
{
	seed = 0;
	node->getNamedChild("seed", seed, false);
	skytexturestatic = "";
	nosunfog = false; nohorizonglow = false; nosunblend = false;
	if (!node->getNamedChild("detail", detail)) return false;
	if (!node->getNamedChild("magmasmall", magmasmall)) return false;
	if (!node->getNamedChild("scorch", scorch)) return false;
	if (!node->getNamedChild("fog", fog)) return false;
	if (!node->getNamedChild("suncolor", suncolor)) return false;
	if (!node->getNamedChild("suntexture", suntexture)) return false;
	node->getNamedChild("suntexturemask", suntexturemask, false);
	if (!node->getNamedChild("fogdensity", fogdensity)) return false;
	if (!node->getNamedChild("skytexture", skytexture)) return false;
	node->getNamedChild("skytexturestatic", skytexturestatic, false);
	node->getNamedChild("nosunfog", nosunfog, false);
	node->getNamedChild("nohorizonglow", nohorizonglow, false);
	node->getNamedChild("nosunblend", nosunblend, false);
	node->getNamedChild("skyline", skyline, false);
	node->getNamedChild("skylinemask", skylinemask, false);
	if (!node->getNamedChild("skytexturemask", skytexturemask)) return false;
	if (!node->getNamedChild("skycolormap", skycolormap)) return false;
	if (!node->getNamedChild("skytimeofday", skytimeofday)) return false;
	if (!node->getNamedChild("skysunxy", skysunxy)) return false;
	if (!node->getNamedChild("skysunyz", skysunyz)) return false;
	if (!node->getNamedChild("skydiffuse", skydiffuse)) return false;
	if (!node->getNamedChild("skyambience", skyambience)) return false;

	if (!S3D::checkDataFile(detail.c_str())) return false;
	if (!S3D::checkDataFile(magmasmall.c_str())) return false;
	if (!S3D::checkDataFile(scorch.c_str())) return false;
	if (!S3D::checkDataFile(skytexture.c_str())) return false;
	if (!S3D::checkDataFile(skytexturemask.c_str())) return false;
	if (!S3D::checkDataFile(skycolormap.c_str())) return false;
	if (!S3D::checkDataFile(suntexture.c_str())) return false;

	{
		XMLNode *borderNode;
		std::string bordertype;
		if (!node->getNamedChild("border", borderNode)) return false;
		if (!borderNode->getNamedParameter("type", bordertype)) return false;
		if (!(border = fetchBorderTexType(bordertype.c_str()))) return false;
		if (!border->readXML(borderNode)) return false;
	}
	{
		XMLNode *textureNode;
		std::string texturetype;
		if (!node->getNamedChild("texture", textureNode)) return false;
		if (!textureNode->getNamedParameter("type", texturetype)) return false;
		if (!(texture = fetchTextureTexType(texturetype.c_str()))) return false;
		if (!texture->readXML(textureNode)) return false;
	}
	{
		XMLNode *precipitationNode;
		std::string precipitationtype;
		if (node->getNamedChild("precipitation", precipitationNode, false))
		{
			if (!precipitationNode->getNamedParameter("type", precipitationtype)) return false;
			if (!(precipitation = fetchPrecipitationTexType(precipitationtype.c_str()))) return false;
			if (!precipitation->readXML(precipitationNode)) return false;
		}
		else
		{
			precipitation = new LandscapeTexTypeNone();
		}
	}

	if (!texDefn.readXML(definitions, node)) return false;

	return node->failChildren();
}

