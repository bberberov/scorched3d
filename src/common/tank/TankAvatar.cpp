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

#include <tank/TankAvatar.h>
#include <common/Defines.h>
#include <image/ImagePngFactory.h>
#include <stdio.h>
#include <zlib.h>

#ifndef S3D_SERVER
#include <GLEXT/GLTexture.h>

GLTexture *TankAvatar::defaultTexture_ = 0;
std::list<TankAvatar::AvatarStore> TankAvatar::storeEntries_;
#endif

static NetBuffer tmpBuffer;

TankAvatar::TankAvatar()
{
#ifndef S3D_SERVER
	texture_ = 0;
#endif
	file_ = new NetBuffer();
}

TankAvatar::~TankAvatar()
{
	delete file_;
}

bool TankAvatar::writeMessage(NamedNetBuffer &buffer)
{
	NamedNetBufferSection section(buffer, "TankAvatar");

	buffer.addToBufferNamed("name", name_);
	buffer.addToBufferNamed("file", *file_);
	return true;
}

void TankAvatar::clear()
{
#ifndef S3D_SERVER
	texture_ = 0;
#endif
	name_ = "";
	file_->reset();
}

bool TankAvatar::readMessage(NetBufferReader &reader)
{
	std::string name;
	if (!reader.getFromBuffer(name)) return false;
	if (!reader.getFromBuffer(tmpBuffer)) return false;
	setFromBuffer(name, tmpBuffer);
	return true;
}

bool TankAvatar::loadFromFile(const std::string &fileName)
{
	FILE *in = fopen(fileName.c_str(), "rb");
	if (in)
	{
		name_ = fileName;
		tmpBuffer.reset();
		unsigned char readBuf[512];
		while (unsigned int size = (unsigned int) fread(readBuf, sizeof(unsigned char), 512, in))
		{
			tmpBuffer.addDataToBuffer(readBuf, size);
		}
		fclose(in);
		return setFromBuffer(fileName, tmpBuffer);
	}
	return false;
}

unsigned int TankAvatar::getCrc()
{
	unsigned int crc =  crc32(0L, Z_NULL, 0);
	crc = crc32(crc, (unsigned char *) 
		file_->getBuffer(), file_->getBufferUsed());	
	return crc;
}

bool TankAvatar::setFromBuffer(const std::string &fileName, NetBuffer &buffer)
{
	if (buffer.getBufferUsed() == 0) return false;

	Image png = ImagePngFactory::loadFromBuffer(buffer);
	if (!png.getBits()) return false;
	if (png.getWidth() != 32 || 
		png.getHeight() != 32) return false;

	name_ = fileName;
	file_->reset();
	file_->addDataToBuffer(buffer.getBuffer(), 
		buffer.getBufferUsed());

#ifndef S3D_SERVER
	{
		texture_ = 0;
		unsigned int crc = getCrc();
		std::list<AvatarStore>::iterator itor;
		for (itor = storeEntries_.begin();
			itor != storeEntries_.end();
			++itor)
		{
			AvatarStore &store = (*itor);
			if (store.crc_ == crc &&
				0 == strcmp(store.name_.c_str(), name_.c_str()))
			{
				texture_ = store.texture_;
				break;
			}
		}
		if (!texture_)
		{
			texture_ = new GLTexture;
			texture_->create(png);
			AvatarStore store;
			store.crc_ = crc;
			store.name_ = name_;
			store.texture_ = texture_;
			storeEntries_.push_back(store);
		}
	}
#endif

	return true;
}

#ifndef S3D_SERVER
GLTexture *TankAvatar::getDefaultTexture()
{
	if (!defaultTexture_)
	{
		defaultTexture_ = new GLTexture();
		std::string file = S3D::getDataFile("data/avatars/player.png");
		Image png = ImagePngFactory::loadFromFile(file.c_str());
		defaultTexture_->create(png);
	}
	return defaultTexture_;
}

GLTexture *TankAvatar::getTexture()
{
	if (!texture_) return getDefaultTexture();
	return texture_; 
}
#endif
