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

#ifndef _GLWIconTable_h
#define _GLWIconTable_h

#include <GLW/GLWScrollWBackwards.h>
#include <GLW/GLWTextButton.h>
#include <GLW/GLWToolTip.h>
#include <vector>
#include <list>

class GLWIconTableI
{
public:
	virtual void drawColumn(unsigned int id, int row, int column, float x, float y, float w) = 0;
	virtual void rowSelected(unsigned int id, int row) = 0;
	virtual void columnSelected(unsigned int id, int col) = 0;
	virtual void rowChosen(unsigned int id, int row) = 0;
};

class GLWIconTable : 
	public GLWidget,
	public GLWButtonI
{
public:
	struct Column
	{
		Column(const LangString &name_ = LangString(), float width_ = 0.0f) :
			name(name_), width(width_)
			{
			}

		LangString name;
		float width;
	};

	GLWIconTable(float x = 0.0f, float y = 0.0f, 
		float w = 0.0f, float h = 0.0f,
		std::list<Column> *columns = 0,
		float rowHeight = 20.0f);
	virtual ~GLWIconTable();

	void setItemCount(int items);
	int getItemCount() { return itemCount_; }

	void setHandler(GLWIconTableI *handler) { handler_ = handler; }
	int getSelected() { return selected_; }

	// Inhertied from GLWidget
	virtual void draw();
	virtual void simulate(float frameTime);
	virtual void mouseDown(int button, float x, float y, bool &skipRest);
	virtual void mouseUp(int button, float x, float y, bool &skipRest);
	virtual void mouseDrag(int button, float mx, float my, float x, float y, 
						   bool &skipRest);
	virtual void mouseWheel(float x, float y, float z, bool &skipRest);

	// GLWButtonI
	virtual void buttonDown(unsigned int id);

	REGISTER_CLASS_HEADER(GLWIconTable);
protected:
	std::vector<GLWTextButton *> columns_;
	GLWScrollWBackwards scrollBar_;
	GLWIconTableI *handler_;
	float rowHeight_;
	int selected_;
	int itemCount_;

private:
	GLWIconTable(const GLWIconTable &);
	const GLWIconTable & operator=(const GLWIconTable &);
};

#endif // _GLWIconTable_h

