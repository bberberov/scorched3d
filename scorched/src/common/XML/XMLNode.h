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

#if !defined(__INCLUDE_XMLNodeh_INCLUDE__)
#define __INCLUDE_XMLNodeh_INCLUDE__

#include <list>
#include <lang/LangString.h>
#include <common/Vector.h>
#include <common/FixedVector4.h>
#include <common/FileLines.h>
#include <common/NumberParser.h>

class XMLNode
{
public:
	enum NodeType
	{
		XMLNodeType,
		XMLParameterType,
		XMLCommentType,
		XMLContentType
	};

	XMLNode(const char *name, const char *content = "", 
		NodeType = XMLNode::XMLNodeType);
	XMLNode(const char *name, const std::string &content, 
		NodeType = XMLNode::XMLNodeType);
	XMLNode(const char *name, const LangString &content, 
		NodeType = XMLNode::XMLNodeType);
	XMLNode(const char *name, NumberParser &content,
		NodeType = XMLNode::XMLNodeType);
	XMLNode(const char *name, float content, 
		NodeType = XMLNode::XMLNodeType);
	XMLNode(const char *name, bool content, 
		NodeType = XMLNode::XMLNodeType);
	XMLNode(const char *name, int content, 
		NodeType = XMLNode::XMLNodeType);
	XMLNode(const char *name, Vector &content, 
		NodeType = XMLNode::XMLNodeType);
	XMLNode(const char *name, unsigned int content, 
		NodeType = XMLNode::XMLNodeType);
	XMLNode(const char *name, fixed content, 
		NodeType = XMLNode::XMLNodeType);
	XMLNode(const char *name, FixedVector &content, 
		NodeType = XMLNode::XMLNodeType);
	XMLNode(const char *name, FixedVector4 &content, 
		NodeType = XMLNode::XMLNodeType);

	virtual ~XMLNode();

	bool writeToFile(const std::string &fileName);
	void addNodeToFile(FileLines &lines, int spacing);

	NodeType getType() { return type_; }
	const char *getName() { return name_.c_str(); }
	const char *getContent();
	const char *getSource() { return source_.c_str(); }
	const XMLNode *getParent() { return parent_; }

	std::list<XMLNode *> &getChildren() { return children_; }
	std::list<XMLNode *> &getParameters() { return parameters_; }

	bool getNamedParameter(const char *name, XMLNode *&node, 
		bool failOnError = true, bool remove = true);
	bool getNamedParameter(const char *name, std::string &value,
		bool failOnError = true, bool remove = true);
	bool getNamedParameter(const char *name, LangString &value,
		bool failOnError = true, bool remove = true);
	bool getNamedChild(const char *name, XMLNode *&node, 
		bool failOnError = true, bool remove = true);
	bool getNamedChild(const char *name, LangString &node, 
		bool failOnError = true, bool remove = true);
	bool getNamedChild(const char *name, std::string &value,
		bool failOnError = true, bool remove = true);
	bool getNamedChild(const char *name, bool &value,
		bool failOnError = true, bool remove = true);
	bool getNamedChild(const char *name, NumberParser &value,
		bool failOnError = true, bool remove = true);
	bool getNamedChild(const char *name, float &value,
		bool failOnError = true, bool remove = true);
	bool getNamedChild(const char *name, int &value,
		bool failOnError = true, bool remove = true);
	bool getNamedChild(const char *name, unsigned int &value,
		bool failOnError = true, bool remove = true);
	bool getNamedChild(const char *name, fixed &value,
		bool failOnError = true, bool remove = true);
	bool getNamedChild(const char *name, Vector &value,
		bool failOnError = true, bool remove = true);
	bool getNamedChild(const char *name, FixedVector &value,
		bool failOnError = true, bool remove = true);

	bool failChildren();
	bool failContent();
	void resurrectRemovedChildren();

	void setSource(const char *source);
	void setLine(int line, int col);
	void setUseContentNodes(bool useContentNodes) 
		{ useContentNodes_ = useContentNodes; }

	bool returnError(const std::string &error);
	void addChild(XMLNode *node); 
	void addParameter(XMLNode *node);
	void addContent(const char *data, int len);

	void clear();

	static void removeSpecialChars(const std::string &content,
		std::string &result);
	static void addSpecialChars(const std::string &content, 
		std::string &result);
	static const char *getSpacer(int space);

protected:
	bool useContentNodes_;
	NodeType type_;
	XMLNode *parent_;
	std::list<XMLNode *> children_;
	std::list<XMLNode *> removedChildren_; // So they are tidied up as well
	std::list<XMLNode *> parameters_;
	std::list<XMLNode *> removedParameters_; // Tidied
	std::string name_;
	std::string content_;
	std::string source_;
	int line_, col_;
};

#endif
