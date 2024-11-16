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

#if !defined(__INCLUDE_GLWSelectorParth_INCLUDE__)
#define __INCLUDE_GLWSelectorParth_INCLUDE__

#include <GLW/GLWSelector.h>

class GLWSelectorPart
{
public:
	GLWSelectorPart(GLWSelectorI *user,
		int basePosition,
		float x, float y,
		std::list<GLWSelectorEntry> &entries,
		bool transparent,
		GLWSelectorPart *parent,
		int parentPosition);
	virtual ~GLWSelectorPart();

	void draw();
	void mouseDown(float x, float y, bool &hit);

	// Links the SelectorParts together for popups
	GLWSelectorPart *getParent() { return parent_; }
	GLWSelectorPart *getChild() { return child_; }
	int getParentPosition() { return parentPosition_; }
	
	float getSelectedHeight() { return selectedHeight_; }
	float getSelectedWidth() { return selectedWidth_; }

protected:
	GLWSelectorI *user_;
	std::list<GLWSelectorEntry> entries_;
	float selectedHeight_, selectedWidth_;
	float selectedX_, selectedY_;
	float selectedIndent_;
	int basePosition_;
	bool transparent_;
	bool hasSelectedEntry_, hasPopupEntry_;
	GLWSelectorPart *parent_; // If this is popup it will have a parent
	GLWSelectorPart *child_; // If this has a popup it may have a child
	int parentPosition_;

	void calculateDimensions(float x, float y);

};

#endif
