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

#ifndef _GLWIconList_h
#define _GLWIconList_h

#include <GLW/GLWScrollWBackwards.h>
#include <GLW/GLWToolTip.h>
#include <vector>

class GLWIconListI
{
public:
	virtual void selected(unsigned int id, int position) = 0;
	virtual void chosen(unsigned int id, int position) = 0;
};

class GLWIconListItem
{
public:
	virtual void draw(float x, float y, float w) = 0;
};

class GLWIconList : public GLWidget
{
public:
	enum Flags
	{
		eNoDrawSelected = 1
	};

	GLWIconList(float x = 0.0f, float y = 0.0f, 
		float w = 0.0f, float h = 0.0f,
		float squaresHeight = 40.0f,
		unsigned int flags = 0);
	virtual ~GLWIconList();

	void addItem(GLWIconListItem *item);
	void clear();

	void setFlags(unsigned int flags) { flags_ = flags; }
	void setHandler(GLWIconListI *handler) { handler_ = handler; }

	GLWIconListItem *getSelected();
	std::vector<GLWIconListItem *> &getItems() { return items_; }

	// Inhertied from GLWidget
	virtual void draw();
	virtual void simulate(float frameTime);
	virtual void mouseDown(int button, float x, float y, bool &skipRest);
	virtual void mouseUp(int button, float x, float y, bool &skipRest);
	virtual void mouseDrag(int button, float mx, float my, float x, float y, 
						   bool &skipRest);
	virtual void mouseWheel(float x, float y, float z, bool &skipRest);

	REGISTER_CLASS_HEADER(GLWIconList);
protected:
	GLWScrollWBackwards scrollBar_;
	GLWIconListI *handler_;
	unsigned int flags_;
	float squaresHeight_;
	int selected_;
	std::vector<GLWIconListItem *> items_;

private:
	GLWIconList(const GLWIconList &);
	const GLWIconList & operator=(const GLWIconList &);
};

#endif // _GLWIconList_h

