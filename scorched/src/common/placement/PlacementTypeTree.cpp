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

#include <placement/PlacementTypeTree.h>
#include <landscapemap/LandscapeMaps.h>
#include <engine/ScorchedContext.h>
#include <common/ProgressCounter.h>
#include <common/RandomGenerator.h>
#include <common/Defines.h>
#include <XML/XMLParser.h>
#include <image/ImageFactory.h>

PlacementTypeTree::PlacementTypeTree() : 
	mincloseness(0), maxobjects(2000)
{
}

PlacementTypeTree::~PlacementTypeTree()
{
}

bool PlacementTypeTree::readXML(XMLNode *node)
{
	node->getNamedChild("mask", mask, false);
	if (!node->getNamedChild("numobjects", numobjects)) return false;
	if (!node->getNamedChild("numclusters", numclusters)) return false;
	if (!node->getNamedChild("minheight", minheight)) return false;
	if (!node->getNamedChild("maxheight", maxheight)) return false;
	node->getNamedChild("mincloseness", mincloseness, false);
	node->getNamedChild("maxobjects", maxobjects, false);
	return PlacementType::readXML(node);
}

void PlacementTypeTree::getPositions(ScorchedContext &context,
	RandomGenerator &generator,
	std::list<Position> &returnPositions,
	ProgressCounter *counter)
{
	Image bmap(256, 256);
	Image map = bmap;
	if (mask.c_str()[0])
	{	
		map = ImageFactory::loadImage(S3D::eModLocation, mask);
		if (!map.getBits())
		{
			S3D::dialogExit("PlacementTypeTree",
				S3D::formatStringBuffer("Error: failed to find mask \"%s\"",
				mask.c_str()));
		}
		if (!map.getLossless())
		{
			S3D::dialogExit("PlacementTypeTree", S3D::formatStringBuffer(
				"Error: Placement mask \"%s\" is not a lossless image format",
				mask.c_str()));
		}
	}

	// Generate a map of where the trees should go
	unsigned char objectMap[64 * 64];
	memset(objectMap, 0, sizeof(unsigned char) * 64 * 64);

	int groundMapWidth = context.getLandscapeMaps().getGroundMaps().getLandscapeWidth();
	int groundMapHeight = context.getLandscapeMaps().getGroundMaps().getLandscapeHeight();

	// A few points where trees will be clustered around
	int treeMapMultWidth  = groundMapWidth / 64;
	int treeMapMultHeight = groundMapHeight / 64;
	for (int i=0; i<numclusters; i++)
	{
		// Get a random point
		int x = (generator.getRandFixed("PlacementTypeTree") * 64).asInt();
		int y = (generator.getRandFixed("PlacementTypeTree") * 64).asInt();

		// Check point is in the correct height band
		fixed height = 
			context.getLandscapeMaps().
				getGroundMaps().getHeight(
					x * treeMapMultWidth, y * treeMapMultHeight);
		FixedVector &normal =
			context.getLandscapeMaps().
				getGroundMaps().getNormal(
					x * treeMapMultWidth, y * treeMapMultHeight);

		int mx = int(map.getWidth() * (fixed(x) / 64).asInt());
		int my = int(map.getHeight() * (fixed(y) / 64).asInt());
		unsigned char *bits = map.getBits() +
			mx * 3 + my * map.getWidth() * 3;
		if (bits[0] > 127 &&
			height > minheight && 
			height < maxheight && 
			normal[2] > fixed(true, 7000))
		{
			// Group other areas around this point that are likely to get trees
			// Do a few groups
			int n = (generator.getRandFixed("PlacementTypeTree") * 10).asInt() + 5;
			for (int j=0; j<n; j++)
			{
				// Check groups is within bounds
				int newX = x + (generator.getRandFixed("PlacementTypeTree") * 8).asInt() - 4;
				int newY = y + (generator.getRandFixed("PlacementTypeTree") * 8).asInt() - 4;
				if (newX >= 0 && newX < 64 &&
					newY >= 0 && newY < 64)
				{
					FixedVector &normal =
						context.getLandscapeMaps().
						getGroundMaps().getNormal(
							newX * treeMapMultWidth, newY * treeMapMultHeight);
					height = 
						context.getLandscapeMaps().
						getGroundMaps().getHeight(
							newX * treeMapMultWidth, newY * treeMapMultHeight);
					if (height > minheight && 
						height < maxheight && 
						normal[2] > fixed(true, 7000))
					{
						objectMap[newX + 64 * newY] = 64;
					}
				}
			}

			// Add the original point
			objectMap[x + 64 * y] = 255;
		}
	}

	// Smooth the treemap
	unsigned char objectMapCopy[64 * 64];
	memcpy(objectMapCopy, objectMap, sizeof(unsigned char) * 64 * 64);
	fixed matrix[3][3];
	for (int i=0; i<3; i++)
	{
		for (int j=0; j<3; j++)
		{
			matrix[i][j] = fixed(true, 7000); // How much smoothing is done (> is more)
			if (i==1 && j==1) matrix[i][j] = 4;
		}
	}
	for (int y=0; y<64; y++)
	{
		for (int x=0; x<64; x++)
		{
			// Total is used to catch corner cases
			fixed total = 0;
			fixed inc = 0;
			for (int i=0; i<3; i++)
			{
				for (int j=0; j<3; j++)
				{
					int newX = i + x - 1;
					int newY = j + y - 1;
					
					if (newX >= 0 && newX < 64 &&
						newY >= 0 && newY < 64)
					{
						inc += objectMapCopy[newX + 64 * newY];
						total += matrix[i][j];
					}
				}
			}
	 
			objectMap[x + 64 * y] = (unsigned char)(inc/total).asInt();
		}
	}

	// Add lots of trees, more chance of adding a tree where
	// the map is stongest
	int objectCount = 0;
	const int NoIterations = numobjects;
	for (int i=0; i<NoIterations && objectCount < maxobjects; i++)
	{
		if (i % 1000 == 0) if (counter) 
			counter->setNewPercentage(
				MAX(float(i)/float(NoIterations), float(objectCount) / float(maxobjects)) *100.0f);

		fixed lx = generator.getRandFixed("PlacementTypeTree") * fixed(groundMapWidth);
		fixed ly = generator.getRandFixed("PlacementTypeTree") * fixed(groundMapHeight);
		int rx = (lx.asInt());
		int ry = (ly.asInt());
		int nx = rx / treeMapMultWidth;
		int ny = ry / treeMapMultHeight;
		int r = objectMap[nx + 64 * ny];
		int nr = (generator.getRandFixed("PlacementTypeTree") * 512).asInt();

		if (nr < r)
		{
			fixed height = 
				context.getLandscapeMaps().
					getGroundMaps().getInterpHeight(lx, ly);

			if (height > minheight + fixed(true, 5000))
			{
				objectCount ++;
				Position position;
				position.position[0] = lx;
				position.position[1] = ly;
				position.position[2] = height;
				if (checkCloseness(position.position, context, 
					returnPositions, mincloseness))
				{
					returnPositions.push_back(position);
				}
			}
		}
	}
}
