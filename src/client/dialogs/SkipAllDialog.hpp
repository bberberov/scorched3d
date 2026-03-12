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

#ifndef __INCLUDE_SkipAllDialog_hpp_INCLUDE__
#define __INCLUDE_SkipAllDialog_hpp_INCLUDE__

#include <GLW/GLWWindow.hpp>
#include <GLW/GLWButton.hpp>
#include <GLW/GLWLabel.hpp>

// SINGLETON
class SkipAllDialog : public GLWWindow, public GLWButtonI
{
public:
	static SkipAllDialog* instance();

	virtual void buttonDown( unsigned int id );
	virtual void windowInit( const unsigned int state );
	virtual void simulate( float frameTime );
	virtual void display();

protected:
	static SkipAllDialog* instance_;

	unsigned int cancelId_, nowId_;
	GLWLabel*    label_;
	unsigned int startTime_;
	bool         skipAll_;

private:
	SkipAllDialog();
	virtual ~SkipAllDialog();
};

#endif  // __INCLUDE_SkipAllDialog_hpp_INCLUDE__
