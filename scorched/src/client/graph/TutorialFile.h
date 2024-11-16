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

#if !defined(__INCLUDE_TutorialFileh_INCLUDE__)
#define __INCLUDE_TutorialFileh_INCLUDE__

#include <XML/XMLFile.h>
#include <map>

class TutorialFile;
class TutorialFileEntry;
class TutorialCondition
{
public:
	virtual TutorialFileEntry *checkCondition() = 0;
	virtual bool parseXML(TutorialFile *file, XMLNode *node) = 0;

	static TutorialCondition *create(const char *type);
};

class TutorialConditionFirstMove : public TutorialCondition
{
public:
	virtual TutorialFileEntry *checkCondition();
	virtual bool parseXML(TutorialFile *file, XMLNode *node);

protected:
	TutorialFileEntry *next_;
};

class TutorialConditionTankDead : public TutorialCondition
{
public:
	virtual TutorialFileEntry *checkCondition();
	virtual bool parseXML(TutorialFile *file, XMLNode *node);

protected:
	TutorialFileEntry *next_;
};

class TutorialConditionWindowVisible : public TutorialCondition
{
public:
	virtual TutorialFileEntry *checkCondition();
	virtual bool parseXML(TutorialFile *file, XMLNode *node);

protected:
	std::string window_;
	TutorialFileEntry *next_;
};

class TutorialConditionWindowWait : public TutorialCondition
{
public:
	virtual TutorialFileEntry *checkCondition();
	virtual bool parseXML(TutorialFile *file, XMLNode *node);

protected:
	std::string window_;
	TutorialFileEntry *next_;
};

class TutorialFileEntry
{
public:
	std::list<TutorialCondition *> conditions_;
	std::string name_;
	XMLNode *text_;

	TutorialFileEntry *checkConditions();
};

class TutorialFile
{
public:
	TutorialFile();
	virtual ~TutorialFile();

	bool parseFile(const std::string &file);

	TutorialFileEntry *getStartEntry() { return start_; }
	TutorialFileEntry *getEntry(const char *name); 

protected:
	XMLFile file_;
	std::map<std::string, TutorialFileEntry *> entries_;
	TutorialFileEntry *start_;
};

#endif // __INCLUDE_TutorialFileh_INCLUDE__
