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

#if !defined(AFX_TARGETCAMERA_H__97593EBB_5901_4D81_BAEB_8ADC76CFB627__INCLUDED_)
#define AFX_TARGETCAMERA_H__97593EBB_5901_4D81_BAEB_8ADC76CFB627__INCLUDED_

#include <GLEXT/GLCamera.h>
#include <GLW/GLWToolTip.h>
#include <engine/GameState.h>
#include <graph/ParticleEmitter.h>
#include <common/Keyboard.h>

class Tank;
class TargetCamera 
{
public:
	enum CamType
	{
		CamTop = 0,
		CamBehind,
		CamTank,
		CamShot,
		CamAim,
		CamExplosion,
		CamAction,
		CamLeft,
		CamRight,
		CamLeftFar,
		CamRightFar,
		CamSpectator,
		CamObject,
		CamFree
	};

	TargetCamera();
	virtual ~TargetCamera();

	GLCamera &getCamera() { return mainCam_; }
	CamType getCameraType() { return cameraPos_; }
	ParticleEngine &getPrecipitationEngine() { return particleEngine_; }
	void setCameraType(CamType type) { cameraPos_ = type; }
	void resetCam();

	void simulate(float frameTime, bool playing);
	void draw();
	void drawPrecipitation();
	void mouseWheel(int x, int y, int z, bool &skipRest);
	void mouseDown(GameState::MouseButton button, 
		int x, int y, bool &skipRest);
	void mouseUp(GameState::MouseButton button, 
		int x, int y, bool &skipRest);
	void mouseDrag(GameState::MouseButton button,
		int mx, int my, int x, int y, bool &skipRest);
	bool keyboardCheck(
		float frameTime, 
		char *buffer, unsigned int keyState,
		KeyboardHistory::HistoryElement *history, int hisCount, 
		bool &skipRest);

	static const char **getCameraNames();
	static ToolTip *getCameraToolTips();
	static int getNoCameraNames();
	static float minHeightFunc(int x, int y, void *heightData);
	static float maxHeightFunc(int x, int y, void *heightData);

	static TargetCamera *getCurrentTargetCamera() { return currentTargetCamera_; }

protected:
	static TargetCamera *currentTargetCamera_;
	GLCamera mainCam_;
	CamType cameraPos_;
	ParticleEmitter rainEmitter_, snowEmitter_;
	ParticleEngine particleEngine_;
	float totalTime_, objectTime_;
	int viewObject_;
	int dragXStart_, dragYStart_;
	bool dragging_;
	bool lastLandIntersectValid_;
	Vector lastLandIntersect_;

	void moveCamera();
	bool getLandIntersect(int x, int y, Vector &intersect);
	void viewBehindTank(Tank *tank);
	void viewSpectator();
};

#endif // !defined(AFX_TARGETCAMERA_H__97593EBB_5901_4D81_BAEB_8ADC76CFB627__INCLUDED_)

