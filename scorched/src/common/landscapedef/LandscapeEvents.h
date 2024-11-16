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

#if !defined(__INCLUDE_LandscapeEventsh_INCLUDE__)
#define __INCLUDE_LandscapeEventsh_INCLUDE__

#include <common/fixed.h>
#include <common/ModelID.h>

class ScorchedContext;
class LandscapeCondition
{
public:
	virtual fixed getNextEventTime(ScorchedContext &context, 
		int eventNumber) = 0;
	virtual bool fireEvent(ScorchedContext &context, 
		fixed timeLeft, int eventNumber) = 0;
	virtual bool readXML(XMLNode *node) = 0;

	static LandscapeCondition *create(const char *name);
};

class LandscapeConditionGroupSize : public LandscapeCondition
{
public:
	int groupsize;
	std::string groupname;

	virtual fixed getNextEventTime(ScorchedContext &context, 
		int eventNumber);
	virtual bool fireEvent(ScorchedContext &context, 
		fixed timeLeft, int eventNumber);
	virtual bool readXML(XMLNode *node);	
};

class LandscapeConditionTime : public LandscapeCondition
{
public:
	fixed mintime;
	fixed maxtime;
	bool singletimeonly;

	virtual fixed getNextEventTime(ScorchedContext &context, 
		int eventNumber);
	virtual bool fireEvent(ScorchedContext &context, 
		fixed timeLeft, int eventNumber);
	virtual bool readXML(XMLNode *node);
};

class LandscapeConditionRandom : public LandscapeCondition
{
public:
	fixed randomchance;
	fixed randomdelay;

	virtual fixed getNextEventTime(ScorchedContext &context, 
		int eventNumber);
	virtual bool fireEvent(ScorchedContext &context, 
		fixed timeLeft, int eventNumber);
	virtual bool readXML(XMLNode *node);
};

class LandscapeAction
{
public:
	virtual bool readXML(XMLNode *node) = 0;
	virtual void fireAction(ScorchedContext &context) = 0;

	static LandscapeAction *create(const char *name);
};

class LandscapeActionFireWeapon : public LandscapeAction
{
public:
	std::string weapon;

	virtual void fireAction(ScorchedContext &context);
	virtual bool readXML(XMLNode *node);
};

class LandscapeActionFireWeaponFromGroup : public LandscapeAction
{
public:
	std::string weapon;
	std::string groupname;

	virtual void fireAction(ScorchedContext &context);
	virtual bool readXML(XMLNode *node);
};

class LandscapeEvent
{
public:
	LandscapeEvent();
	virtual ~LandscapeEvent();

	LandscapeCondition *condition;
	LandscapeAction *action;

	virtual bool readXML(XMLNode *node);
};

#endif // __INCLUDE_LandscapeEventsh_INCLUDE__
