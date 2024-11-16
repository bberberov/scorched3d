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

#include <landscapedef/LandscapeDefn.h>
#include <landscapedef/LandscapeDefnTankStart.h>
#include <landscapedef/LandscapeDefinitions.h>
#include <common/Defines.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

static LandscapeDefnType *fetchHeightMapDefnType(const char *type)
{
	if (0 == strcmp(type, "generate")) return new LandscapeDefnHeightMapGenerate;
	if (0 == strcmp(type, "file")) return new LandscapeDefnHeightMapFile;
	S3D::dialogMessage("LandscapeDefnType", S3D::formatStringBuffer("Unknown heightmap type %s", type));
	return 0;
}

static LandscapeDefnType *fetchRoofMapDefnType(const char *type)
{
	if (0 == strcmp(type, "sky")) return new LandscapeDefnTypeNone;
	if (0 == strcmp(type, "cavern")) return new LandscapeDefnRoofCavern;
	S3D::dialogMessage("LandscapeDefnType", S3D::formatStringBuffer("Unknown roof type %s", type));
	return 0;
}

static LandscapeDefnType *fetchDeformType(const char *type)
{
	if (0 == strcmp(type, "solid")) return new LandscapeDefnDeformSolid;
	if (0 == strcmp(type, "deform")) return new LandscapeDefnDeformDeform;
	if (0 == strcmp(type, "file")) return new LandscapeDefnDeformFile;
	S3D::dialogMessage("LandscapeDefnType", S3D::formatStringBuffer("Unknown deform type %s", type));
	return 0;
}

static bool parseMinMax(XMLNode *parent, const char *name, 
	fixed &min, fixed &max)
{
	XMLNode *node = 0;
	if (!parent->getNamedChild(name, node)) return false;
	if (!node->getNamedChild("max", max)) return false;
	if (!node->getNamedChild("min", min)) return false;
	return node->failChildren();
}

static bool parseMinMaxInt(XMLNode *parent, const char *name, 
	int &min, int &max)
{
	XMLNode *node = 0;
	if (!parent->getNamedChild(name, node)) return false;
	if (!node->getNamedChild("max", max)) return false;
	if (!node->getNamedChild("min", min)) return false;
	return node->failChildren();
}

bool LandscapeDefnTypeNone::readXML(XMLNode *node)
{
	return node->failChildren();
}

LandscapeDefnRoofCavern::LandscapeDefnRoofCavern() : 
	heightmap(0), deform(0)
{
}

LandscapeDefnRoofCavern::~LandscapeDefnRoofCavern()
{
	delete heightmap;
	delete deform;
}

bool LandscapeDefnRoofCavern::readXML(XMLNode *node)
{
	if (!node->getNamedChild("width", width)) return false;
	if (!node->getNamedChild("height", height)) return false;
	{
		XMLNode *heightNode;
		std::string heightmaptype;
		if (!node->getNamedChild("heightmap", heightNode)) return false;
		if (!heightNode->getNamedParameter("type", heightmaptype)) return false;
		if (!(heightmap = fetchHeightMapDefnType(heightmaptype.c_str()))) return false;
		if (!heightmap->readXML(heightNode)) return false;
	}	
	{
		XMLNode *deformNode;
		std::string deformtype;
		if (node->getNamedChild("deform", deformNode, false))
		{
			if (!deformNode->getNamedParameter("type", deformtype)) return false;
			if (!(deform = fetchDeformType(deformtype.c_str()))) return false;
			if (!deform->readXML(deformNode)) return false;
		}
		else
		{
			deform = new LandscapeDefnDeformDeform();
		}
	}
	return node->failChildren();
}

bool LandscapeDefnDeformFile::readXML(XMLNode *node)
{
	if (!node->getNamedChild("file", file)) return false;
	if (!node->getNamedChild("levelsurround", levelsurround)) return false;
	if (!S3D::checkDataFile(file.c_str())) return false;
	return node->failChildren();
}

bool LandscapeDefnDeformSolid::readXML(XMLNode *node)
{
	return true;
}

bool LandscapeDefnDeformDeform::readXML(XMLNode *node)
{
	return true;
}

bool LandscapeDefnHeightMapFile::readXML(XMLNode *node)
{
	if (!node->getNamedChild("file", file)) return false;
	if (!node->getNamedChild("levelsurround", levelsurround)) return false;
	if (!S3D::checkDataFile(file.c_str())) return false;
	return node->failChildren();
}

