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

#if !defined(__INCLUDE_LandscapeTexh_INCLUDE__)
#define __INCLUDE_LandscapeTexh_INCLUDE__

#include <landscapedef/LandscapeTexDefn.h>
#include <common/ModelID.h>
#include <XML/XMLFile.h>
#include <string>
#include <vector>

class ScorchedContext;
class LandscapeTexType
{
public:
	enum TexType
	{
		eNone,
		ePrecipitationRain,
		ePrecipitationSnow,
		eWater,
		eTextureGenerate,
		eTextureFile
	};

	virtual bool readXML(XMLNode *node) = 0;
	virtual TexType getType() = 0;
};

class LandscapeTexTypeNone : public LandscapeTexType
{
public:
	virtual bool readXML(XMLNode *node);
	virtual TexType getType() { return eNone; }
};

class LandscapeTexPrecipitation : public LandscapeTexType
{
public:
	int particles;

	virtual bool readXML(XMLNode *node);
};

class LandscapeTexPrecipitationRain : public LandscapeTexPrecipitation
{
public:
	virtual TexType getType() { return ePrecipitationRain; }
};

class LandscapeTexPrecipitationSnow : public LandscapeTexPrecipitation
{
public:
	virtual TexType getType() { return ePrecipitationSnow; }
};

class LandscapeTexBorderWater : public LandscapeTexType
{
public:
	// Non-shader
	std::string reflection;
	std::string texture;
	std::string foam;

	// Shader
	Vector wavetopa;
	Vector wavetopb;
	Vector wavebottoma;
	Vector wavebottomb;
	Vector wavelight;

	// Both
	fixed height;
	float waterTransparency;
	
	virtual bool readXML(XMLNode *node);
	virtual TexType getType() { return eWater; }
};

class LandscapeTexTextureGenerate : public LandscapeTexType
{
public:
	std::string roof;
	std::string rockside;
	std::string shore;
	std::string texture0;
	std::string texture1;
	std::string texture2;
	std::string texture3;

	virtual bool readXML(XMLNode *node);
	virtual TexType getType() { return eTextureGenerate; }
};

class LandscapeTexTextureFile : public LandscapeTexType
{
public:
	std::string texture;
	std::string surroundTexture;

	virtual bool readXML(XMLNode *node);
	virtual TexType getType() { return eTextureFile; }
};

class LandscapeTex
{
public:
	LandscapeTex();
	virtual ~LandscapeTex();

	unsigned int seed;
	std::string detail;
	std::string magmasmall;
	std::string scorch;
	Vector fog;
	Vector suncolor;
	bool nosunfog;
	bool nohorizonglow;
	bool nosunblend;
	std::string suntexture;
	std::string suntexturemask;
	float fogdensity;
	std::string skytexture;
	std::string skytexturestatic;
	std::string skytexturemask;
	std::string skycolormap;
	std::string skyline;
	std::string skylinemask;
	int skytimeofday;
	float skysunxy;
	float skysunyz;
	Vector skydiffuse;
	Vector skyambience;

	LandscapeTexType *border;
	LandscapeTexType *texture;
	LandscapeTexType *precipitation;
	LandscapeTexDefn texDefn;

	bool readXML(LandscapeDefinitions *definitions, XMLNode *node);

private:
	LandscapeTex(const LandscapeTex &other);
	LandscapeTex &operator=(LandscapeTex &other);
};

#endif
