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

#if !defined(__INCLUDE_SettingsSubSelectDialogh_INCLUDE__)
#define __INCLUDE_SettingsSubSelectDialogh_INCLUDE__

#include <GLW/GLWWindow.h>
#include <GLW/GLWTextButton.h>
#include <GLW/GLWCheckBox.h>
#include <GLW/GLWOptionEntry.h>
#include <GLW/GLWIconList.h>
#include <GLW/GLWTab.h>
#include <GLW/GLWIcon.h>
#include <common/ToolTip.h>
#include <common/OptionsGame.h>

class SettingsSubSelectDialogListItem : public GLWIconListItem
{
public:
	SettingsSubSelectDialogListItem(
		const char *icon, 
		const char *name, const char *description,
		bool selected);
	virtual ~SettingsSubSelectDialogListItem();

	const char *getName() { return name_.c_str(); }
	bool getSelected() { return selected_.getState(); }
	void setSelected(bool selected) { selected_.setState(selected); }

	// GLWIconListItem
	virtual void draw(float x, float y, float w);

protected:
	std::string name_;
	GLWIcon icon_;
	GLWCheckBox selected_;
	ToolTip tip_;
};

class SettingsSubSelectDialog : 
	public GLWWindow,
	public GLWButtonI,
	public GLWIconListI
{
public:
	static SettingsSubSelectDialog *instance();

	// GLWButtonI
	virtual void buttonDown(unsigned int id);

	// GLWWindow
	virtual void display();

	// GLWIconListI
	virtual void selected(unsigned int id, int position);
	virtual void chosen(unsigned int id, int position);

protected:
	static SettingsSubSelectDialog *instance_;

	GLWTab *mainTab_, *moneyTab_, *weaponsTab_;
	GLWTab *scoreTab_, *envTab_, *landTab_;
	GLWIconList *landList_;
	std::list<GLWOptionEntry> controls_;
	unsigned int cancelId_, okId_, advancedId_;
	unsigned int selectAllId_, selectNoneId_;

	void displayLand();

private:
	SettingsSubSelectDialog();
	virtual ~SettingsSubSelectDialog();

};

#endif
