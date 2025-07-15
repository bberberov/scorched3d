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

#if !defined(__INCLUDE_ModelIDh_INCLUDE__)
#define __INCLUDE_ModelIDh_INCLUDE__

#include <string>

class NetBuffer;
class NetBufferReader;
class XMLNode;
class ModelID
{
public:
	ModelID();
	ModelID(const ModelID &other);
	virtual ~ModelID();

	ModelID &operator=(const ModelID &other);

	bool initFromNode(XMLNode *modelNode);

	bool initFromString(
		const char *type,
		const char *meshName,
		const char *skinName);

	// Not very generic but it will do for now!!
	const char *getStringHash();
	const char *getSkinName() { return skinName_.c_str(); }
	const char *getMeshName() { return meshName_.c_str(); }
	const char *getType() { return type_.c_str(); }
	bool modelValid() { return !type_.empty(); }

	// Serialize
	virtual bool writeModelID(NetBuffer &buffer);
	virtual bool readModelID(NetBufferReader &reader);

protected:
	std::string type_;
	std::string meshName_;
	std::string skinName_;
	std::string hash_;
};

#endif
