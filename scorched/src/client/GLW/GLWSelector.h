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

#if !defined(__INCLUDE_GLWSelectorh_INCLUDE__)
#define __INCLUDE_GLWSelectorh_INCLUDE__

#include <GLW/GLWWindow.h>
#include <GLW/GLWToolTip.h>
#include <GLEXT/GLTexture.h>
#include <lang/LangString.h>
#include <string>
#include <list>
#include <map>

/**
Defines the contents of one row of the selection dialog.
*/
class GLWSelectorEntry
{
public:
	GLWSelectorEntry(const LangString &text = LangString(), 
		ToolTip *tooltip = 0, 
		bool selected = false,
		GLTextureBase *icon = 0,
		void *userData = 0,
		const std::string &dataText = "");
	
	LangString &getText() { return text_; }
	const char *getDataText() { return dataText_.c_str(); }
	ToolTip *getToolTip() { return tip_; }
	GLTextureBase *getIcon() { return icon_; }
	bool getSelected() { return selected_; }
	bool getSeperator() { return seperator_; }
	void setSeperator() { seperator_ = true; }
	void *getUserData() { return userData_; }
	Vector &getColor() { return color_; }
	int &getTextureWidth() { return textureWidth_; }

	std::list<GLWSelectorEntry> &getPopups() { return popups_; }
	
protected:
	LangString text_;
	std::string dataText_;
	GLTextureBase *icon_;
	int textureWidth_;
	ToolTip *tip_;
	Vector color_;
	bool selected_, seperator_;
	void *userData_;
	std::list<GLWSelectorEntry> popups_;
};

/**
The user interested in the chosen selection.
*/
class GLWSelectorI
{
public:
	virtual void itemSelected(GLWSelectorEntry *entry, int position) = 0;
	virtual void noItemSelected() { };
};

/**
A class that presents the user with an on screen menu and
allows them to select one item.
This class is used by other classes that throw up a selection
window to the user.
*/
class GLWSelectorPart;
class GLWSelector : public GLWWindow
{
public:
    static GLWSelector *instance();

	// Show the selector as the specified position
	void showSelector(
		GLWSelectorI *user,
		float x, float y,
		std::list<GLWSelectorEntry> &entries,
		unsigned int showState = 0,
		bool transparent = true);
	// Hide the selector
	void hideSelector();

	// Add/remove a popup
	GLWSelectorI *getUser() { return user_; }
	void addPart(GLWSelectorPart *part);
	void rmPart(GLWSelectorPart *part);

	// Inherited from GLWWindow
	virtual void draw();
	virtual void mouseDown(int button, float x, float y, bool &skipRest);
	virtual void mouseUp(int button, float x, float y, bool &skipRest);
	virtual void mouseDrag(int button, float mx, float my, float x, float y, bool &skipRest);
	virtual void keyDown(char *buffer, unsigned int keyState, 
		KeyboardHistory::HistoryElement *history, int hisCount, 
		bool &skipRest);

protected:
	static GLWSelector *instance_;
	std::list<GLWSelectorPart *> parts_;
	unsigned int showState_;
	GLWSelectorI *user_;

private:
	GLWSelector();
	virtual ~GLWSelector();

};

#endif
