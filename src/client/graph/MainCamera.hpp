////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011, 2025, 2026
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

#ifndef __INCLUDE_MainCamera_hpp_INCLUDE__
#define __INCLUDE_MainCamera_hpp_INCLUDE__

#include <GLEXT/GLMenuI.hpp>
#include <engine/GameStateI.hpp>
#include <graph/TargetCamera.hpp>
#include <common/Vector.hpp>
#include <map>

// SINGLETON
class MainCamera : public GameStateI, public GLMenuI
{
public:
	static MainCamera* instance();

	// clang-format off
	// uncrustify off
	GLCamera&     getCamera() { return targetCam_.getCamera(); }
	TargetCamera& getTarget() { return targetCam_; }

	bool getCameraSelected() { return mouseDown_ || keyDown_ || scrolling_; }
	bool getShowArena()      { return showArena_; }
	// uncrustify on
	// clang-format on

	// Inherited from GameStateI
	virtual void simulate( const unsigned int state, float frameTime );
	virtual void draw( const unsigned int state );
	virtual void mouseWheel( const unsigned int state, int x, int y, int z, bool& skipRest );
	virtual void mouseDown( const unsigned int state, GameState::MouseButton button, int x, int y, bool& skipRest );
	virtual void mouseUp( const unsigned int state, GameState::MouseButton button, int x, int y, bool& skipRest );
	virtual void mouseDrag(
		const unsigned int     state,
		GameState::MouseButton button,
		int                    mx,
		int                    my,
		int                    x,
		int                    y,
		bool&                  skipRest
	);
	virtual void keyboardCheck(
		const unsigned int               state,
		float                            frameTime,
		char*                            buffer,
		unsigned int                     keyState,
		KeyboardHistory::HistoryElement* history,
		int                              hisCount,
		bool&                            skipRest
	);

	// Inherited from GLMenuI
	virtual bool getEnabled( const char* menuName );
	virtual void menuSelection( const char* menuName, const int position, GLMenuItem& item );
	virtual bool getMenuItems( const char* menuName, std::list< GLMenuItem >& result );

	// Class to save the screen shots
	class SaveScreen : public GameStateI
	{
	public:
		SaveScreen() : GameStateI( "SaveScreen" ), saveScreen_( false ), saveScreenTest_( false ) {}

		virtual void draw( const unsigned int state );

		bool saveScreen_;
		bool saveScreenTest_;
	} saveScreen_;

	class Precipitation : public GameStateI
	{
	public:
		Precipitation() : GameStateI( "Precipitation" ) {}

		virtual void draw( const unsigned int state );
	} precipitation_;

protected:
	static MainCamera* instance_;

	float                                        waterTransparency_;
	bool                                         mouseDown_;
	bool                                         keyDown_;
	bool                                         scrolling_;
	bool                                         showArena_;
	TargetCamera                                 targetCam_;
	// Quick key settings
	std::map< int, std::pair< Vector, Vector > > quickKeys_;

	void setQuick( int key );
	void useQuick( int key );

private:
	MainCamera();
	virtual ~MainCamera();
};

#endif  // __INCLUDE_MainCamera_hpp_INCLUDE__
