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

#include <XML/XMLNode.h>
#include <common/Defines.h>
#include <stdlib.h>
#include <stdio.h>
#include <common/NumberParser.h>

void XMLNode::removeSpecialChars(const std::string &content, std::string &result)
{
	result = "";
	for (char *c=(char *) content.c_str(); *c; c++)
	{
		if (*c == '\n') result += "&#10;";
		else if (*c < 32 || *c > 126) result += " ";
		else if (*c == '>') result += "&gt;";
		else if (*c == '<') result += "&lt;";
		else if (*c == '\'') result += "&apos;";
		else if (*c == '"') result += "&quot;";
		else if (*c == '&') result += "&amp;";
		else if (*c == '%') result += "&#37;";
		else result += *c;
	}
}

void XMLNode::addSpecialChars(const std::string &content, std::string &result)
{
	result = "";
	for (char *c=(char *) content.c_str(); *c; c++)
	{
		if (*c == '&')
		{
			if (strstr(c, "&gt;") == c) { result += '>'; c+=3; }
			else if (strstr(c, "&lt;") == c) { result += '<'; c+=3; }
			else if (strstr(c, "&apos;") == c) { result += '\''; c+=5; }
			else if (strstr(c, "&quot;") == c) { result += '"'; c+=5; }
			else if (strstr(c, "&amp;") == c) { result += '&'; c+=4; }
			else if (strstr(c, "&#") == c)
			{
				char *pos = strchr(c, ';');
				if (pos <= c + 3)
				{
					c+=2;
					result += S3D::formatStringBuffer("%c", atoi(c));
					*pos = '\0';
					c = pos + 1;
				}
			}
			else result += *c;
		}
		else result += *c;
	}	
}

const char *XMLNode::getSpacer(int space)
{
	static std::string spacestr;
	spacestr = "";

	for (int i=0; i<space; i++) spacestr+="\t";
	return spacestr.c_str();
}

XMLNode::XMLNode(const char *name, const char *content, NodeType type) : 
	name_(name), parent_(0), type_(type),
	useContentNodes_(false)
{
	addContent(content, (int) strlen(content));
}

XMLNode::XMLNode(const char *name, const std::string &content, NodeType type) : 
	name_(name), parent_(0), type_(type),
	useContentNodes_(false)
{
	addContent(content.c_str(), (int) content.size());
}

XMLNode::XMLNode(const char *name, const LangString &langStringContent, NodeType type) : 
	name_(name), parent_(0), type_(type),
	useContentNodes_(false)
{
	std::string content;
	content = LangStringUtil::convertFromLang(langStringContent);
	addContent(content.c_str(), (int) content.size());
}

XMLNode::XMLNode(const char *name, float content, NodeType type) :
	name_(name), parent_(0), type_(type), useContentNodes_(false)
{
	char buffer[20];
	snprintf(buffer, 20, "%.2f", content);
	addContent(buffer, (int) strlen(buffer));
}

XMLNode::XMLNode(const char *name, int content, NodeType type) :
	name_(name), parent_(0), type_(type), useContentNodes_(false)
{
	char buffer[20];
	snprintf(buffer, 20, "%i", content);
	addContent(buffer, (int) strlen(buffer));
}

XMLNode::XMLNode(const char *name, unsigned int content, NodeType type) :
	name_(name), parent_(0), type_(type), useContentNodes_(false)
{
	char buffer[20];
	snprintf(buffer, 20, "%u", content);
	addContent(buffer, (int) strlen(buffer));
}

XMLNode::XMLNode(const char *name, bool content, NodeType type) :
	name_(name), parent_(0), type_(type), useContentNodes_(false)
{
	const char *buffer = content?"true":"false";
	addContent(buffer, (int) strlen(buffer));
}

XMLNode::XMLNode(const char *name, fixed content, NodeType type) :
	name_(name), parent_(0), type_(type), useContentNodes_(false)
{
	const char *buffer = content.asString();
	addContent(buffer, (int) strlen(buffer));
}

XMLNode::XMLNode(const char *name, FixedVector &content, NodeType type) :
	name_(name), parent_(0), type_(type), useContentNodes_(false)
{
	XMLNode *nodeA = new XMLNode("a");
	addChild(nodeA);
	XMLNode *nodeB = new XMLNode("b");
	addChild(nodeB);
	XMLNode *nodeC = new XMLNode("c");
	addChild(nodeC);

	const char *buffer = content[0].asString();
	nodeA->addContent(buffer, (int) strlen(buffer));

	buffer = content[1].asString();
	nodeB->addContent(buffer, (int) strlen(buffer));

	buffer = content[2].asString();
	nodeC->addContent(buffer, (int) strlen(buffer));
}

