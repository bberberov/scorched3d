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

#include <math.h>
#include <stdlib.h>
#include <limits.h>
#include <landscapemap/HeightMapModifier.h>
#include <landscapedef/LandscapeDefn.h>
#include <common/Defines.h>
#include <image/ImageFactory.h>
#include <lang/LangResource.h>

void HeightMapModifier::levelSurround(HeightMap &hmap)
{
	for (int x=0; x<=hmap.getMapWidth(); x++)
	{	
		hmap.setHeight(x, 0, fixed(0));
		hmap.setHeight(x, 1, fixed(0));
		hmap.setHeight(x, hmap.getMapHeight(), fixed(0));
		hmap.setHeight(x, hmap.getMapHeight()-1, fixed(0));
	}

	for (int y=0; y<=hmap.getMapHeight(); y++)
	{
		hmap.setHeight(0, y, fixed(0));
		hmap.setHeight(1, y, fixed(0));
		hmap.setHeight(hmap.getMapWidth(), y, fixed(0));
		hmap.setHeight(hmap.getMapWidth()-1, y, fixed(0));
	}
}

static int noiseFn(int x, int y, int random)
{
    int n = x + y * 57 + random * 131;
    n = (n<<13) ^ n;
    return ((n * (n * n * 15731 + 789221) +
		1376312589)&0x7fffffff);
}

void HeightMapModifier::noise(HeightMap &hmap, 
	LandscapeDefnHeightMapGenerate &defn,
	RandomGenerator &generator,
	ProgressCounter *counter)
{
	if (defn.noisefactor == 0) return;
	if (counter) counter->setNewOp(LANG_RESOURCE("NOISE", "Noise"));

	unsigned int randomU = generator.getRandUInt("HeightMapModifier") % 5000;
	int random = (int) randomU;

	fixed *noisemap = new fixed[defn.noisewidth * defn.noiseheight];
	for (int y=0; y<defn.noiseheight; y++)
	{
		for (int x=0; x<defn.noisewidth; x++)
		{
			int noise = (noiseFn(x,  y,  random) -
				(INT_MAX / 2)) / 100000;
			noisemap[x + y * defn.noisewidth] = 
				fixed(true, noise) * defn.noisefactor;
		}
	}

	// Should perhaps lineraly interp, but smoothing is done afterwards
	for (int x=0; x<=hmap.getMapWidth(); x++)
	{
		if (counter) counter->setNewPercentage((100.0f * float(x)) / float(hmap.getMapWidth()));
		for (int y=0; y<=hmap.getMapHeight(); y++)
		{
			fixed nx = fixed(defn.noisewidth) * fixed(x) / fixed(hmap.getMapWidth());
			fixed ny = fixed(defn.noiseheight) * fixed(y) / fixed(hmap.getMapHeight());
			if (nx >= defn.noisewidth) nx = defn.noisewidth - 1;
			if (ny >= defn.noiseheight) ny = defn.noiseheight - 1;

			fixed height = hmap.getHeight(x, y);
			fixed newHeight = height + 
				noisemap[nx.asInt() + ny.asInt() * defn.noisewidth];
			newHeight = MAX(newHeight, 0);
			hmap.setHeight(x, y, newHeight);
		}
	}
}

