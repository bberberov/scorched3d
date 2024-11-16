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

#include <actions/ActionParams.h>
#include <lua/LUAUtil.h>
#include <XML/XMLNode.h>

void FixedActionParam::copy(ActionParam *other)
{
	value_ = ((FixedActionParam *) other)->value_;
}

bool FixedActionParam::parseXML(XMLNode *accessoryNode)
{
	return accessoryNode->getNamedChild(name_.c_str(), value_);
}

void FixedActionParam::initLUA(lua_State *L, int position)
{
	value_ = LUAUtil::getNumberFromTable(L, position, name_.c_str(), value_);
}

void FixedActionParam::initXML(ScorchedContext &context)
{
	value_ = parser_.getValue(context);
}

ActionParams::ActionParams()
{
}

bool ActionParams::parseXML(XMLNode *accessoryNode)
{
	std::vector<ActionParam *>::iterator itor;
	for (itor = params_.begin();
		itor != params_.end();
		++itor)
	{
		if (!(*itor)->parseXML(accessoryNode)) return false;
	}

	return true;
}

void ActionParams::initLUA(lua_State *L, int position)
{
	luaL_checktype(L, position, LUA_TTABLE);

	std::vector<ActionParam *>::iterator itor;
	for (itor = params_.begin();
		itor != params_.end();
		++itor)
	{
		(*itor)->initLUA(L, position);
	}
}

void ActionParams::initXML(ScorchedContext &context)
{
	std::vector<ActionParam *>::iterator itor;
	for (itor = params_.begin();
		itor != params_.end();
		++itor)
	{
		(*itor)->initXML(context);
	}
}

void ActionParams::copy(ActionParams &other)
{
	for (unsigned int i=0; i<params_.size(); i++)
	{
		params_[i]->copy(other.params_[i]);
	}
}

TestActionParams::TestActionParams()
{
	FIXED_ACTION_PARAM_DEFN(testName, "testname", fixed(true, 10));
}
