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

#if !defined(__INCLUDE_SoundBufferDynamicOVh_INCLUDE__)
#define __INCLUDE_SoundBufferDynamicOVh_INCLUDE__

#ifdef HAVE_OGG

#include <sound/SoundBuffer.h>
#ifdef __DARWIN__
#include <OpenAL/al.h>
#else
#include <AL/al.h>
#endif
#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

class SoundBufferDynamicOVSourceInstance : public SoundBufferSourceInstance
{
public:
	SoundBufferDynamicOVSourceInstance(
		unsigned int source, const char *fileName);
	virtual ~SoundBufferDynamicOVSourceInstance();

	virtual void play(bool loop);
	virtual void stop();
	virtual void simulate(bool loop);

protected:
	unsigned int buffers_[2];
	ALenum format_;
	OggVorbis_File oggStream_;
	vorbis_info *vorbisInfo_;

	bool addDataToBuffer(unsigned int buffer, bool loop);
};

#endif // HAVEOGG

#endif // __INCLUDE_SoundBufferDynamicOVh_INCLUDE__