XMLNode::XMLNode(const char *name, FixedVector4 &content, NodeType type) :
	name_(name), parent_(0), type_(type), useContentNodes_(false)
{
	XMLNode *nodeA = new XMLNode("a");
	addChild(nodeA);
	XMLNode *nodeB = new XMLNode("b");
	addChild(nodeB);
	XMLNode *nodeC = new XMLNode("c");
	addChild(nodeC);
	XMLNode *nodeD = new XMLNode("d");
	addChild(nodeD);

	const char *buffer = content[0].asString();
	nodeA->addContent(buffer, (int) strlen(buffer));

	buffer = content[1].asString();
	nodeB->addContent(buffer, (int) strlen(buffer));

	buffer = content[2].asString();
	nodeC->addContent(buffer, (int) strlen(buffer));

	buffer = content[3].asString();
	nodeD->addContent(buffer, (int) strlen(buffer));
}

XMLNode::XMLNode(const char *name, Vector &content, NodeType type) :
	name_(name), parent_(0), type_(type), useContentNodes_(false)
{
	XMLNode *nodeA = new XMLNode("a");
	addChild(nodeA);
	XMLNode *nodeB = new XMLNode("b");
	addChild(nodeB);
	XMLNode *nodeC = new XMLNode("c");
	addChild(nodeC);

	char buffer[20];
	snprintf(buffer, 20, "%.2f", content[0]);
	nodeA->addContent(buffer, (int) strlen(buffer));

	snprintf(buffer, 20, "%.2f", content[1]);
	nodeB->addContent(buffer, (int) strlen(buffer));

	snprintf(buffer, 20, "%.2f", content[2]);
	nodeC->addContent(buffer, (int) strlen(buffer));
}

XMLNode::~XMLNode()
{
	clear();
}

void XMLNode::clear()
{
	while (!children_.empty())
	{
		XMLNode *node = children_.front();
		children_.pop_front();
		delete node;
	}
	while (!removedChildren_.empty())
	{
		XMLNode *node = removedChildren_.front();
		removedChildren_.pop_front();
		delete node;
	}
	while (!parameters_.empty())
	{
		XMLNode *node = parameters_.front();
		parameters_.pop_front();
		delete node;
	}
	while (!removedParameters_.empty())
	{
		XMLNode *node = removedParameters_.front();
		removedParameters_.pop_front();
		delete node;
	}
}

bool XMLNode::writeToFile(const std::string &filename)
{
	FileLines lines;
	addNodeToFile(lines, 0);
	return lines.writeFile(filename);
}

void XMLNode::addNodeToFile(FileLines &lines, int spacing)
{
	if (type_ == XMLNodeType)
	{
		std::string params;
		std::list<XMLNode *>::iterator pitor;
		for (pitor = parameters_.begin();
			pitor != parameters_.end();
			++pitor)
		{
			XMLNode *node = (*pitor);
			DIALOG_ASSERT(node->type_ == XMLParameterType);
			
			std::string oldContent(node->getContent());
			std::string newContent;
			removeSpecialChars(oldContent, newContent);
			
			params += " " + node->name_ + "='" + newContent + "'";
		}

		if (children_.empty())
		{
			std::string oldContent(getContent());
			std::string newContent;
			removeSpecialChars(oldContent, newContent);
			
			lines.addLine(S3D::formatStringBuffer("%s<%s%s>%s</%s>", 
				getSpacer(spacing),
				name_.c_str(), params.c_str(), 
				newContent.c_str(), name_.c_str()));
		}
		else
		{
			lines.addLine(S3D::formatStringBuffer("%s<%s%s>", 
				getSpacer(spacing),
				name_.c_str(), params.c_str()));

			std::list<XMLNode *>::iterator itor;
			for (itor = children_.begin();
				itor != children_.end();
				++itor)
			{
				XMLNode *node = (*itor);
				node->addNodeToFile(lines, spacing + 1);
			}

			lines.addLine(S3D::formatStringBuffer("%s</%s>", 
				getSpacer(spacing), name_.c_str()));
		}
	}
	else if (type_ == XMLCommentType)
	{
		lines.addLine(S3D::formatStringBuffer("%s<!-- %s -->", 
			getSpacer(spacing), getContent()));
	}
}

bool XMLNode::failChildren()
{
	if (useContentNodes_)
	{
		std::list<XMLNode *>::iterator itor;
		for (itor = getChildren().begin();
			itor != getChildren().end();
			++itor)
		{
			XMLNode	*node = (*itor);
			if (node->getType() == XMLNodeType)
			{
				node->returnError(S3D::formatStringBuffer("Unrecognised node."));
				return false;
			}
		}
	}
	else
	{
		if (!children_.empty())
		{
			XMLNode *node = children_.front();
			node->returnError(S3D::formatStringBuffer("Unrecognised node."));
			return false;
		}
	}

	return true;
}

