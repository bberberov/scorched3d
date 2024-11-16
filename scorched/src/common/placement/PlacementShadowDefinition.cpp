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

#include <placement/PlacementShadowDefinition.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscapemap/DeformLandscape.h>
#include <common/Defines.h>
#include <engine/ScorchedContext.h>
#include <image/ImageStore.h>

PlacementShadowDefinition::PlacementShadowDefinition() :
	drawShadow_(true), flattenArea_(0)
{
}

PlacementShadowDefinition::~PlacementShadowDefinition()
{
}

bool PlacementShadowDefinition::readXML(XMLNode *node)
{
	node->getNamedChild("drawshadow", drawShadow_, false);
	node->getNamedChild("flattenarea", flattenArea_, false);

	XMLNode *groundMap = 0;
	if (node->getNamedChild("groundmap", groundMap, false))
	{
		if (!groundMap_.initFromNode(groundMap)) return false;
	}

	return true;
}

void PlacementShadowDefinition::updateLandscapeHeight(
	ScorchedContext &context,
	FixedVector &position, FixedVector &size)
{
	if (flattenArea_ != 0)
	{
		fixed areaSize = flattenArea_;
		if (areaSize < 0)
		{
			areaSize = MAX(size[0], size[1])/2 + 1;
		}

		DeformLandscape::flattenArea(context, position, false, areaSize);
	}
}

#ifndef S3D_SERVER

#include <GLEXT/GLImageModifier.h>
void PlacementShadowDefinition::updateLandscapeTexture(
	bool useShadows,
	ScorchedContext &context,
	FixedVector &position, FixedVector &size)
{
	if (!groundMap_.getImageName().empty())
	{
		Image image = ImageStore::instance()->loadImage(groundMap_);
		ImageModifier::addBitmapToLandscape(
			context,
			image,
			position[0].asFloat(), 
			position[1].asFloat(),
			0.25f, 0.25f);
	}

	if (drawShadow_ && useShadows)
	{
		ImageModifier::addCircleToLandscape(
			context,
			position[0].asFloat(), 
			position[1].asFloat(), 
			MAX(size[0], size[1]).asFloat(), 1.0f);
	}
}

#endif //S3D_SERVER