void HeightMapModifier::edgeEnhance(HeightMap &hmap, 
	LandscapeDefnHeightMapGenerate &defn,
	RandomGenerator &generator,
	ProgressCounter *counter)
{
	if (counter) counter->setNewOp(LANG_RESOURCE("EDGE_ENHANCE", "Edge Enhance"));
	
	fixed *newhMap_ = new fixed[(hmap.getMapWidth()+1) * (hmap.getMapHeight()+1)];
	fixed *point = newhMap_;
	fixed max, pixel;
	for (int y=0; y<=hmap.getMapHeight(); y++)
	{
		if (counter) counter->setNewPercentage((100.0f * float(y)) / float(hmap.getMapHeight()));
		for (int x=0; x<=hmap.getMapWidth(); x++, point++)
        {
			fixed height = hmap.getHeight(x, y);
			if (x>0 && y>0 && x<hmap.getMapWidth() && y<hmap.getMapHeight())
			{
				max = (hmap.getHeight(x + 1, y + 1) - hmap.getHeight(x - 1, y - 1)).abs();
				
				pixel = (hmap.getHeight(x - 1, y + 1) - hmap.getHeight(x + 1, y - 1)).abs();
				if (pixel > max) max = pixel;

				pixel = (hmap.getHeight(x - 1, y) - hmap.getHeight(x + 1, y)).abs();
				if (pixel > max) max = pixel;

				pixel = (hmap.getHeight(x, y - 1) - hmap.getHeight(x, y + 1)).abs();
				if (pixel > max) max = pixel;

				*point = MAX(height, max + height);
			}
			else
			{
				*point = height;
			}
			*point = MAX(*point, 0);
        }
    }    

	fixed *start = newhMap_;
	for (int y=0; y<=hmap.getMapHeight(); y++)
	{
		for (int x=0; x<=hmap.getMapWidth(); x++)
		{
			hmap.setHeight(x, y, *(start++));
		}
	}
	delete [] newhMap_;
}

static void getPos(int pos, int &x, int &y, int dist)
{
	switch (pos)
	{
	case 0:
		x = -dist;
		y = -dist;
		break;
	case 1:
		x = dist;
		y = dist;
		break;
	case 2:
		x = dist;
		y = -dist;
		break;
	case 3:
		x = -dist;
		y = dist;
		break;
	case 4:
		x = -dist;
		y = 0;
		break;
	case 5:
		x = dist;
		y = 0;
		break;
	case 6:
		x = 0;
		y = dist;
		break;
	case 7:
		x = 0;
		y = -dist;
		break;
	}
}

void HeightMapModifier::waterErrosion(HeightMap &hmap, 
	LandscapeDefnHeightMapGenerate &defn,
	RandomGenerator &generator,
	ProgressCounter *counter)
{
	if (defn.errosions == 0) return;
	if (counter) counter->setNewOp(LANG_RESOURCE("WATER_ERROSION", "Water Errosion"));

	// Copy the height map, so we don't keep running down the same paths
	// as we have already created
	fixed *newhMap = new fixed[(hmap.getMapWidth()+1) * (hmap.getMapHeight()+1)];
	fixed *start = newhMap;
	for (int y=0; y<=hmap.getMapHeight(); y++)
	{
		for (int x=0; x<=hmap.getMapWidth(); x++, start++)
		{
			*start = hmap.getHeight(x, y);
		}
	}

	// Keep a count of how many paths have used the same squares
	int *seen = new int[(hmap.getMapWidth()+1) * (hmap.getMapHeight()+1)];
	memset(seen, 0, sizeof(int) * (hmap.getMapWidth()+1) * (hmap.getMapHeight()+1));

	// For each errosion stream
	for (int o=0; o<defn.errosions; o++) 
	{
		if (counter) counter->setNewPercentage((100.0f * float(o)) / float(30));

		// Choose a random start
		int x = generator.getRandUInt("HeightMapModifier") % hmap.getMapWidth();
		int y = generator.getRandUInt("HeightMapModifier") % hmap.getMapHeight();
		
		// Set the position and starting height
		int startx = x;
		int starty = y;
		fixed startHeight = hmap.getHeight(x, y);

		// For a set number of itterations
		int nx, ny;
		for (int i=0; i<500; i++)
		{
			// Find next position to be lowered
			fixed minHeight = fixed::MAX_FIXED;
			int distFromStart = (x - startx) * (x - startx) +
				(y - starty) * (y - starty);

			// Find the lowest square around the current square
			// this must be further away from the start than the current
			int minIndex = -1;
			for (int a=0; a<4; a++)
			{
				getPos(a, nx, ny, 1);
				if (x + nx >= 0 &&
					y + ny >= 0 &&
					x + nx <= hmap.getMapWidth() &&
					y + ny <= hmap.getMapHeight())
				{
					int newDistFromStart = 
						(x + nx - startx) * (x + nx - startx) +
						(y + ny - starty) * (y + ny - starty);
					if (newDistFromStart > distFromStart)
					{
						fixed newHeight = newhMap[x + nx + (y + ny) * (hmap.getMapWidth()+1)];
						if (newHeight < minHeight) 
						{
							minIndex = a;
							minHeight = newHeight;
						}
					}
				}
			}

			// Check if have found any, or if we have been here with too many streams
			if (minIndex == -1 || seen[x + y * (hmap.getMapWidth()+1)] > defn.errosionlayering) 
			{
				break;
			}
			seen[x + y * (hmap.getMapWidth()+1)]++;

			// Set new height, make sure it is lower than the start by a certain amount
			fixed lowered = fixed(i) * defn.errosionforce;
			lowered = MIN(lowered, defn.errosionmaxdepth);
			fixed currentheight = hmap.getHeight(x, y);
			startHeight = MIN(startHeight, currentheight);
			fixed newheight = MAX(startHeight - lowered, 0);
			hmap.setHeight(x, y, newheight);

			// Lower the surrounding area so its not so severe
			int size = 1;
			while (true)
			{
				fixed raised = fixed(size) * defn.errosionsurroundforce;
				for (int a=-size; a<=size; a++)
				{
					for (int b=-size; b<=size; b++)
					{
						if (a==-size || a==size || b==-size || b==size)
						{
							fixed newSurroundHeight = startHeight - lowered + raised + generator.getRandFixed("HeightMapModifier") * 2;
							newSurroundHeight = MAX(newSurroundHeight, 0);
							fixed oldSurroundHeight = hmap.getHeight(x + a, y + b);
							if (oldSurroundHeight > newSurroundHeight)
							{
								hmap.setHeight(x + a , y + b, newSurroundHeight);
							}
						}
					}
				}
				size++;
				if (size > defn.errosionsurroundsize) break;
			}

			// Move to the next x,y
			{
				getPos(minIndex, nx, ny, 1);
				x += nx;
				y += ny;
			}
		}
	}

	delete [] newhMap;
	delete [] seen;
}

