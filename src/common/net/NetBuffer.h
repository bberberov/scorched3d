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

#ifndef _NETBUFFER_H_
#define _NETBUFFER_H_

#include <lang/LangString.h>
#include <common/FixedVector4.h>
#include <common/FixedVector.h>
#include <SDL/SDL.h>
#ifdef __MACOSX__
#include <SDL_net/SDL_net.h>
#else
#include <SDL/SDL_net.h>
#endif

class NetBuffer;
class NamedNetBuffer
{
public:
	virtual void startSection(const char *name) {}
	virtual void stopSection(const char *name) {}

	virtual void addToBufferNamed(const char *name, Vector &vector) = 0;
	virtual void addToBufferNamed(const char *name, FixedVector &vector) = 0;
	virtual void addToBufferNamed(const char *name, FixedVector4 &vector) = 0;
	virtual void addToBufferNamed(const char *name, const char *add) = 0;
	virtual void addToBufferNamed(const char *name, std::string &string) = 0;
	virtual void addToBufferNamed(const char *name, const std::string &string) = 0;
	virtual void addToBufferNamed(const char *name, LangString &string) = 0;
	virtual void addToBufferNamed(const char *name, const LangString &string) = 0;
	virtual void addToBufferNamed(const char *name, const char add) = 0;
	virtual void addToBufferNamed(const char *name, const unsigned char add) = 0;
	virtual void addToBufferNamed(const char *name, const int add) = 0;
	virtual void addToBufferNamed(const char *name, const float add) = 0;
	virtual void addToBufferNamed(const char *name, const bool add) = 0;
	virtual void addToBufferNamed(const char *name, const unsigned int add) = 0;
	virtual void addToBufferNamed(const char *name, const fixed add) = 0;
	virtual void addToBufferNamed(const char *name, NetBuffer &add) = 0;
};

class NamedNetBufferSection
{
public:
	NamedNetBufferSection(NamedNetBuffer &buffer, const char *name);
	virtual ~NamedNetBufferSection();

protected:
	NamedNetBuffer &buffer_;
	const char *name_;
};

class NetBuffer : public NamedNetBuffer
{
public:
	NetBuffer();
	NetBuffer(unsigned startSize, void *startBuffer = 0);
	NetBuffer(const NetBuffer &other);
	virtual ~NetBuffer();

	void reset();
	void clear();
	void resize(unsigned newBufferSize);
	void allocate(unsigned size);
	void setBufferUsed(unsigned size) { usedSize_ = size; }

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

	void addToBuffer(Vector &vector);
	void addToBuffer(FixedVector &vector);
	void addToBuffer(FixedVector4 &vector);
	void addToBuffer(const char *add);
	void addToBuffer(std::string &string);
	void addToBuffer(const std::string &string);
	void addToBuffer(LangString &string);
	void addToBuffer(const LangString &string);
	void addToBuffer(const char add);
	void addToBuffer(const unsigned char add);
	void addToBuffer(const int add);
	void addToBuffer(const float add);
	void addToBuffer(const bool add);
	void addToBuffer(const unsigned int add);
	void addToBuffer(const fixed add);
	void addToBuffer(NetBuffer &add);

	bool compressBuffer();
	bool uncompressBuffer();

	char *getBuffer() { return buffer_; }
	unsigned getTotalBufferSize() { return bufferSize_; }
	unsigned getBufferUsed() { return usedSize_; }
	unsigned getCrc();

	// Adds raw data to into the buffer
	// NOTE: Care must be taken to ensure that the added data
	// is in network byte ordering
	void addDataToBuffer(const void *add, unsigned len);

protected:
	char *buffer_;
	unsigned usedSize_;
	unsigned bufferSize_;
};

class NetBufferReader
{
public:
	NetBufferReader();
	NetBufferReader(NetBuffer &buffer);
	virtual ~NetBufferReader();

	void reset();

	unsigned getBufferSize() { return bufferSize_; }
	unsigned getReadSize() { return readSize_; }
	char *getBuffer() { return buffer_; }

	void setBuffer(char *buffer) { buffer_ = buffer; }
	void setReadSize(unsigned size) { readSize_ = size; }
	void setBufferSize(unsigned size) { bufferSize_ = size; }

	bool getFromBuffer(Vector &result);
	bool getFromBuffer(FixedVector &result);
	bool getFromBuffer(FixedVector4 &result);
	bool getFromBuffer(fixed &result);
	bool getFromBuffer(char &result);
	bool getFromBuffer(unsigned char &result);
	bool getFromBuffer(int &result);
	bool getFromBuffer(float &result);
	bool getFromBuffer(bool &result);
	bool getFromBuffer(unsigned int &result);
	bool getFromBuffer(std::string &string, bool safe = true);
	bool getFromBuffer(LangString &string);
	bool getFromBuffer(NetBuffer &buffer);

	// Gets raw data from the buffer
	// NOTE: Care must be taken to ensure that the data
	// is in network byte ordering	
	bool getDataFromBuffer(void *dest, int len);

protected:
	char *buffer_;
	unsigned bufferSize_;
	unsigned readSize_;

};

#endif /* _NETBUFFER_H_ */
