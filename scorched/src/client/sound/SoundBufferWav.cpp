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

#include <sound/SoundBufferWav.h>
#include <sound/SoundBufferStaticSourceInstance.h>
#include <sound/Sound.h>
#ifdef __DARWIN__
#include <OpenAL/al.h>
#include <OpenAL/alut.h>
#else
#include <AL/al.h>
#include <AL/alut.h>
#endif

SoundBufferWav::SoundBufferWav(const char *fileName) : 
	SoundBuffer(fileName),
	buffer_(0)
{
	unsigned int error;

	// Create a buffer
	alGetError();
	alGenBuffers(1, &buffer_);
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		return;
	}

	// Load WAV
	void *data;
	ALenum format;
	ALsizei size;
	ALsizei freq;
	ALboolean loop;

#ifdef __DARWIN__
	alutLoadWAVFile((ALbyte*) fileName,&format,&data,&size,&freq);
#else
	alutLoadWAVFile((ALbyte*) fileName,&format,&data,&size,&freq,&loop);
#endif

	if ((error = alGetError()) != AL_NO_ERROR)
	{
		return;
	}

	// Load WAV into buffer
	alBufferData(buffer_,format,data,size,freq);
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		return;
	}

	// Delete WAV memory
	alutUnloadWAV(format,data,size,freq);
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		return;
	}
}

SoundBufferWav::~SoundBufferWav()
{
	if (buffer_) alDeleteBuffers (1, &buffer_);
	buffer_ = 0;
}

SoundBufferSourceInstance *SoundBufferWav::createSourceInstance(unsigned int source)
{
	return new SoundBufferStaticSourceInstance(source, buffer_);
}
