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

#if !defined(__INCLUDE_TreeModelFactoryh_INCLUDE__)
#define __INCLUDE_TreeModelFactoryh_INCLUDE__

#include <3dsparse/Model.h>

class TreeModelFactory
{
public:
	enum TreeType
	{
		eNone,
		ePineNormal,
		ePineBurnt,
		ePineYellow,
		ePineLight,
		ePineSnow,
		ePine2,
		ePine3,
		ePine4,
		ePine2Snow,
		ePine3Snow,
		ePine4Snow,
		ePalmNormal,
		ePalmBurnt,
		ePalm2,
		ePalm3,
		ePalm4,
		ePalmB,
		ePalmB2,
		ePalmB3,
		ePalmB4,
		ePalmB5,
		ePalmB6,
		ePalmB7,
		eOak,
		eOak2,
		eOak3,
		eOak4
	};

	TreeModelFactory();
	virtual ~TreeModelFactory();

	Model *createModel(const char *fileName);

	static bool getTypes(const char *type, bool snow, 
		TreeType &normalType, TreeType &burntType);

protected:

};

#endif // __INCLUDE_TreeModelFactoryh_INCLUDE__
