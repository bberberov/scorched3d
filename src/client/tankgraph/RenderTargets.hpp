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

#ifndef __INCLUDE_RenderTargets_hpp_INCLUDE__
#define __INCLUDE_RenderTargets_hpp_INCLUDE__

#include <engine/GameStateI.hpp>
#include <tankgraph/TankMenus.hpp>

// SINGLETON
class RenderTargets
{
public:
	static RenderTargets* instance();

	struct Renderer3D : public GameStateI
	{
		Renderer3D() : GameStateI( "RenderTargets3D" ), stepTime( 0.0f ) {}

		// Inherited from GameStateI
		virtual void draw( const unsigned int state );
		virtual void simulate( const unsigned int state, float simTime );
		virtual void enterState( const unsigned int state );

		float stepTime;
	} render3D;

	struct Renderer2D : public GameStateI
	{
		Renderer2D() : GameStateI( "RenderTargets2D" ) {}

		// Inherited from GameStateI
		virtual void draw( const unsigned int state );
		virtual void simulate( const unsigned int state, float simTime );
	} render2D;

	void shadowDraw();
	void draw2d();
	void draw( bool reflection );

	// clang-format off
	// uncrustify off
	unsigned int getTreesDrawn() { return treesDrawn_; }
	unsigned int getTargetsDrawn() { return targetsDrawn_; }
	// uncrustify on
	// clang-format on

	friend struct Renderer3D;
	friend struct Renderer2D;

protected:
	static RenderTargets* instance_;

	unsigned int treesDrawn_, targetsDrawn_;
	TankMenus    menus_;

private:
	RenderTargets();
	virtual ~RenderTargets();
};

#endif  // __INCLUDE_RenderTargets_hpp_INCLUDE__