bool LandscapeDefnHeightMapGenerate::readXML(XMLNode *node)
{
	if (!node->getNamedChild("mask", mask)) return false;
	if (!parseMinMaxInt(node, "landhills", 
		landhillsmin, landhillsmax)) return false;
	if (!parseMinMax(node, "landheight", 
		landheightmin, landheightmax)) return false;
	if (!parseMinMax(node, "landpeakwidthx", 
		landpeakwidthxmin, landpeakwidthxmax)) return false;
	if (!parseMinMax(node, "landpeakwidthy", 
		landpeakwidthymin, landpeakwidthymax)) return false;
	if (!parseMinMax(node, "landpeakheight", 
		landpeakheightmin, landpeakheightmax)) return false;
	if (!node->getNamedChild("landsmoothing", landsmoothing)) return false;
	if (!node->getNamedChild("levelsurround", levelsurround)) return false;

	noisefactor = 1;
	noiseheight = noisewidth = 64;
	node->getNamedChild("noisefactor", noisefactor, false);
	node->getNamedChild("noisewidth", noisewidth, false);
	node->getNamedChild("noiseheight", noiseheight, false);

	errosions = 0;
	errosionlayering = 0;
	errosionsurroundsize = 25;
	errosionforce = fixed(1) / fixed(25);
	errosionmaxdepth = fixed(4);
	errosionsurroundforce = fixed(1);
	node->getNamedChild("errosions", errosions, false);
	node->getNamedChild("errosionlayering", errosionlayering, false);
	node->getNamedChild("errosionforce", errosionforce, false);
	node->getNamedChild("errosionmaxdepth", errosionmaxdepth, false);
	node->getNamedChild("errosionsurroundforce", errosionsurroundforce, false);
	node->getNamedChild("errosionsurroundsize", errosionsurroundsize, false);

	if (!mask.empty())
	{
		if (!S3D::checkDataFile(mask.c_str())) return false;
	}
	return node->failChildren();
}

LandscapeDefn::LandscapeDefn() :
	heightmap(0), tankstart(0), roof(0), deform(0)
{
}

LandscapeDefn::~LandscapeDefn()
{
	delete roof;
	delete tankstart;
	delete heightmap;
	delete deform;
}

bool LandscapeDefn::readXML(LandscapeDefinitions *definitions, XMLNode *node)
{
	if (!node->getNamedChild("minplayers", minplayers)) return false;
	if (!node->getNamedChild("maxplayers", maxplayers)) return false;
	if (!node->getNamedChild("landscapewidth", landscapewidth)) return false;
	if (!node->getNamedChild("landscapeheight", landscapeheight)) return false;

	if (landscapewidth % 32 != 0 ||
		landscapeheight % 32 != 0)
	{
		S3D::dialogMessage("Scorched3D",
			S3D::formatStringBuffer(
				"ERROR: Landscape width and height must each be a multiple of 32.\n"
				"Specified size : %ix%i", landscapewidth, landscapeheight));
		return false;
	}
	if (!node->getNamedChild("arenawidth", arenawidth, false)) arenawidth = landscapewidth;
	if (!node->getNamedChild("arenaheight", arenaheight, false)) arenaheight = landscapeheight;
	if (arenawidth > landscapewidth ||
		arenaheight > landscapeheight)
	{
		S3D::dialogMessage("Scorched3D",
			S3D::formatStringBuffer(
				"ERROR: Arena width and height must each be less than or equal to the landscape size.\n"
				"Specified size : %ix%i", arenawidth, arenaheight));
		return false;
	}
	arenax = (landscapewidth - arenawidth) / 2;
	arenay = (landscapeheight - arenaheight) / 2;

	{
		XMLNode *startNode;
		std::string tankstarttype;
		if (!node->getNamedChild("tankstart", startNode)) return false;
		if (!startNode->getNamedParameter("type", tankstarttype)) return false;
		if (!(tankstart = LandscapeDefnTypeTankStart::createType(tankstarttype.c_str()))) return false;
		if (!tankstart->readXML(startNode)) return false;
	}
	{
		XMLNode *heightNode;
		std::string heightmaptype;
		if (!node->getNamedChild("heightmap", heightNode)) return false;
		if (!heightNode->getNamedParameter("type", heightmaptype)) return false;
		if (!(heightmap = fetchHeightMapDefnType(heightmaptype.c_str()))) return false;
		if (!heightmap->readXML(heightNode)) return false;
	}
	{
		XMLNode *roofNode;
		std::string rooftype;
		if (!node->getNamedChild("roof", roofNode)) return false;
		if (!roofNode->getNamedParameter("type", rooftype)) return false;
		if (!(roof = fetchRoofMapDefnType(rooftype.c_str()))) return false;
		if (!roof->readXML(roofNode)) return false;
	}
	{
		XMLNode *deformNode;
		std::string deformtype;
		if (node->getNamedChild("deform", deformNode, false))
		{
			if (!deformNode->getNamedParameter("type", deformtype)) return false;
			if (!(deform = fetchDeformType(deformtype.c_str()))) return false;
			if (!deform->readXML(deformNode)) return false;
		}
		else
		{
			deform = new LandscapeDefnDeformDeform();
		}
	}

	if (!texDefn.readXML(definitions, node)) return false;

	return node->failChildren();
}

