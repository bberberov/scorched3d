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

#if !defined(__INCLUDE_RandomGeneratorh_INCLUDE__)
#define __INCLUDE_RandomGeneratorh_INCLUDE__

#include <common/fixed.h>
#include <net/NetBuffer.h>

class RandomGenerator
{
public:
	virtual ~RandomGenerator();

	virtual unsigned int getRandUInt(const char *source) = 0;
	virtual fixed getRandFixed(const char *source) = 0;
};

class ScorchedContext;
class FileRandomGenerator : public RandomGenerator
{
public:
	FileRandomGenerator();
	virtual ~FileRandomGenerator();

	void setScorchedContext(ScorchedContext *context) { context_ = context; }

	void seed(unsigned int seed);
	unsigned int getSeed();

	virtual unsigned int getRandUInt(const char *source);
	virtual fixed getRandFixed(const char *source);

	bool writeMessage(NetBuffer &buffer);
	bool readMessage(NetBufferReader &reader);

protected:
	static unsigned int bufferSize_;
	static unsigned int *buffer_;
	ScorchedContext *context_;
	unsigned int position_;

private:
	FileRandomGenerator(const FileRandomGenerator &other);
	FileRandomGenerator &operator=(const FileRandomGenerator &other);

};

#endif
