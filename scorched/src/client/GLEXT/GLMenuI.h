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


// GLMenuI.h: interface for the GLMenuI class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLMENUI_H__7171A46C_8265_4A9F_A984_889EF2547CB1__INCLUDED_)
#define AFX_GLMENUI_H__7171A46C_8265_4A9F_A984_889EF2547CB1__INCLUDED_

#include <list>
#include <string>
#include <GLW/GLWToolTip.h>
#include <lang/LangString.h>

class GLTextureBase;
class GLMenuItem
{
public:
	GLMenuItem(const LangString &text, 
		ToolTip *tooltip = 0, bool selected = false, 
		GLTextureBase *texture = 0,
		void *userData = 0);

	const LangString &getText() { return menuText_; }
	ToolTip *getToolTip() { return tip_; }
	GLTextureBase *getTexture() { return texture_; }
	bool getSelected() { return selected_; }
	void *getUserData() { return userData_; }
	bool getSeperator() { return seperator_; }
	void setSeperator() { seperator_ = true; }

protected:
	LangString menuText_;
	ToolTip *tip_;
	GLTextureBase *texture_;
	bool selected_, seperator_;
	void *userData_;
};

class GLMenuI  
{
public:
	virtual ~GLMenuI();

	virtual void menuSelection(const char* menuName, const int position, GLMenuItem &item);
	virtual LangString *getMenuText(const char* menuName);
	virtual bool getEnabled(const char* menuName);
	virtual bool getMenuItems(const char* menuName, std::list<GLMenuItem> &result);
	virtual bool menuOpened(const char* menuName);
	virtual LangStringStorage *getMenuToolTip(const char* menuName);
};

#endif // !defined(AFX_GLMENUI_H__7171A46C_8265_4A9F_A984_889EF2547CB1__INCLUDED_)
