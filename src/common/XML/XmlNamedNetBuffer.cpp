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

#include <XML/XmlNamedNetBuffer.h>

XmlNamedNetBuffer::XmlNamedNetBuffer() :
	rootNode_("XmlNamedNetBuffer")
{
	currentNode_ = &rootNode_;
}

XmlNamedNetBuffer::~XmlNamedNetBuffer()
{
}

void XmlNamedNetBuffer::clear()
{
	rootNode_.clear();
	currentNode_ = &rootNode_;
}

void XmlNamedNetBuffer::startSection(const char *name)
{
	XMLNode *newNode = new XMLNode(name);
	currentNode_->addChild(newNode);
	currentNode_ = newNode;
}

void XmlNamedNetBuffer::stopSection(const char *name)
{
	if (currentNode_->getParent())
	{
		currentNode_ = (XMLNode *) currentNode_->getParent();
	}
}

void XmlNamedNetBuffer::addToBufferNamed(const char *name, Vector &vector) 
{ 
	currentNode_->addChild(new XMLNode(formName(name), vector));
}

void XmlNamedNetBuffer::addToBufferNamed(const char *name, FixedVector &vector)
{ 
	currentNode_->addChild(new XMLNode(formName(name), vector));
}

void XmlNamedNetBuffer::addToBufferNamed(const char *name, FixedVector4 &vector)
{ 
	currentNode_->addChild(new XMLNode(formName(name), vector));
}

void XmlNamedNetBuffer::addToBufferNamed(const char *name, const char *add)
{ 
	currentNode_->addChild(new XMLNode(formName(name), add));
}

void XmlNamedNetBuffer::addToBufferNamed(const char *name, std::string &string)
{ 
	currentNode_->addChild(new XMLNode(formName(name), string));
}

void XmlNamedNetBuffer::addToBufferNamed(const char *name, const std::string &string)
{ 
	currentNode_->addChild(new XMLNode(formName(name), string));
}

void XmlNamedNetBuffer::addToBufferNamed(const char *name, LangString &string)
{ 
	currentNode_->addChild(new XMLNode(formName(name), string));
}

void XmlNamedNetBuffer::addToBufferNamed(const char *name, const LangString &string)
{ 
	currentNode_->addChild(new XMLNode(formName(name), string));
}

void XmlNamedNetBuffer::addToBufferNamed(const char *name, const char add)
{ 
	currentNode_->addChild(new XMLNode(formName(name), (int) add));
}

void XmlNamedNetBuffer::addToBufferNamed(const char *name, const unsigned char add)
{ 
	currentNode_->addChild(new XMLNode(formName(name), (int) add));
}

void XmlNamedNetBuffer::addToBufferNamed(const char *name, const int add)
{ 
	currentNode_->addChild(new XMLNode(formName(name), add));
}

void XmlNamedNetBuffer::addToBufferNamed(const char *name, const float add)
{ 
	currentNode_->addChild(new XMLNode(formName(name), add));
}

void XmlNamedNetBuffer::addToBufferNamed(const char *name, const bool add)
{ 
	currentNode_->addChild(new XMLNode(formName(name), add));
}

void XmlNamedNetBuffer::addToBufferNamed(const char *name, const unsigned int add)
{ 
	currentNode_->addChild(new XMLNode(formName(name), add));
}

void XmlNamedNetBuffer::addToBufferNamed(const char *name, const fixed add)
{ 
	currentNode_->addChild(new XMLNode(formName(name), add));
}

void XmlNamedNetBuffer::addToBufferNamed(const char *name, NetBuffer &add)
{ 
	currentNode_->addChild(new XMLNode(formName(name), add.getBufferUsed()));
}

const char *XmlNamedNetBuffer::formName(const char *name)
{
	return name;
}

