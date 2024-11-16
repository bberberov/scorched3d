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

#if !defined(__INCLUDE_PlayerDialogh_INCLUDE__)
#define __INCLUDE_PlayerDialogh_INCLUDE__

#include <GLW/GLWWindow.h>
#include <GLW/GLWTextButton.h>
#include <GLW/GLWTextBox.h>
#include <GLW/GLWImageList.h>
#include <GLW/GLWLabel.h>
#include <GLW/GLWDropDownColor.h>
#include <GLEXT/GLTexture.h>
#include <GLW/GLWTankViewer.h>

class Tank;
class PlayerDialog : public GLWWindow,
	public GLWButtonI, public GLWDropDownI
{
public:
	void initialize();

	// Inherited from GLWWindow
	virtual void keyDown(char *buffer, unsigned int keyState, 
		KeyboardHistory::HistoryElement *history, int hisCount, 
		bool &skipRest);

	// Inherited from GLWButtonI
	virtual void buttonDown(unsigned int id);

	// GLWDropDownI
	virtual void select(unsigned int id, const int pos, 
		GLWSelectorEntry value);

protected:
	PlayerDialog(const std::string &name, 
		unsigned int states, 
		const std::string &description);
	virtual ~PlayerDialog();

	GLWDropDownText *aiTypeDropDown_;
	GLWDropDownText *teamDropDown_;
	GLWDropDownText *tankTypeDropDown_;
	GLWDropDownColor *colorDropDown_;
	GLWLabel *colorLabel_;
	GLWLabel *teamLabel_;
	GLWTankViewer *viewer_;
	GLWTextBox *playerName_;
	GLWImageList *imageList_;
	ToolTip avatarTip1_;
	ToolTip humanToolTip_;
	GLTexture colorTexture_;
	GLWTextButton *cancelButton_;
	GLWTextButton *spectateButton_;
	GLWTextButton *okButton_;

	virtual void okButton(bool spectate) = 0;
	virtual void cancelButton() = 0;

};

#endif
