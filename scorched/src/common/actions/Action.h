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

#if !defined(AFX_ACTION_H__2C00E711_B337_4665_AB54_C6661FD67E5D__INCLUDED_)
#define AFX_ACTION_H__2C00E711_B337_4665_AB54_C6661FD67E5D__INCLUDED_

#include <common/fixed.h>
#include <string>

class Action;
class ScorchedContext;
class ActionRenderer
{
public:
	friend class Action;

	ActionRenderer();
	virtual ~ActionRenderer();

	virtual void draw(Action *action) = 0;
	virtual void simulate(Action *action, float frametime, bool &removeAction);

};

class Action
{
public:
	Action(bool actionReferenced);
	virtual ~Action();

	virtual void init() = 0;

	virtual void draw();
	virtual void simulate(fixed frameTime, bool &removeAction);	
	virtual bool getActionSyncCheck() { return true; }
	virtual std::string getActionType() = 0;
	virtual std::string getActionDetails() { return ""; }

	void setActionRender(ActionRenderer *renderer);
	void setScorchedContext(ScorchedContext *context);
	ScorchedContext *getScorchedContext();
	bool getReferenced() { return actionReferenced_; }
	void setActionStartTime(fixed time) { actionStartTime_ = time; }
	fixed getActionStartTime() { return actionStartTime_; }

protected:
	const char *name_;
	bool actionReferenced_;
	ActionRenderer *renderer_;
	ScorchedContext *context_;
	fixed actionStartTime_;
};

#ifndef S3D_SERVER

class SpriteAction : public Action
{
public:
	SpriteAction(ActionRenderer *render = 0);
	virtual ~SpriteAction();

	virtual void init();
	virtual std::string getActionType() { return "SpriteAction"; }
	virtual bool getActionSyncCheck() { return false; }
};

#endif

#endif // !defined(AFX_ACTION_H__2C00E711_B337_4665_AB54_C6661FD67E5D__INCLUDED_)
