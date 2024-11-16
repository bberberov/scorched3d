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

#include <landscapemap/RoofMaps.h>
#include <landscapemap/HeightMapLoader.h>
#include <image/ImageFactory.h>
#include <common/ProgressCounter.h>
#include <engine/ScorchedContext.h>
#include <landscapedef/LandscapeDefinitionCache.h>
#include <landscapedef/LandscapeDefn.h>

RoofMaps::RoofMaps(LandscapeDefinitionCache &defnCache) :
	defnCache_(defnCache), roofBaseHeight_(0)
{
}

RoofMaps::~RoofMaps()
{
}

void RoofMaps::generateMaps(
	ScorchedContext &context,
	ProgressCounter *counter)
{
	generateRMap(context, counter);
}

bool RoofMaps::getRoofOn()
{
	return (defnCache_.getDefn()->roof->getType() == LandscapeDefnType::eRoofCavern);
}

fixed RoofMaps::getRoofHeight(int x, int y)
{
	if (!getRoofOn())
	{
		return fixed::MAX_FIXED;
	}

	if (x >= 0 && y >= 0 && x<=rmap_.getMapWidth() && y<=rmap_.getMapHeight())
	{
		return rmap_.getHeight(x, y);
	}
	return roofBaseHeight_;
}

fixed RoofMaps::getInterpRoofHeight(fixed x, fixed y)
{
	if (!getRoofOn())
	{
		return fixed::MAX_FIXED;
	}

	if (x >= 0 && y >= 0 && x<=rmap_.getMapWidth() && y<=rmap_.getMapHeight())
	{
		return rmap_.getInterpHeight(x, y);
	}
	return roofBaseHeight_;
}

void RoofMaps::generateRMap(
	ScorchedContext &context,
	ProgressCounter *counter)
{
	// calculate roof size and set it
	int mapWidth = defnCache_.getDefn()->getLandscapeWidth();
	int mapHeight = defnCache_.getDefn()->getLandscapeHeight();
	rmap_.create(mapWidth, mapHeight, true);
	deformRMap_.create(rmap_.getMapWidth(), rmap_.getMapHeight(), true);
	roofBaseHeight_ = 0;

	// Generate the roof
	if (getRoofOn())
	{
		LandscapeDefnRoofCavern *cavern = 
			(LandscapeDefnRoofCavern *) defnCache_.getDefn()->roof;

		roofBaseHeight_ = fixed(cavern->height);

		bool smooth = false;
		if (!HeightMapLoader::generateTerrain(
			defnCache_.getSeed() + 1,
			cavern->heightmap,
			rmap_,
			smooth,
			counter))
		{
			S3D::dialogExit("Landscape", "Failed to generate roof");
		}

		// Reverse heights (up-side-down)
		for (int j=0; j<=rmap_.getMapHeight(); j++)
		{
			for (int i=0; i<=rmap_.getMapWidth(); i++)
			{
				fixed height = rmap_.getHeight(i, j);
				height = fixed(cavern->height) - height;
				rmap_.setHeight(i, j, height);
			}
		}

		// Generate the deform map (if any)
		if (cavern->deform->getType() == LandscapeDefnType::eDeformFile)
		{
			LandscapeDefnDeformFile *file = 
				(LandscapeDefnDeformFile *) cavern->deform;

			// Load the landscape
			Image image = ImageFactory::loadImage(S3D::eModLocation, file->file);
			if (!image.getBits())
			{
				S3D::dialogExit("HeightMapLoader", S3D::formatStringBuffer(
					"Error: Unable to find deform roof map \"%s\"",
					file->file.c_str()));
			}
			if (!image.getLossless())
			{
				S3D::dialogExit("HeightMapLoader", S3D::formatStringBuffer(
					"Error: Deform landscape map \"%s\" is not a lossless image format",
					file->file.c_str()));
			}
			HeightMapLoader::loadTerrain(
				deformRMap_,
				image, 
				file->levelsurround,
				counter);

			for (int j=0; j<=deformRMap_.getMapHeight(); j++)
			{
				for (int i=0; i<=deformRMap_.getMapWidth(); i++)
				{
					fixed height = deformRMap_.getHeight(i, j);
					height = fixed(cavern->height) - height;
					deformRMap_.setHeight(i, j, height);
				}
			}
		}
		else
		{
			for (int x=0; x<rmap_.getMapWidth(); x++)
			{
				for (int y=0; y<rmap_.getMapHeight(); y++)
				{
					deformRMap_.setHeight(x, y, rmap_.getHeight(x, y));
				}
			}
		}
	}
}
