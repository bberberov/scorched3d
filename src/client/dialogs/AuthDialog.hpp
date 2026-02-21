////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011, 2026
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

#ifndef __INCLUDE_AuthDialog_hpp_INCLUDE__
#define __INCLUDE_AuthDialog_hpp_INCLUDE__

#include <GLW/GLWWindow.hpp>
#include <GLW/GLWButton.hpp>
#include <GLW/GLWTextBox.hpp>

// SINGLETON
class AuthDialog : public GLWWindow, public GLWButtonI
{
public:
	static AuthDialog *instance();

	enum AuthRequired
	{
		eNameRequired = 1,
		ePasswordRequired = 2
	};

	// Inherited from GLWButtonI
	virtual void buttonDown(unsigned int id);

	// Inherited from GLWWindow
	virtual void display();

	void setRequiredAuth(unsigned int auth) { auth_ = auth; }

protected:
	static AuthDialog *instance_;

	unsigned int okId_, cancelId_;
	unsigned int auth_;

	GLWTextBox *username_;
	GLWTextBox *password_;

private:
	AuthDialog();
	virtual ~AuthDialog();
};

#endif // __INCLUDE_AuthDialog_hpp_INCLUDE__
