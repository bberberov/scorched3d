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

#ifndef __INCLUDE_GLWWindow_hpp_INCLUDE__
#define __INCLUDE_GLWWindow_hpp_INCLUDE__

#include <string>
#include <GLW/GLWPanel.hpp>
#include <GLW/GLWToolTip.hpp>
#include <GLEXT/GLTextureReference.hpp>

class GLWWindow : public GLWPanel
{
public:
	enum PossibleStates
	{
		eNoTitle          = 1,
		eSmallTitle       = 2,
		eTransparent      = 4,
		eResizeable       = 8,
		eCircle           = 16,
		eNoDraw           = 32,
		eSavePosition     = 64,
		eSemiTransparent  = 128,
		eNoMove           = 256,
		eClickTransparent = 512,
		eHideName         = 1024
	};

	GLWWindow(
		const std::string& name        = "None",
		float              x           = 0.0f,
		float              y           = 0.0f,
		float              w           = 0.0f,
		float              h           = 0.0f,
		unsigned int       states      = 0,
		const std::string& description = "None"
	);
	GLWWindow( const std::string& name, float w, float h, unsigned int states, const std::string& description );
	virtual ~GLWWindow();

	virtual bool initFromXML( XMLNode* node );
	virtual void windowInit( const unsigned int state );
	virtual void draw();
	virtual void mouseDown( int button, float x, float y, bool& skipRest );
	virtual void mouseUp( int button, float x, float y, bool& skipRest );
	virtual void mouseDrag( int button, float mx, float my, float x, float y, bool& skipRest );
	virtual void keyDown(
		char*                            buffer,
		unsigned int                     keyState,
		KeyboardHistory::HistoryElement* history,
		int                              hisCount,
		bool&                            skipRest
	);
	virtual void mouseWheel( float x, float y, float z, bool& skipRest );

	virtual void saveSettings( XMLNode* node );
	virtual void loadSettings( XMLNode* node, bool resetPositions );

	// clang-format off
	// uncrustify off
	const char* getDescription()                    { return description_.c_str(); }
	ToolTip& getToolTip()                           { return toolTip_; }
	void setWindowLevel( unsigned int windowLevel ) { windowLevel_ = windowLevel; }
	unsigned int getWindowLevel()                   { return windowLevel_; }
	unsigned int getWindowState()                   { return windowState_; }
	void needsCentered()                            { needCentered_ = true; }
	virtual void drawIconBox( float x, float y );
	// uncrustify on
	// clang-format on

	REGISTER_CLASS_HEADER( GLWWindow );

protected:
	enum
	{
		NoDrag,
		TitleDrag,
		SizeDrag
	} dragging_;

	static GLTextureReference moveTexture_;
	static GLTextureReference resizeTexture_;
	bool                      showTitle_;
	bool                      needCentered_;
	bool                      disabled_;
	bool                      initPosition_;
	unsigned int              windowState_;
	unsigned int              windowLevel_;
	std::string               description_;
	float                     maxWindowSize_;
	ToolTip                   toolTip_;

	virtual void drawWindowCircle( float x, float y, float w, float h );
	virtual void drawOutlinePoints( float x, float y, float w, float h );
	virtual void drawBackSurface( float x, float y, float w, float h );
	virtual void drawTitleBar( float x, float y, float w, float h );
	virtual void drawSurround( float x, float y, float w, float h );
	virtual void drawMaximizedWindow();
	virtual void drawInfoBox( float x, float y, float w );
	virtual void drawJoin( float x, float y );
};

#endif  // __INCLUDE_GLWWindow_hpp_INCLUDE__