bool XMLNode::failContent()
{
	for (const char *c=getContent(); *c; c++)
	{
		if (*c != '\n' &&
			*c != '\r' &&
			*c != '\t' &&
			*c != ' ')
		{
			returnError(S3D::formatStringBuffer("Unexpected context : %s", 
					getContent()));
			return false;
		}
	}
	return true;
}

void XMLNode::resurrectRemovedChildren()
{
	while (!removedChildren_.empty())
	{
		XMLNode *node = removedChildren_.front();
		removedChildren_.pop_front();
		children_.push_back(node);
		
		node->resurrectRemovedChildren();
	}
	while (!removedParameters_.empty())
	{
		XMLNode *node = removedParameters_.front();
		removedParameters_.pop_front();
		parameters_.push_back(node);
	}
}

bool XMLNode::getNamedChild(const char *name, XMLNode *&value,
	bool failOnError, bool remove)
{
	std::list<XMLNode *>::iterator itor;
	for (itor = children_.begin();
		itor != children_.end();
		++itor)
	{
		XMLNode *node = (*itor);
		if (strcmp(name, node->getName()) == 0) 
		{
			if (remove)
			{
				removedChildren_.push_back(node);
				children_.erase(itor);
			}
			value = node;
			return true;
		}
	}

	if (failOnError)
	{
		returnError(S3D::formatStringBuffer("Failed to find \"%s\" node", name));
	}
	return false;
}

bool XMLNode::getNamedParameter(const char *name, XMLNode *&value,
	bool failOnError, bool remove)
{
	std::list<XMLNode *>::iterator itor;
	for (itor = parameters_.begin();
		itor != parameters_.end();
		++itor)
	{
		XMLNode *node = (*itor);
		if (strcmp(name, node->getName()) == 0)
		{
			if (remove)
			{
				removedParameters_.push_back(node);
				parameters_.erase(itor);
			}
			value = node;
			return true;
		}
	}

	if (failOnError)
	{
		returnError(S3D::formatStringBuffer("Failed to find \"%s\" parameter", name));
	}
	return false;
}

bool XMLNode::getNamedParameter(const char *name, std::string &value,
	bool failOnError, bool remove)
{
	XMLNode *node;
	if (!getNamedParameter(name, node, failOnError, remove)) return false;
	value = node->getContent();
	return true;
}

bool XMLNode::getNamedParameter(const char *name, LangString &value,
	bool failOnError, bool remove)
{
	XMLNode *node;
	if (!getNamedParameter(name, node, failOnError, remove)) return false;
	value = LANG_STRING(node->getContent());
	return true;
}

bool XMLNode::getNamedChild(const char *name, LangString &value,
	bool failOnError, bool remove)
{
	XMLNode *node;
	if (!getNamedChild(name, node, failOnError, remove)) return false;
	value = LANG_STRING(node->getContent());
	return true;
}

bool XMLNode::getNamedChild(const char *name, std::string &value,
	bool failOnError, bool remove)
{
	XMLNode *node;
	if (!getNamedChild(name, node, failOnError, remove)) return false;
	value = node->getContent();
	return true;
}

bool XMLNode::getNamedChild(const char *name, bool &value,
	bool failOnError, bool remove)
{
	XMLNode *node;
	if (!getNamedChild(name, node, failOnError, remove)) return false;

	if (0 == strcmp(node->getContent(), "true")) value = true;
	else if (0 == strcmp(node->getContent(), "false")) value = false;
	else 
	{
		return node->returnError(
			"Failed to parse boolean value (should be true or false)");
	}
	return true;
}

bool XMLNode::getNamedChild(const char *name, NumberParser &value,
        bool failOnError, bool remove)
{
	XMLNode *node;
	if (!getNamedChild(name, node, failOnError, remove)) return false;

	if (!value.setExpression(node->getContent()))
			return node->returnError("Failed to parse expression");

	return true;
}

bool XMLNode::getNamedChild(const char *name, float &value,
	bool failOnError, bool remove)
{
	XMLNode *node;
	if (!getNamedChild(name, node, failOnError, remove)) return false;

	if (sscanf(node->getContent(), "%f", &value) != 1) 
		return node->returnError("Failed to parse float value");
	return true;
}

bool XMLNode::getNamedChild(const char *name, int &value,
	bool failOnError, bool remove)
{
	XMLNode *node;
	if (!getNamedChild(name, node, failOnError, remove)) return false;

	if (sscanf(node->getContent(), "%i", &value) != 1)
		return node->returnError("Failed to parse int value");
	return true;
}