void HeightMapModifier::smooth(HeightMap &hmap, 
							   LandscapeDefnHeightMapGenerate &defn,
							   ProgressCounter *counter)
{
	if (defn.landsmoothing == 0) return;
	if (counter) counter->setNewOp(LANG_RESOURCE("SMOOTING", "Smoothing"));

	fixed *newhMap_ = new fixed[(hmap.getMapWidth()+1) * (hmap.getMapHeight()+1)];

	fixed matrix[5][5];
	for (int i=0; i<5; i++)
	{
		for (int j=0; j<5; j++)
		{
			matrix[i][j] = fixed(defn.landsmoothing); // How much smoothing is done (> is more)
			if (i==2 && j==2) matrix[i][j] = fixed(1);
		}
	}

	int x;
	for (x=0; x<=hmap.getMapWidth(); x++)
	{
		if (counter) counter->setNewPercentage((100.0f * float(x)) / float(hmap.getMapWidth()));
		for (int y=0; y<=hmap.getMapHeight(); y++)
		{
			// Total is used to catch corner cases
			fixed total = fixed(0);
			fixed inc = fixed(0);
			for (int i=0; i<5; i++)
			{
				for (int j=0; j<5; j++)
				{
					int newi = i + x - 2;
					int newj = j + y - 2;
					if (newi>=0 && newi <= hmap.getMapWidth() &&
						newj>=0 && newj <= hmap.getMapHeight())
					{
						inc += matrix[i][j] * hmap.getHeight(newi, newj);
						total += matrix[i][j];
					}
				}
			}

			newhMap_[(hmap.getMapWidth()+1) * y + x] = inc / total;
		}
	}

	fixed *start = newhMap_;
	for (int y=0; y<=hmap.getMapHeight(); y++)
	{
		for (x=0; x<=hmap.getMapWidth(); x++)
		{
			hmap.setHeight(x, y, *(start++));
		}
	}
	delete [] newhMap_;
}

