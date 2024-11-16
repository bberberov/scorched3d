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

#include <sound/SoundBufferStaticSourceInstance.h>
#include <sound/Sound.h>
#ifdef __DARWIN__
#include <OpenAL/al.h>
#else
#include <AL/al.h>
#include <AL/alut.h>
#endif

SoundBufferStaticSourceInstance::SoundBufferStaticSourceInstance(
	unsigned int source, unsigned int buffer) :
	SoundBufferSourceInstance(source), buffer_(buffer)
{
}

SoundBufferStaticSourceInstance::~SoundBufferStaticSourceInstance()
{
}

void SoundBufferStaticSourceInstance::play(bool repeat)
{
	if (!buffer_) return;

	alSourcei(source_, AL_BUFFER, 0);
    alSourcei(source_, AL_BUFFER, buffer_);
	alSourcei(source_, AL_LOOPING, (repeat?AL_TRUE:AL_FALSE));
	alSourcePlay(source_);
}

void SoundBufferStaticSourceInstance::stop()
{
	if (!buffer_) return;
	alSourceStop(source_);
}

void SoundBufferStaticSourceInstance::simulate(bool repeat)
{
}
