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

#include <landscapemap/DeformLandscape.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscapedef/LandscapeTex.h>
#include <landscapedef/LandscapeDefn.h>
#include <landscapedef/LandscapeDefinition.h>
#include <target/TargetContainer.h>
#include <target/TargetLife.h>
#include <target/TargetSpace.h>
#include <target/TargetState.h>
#include <engine/ScorchedContext.h>
#include <engine/Simulator.h>
#include <common/OptionsScorched.h>
#include <common/Defines.h>
#include <common/ProgressCounter.h>
#include <lang/LangResource.h>
#include <math.h>
#ifndef S3D_SERVER
	#include <landscape/Landscape.h>
	#include <landscape/DeformTextures.h>
	#include <land/VisibilityPatchGrid.h>
	#include <sprites/ExplosionTextures.h>
#endif

class DeformLandscapeCache
{
public:
	class DeformLandscapeCacheItem
	{
	public:
		DeformLandscapeCacheItem(int radius)
		{
			int diameter = radius * 2 + 1;
			explosionDepth_ = new fixed[diameter * diameter];
			explosionDistance_ = new fixed[diameter * diameter];

			for (int x=-radius; x<=radius; x++)
			{
				for (int y=-radius; y<=radius; y++)
				{
					DIALOG_ASSERT(x+radius<diameter && y+radius<diameter);
					fixed explosionDepth = fixed(0), explosionDistance = fixed(-1);

					FixedVector newPos(x, y, 0);
					fixed dist = newPos.Magnitude();
					if (dist < radius)
					{
						fixed distToRadius = fixed(radius) - dist;
						explosionDepth = ((distToRadius / radius) * fixed::XPIO2).sin() * radius;

						explosionDistance = explosionDepth / radius;
						explosionDistance *= fixed(3);
						if (explosionDistance > fixed(1)) explosionDistance = fixed(1);						
					}

					explosionDepth_[(y+radius) * diameter + (x+radius)] = explosionDepth;
					explosionDistance_[(y+radius) * diameter + (x+radius)] = explosionDistance;
				}
			}
		}
		~DeformLandscapeCacheItem()
		{
			delete [] explosionDepth_;
			delete [] explosionDistance_;
		}

		fixed *explosionDepth_;
		fixed *explosionDistance_;
	};

	DeformLandscapeCache() 
	{ 
		for (int i=0;i<100;i++) landscapeCacheItems[i] = 0; 
	}
	~DeformLandscapeCache() {}

	DeformLandscapeCacheItem &getItem(int size)
	{
		DIALOG_ASSERT(size > 0 && size < 50);
		DeformLandscapeCacheItem *landscapeCacheItem = landscapeCacheItems[size];
		if (!landscapeCacheItem)
		{
			landscapeCacheItem = new DeformLandscapeCacheItem(size);
			landscapeCacheItems[size] = landscapeCacheItem;
		}
		return *landscapeCacheItem;
	}

private:
	DeformLandscapeCacheItem *landscapeCacheItems[100];
};
static DeformLandscapeCache deformCache;

void DeformLandscape::deformLandscape(
	ScorchedContext &context,
	FixedVector &pos, fixed radius, bool down, fixed depthScale,
	const char *deformTexture)
{
	if (context.getOptionsGame().getActionSyncCheck())
	{
		context.getSimulator().addSyncCheck(
			S3D::formatStringBuffer("Deform : %s %s %s", 
				pos.asQuickString(), 
				radius.asQuickString(), (down?"Down":"Up")));
	}

	static DeformPoints deformMap;
	if (down && context.getLandscapeMaps().getRoofMaps().getRoofOn())
	{
		bool hits = deformRoofInternal(context, pos, radius, depthScale, true);
#ifndef S3D_SERVER
		if (hits && !context.getServerMode())
		{
			Landscape::instance()->recalculateRoof();
			VisibilityPatchGrid::instance()->recalculateRoofErrors(pos, radius);
		}
#endif
	}

	bool hits = deformLandscapeInternal(context, pos, radius, down, deformMap, true, depthScale);
#ifndef S3D_SERVER
	if (hits && !context.getServerMode() && deformTexture)
	{
		Landscape::instance()->recalculateLandscape();
		VisibilityPatchGrid::instance()->recalculateLandscapeErrors(pos, radius);

		DeformTextures::deformLandscape(
			pos.asVector(), 
			radius.asFloat(),  
			ExplosionTextures::instance()->getScorchBitmap(deformTexture),
			deformMap);
	}
#endif
}