void HeightMapModifier::scale(HeightMap &hmap, 
							  LandscapeDefnHeightMapGenerate &defn,
							  RandomGenerator &generator, 
							  ProgressCounter *counter)
{
	if (counter) counter->setNewOp(LANG_RESOURCE("SCALING_PHASE_1", "Scaling Phase 1"));

	fixed max = hmap.getHeight(0,0);
	fixed min = hmap.getHeight(0,0);

	int x;
	for (x=0; x<=hmap.getMapWidth(); x++)
	{
		if (counter) counter->setNewPercentage((100.0f * float(x)) / float(hmap.getMapWidth()));
		for (int y=0; y<=hmap.getMapHeight(); y++)
		{
			if (hmap.getHeight(x,y) > max) max = hmap.getHeight(x,y);
			if (hmap.getHeight(x,y) < min) min = hmap.getHeight(x,y);
		}
	}

	if (counter) counter->setNewOp(LANG_RESOURCE("SCALING_PHASE_2", "Scaling Phase 2"));

	fixed realMax = ((fixed(defn.landheightmax) - fixed(defn.landheightmin)) * generator.getRandFixed("HeightMapModifier")) + 
		defn.landheightmin;
	fixed per = realMax / max;

	for (x=0; x<=hmap.getMapWidth(); x++)
	{
		if (counter) counter->setNewPercentage((100.0f * float(x)) / float(hmap.getMapWidth()));
		for (int y=0; y<=hmap.getMapHeight(); y++)
		{
			fixed height = hmap.getHeight(x,y);
			hmap.setHeight(x,y, height * per);
		}
	}
}

void HeightMapModifier::addCirclePeak(HeightMap &hmap, FixedVector &start, 
									  fixed sizew, fixed sizew2, fixed sizeh,
									  RandomGenerator &offsetGenerator)
{
	fixed maxdist = MAX(sizew2, sizew);
	fixed sizewsq = sizew * sizew * fixed(true, 12000);
	int startx = MAX(0, (start[0] - maxdist).asInt());
	int starty = MAX(0, (start[1] - maxdist).asInt());
	int endx = MIN(hmap.getMapWidth(), (start[0] + maxdist).asInt());
	int endy = MIN(hmap.getMapHeight(), (start[1] + maxdist).asInt());

	fixed posX, posY;
	fixed pt2 = fixed(1) / fixed(5);
	for (int x=startx; x<=endx; x++)
	{
		for (int y=starty; y<=endy; y++)
		{
			posX = fixed(x);
			posY = fixed(y);

			fixed distX = (posX - start[0]) * sizew2 / sizew;
			fixed distY = posY - start[1];
			fixed distsq = distX * distX + distY * distY;

			if (distsq < sizewsq)
			{
				fixed dist = distsq.sqrt();
				fixed distRand = (dist / fixed(20)) * pt2;
				if (distRand > pt2) distRand = pt2;
				dist *= offsetGenerator.getRandFixed("HeightMapModifier") * distRand + fixed(1) - (distRand / fixed(2));
				if (dist < sizew)
				{
					fixed newHeight = (dist * fixed::XPI / sizew).cos() * sizeh / fixed(4) + sizeh / fixed(4);
					newHeight += hmap.getHeight(x,y);
					hmap.setHeight(x, y, newHeight);
				}
			}
		}
	}
}

