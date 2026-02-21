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

#ifndef __INCLUDE_Main2DCamera_hpp_INCLUDE__
#define __INCLUDE_Main2DCamera_hpp_INCLUDE__

// Main2DCamera.hpp: interface for the Main2DCamera class.
//
//////////////////////////////////////////////////////////////////////

#include <engine/GameStateI.hpp>
#include <GLEXT/GLViewPort.hpp>

// SINGLETON
class Main2DCamera : public GameStateI
{
public:
	static Main2DCamera *instance();

	virtual void draw(const unsigned state);

	GLViewPort &getViewPort() { return viewPort_; }
	bool getHide() { return hide_; }
	void setHide(bool hide) { hide_ = hide; }

protected:
	static Main2DCamera *instance_;

	GLViewPort viewPort_;
	bool hide_;

private:
	Main2DCamera();
	virtual ~Main2DCamera();
};

#endif // __INCLUDE_Main2DCamera_hpp_INCLUDE__
