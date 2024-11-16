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

#include <landscapemap/GroundMaps.h>
#include <landscapemap/HeightMapLoader.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscapedef/LandscapeInclude.h>
#include <landscapedef/LandscapeTex.h>
#include <landscapedef/LandscapeDefinitions.h>
#include <landscapedef/LandscapeDefinitionCache.h>
#include <engine/ObjectGroups.h>
#include <target/Target.h>
#include <target/TargetContainer.h>
#include <common/Logger.h>
#include <tankai/TankAIAdder.h>
#include <image/ImageFactory.h>
#include <lang/LangResource.h>

GroundMaps::GroundMaps(LandscapeDefinitionCache &defnCache) :
	defnCache_(defnCache), 
	arenaX_(-1),
	arenaY_(-1),
	arenaWidth_(-1), 
	arenaHeight_(-1)
{
}

GroundMaps::~GroundMaps()
{
}

void GroundMaps::generateMaps(
	ScorchedContext &context,
	ProgressCounter *counter)
{
	arenaWidth_ = context.getLandscapeMaps().getDefinitions().getDefn()->getArenaWidth();
	arenaHeight_ = context.getLandscapeMaps().getDefinitions().getDefn()->getArenaHeight();
	arenaX_ = context.getLandscapeMaps().getDefinitions().getDefn()->getArenaX();
	arenaY_ = context.getLandscapeMaps().getDefinitions().getDefn()->getArenaY();

	generateHMap(context, counter);
	nmap_.create(getLandscapeWidth(), getLandscapeHeight());	
}

int GroundMaps::getLandscapeWidth()
{
	return map_.getMapWidth();
}

int GroundMaps::getLandscapeHeight()
{
	return map_.getMapHeight();
}

int GroundMaps::getArenaWidth()
{
	return arenaWidth_;
}

int GroundMaps::getArenaHeight()
{
	return arenaHeight_;
}

int GroundMaps::getArenaX()
{
	return arenaX_;
}

int GroundMaps::getArenaY()
{
	return arenaY_;
}

void GroundMaps::generateHMap(
	ScorchedContext &context, 
	ProgressCounter *counter)
{
	// Initialize the ground and surround maps
	map_.create(defnCache_.getDefn()->getLandscapeWidth(), 
		defnCache_.getDefn()->getLandscapeHeight(), 
		false);
	deformMap_.create(map_.getMapWidth(), map_.getMapHeight(), false);

	// Generate the landscape
	bool levelSurround = false;
	if (!HeightMapLoader::generateTerrain(
		defnCache_.getSeed(),
		defnCache_.getDefn()->heightmap,
		map_,
		levelSurround,
		counter))
	{
		S3D::dialogExit("Landscape", "Failed to generate landscape");
	}

	// Generate the deform map (if any)
	if (defnCache_.getDefn()->deform->getType() == LandscapeDefnType::eDeformFile)
	{
		LandscapeDefnDeformFile *file = 
			(LandscapeDefnDeformFile *) defnCache_.getDefn()->deform;

		// Load the landscape
		Image image = ImageFactory::loadImage(S3D::eModLocation, file->file);
		if (!image.getBits())
		{
			S3D::dialogExit("HeightMapLoader", S3D::formatStringBuffer(
				"Error: Unable to find deform landscape map \"%s\"",
				file->file.c_str()));
		}
		if (!image.getLossless())
		{
			S3D::dialogExit("HeightMapLoader", S3D::formatStringBuffer(
				"Error: Deform landscape map \"%s\" is not a lossless image format",
				file->file.c_str()));
		}
		HeightMapLoader::loadTerrain(
			deformMap_,
			image, 
			file->levelsurround,
			counter);
	}
	else
	{
		for (int x=0; x<map_.getMapWidth(); x++)
		{
			for (int y=0; y<map_.getMapHeight(); y++)
			{
				deformMap_.setHeight(x, y, map_.getHeight(x, y));
			}
		}
	}
}

void GroundMaps::generateObject(RandomGenerator &generator, 
	LandscapeInclude &place,
	ScorchedContext &context,
	unsigned int &playerId,
	ProgressCounter *counter)
{
	if (counter) counter->setNewOp(LANG_RESOURCE("POPULATING_LANDSCAPE", "Populating Landscape"));

	// Generate all the objects using the objects definitions
	for (unsigned int i=0; i<place.placements.size(); i++)
	{
		PlacementType *type = place.placements[i];
		type->createObjects(context, generator, playerId, counter);
	}
}

void GroundMaps::generateObjects(
	ScorchedContext &context,
	ProgressCounter *counter)
{
	LandscapeTex *tex = defnCache_.getTex();
	LandscapeDefn *defn = defnCache_.getDefn();

	// Remove any existing objects
	std::map<unsigned int, Target *> targets = // Note copy
		context.getTargetContainer().getTargets();
	std::map<unsigned int, Target *>::iterator itor;
	for (itor = targets.begin();
		itor != targets.end();
		++itor)
	{
		unsigned int playerId = (*itor).first;
		Target *target = (*itor).second;
		if (target->getType() != Target::TypeTank)
		{
			Target *removedTarget = 
				context.getTargetContainer().removeTarget(playerId);
			delete removedTarget;
		}
	}

	// Remove any existing shadows
	getShadows().clear();

	// Remove any existing groups
	context.getObjectGroups().clearGroups();

	// Add objects to the landscape (if any)
	// Do this now as it adds shadows to the mainmap
	unsigned int playerId = TargetID::MIN_TARGET_ID;
	{
		// Do this for the definition file
		std::vector<LandscapeInclude *>::iterator itor;
		for (itor = defn->texDefn.includes.begin();
			itor != defn->texDefn.includes.end();
			++itor)
		{
			LandscapeInclude *place = (*itor);
			FileRandomGenerator objectsGenerator;
			objectsGenerator.seed(defnCache_.getSeed());
			generateObject(objectsGenerator, *place, 
				context, playerId, counter);
		}
	}
	{
		// Do this for the texture file
		std::vector<LandscapeInclude *>::iterator itor;
		for (itor = tex->texDefn.includes.begin();
			itor != tex->texDefn.includes.end();
			++itor)
		{
			LandscapeInclude *place = (*itor);
			FileRandomGenerator objectsGenerator;
			objectsGenerator.seed(defnCache_.getSeed());
			generateObject(objectsGenerator, *place, 
				context, playerId, counter);
		}
	}

	// Deform the main map with respect to the objects
	{
		std::list<PlacementShadowDefinition::Entry> &shadows = getShadows();
		std::list<PlacementShadowDefinition::Entry>::iterator itor;
		for (itor = shadows.begin();
			itor != shadows.end();
			++itor)
		{
			PlacementShadowDefinition::Entry &entry = (*itor);
			entry.definition_->updateLandscapeHeight(
				context,
				entry.position_, entry.size_);
		}
	}
}

fixed GroundMaps::getHeight(int w, int h)
{
	return map_.getHeight(w, h);
}

fixed GroundMaps::getInterpHeight(fixed w, fixed h)
{
	return map_.getInterpHeight(w, h);
}

FixedVector &GroundMaps::getNormal(int w, int h)
{
	return map_.getNormal(w, h);
}

void GroundMaps::getInterpNormal(fixed w, fixed h, FixedVector &normal)
{
	FixedVector result;
	map_.getInterpNormal(w, h, normal);
}

bool GroundMaps::getIntersect(Line &direction, Vector &intersect)
{
	return map_.getIntersect(direction, intersect);
}

