////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2004, 2025, 2026
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
//    You should have received a copy of the GNU General Public License
//    along with Scorched3D; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

#ifndef __INCLUDE_OptionEntrySetter_hpp_INCLUDE__
#define __INCLUDE_OptionEntrySetter_hpp_INCLUDE__

#include <common/OptionEntry.hpp>
#include <list>

// NOTE: avoid WX warnings
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wabi-tag"
#pragma GCC diagnostic ignored "-Winline"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wpadded"
#pragma GCC diagnostic ignored "-Wredundant-tags"
#pragma GCC diagnostic ignored "-Wsuggest-final-methods"
#pragma GCC diagnostic ignored "-Wswitch-default"
#pragma GCC diagnostic ignored "-Wunused-const-variable"
#pragma GCC diagnostic ignored "-Wuseless-cast"

#include <wx/wx.h>

#pragma GCC diagnostic pop

class OptionEntrySetter
{
public:
	OptionEntrySetter( wxControl* control, OptionEntry& entry );
	virtual ~OptionEntrySetter();

	wxControl*   getControl();
	OptionEntry& getEntry();

protected:
	wxControl*   control_;
	OptionEntry& entry_;
};

class OptionEntrySetterUtil
{
public:
	static OptionEntrySetter createOtherSetter( wxWindow* parent, wxSizer* topsizer, OptionEntry& entry );

	static void updateControls( std::list<OptionEntrySetter>& controls );
	static void updateEntries( std::list<OptionEntrySetter>& controls );
};

#endif  // __INCLUDE_OptionEntrySetter_hpp_INCLUDE__
