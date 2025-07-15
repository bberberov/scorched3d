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

#include <sound/SoundBufferDynamicOVSourceInstance.h>
#include <sound/SoundBufferOV.h>
#include <common/Defines.h>
#include <common/Logger.h>

#ifdef HAVE_OGG

SoundBufferDynamicOVSourceInstance::SoundBufferDynamicOVSourceInstance(
	unsigned int source, const char *fileName) :
	SoundBufferSourceInstance(source)
{
	// Open stream
	if (!SoundBufferOV::openStream(fileName, oggStream_)) return;
	vorbisInfo_ = ov_info(&oggStream_, -1);

	alGenBuffers(2, buffers_);
	if (alGetError() != AL_NO_ERROR) return;
}

SoundBufferDynamicOVSourceInstance::~SoundBufferDynamicOVSourceInstance()
{
	if (buffers_[0]) 
	{
		if (alIsBuffer(buffers_[0]) || 
			alIsBuffer(buffers_[1]))
		{
			stop();

			alGetError();
			alDeleteBuffers(2, buffers_);

			int errorNum = alGetError();
			if(errorNum != AL_NO_ERROR)
			{
				Logger::log("ERROR deleting AL buffers");
			}
		}
	}
	ov_clear(&oggStream_);
}

void SoundBufferDynamicOVSourceInstance::play(bool repeat)
{
	ov_raw_seek(&oggStream_, 0);
	
	if (!addDataToBuffer(buffers_[0], repeat)) return;
	if (!addDataToBuffer(buffers_[1], repeat)) return;

	alSourcei(source_, AL_BUFFER, 0); // Reset the source
	alSourcei(source_, AL_LOOPING, AL_FALSE);

	int error = 0;
    alSourceQueueBuffers(source_, 2, buffers_);
	if ((error = alGetError()) != AL_NO_ERROR) return;
	alSourcePlay(source_);
	if ((error = alGetError()) != AL_NO_ERROR) return;
}

void SoundBufferDynamicOVSourceInstance::stop()
{
	alSourceStop(source_);

    int queued = 2;
	while(queued--)
	{        
		ALuint buffer;            
		alSourceUnqueueBuffers(source_, 1, &buffer);
		alGetError();
	}
}

void SoundBufferDynamicOVSourceInstance::simulate(bool repeat)
{
    int processed = 0;
	alGetSourcei(source_, AL_BUFFERS_PROCESSED, &processed);
	if (processed == 2)
	{
		Logger::log("Warning: OGG falling behind");
	}
	while(processed--)
	{
		//Logger::log(S3D::formatStringBuffer("Processed %u %i", source_, processed));

		ALuint buffer;                
		alSourceUnqueueBuffers(source_, 1, &buffer);
		if (addDataToBuffer(buffer, repeat))
		{
			alSourceQueueBuffers(source_, 1, &buffer);
		}
	}
}

bool SoundBufferDynamicOVSourceInstance::addDataToBuffer(unsigned int buffer, bool loop)
{
    static char data[OGG_BUFFER_SIZE];    
	int size = SoundBufferOV::readData(oggStream_, data, OGG_BUFFER_SIZE);
	if(size == 0)
	{
		if (!loop) return false;
		else
		{
			ov_raw_seek(&oggStream_, 0);
			return addDataToBuffer(buffer, false);
		}
	}

	SoundBufferOV::addDataToBuffer(vorbisInfo_, buffer, data, size);
	return true;
}

#endif // HAVE_OGG