bool DeformLandscape::deformLandscapeInternal(
	ScorchedContext &context,
	FixedVector &pos, fixed radius, bool down, DeformPoints &map, 
	bool setNormals, fixed depthScale)
{
	HeightMap &hmap = context.getLandscapeMaps().getGroundMaps().getHeightMap();
	HeightMap &deformhmap = context.getLandscapeMaps().getGroundMaps().getDeformMap();

	bool hits = false;
	int iradius = (int) radius.asInt() + 1;
	if (iradius > 49) iradius = 49;

	fixed lowestLandscapeHeight = fixed(context.getOptionsGame().getMinimumLandHeight());
	LandscapeDefnType::DefnType deformType = context.getLandscapeMaps().getDefinitions().getDefn()->deform->getType();
	DeformLandscapeCache::DeformLandscapeCacheItem &deformItem = deformCache.getItem(iradius);
	fixed *explosionDepth = deformItem.explosionDepth_;
	fixed *explosionDistance = deformItem.explosionDistance_;

	// Take out or add a chunk into the landsacpe
	for (int y=-iradius; y<=iradius; y++)
	{
		for (int x=-iradius; x<=iradius; x++, explosionDepth++, explosionDistance++)	
		{
			DIALOG_ASSERT(x+iradius<100 && y+iradius<100);
			if (*explosionDistance != fixed(-1))
			{
				int absx = pos[0].asInt() + x;
				int absy = pos[1].asInt() + y;
				if (absx > 0 && absx < hmap.getMapWidth() &&
					absy > 0 && absy < hmap.getMapHeight())
				{
					fixed currentHeight = hmap.getHeight(absx, absy);
					fixed newHeight = currentHeight;
					fixed actualExplosionDepth = *explosionDepth * depthScale;
					if (down)
					{
						if (currentHeight > pos[2] - actualExplosionDepth)
						{
							if (currentHeight > pos[2] + actualExplosionDepth)
							{
								newHeight -= actualExplosionDepth + actualExplosionDepth;
							}
							else
							{
								newHeight = pos[2] - actualExplosionDepth;
							}

							fixed lowestHeight = lowestLandscapeHeight;
							if (deformType != LandscapeDefnType::eDeformDeform)
							{
								fixed deformHeight = deformhmap.getHeight(absx, absy);
								lowestHeight = MAX(deformHeight, lowestLandscapeHeight);
							}

							if (newHeight < lowestHeight)
							{
								if (currentHeight < lowestHeight)
								{
									newHeight = currentHeight;
								}
								else
								{
									newHeight = lowestHeight;
								}
							}
						}
					}
					else
					{
						if (currentHeight < pos[2] + actualExplosionDepth)
						{
							newHeight = currentHeight + actualExplosionDepth;
						}
					}

					if (newHeight != currentHeight)
					{
						hits = true;
						hmap.setHeight(absx, absy, newHeight);
					}
				}
			}

			map.map[x+iradius][y+iradius] = *explosionDistance;
		}
	}

	if (hits && setNormals)
	{
		// Take out or add a chunk into the landsacpe
		for (int x=-iradius-3; x<=iradius+3; x++)
		{
			for (int y=-iradius-3; y<=iradius+3; y++)
			{
				int absx = pos[0].asInt() + x;
				int absy = pos[1].asInt() + y;
				if (absx >= 0 && absx < hmap.getMapWidth() &&
					absy >= 0 && absy < hmap.getMapHeight())
				{
					hmap.getNormal(absx, absy);
				}
			}
		}
	}

	return hits;
}

bool DeformLandscape::deformRoofInternal(ScorchedContext &context,
	FixedVector &pos, fixed radius, fixed depthScale,
	bool setNormals)
{
	HeightMap &hmap = context.getLandscapeMaps().getRoofMaps().getRoofMap();
	HeightMap &deformhmap = context.getLandscapeMaps().getRoofMaps().getDeformRoofMap();

	bool hits = false;
	int iradius = (int) radius.asInt() + 1;
	if (iradius > 49) iradius = 49;

	DeformLandscapeCache::DeformLandscapeCacheItem &deformItem = deformCache.getItem(iradius);
	fixed *explosionDepth = deformItem.explosionDepth_;
	fixed *explosionDistance = deformItem.explosionDistance_;
	LandscapeDefnType::DefnType deformType = 
		((LandscapeDefnRoofCavern *)(context.getLandscapeMaps().getDefinitions().getDefn()->roof))->deform->getType();

	// Take out or add a chunk into the landsacpe
	for (int y=-iradius; y<=iradius; y++)
	{
		for (int x=-iradius; x<=iradius; x++, explosionDepth++, explosionDistance++)	
		{
			DIALOG_ASSERT(x+iradius<100 && y+iradius<100);
			if (*explosionDistance != fixed(-1))
			{
				int absx = pos[0].asInt() + x;
				int absy = pos[1].asInt() + y;
				if (absx > 0 && absx < hmap.getMapWidth() &&
					absy > 0 && absy < hmap.getMapHeight())
				{
					fixed currentHeight = hmap.getHeight(absx, absy);
					fixed newHeight = currentHeight;

					{
						fixed actualExplosionDepth = *explosionDepth * depthScale;
						if (currentHeight < pos[2] + actualExplosionDepth)
						{
							if (currentHeight < pos[2] - actualExplosionDepth)
							{
								newHeight += actualExplosionDepth + actualExplosionDepth;
							}
							else
							{
								newHeight = pos[2] + actualExplosionDepth;
							}

							if (deformType != LandscapeDefnType::eDeformDeform)
							{
								fixed deformHeight = deformhmap.getHeight(absx, absy);
								if (newHeight > deformHeight)
								{
									if (currentHeight > deformHeight)
									{
										newHeight = currentHeight;
									}
									else
									{
										newHeight = deformHeight;
									}
								}
							}
						}
					}

					if (newHeight != currentHeight)
					{
						hits = true;
						hmap.setHeight(absx, absy, newHeight);
					}
				}
			}
		}
	}

	if (hits && setNormals)
	{
		// Take out or add a chunk into the landsacpe
		for (int x=-iradius-3; x<=iradius+3; x++)
		{
			for (int y=-iradius-3; y<=iradius+3; y++)
			{
				int absx = pos[0].asInt() + x;
				int absy = pos[1].asInt() + y;
				if (absx >= 0 && absx < hmap.getMapWidth() &&
					absy >= 0 && absy < hmap.getMapHeight())
				{
					hmap.getNormal(absx, absy);
				}
			}
		}
	}

	return hits;
}

