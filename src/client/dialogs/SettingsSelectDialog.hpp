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

#if !defined(__INCLUDE_SettingsSelectDialogh_INCLUDE__)
#define __INCLUDE_SettingsSelectDialogh_INCLUDE__

#include <GLW/GLWWindow.h>
#include <GLW/GLWTextButton.h>
#include <GLW/GLWOptionEntry.h>
#include <common/OptionsGame.h>

class SettingsSelectDialog : 
	public GLWWindow,
	public GLWButtonI
{
public:
	static SettingsSelectDialog *instance();

	OptionsGame &getOptions() { return options_; }

	// GLWButtonI
	virtual void buttonDown(unsigned int id);

	// GLWWindow
	virtual void display();

protected:
	static SettingsSelectDialog *instance_;

	std::list<GLWOptionEntry> controls_;
	OptionsGame options_;
	unsigned int cancelId_, okId_, advancedId_;

private:
	SettingsSelectDialog();
	virtual ~SettingsSelectDialog();

};

#endif
