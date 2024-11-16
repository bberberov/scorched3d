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

#ifndef _SOUNDBUFFER_H_
#define _SOUNDBUFFER_H_

#include <string>

class SoundBufferSourceInstance
{
public:
	SoundBufferSourceInstance(unsigned int source);
	virtual ~SoundBufferSourceInstance();

	virtual void play(bool loop) = 0;
	virtual void stop() = 0;
	virtual void simulate(bool loop) = 0;

protected:
	unsigned int source_;
};

class SoundBuffer  
{
public:
	SoundBuffer(const char *fileName);
	virtual ~SoundBuffer();

	virtual SoundBufferSourceInstance *createSourceInstance(unsigned int source) = 0;
	const char *getFileName() { return fileName_.c_str(); }

protected:
	std::string fileName_;
};

#endif /* _SOUNDBUFFER_H_ */
