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

#include <graph/TutorialFile.h>
#include <GLW/GLWWindowManager.h>
#include <client/ScorchedClient.h>
#include <common/OptionsTransient.h>
#include <target/TargetContainer.h>
#include <server/ServerSimulator.h>
#include <server/ScorchedServer.h>
#include <simactions/AdminSimAction.h>

TutorialFileEntry *TutorialConditionWindowWait::checkCondition()
{
	GLWWindow *window = 
		GLWWindowManager::instance()->getWindowByName(window_.c_str());

	if (window &&
		GLWWindowManager::instance()->windowVisible(window->getId()))
	{
		return next_;
	}
	return 0;
}

bool TutorialConditionWindowWait::parseXML(TutorialFile *file, XMLNode *node)
{
	std::string next;
	if (!node->getNamedChild("window", window_)) return false;
	if (!node->getNamedChild("next", next)) return false;

	if (!(next_ = file->getEntry(next.c_str())))
	{
		return node->returnError(
			"Failed to find the tutorial pointed to by next");
	}	

	return true;
}

TutorialFileEntry *TutorialConditionWindowVisible::checkCondition()
{
	GLWWindow *window = 
		GLWWindowManager::instance()->getWindowByName(window_.c_str());

	if (!window || 
		!GLWWindowManager::instance()->windowVisible(window->getId()))
	{
		return next_;
	}
	return 0;
}

bool TutorialConditionWindowVisible::parseXML(TutorialFile *file, XMLNode *node)
{
	std::string next;
	if (!node->getNamedChild("window", window_)) return false;
	if (!node->getNamedChild("next", next)) return false;

	if (!(next_ = file->getEntry(next.c_str())))
	{
		return node->returnError(
			"Failed to find the tutorial pointed to by next");
	}	

	return true;
}

TutorialFileEntry *TutorialConditionFirstMove::checkCondition()
{
	if (ScorchedClient::instance()->getOptionsTransient().getCurrentRoundNo() > 1)
	{
		return next_;
	}
	return 0;
}

bool TutorialConditionFirstMove::parseXML(TutorialFile *file, XMLNode *node)
{
	std::string next;
	if (!node->getNamedChild("next", next)) return false;

	if (!(next_ = file->getEntry(next.c_str())))
	{
		return node->returnError(
			"Failed to find the tutorial pointed to by next");
	}	

	return true;
}

TutorialFileEntry *TutorialConditionTankDead::checkCondition()
{
	if (ScorchedClient::instance()->getTargetContainer().getNoOfTanks() -
		ScorchedClient::instance()->getTargetContainer().aliveCount() > 1)
	{
		AdminSimAction *simAction = new AdminSimAction(AdminSimAction::eKillAll, 0, 0);
		ScorchedServer::instance()->getServerSimulator().addSimulatorAction(simAction);
		return next_;
	}
	return 0;
}

bool TutorialConditionTankDead::parseXML(TutorialFile *file, XMLNode *node)
{
	std::string next;
	if (!node->getNamedChild("next", next)) return false;

	if (!(next_ = file->getEntry(next.c_str())))
	{
		return node->returnError(
			"Failed to find the tutorial pointed to by next");
	}	

	return true;
}

TutorialCondition *TutorialCondition::create(const char *type)
{
	if (0 == strcmp(type, "WindowVisible")) return new TutorialConditionWindowVisible;
	else if (0 == strcmp(type, "WindowWait")) return new TutorialConditionWindowWait;
	else if (0 == strcmp(type, "FirstMove")) return new TutorialConditionFirstMove;
	else if (0 == strcmp(type, "TankDead")) return new TutorialConditionTankDead;
	return 0;
}

TutorialFileEntry *TutorialFileEntry::checkConditions()
{
	std::list<TutorialCondition *>::iterator itor;
	for (itor = conditions_.begin();
		itor != conditions_.end();
		++itor)
	{
		TutorialCondition *condition = (*itor);

		TutorialFileEntry *entry = condition->checkCondition();
		if (entry) return entry;
	}
	return 0;
}

TutorialFile::TutorialFile() : file_(true)
{
}

TutorialFile::~TutorialFile()
{
}

bool TutorialFile::parseFile(const std::string &file)
{
	if (!file_.readFile(file) ||
		!file_.getRootNode())
	{
		S3D::dialogMessage("Scorched Tutorial", S3D::formatStringBuffer(
					  "Failed to parse \"%s\"\n%s", 
					  file.c_str(),
					  file_.getParserError()));
		return false;
	}

	XMLNode *steps = 0;
	if (!file_.getRootNode()->getNamedChild("steps", steps)) return false;

	// Parse all the steps
	std::list<std::pair<TutorialCondition *, XMLNode *> > conditions;
	std::list<XMLNode *>::iterator stepitor;
	for (stepitor = steps->getChildren().begin();
		stepitor != steps->getChildren().end();
		++stepitor)
	{
		XMLNode *node = (*stepitor);

		if (node->getType() == XMLNode::XMLNodeType)
		{
			TutorialFileEntry *entry = new TutorialFileEntry();
			if (!node->getNamedChild("name", entry->name_)) return false;
			if (!node->getNamedChild("text", entry->text_)) return false;

			XMLNode *conditionsNode;
			if (node->getNamedChild("conditions", conditionsNode, false))
			{
				XMLNode *conditionNode;
				while (conditionsNode->getNamedChild("condition", conditionNode, false))
				{
					std::string type;
					if (!conditionNode->getNamedParameter("type", type)) return false;

					// Create condition
					TutorialCondition *condition =
						TutorialCondition::create(type.c_str());
					if (!condition) return 
						conditionNode->returnError("Failed to find condition type");
					entry->conditions_.push_back(condition);

					// Store this condition to be parsed later
					std::pair<TutorialCondition *, XMLNode *> 
						cond(condition, conditionNode);
					conditions.push_back(cond);
				}
			}

			if (entries_.find(entry->name_) != entries_.end())
			{
				return node->returnError("Duplicate name exists in file");
			}
			entries_[entry->name_] = entry;

			if (!node->failChildren()) return false;
		}
	}

	// Parse the conditions afterwards, as they may point to any of the
	// file entries.  So all the file entries need to be constructed 1st
	std::list<std::pair<TutorialCondition *, XMLNode *> >::iterator conditor;
	for (conditor = conditions.begin();
		conditor != conditions.end();
		++conditor)
	{
		TutorialCondition *condition = (*conditor).first;
		XMLNode *conditionNode = (*conditor).second;

		if (!condition->parseXML(
			this, conditionNode)) return false;
		if (!conditionNode->failChildren()) return false;
	}

	// Get the startnode
	std::string start;
	if (!file_.getRootNode()->getNamedChild("start", start)) return false;
	start_ = getEntry(start.c_str());
	if (!start_)
	{
		S3D::dialogMessage("TutorialFile", "Failed to find start node");
		return false;
	}

	// Check the file has been parsed
	if (!file_.getRootNode()->failChildren()) return false;
	return true;
}

TutorialFileEntry *TutorialFile::getEntry(const char *name)
{
	std::map<std::string, TutorialFileEntry *>::iterator findItor 
		= entries_.find(name);
	if (findItor == entries_.end())
	{
		return 0;
	}
	return (*findItor).second;
}
