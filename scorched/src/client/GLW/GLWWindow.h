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

#if !defined(AFX_GLWWINDOW_H__DF296D0F_BC67_4A40_B8F9_3B70E8AC1F65__INCLUDED_)
#define AFX_GLWWINDOW_H__DF296D0F_BC67_4A40_B8F9_3B70E8AC1F65__INCLUDED_

#include <string>
#include <GLW/GLWPanel.h>
#include <GLW/GLWToolTip.h>
#include <GLEXT/GLTextureReference.h>

class GLWWindow : public GLWPanel
{
public:
	enum PossibleStates
	{
		eNoTitle = 1,
		eSmallTitle = 2,
		eTransparent = 4,
		eResizeable = 8,
		eCircle = 16,
		eNoDraw = 32,
		eSavePosition = 64,
		eSemiTransparent = 128,
		eNoMove = 256,
		eClickTransparent = 512,
		eHideName = 1024
	};

	GLWWindow(const std::string &name = "None", 
		float x = 0.0f, float y = 0.0f, 
		float w = 0.0f, float h = 0.0f,
		unsigned int states = 0, 
		const std::string &description = "None");
	GLWWindow(const std::string &name, float w, float h,
		unsigned int states,
		const std::string &description);
	virtual ~GLWWindow();

	virtual bool initFromXML(XMLNode *node);
	virtual void windowInit(const unsigned state);
	virtual void draw();
	virtual void mouseDown(int button, float x, float y, bool &skipRest);
	virtual void mouseUp(int button, float x, float y, bool &skipRest);
	virtual void mouseDrag(int button, float mx, float my, float x, float y, bool &skipRest);
	virtual void keyDown(char *buffer, unsigned int keyState, 
		KeyboardHistory::HistoryElement *history, int hisCount, 
		bool &skipRest);
	virtual void mouseWheel(float x, float y, float z, bool &skipRest);

	virtual void saveSettings(XMLNode *node);
	virtual void loadSettings(XMLNode *node, bool resetPositions);

	const char *getDescription() { return description_.c_str(); }
	ToolTip &getToolTip() { return toolTip_; }
	void setWindowLevel(unsigned int windowLevel) { windowLevel_ = windowLevel; }
	unsigned int getWindowLevel() { return windowLevel_; }
	unsigned int getWindowState() { return windowState_; }
	void needsCentered() { needCentered_ = true; }
	virtual void drawIconBox(float x, float y);

	REGISTER_CLASS_HEADER(GLWWindow);

protected:
	enum
	{
		NoDrag,
		TitleDrag,
		SizeDrag
	} dragging_;

	ToolTip toolTip_;
	static GLTextureReference moveTexture_, resizeTexture_;
	bool showTitle_;
	bool needCentered_;
	bool disabled_;
	bool initPosition_;
	unsigned int windowState_;
	unsigned int windowLevel_;
	float maxWindowSize_;
	std::string description_;

	virtual void drawWindowCircle(float x, float y, float w, float h);
	virtual void drawOutlinePoints(float x, float y, float w, float h);
	virtual void drawBackSurface(float x, float y, float w, float h);
	virtual void drawTitleBar(float x, float y, float w, float h);
	virtual void drawSurround(float x, float y, float w, float h);
	virtual void drawMaximizedWindow();
	virtual void drawInfoBox(float x, float y, float w);
	virtual void drawJoin(float x, float y);

};

#endif // !defined(AFX_GLWWINDOW_H__DF296D0F_BC67_4A40_B8F9_3B70E8AC1F65__INCLUDED_)
