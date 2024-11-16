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

#if !defined(__INCLUDE_GLWPlanViewh_INCLUDE__)
#define __INCLUDE_GLWPlanViewh_INCLUDE__

#include <GLW/GLWidget.h>
#include <GLEXT/GLTextureReference.h>
#include <common/Vector.h>
#include <list>
#include <map>

class GLWPlanView : public GLWidget
{
public:
	GLWPlanView(float x = 0.0f, float y = 0.0f,
		float w = 0.0f, float h = 0.0f);
	virtual ~GLWPlanView();

	virtual void simulate(float frameTime);
	virtual void mouseDown(int button, float x, float y, bool &skipRest);
	virtual void mouseDrag(int button, float mx, float my, float x, float y, bool &skipRest);
	virtual void mouseUp(int button, float x, float y, bool &skipRest);
	virtual void draw();

	void addRecievePoints(unsigned int playerId, std::list<Vector> &recievepoints);

	REGISTER_CLASS_HEADER(GLWPlanView);

protected:
	struct PlayerDrawnInfo
	{
		unsigned int playerId;
		std::list<Vector> points;
		std::list<Vector> recievepoints;
	};

	GLTextureReference arrowTex_, tankTex_, circleTex_;
	float arenaX_, arenaY_, arenaWidth_, arenaHeight_;
	float landscapeWidth_, landscapeHeight_;
	float animationTime_;
	float flashTime_;
	float totalTime_;
	float pointTime_;
	float planColor_;
	bool flash_, firstTime_;
	bool dragging_;
	float dragLastX_, dragLastY_;
	PlayerDrawnInfo localPoints_;
	std::list<PlayerDrawnInfo> dragPoints_;
	std::list<Vector> sendPoints;

	void drawMap();
	void drawTexture();
	void drawWall();
	void drawCameraPointer();
	void drawTanks();
	void drawLines();
	void drawLine(PlayerDrawnInfo &info);
	void drawBuoys();
	bool simulateLine(PlayerDrawnInfo &info);

};

#endif
