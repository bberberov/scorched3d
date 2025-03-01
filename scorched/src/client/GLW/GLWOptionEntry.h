////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011, 2025
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

#if !defined(__INCLUDE_GLWOptionEntryh_INCLUDE__)
#define __INCLUDE_GLWOptionEntryh_INCLUDE__

#include <common/OptionEntry.h>
#include <GLW/GLWidget.h>
#include <list>

class GLWOptionEntry
{
public:
	GLWOptionEntry(GLWidget *control, OptionEntry *entry);
	virtual ~GLWOptionEntry();

	static void createEntry( std::list<GLWOptionEntry> &controls, GLWPanel *parent, OptionEntry &entry);

	static void updateControls(std::list<GLWOptionEntry> &controls);
	static void updateEntries(std::list<GLWOptionEntry> &controls);

	OptionEntry *getEntry() { return entry_; }
	GLWidget *getControl() { return control_; }

protected:
	GLWidget *control_;
	OptionEntry *entry_;
};

#endif // __INCLUDE_GLWOptionEntryh_INCLUDE__
