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

#if !defined(__INCLUDE_ToolTipResourceh_INCLUDE__)
#define __INCLUDE_ToolTipResourceh_INCLUDE__

#include <common/ToolTip.h>

class ToolTipResource : public ToolTip
{
public:
	ToolTipResource(unsigned int type, 
		const std::string &titleKey, const std::string &titleValue,
		const std::string &textKey, const std::string &textValue);
	virtual ~ToolTipResource();

	// Called just before the ToolTipResource is shown
	// can be used to dynamically populate the title and text fields
	virtual void populate();

protected:
	std::string titleKey_, titleValue_;
	std::string textKey_, textValue_;
};
#endif // __INCLUDE_ToolTipResourceh_INCLUDE__
