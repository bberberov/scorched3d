////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011, 2026
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

#ifndef __INCLUDE_LandscapeDefinitions_hpp_INCLUDE__
#define __INCLUDE_LandscapeDefinitions_hpp_INCLUDE__

#include <landscapedef/LandscapeDefinitionsBase.hpp>
#include <landscapedef/LandscapeDefinitionsItem.hpp>
#include <landscapedef/LandscapeDefinition.hpp>

class LandscapeDefinitions;
class LandscapeDefn;
class LandscapeTex;
class LandscapeInclude;
class OptionsGame;
class TargetContainer;

class LandscapeDefinitions : public LandscapeDefinitionsBase
{
public:
	LandscapeDefinitions();
	virtual ~LandscapeDefinitions();

	virtual bool readLandscapeDefinitions();
	virtual void clearLandscapeDefinitions();

	void                checkEnabled( OptionsScorched& context );
	LandscapeDefinition getBlankLandscapeDefn();
	LandscapeDefinition getRandomLandscapeDefn( OptionsScorched& context, TargetContainer& TargetContainer );
	LandscapeTex*       getTex( const char* file, bool load = false );
	LandscapeDefn*      getDefn( const char* file, bool load = false );
	LandscapeInclude*   getInclude( const char* file, bool load = false );

protected:
	static unsigned int          lastDefinitionNumber_;
	LandscapeDefinitionsEntry*   lastDefinition_;
	LandscapeDefinitionsEntry    blankDefinition_;
	std::map< std::string, int > usedFiles_;

	LandscapeDefinitionsItem< LandscapeTex >     texs_;
	LandscapeDefinitionsItem< LandscapeDefn >    defns_;
	LandscapeDefinitionsItem< LandscapeInclude > include_;

	const std::string          getLeastUsedFile( OptionsScorched& context, std::vector< std::string >& files );
	LandscapeDefinitionsEntry* getRandomLandscapeDefnEntry(
		OptionsScorched&                        context,
		std::list< LandscapeDefinitionsEntry* > passedLandscapes
	);
	bool readLandscapeDefinitionsEntry( LandscapeDefinitionsEntry& entry );
};

#endif  // __INCLUDE_LandscapeDefinitions_hpp_INCLUDE__
