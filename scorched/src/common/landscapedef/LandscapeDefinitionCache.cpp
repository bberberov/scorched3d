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

#include <landscapedef/LandscapeDefinitionCache.h>
#include <landscapedef/LandscapeDefinition.h>
#include <landscapedef/LandscapeDefinitions.h>
#include <engine/ScorchedContext.h>
#include <limits.h>

LandscapeDefinitionCache::LandscapeDefinitionCache() : 
	cachedTex_(0), cachedDefn_(0)
{
}

LandscapeDefinitionCache::~LandscapeDefinitionCache()
{
}

void LandscapeDefinitionCache::setDefinition(
	ScorchedContext &context, LandscapeDefinition &defn)
{
	defn_ = defn;
	bool load = (defn.getDefinitionNumber() == UINT_MAX);
	cachedTex_ = context.getLandscapes().getTex(defn_.getTex(), load);
	cachedDefn_ = context.getLandscapes().getDefn(defn_.getDefn(), load);
	if (!cachedTex_ || !cachedDefn_)
	{
		S3D::dialogExit("LandscapeMaps", S3D::formatStringBuffer(
			"ERROR: Failed to find a tex \"%s\" or defn \"%s\"",
			defn_.getTex(), defn_.getDefn()));
	}
}