void HeightMapModifier::generateTerrain(HeightMap &hmap, 
										LandscapeDefnHeightMapGenerate &defn,
										RandomGenerator &generator,
										RandomGenerator &offsetGenerator,
										ProgressCounter *counter)
{
	if (counter) counter->setNewOp(LANG_RESOURCE("TERAFORM_LANDSCAPE", "Teraform Landscape"));

	// Create a default mask that allows everything
	Image bmap(256, 256);
	Image maskMap = bmap;

	// Check if we need to load a new mask
	if (!defn.mask.empty())
	{
		maskMap = ImageFactory::loadImage(S3D::eModLocation, defn.mask);
		if (!maskMap.getBits())
		{
			S3D::dialogExit("Landscape", S3D::formatStringBuffer(
				"Error: Failed to find mask map \"%s\"",
				defn.mask.c_str()));
		}
		if (!maskMap.getLossless())
		{
			S3D::dialogExit("Landscape", S3D::formatStringBuffer(
				"Error: Mask map \"%s\" is not a lossless image format",
				defn.mask.c_str()));
		}
	}

	// Generate the landscape
	hmap.reset();
	fixed useWidthX = fixed(hmap.getMapWidth()); // Use all the width
	fixed useWidthY = fixed(hmap.getMapHeight());
	fixed useBorderX = (fixed(hmap.getMapWidth()) - useWidthX) / fixed(2);
	fixed useBorderY = (fixed(hmap.getMapHeight()) - useWidthY) / fixed(2);
	fixed maskMultX = fixed(maskMap.getWidth()) / fixed(hmap.getMapWidth());
	fixed maskMultY = fixed(maskMap.getHeight()) / fixed(hmap.getMapHeight());

	const int noItter = (fixed(defn.landhillsmax - defn.landhillsmin) *
		generator.getRandFixed("HeightMapModifier") + fixed(defn.landhillsmin)).asInt();

	for (int i=0; i<noItter; i++)
	{
		if (counter) counter->setNewPercentage((100.0f * float(i)) / float(noItter));

		// Choose settings for a random hemisphere
		fixed sizew = (fixed(defn.landpeakwidthxmax) - fixed(defn.landpeakwidthxmin)) * generator.getRandFixed("HeightMapModifier") 
			+ fixed(defn.landpeakwidthxmin);
		fixed sizew2 = (fixed(defn.landpeakwidthymax) - fixed(defn.landpeakwidthymin)) * generator.getRandFixed("HeightMapModifier") 
			+ fixed(defn.landpeakwidthymin) + sizew;
		fixed sizeh = ((fixed(defn.landpeakheightmax) - fixed(defn.landpeakheightmin)) * generator.getRandFixed("HeightMapModifier") 
					   + defn.landpeakheightmin) * MAX(sizew, sizew2);

		// Choose a border around this hemisphere
		fixed bordersizex = fixed(0);
		fixed bordersizey = fixed(0);
		if (defn.levelsurround)
		{
			fixed bordersize = MAX(sizew, sizew2) * fixed(true, 12000); // 1.2
			bordersizex = bordersize + useBorderX;
			bordersizey = bordersize + useBorderY;
		}

		// Choose a point for this hemisphere
		fixed sx = (generator.getRandFixed("HeightMapModifier") * (fixed(hmap.getMapWidth()) - 
												(bordersizex * fixed(2)))) + bordersizex;
		fixed sy = (generator.getRandFixed("HeightMapModifier") * (fixed(hmap.getMapHeight()) - 
												(bordersizey * fixed(2)))) + bordersizey;

		// Check if the point passes the mask
		bool ok = true;
		if (sx.asInt() >= 0 && sx.asInt() < hmap.getMapWidth() &&
			sy.asInt() >= 0 && sy.asInt() < hmap.getMapHeight())
		{
			int bx = (sx * maskMultX).asInt();
			int by = maskMap.getWidth() - (sy * maskMultY).asInt();
			if (bx >= 0 && bx < maskMap.getWidth() && 
				by >= 0 && by < maskMap.getHeight())
			{
			unsigned char maskPt = maskMap.getBits()[(bx * 3) + (by * maskMap.getWidth() * 3)];

			//printf("%i %i %i %s\n", maskPt, bx, by, defn.mask.c_str());
			ok = ((generator.getRandFixed("HeightMapModifier") * fixed(255)) < fixed(maskPt));
			}
		}

		if (ok)
		{
			// Actually add the hemisphere
			FixedVector start(sx, sy, fixed(0));
			addCirclePeak(hmap, start, sizew, sizew2, sizeh, offsetGenerator);
		}
	}

	scale(hmap, defn, generator, counter);
	noise(hmap, defn, generator, counter);
	//edgeEnhance(hmap, defn, generator, counter);
	waterErrosion(hmap, defn, generator, counter);
	if (defn.levelsurround) levelSurround(hmap);
	smooth(hmap, defn, counter);
	if (defn.levelsurround) levelSurround(hmap);
}

