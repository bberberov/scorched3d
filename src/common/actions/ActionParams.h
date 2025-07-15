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

#if !defined(__INCLUDE_ActionParamsh_INCLUDE__)
#define __INCLUDE_ActionParamsh_INCLUDE__

#include <lang/LangString.h>
#include <common/NumberParser.h>
#include <vector>

#define FIXED_ACTION_PARAM_DECL(x) \
	private: \
	FixedActionParam x_; \
	public: \
	fixed get##x() { return x_.getValue(); }
#define FIXED_ACTION_PARAM_DEFN(x, y, z) \
	x_.setValue(z); \
	x_.setName(y); \
	params_.push_back(&x_);

struct lua_State;
class XMLNode;
class ActionParam
{
public:
	const std::string &getName() { return name_; }
	void setName(const std::string &name) { name_ = name; }

	virtual void copy(ActionParam *other) = 0;
	virtual bool parseXML(XMLNode *accessoryNode) = 0;
	virtual void initLUA(lua_State *L, int position) = 0;
	virtual void initXML(ScorchedContext &context) = 0;
		
protected:
	std::string name_;
};

class FixedActionParam : public ActionParam
{
public:
	FixedActionParam() : parser_("FixedActionParam") {}

	fixed getValue() { return value_; }
	void setValue(fixed value) { value_ = value; }

	virtual void copy(ActionParam *other);
	virtual bool parseXML(XMLNode *accessoryNode);
	virtual void initLUA(lua_State *L, int position);
	virtual void initXML(ScorchedContext &context);

protected:
	NumberParser parser_;
	fixed value_;
};

class ActionParams 
{
public:
	ActionParams();

	bool parseXML(XMLNode *accessoryNode);
	void initLUA(lua_State *L, int position);
	void initXML(ScorchedContext &context);

	void copy(ActionParams &other);

protected:
	std::vector<ActionParam *> params_;

private:
	ActionParams(const ActionParams &other);
	ActionParams &operator=(const ActionParams &other);
};

class TestActionParams : public ActionParams
{
	TestActionParams();

	FIXED_ACTION_PARAM_DECL(testName);
};

#endif
