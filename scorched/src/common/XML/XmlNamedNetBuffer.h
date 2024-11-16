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

#ifndef _XmlNamedNetBuffer_H_
#define _XmlNamedNetBuffer_H_

#include <net/NetBuffer.h>
#include <lang/LangString.h>
#include <common/FixedVector4.h>
#include <common/FixedVector.h>
#include <XML/XMLNode.h>

class XmlNamedNetBuffer : public NamedNetBuffer
{
public:
	XmlNamedNetBuffer();
	virtual ~XmlNamedNetBuffer();

	void clear();

	virtual void startSection(const char *name);
	virtual void stopSection(const char *name);

	virtual void addToBufferNamed(const char *name, Vector &vector);
	virtual void addToBufferNamed(const char *name, FixedVector &vector);
	virtual void addToBufferNamed(const char *name, FixedVector4 &vector);
	virtual void addToBufferNamed(const char *name, const char *add);
	virtual void addToBufferNamed(const char *name, std::string &string);
	virtual void addToBufferNamed(const char *name, const std::string &string);
	virtual void addToBufferNamed(const char *name, LangString &string);
	virtual void addToBufferNamed(const char *name, const LangString &string);
	virtual void addToBufferNamed(const char *name, const char add);
	virtual void addToBufferNamed(const char *name, const unsigned char add);
	virtual void addToBufferNamed(const char *name, const int add);
	virtual void addToBufferNamed(const char *name, const float add);
	virtual void addToBufferNamed(const char *name, const bool add);
	virtual void addToBufferNamed(const char *name, const unsigned int add);
	virtual void addToBufferNamed(const char *name, const fixed add);
	virtual void addToBufferNamed(const char *name, NetBuffer &add);

	XMLNode &getBuffer() { return rootNode_; }

protected:
	XMLNode rootNode_, *currentNode_;

	const char *formName(const char *name);
};

#endif /* _XmlNamedNetBuffer_H_ */