void DeformLandscape::flattenArea(
	ScorchedContext &context, FixedVector &tankPos, 
	bool removeObjects, fixed size)
{
	if (context.getOptionsGame().getActionSyncCheck())
	{
		context.getSimulator().addSyncCheck(
			S3D::formatStringBuffer("Flatten : %s %s", 
			tankPos.asQuickString(), 
			size.asQuickString()));
	}

	flattenAreaInternal(context, tankPos, removeObjects, size, true);
	if (removeObjects)
	{
		// Remove any targets in this location
		std::map<unsigned int, Target *> collisionTargets;
		context.getTargetSpace().getCollisionSet(tankPos, size * fixed(true, 15000), collisionTargets);
		std::map<unsigned int, Target *>::iterator itor;
		for (itor = collisionTargets.begin();
			itor != collisionTargets.end();
			++itor)
		{
			Target *target = (*itor).second;
			if (target->getType() != Target::TypeTank &&
				target->getTargetState().getFlattenDestroy())
			{
				Target *removedTarget = 
					context.getTargetContainer().removeTarget(target->getPlayerId());
				delete removedTarget;
			}
		}

#ifndef S3D_SERVER
		if (!context.getServerMode())
		{
			Landscape::instance()->recalculateLandscape();
			VisibilityPatchGrid::instance()->recalculateLandscapeErrors(tankPos, size);
		}
#endif
	}
}


void DeformLandscape::flattenAreaInternal(
	ScorchedContext &context, FixedVector &tankPos, 
	bool removeObjects, fixed size, bool setNormals)
{
	int iSize = size.asInt();
	HeightMap &hmap = context.getLandscapeMaps().getGroundMaps().getHeightMap();
	HeightMap &deformhmap = context.getLandscapeMaps().getGroundMaps().getDeformMap();
	int posX = tankPos[0].asInt();
	int posY = tankPos[1].asInt();

	fixed lowestLandscapeHeight = fixed(context.getOptionsGame().getMinimumLandHeight());
	LandscapeDefnType::DefnType deformType = context.getLandscapeMaps().getDefinitions().getDefn()->deform->getType();

	// Flatten a small area around the tank
	for (int x=-iSize; x<=iSize; x++)
	{
		for (int y=-iSize; y<=iSize; y++)
		{
			int ix = posX + x;
			int iy = posY + y;
			if (ix >= 0 && iy >= 0 &&
				ix < hmap.getMapWidth() &&
				iy < hmap.getMapHeight())
			{
				fixed newHeight = tankPos[2];
				fixed currentHeight = hmap.getHeight(ix, iy);

				fixed lowestHeight = lowestLandscapeHeight;
				if (deformType != LandscapeDefnType::eDeformDeform)
				{
					fixed deformHeight = deformhmap.getHeight(ix, iy);
					lowestHeight = MAX(deformHeight, lowestLandscapeHeight);
				}

				if (newHeight < lowestHeight)
				{
					if (currentHeight < lowestHeight)
					{
						newHeight = currentHeight;
					}
					else
					{
						newHeight = lowestHeight;
					}
				}

				hmap.setHeight(ix, iy, newHeight);
			}
		}
	}

	if (setNormals)
	{
		for (int x=-iSize-3; x<=iSize+3; x++)
		{
			for (int y=-iSize-3; y<=iSize+3; y++)
			{
				int ix = posX + x;
				int iy = posY + y;
				if (ix >= 0 && iy >= 0 &&
					ix < hmap.getMapWidth() &&
					iy < hmap.getMapHeight())
				{
					hmap.getNormal(ix, iy);
				}
			}
		}
	}
}