bool XMLNode::getNamedChild(const char *name, unsigned int &value,
	bool failOnError, bool remove)
{
	XMLNode *node;
	if (!getNamedChild(name, node, failOnError, remove)) return false;

	if (sscanf(node->getContent(), "%u", &value) != 1)
		return node->returnError("Failed to parse unsigned int value");
	return true;
}

bool XMLNode::getNamedChild(const char *name, fixed &value,
	bool failOnError, bool remove)
{
	std::string v;
	if (!getNamedChild(name, v, failOnError, remove)) return false;
	value = fixed(v.c_str());
	return true;
}

bool XMLNode::getNamedChild(const char *name, FixedVector &value, 
	bool failOnError, bool remove)
{
	XMLNode *node;
	if (!getNamedChild(name, node, failOnError, remove)) return false;

	std::string a, b, c;
	if (!node->getNamedChild("A", a, false, true) &&
		!node->getNamedChild("a", a, false, true))
	{
		if (failOnError) node->returnError("Failed to find a node");
		return false;
	}
	if (!node->getNamedChild("B", b, false, true) &&
		!node->getNamedChild("b", b, false, true))
	{
		if (failOnError) node->returnError("Failed to find b node");
		return false;
	}
	if (!node->getNamedChild("C", c, false, true) &&
		!node->getNamedChild("c", c, false, true))
	{
		if (failOnError) node->returnError("Failed to find c node");
		return false;
	}
	if (failOnError && !node->failChildren()) return false;

	value[0] = fixed(a.c_str());
	value[1] = fixed(b.c_str());
	value[2] = fixed(c.c_str());
	return true;
}

bool XMLNode::getNamedChild(const char *name, Vector &value, 
	bool failOnError, bool remove)
{
	XMLNode *node;
	if (!getNamedChild(name, node, failOnError, remove)) return false;

	Vector tmpValue;
	if (!node->getNamedChild("A", tmpValue[0], false, true) &&
		!node->getNamedChild("a", tmpValue[0], false, true))
	{
		if (failOnError) node->returnError("Failed to find a node");
		return false;
	}
	if (!node->getNamedChild("B", tmpValue[1], false, true) &&
		!node->getNamedChild("b", tmpValue[1], false, true))
	{
		if (failOnError) node->returnError("Failed to find b node");
		return false;
	}
	if (!node->getNamedChild("C", tmpValue[2], false, true) &&
		!node->getNamedChild("c", tmpValue[2], false, true))
	{
		if (failOnError) node->returnError("Failed to find c node");
		return false;
	}
	if (failOnError && !node->failChildren()) return false;

	value = tmpValue;
	return true;
}

const char *XMLNode::getContent()
{
	if (useContentNodes_ &&
		getType() == XMLNodeType)
	{
		static std::string result;

		result = "";
		std::list<XMLNode *>::iterator itor;
		for (itor = getChildren().begin();
			itor != getChildren().end();
			++itor)
		{
			XMLNode *node = (*itor);
			if (node->getType() == XMLContentType)
			{
				result += node->content_.c_str();
			}
		}
		return result.c_str();
	}
	else
	{
		return content_.c_str();
	}
}

void XMLNode::setSource(const char *source)
{ 
	source_ = source; 
}

void XMLNode::setLine(int line, int col)
{
	line_ = line;
	col_ = col;
}

void XMLNode::addChild(XMLNode *node) 
{ 
	children_.push_back(node); 
	node->setUseContentNodes(useContentNodes_);
	node->parent_ = this; 
	node->source_ = source_; 
}

void XMLNode::addParameter(XMLNode *node) 
{ 
	parameters_.push_back(node); 
	node->parent_ = this; 
	node->source_ = source_; 
}

void XMLNode::addContent(const char *data, int len)
{
	std::string oldContent, newContent;
	oldContent.append(data, len);
	addSpecialChars(oldContent, newContent);

	if (useContentNodes_)
	{
		XMLNode *newNode = 
			new XMLNode("__TEXT__", "", XMLNode::XMLContentType);
		newNode->setLine(line_, col_);
		newNode->content_.append(newContent);
		addChild(newNode);
	}
	else
	{
		content_.append(newContent); 
	}
}

bool XMLNode::returnError(const std::string &error)
{
	S3D::dialogMessage("XMLNode",
		S3D::formatStringBuffer("Parse Error, File:%s Line:%i Col:%i Node:%s Error:%s",
		source_.c_str(), line_, col_, getName(), error.c_str()));
	return false;
}
