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

#if !defined(__INCLUDE_TankAvatarh_INCLUDE__)
#define __INCLUDE_TankAvatarh_INCLUDE__

#include <string>
#include <net/NetBuffer.h>
#include <list>

class GLTexture;
class TankAvatar
{
public:
	TankAvatar();
	virtual ~TankAvatar();
	
	bool writeMessage(NamedNetBuffer &buffer);
	bool readMessage(NetBufferReader &reader);

	void clear();
	bool loadFromFile(const std::string &fileName);
	bool setFromBuffer(const std::string &fileName, NetBuffer &buffer);

#ifndef S3D_SERVER
	GLTexture *getTexture();
	static GLTexture *getDefaultTexture();
#endif

	const char *getName() { return name_.c_str(); }
	NetBuffer &getFile() { return *file_; }
	unsigned int getCrc();
	
protected:
#ifndef S3D_SERVER
	static GLTexture *defaultTexture_;
	class AvatarStore
	{
	public:
		GLTexture *texture_;
		unsigned int crc_;
		std::string name_;
	};
	static std::list<AvatarStore> storeEntries_;
	GLTexture *texture_;
#endif

	NetBuffer *file_;
	std::string name_;
};

#endif // __INCLUDE_TankAvatarh_INCLUDE__
