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

#if !defined(__INCLUDE_CameraPositionActionh_INCLUDE__)
#define __INCLUDE_CameraPositionActionh_INCLUDE__

#include <actions/Action.h>
#include <tank/TankViewPoints.h>

class CameraPositionAction : public Action
{
public:
	CameraPositionAction(
		unsigned int playerId,
		TankViewPointProvider *provider,
		fixed showTime,
		unsigned int priority,
		bool explosion);
	virtual ~CameraPositionAction();

	virtual void init();
	virtual void simulate(fixed frameTime, bool &remove);
	virtual std::string getActionType() { return "CameraPositionAction"; }
	virtual bool getActionSyncCheck() { return false; }

	fixed getShowTime() { return showTime_; }
	TankViewPointProvider *getProvider() { return provider_; }
	unsigned int &getShowPriority() { return showPriority_; }

protected:
	fixed totalTime_;
	fixed showTime_;
	TankViewPointProvider *provider_;
	unsigned int playerId_;
	unsigned int showPriority_;
	bool explosion_;
};

class CameraPositionActionRegistry
{
public:
	static void addCameraPositionAction(CameraPositionAction *action);
	static void rmCameraPositionAction(CameraPositionAction *action);

	static CameraPositionAction *getCurrentAction();

protected:
	static std::set<CameraPositionAction *> actions_;
	static CameraPositionAction *currentAction_;

	static CameraPositionAction *getCurrentBest();
};

#endif
