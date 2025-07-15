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

#if !defined(__INCLUDE_SaveDialogh_INCLUDE__)
#define __INCLUDE_SaveDialogh_INCLUDE__

#include <GLW/GLWWindow.h>
#include <GLW/GLWButton.h>
#include <GLW/GLWTextBox.h>

class SaveDialog : public GLWWindow,
				public GLWButtonI
{
public:
	static SaveDialog *instance();

	virtual void buttonDown(unsigned int id);
	virtual void display();

protected:
	unsigned int okId_, cancelId_;
	static SaveDialog *instance_;
	GLWTextBox *textBox_;

private:
	SaveDialog();
	virtual ~SaveDialog();

};


#endif
