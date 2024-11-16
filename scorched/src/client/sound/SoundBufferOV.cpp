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

#include <sound/SoundBufferOV.h>
#include <sound/SoundBufferStaticSourceInstance.h>
#include <sound/SoundBufferDynamicOVSourceInstance.h>
#include <sound/Sound.h>
#ifdef __DARWIN__
#include <OpenAL/al.h>
#else
#include <AL/al.h>
#include <AL/alut.h>
#endif

#ifdef HAVE_OGG

SoundBufferOV::SoundBufferOV(const char *fileName) : 
	SoundBuffer(fileName),
	buffer_(0)
{
	// Opean stream
	OggVorbis_File oggStream;
	if (!openStream(fileName, oggStream)) return;

	// Read file
	ov_raw_seek(&oggStream, 0);
	int dataBufferSize = OGG_BUFFER_SIZE * 2;
	static char dataBuffer[OGG_BUFFER_SIZE * 2];
	int size = readData(oggStream, dataBuffer, dataBufferSize);

	// Check if can fit in one go
	if (size < dataBufferSize)
	{
		int error = 0;

		// Create a buffer
		alGetError();
		alGenBuffers(1, &buffer_);
		if ((error = alGetError()) != AL_NO_ERROR)
		{
			return;
		}

		vorbis_info *vorbisInfo = ov_info(&oggStream, -1);
		addDataToBuffer(vorbisInfo, buffer_, dataBuffer, size);
	}
	else
	{
		// Dynamic
		buffer_ = 0;
	}

	// Cleanup ogg
	ov_clear(&oggStream);
}

bool SoundBufferOV::openStream(const char *fileName, OggVorbis_File &oggStream)
{
	ov_callbacks callbacks = {
		read_func,
		seek_func,
		close_func,
		tell_func 
	};

	// Open file
	FILE *oggFile = fopen(fileName, "rb");
	if (!oggFile)
	{
		S3D::dialogMessage("OGG Vorbis",
			S3D::formatStringBuffer("Could not open ogg file \"%s\"",
			fileName));
		return false;
	}

	// Open stream
	int result = ov_open_callbacks((void *) oggFile, &oggStream, 0, 0, callbacks);
	if(result < 0)
	{        
		fclose(oggFile);
		S3D::dialogMessage("OGG Vorbis",
			S3D::formatStringBuffer("Could not open ogg stream \"%s\" : %s",
			fileName, errorString(result)));
		return false; 
	}

	return true;
}

int SoundBufferOV::readData(OggVorbis_File &oggStream, char *buffer, int bufferSize)
{    
	int size = 0;    
	int section;    
	while(size < bufferSize)
	{        
		int result = ov_read(&oggStream, buffer + size, 
			bufferSize - size, ((SDL_BYTEORDER == SDL_LIL_ENDIAN)?0:1), 2, 1, &section);            
		if (result > 0) size += result;        
		else if (result < 0) return -1;
		else break;    
	}
	return size;
}

bool SoundBufferOV::addDataToBuffer(vorbis_info *vorbisInfo, int buffer, char *dataBuffer, int size)
{
	ALenum format;
    if(vorbisInfo->channels == 1) format = AL_FORMAT_MONO16;
	else format = AL_FORMAT_STEREO16;

	// Load OV into buffer
	int error = 0;
	alGetError();
	alBufferData(buffer, format, dataBuffer, size, vorbisInfo->rate);  
	if ((error = alGetError()) != AL_NO_ERROR)
	{
		return false;
	}

	return true;
}

const char *SoundBufferOV::errorString(int code)
{    
	switch(code)    
	{        
	case OV_EREAD:            
		return ("Read from media.");        
	case OV_ENOTVORBIS:            
		return ("Not Vorbis data.");        
	case OV_EVERSION:            
		return ("Vorbis version mismatch.");        
	case OV_EBADHEADER:            
		return ("Invalid Vorbis header.");        
	case OV_EFAULT:            
		return ("Internal logic fault (bug or heap/stack corruption.");        
	default:            
		return ("Unknown Ogg error.");    
	}
}

size_t SoundBufferOV::read_func(void *ptr, size_t size, size_t nmemb, void *datasource)
{
	return fread(ptr, size, nmemb, (FILE *) datasource);
}

int SoundBufferOV::seek_func(void *datasource, ogg_int64_t offset, int whence)
{
	return fseek((FILE*) datasource, (long) offset, whence);
}

int SoundBufferOV::close_func(void *datasource)
{
	return fclose((FILE *) datasource);
}

long SoundBufferOV::tell_func(void *datasource)
{
	return ftell((FILE *) datasource);
}

SoundBufferOV::~SoundBufferOV()
{
	if (buffer_) alDeleteBuffers (1, &buffer_);
	buffer_ = 0;
}

SoundBufferSourceInstance *SoundBufferOV::createSourceInstance(unsigned int source)
{
	if (buffer_ == 0)
	{
		return new SoundBufferDynamicOVSourceInstance(source, fileName_.c_str());
	}
	else
	{
		return new SoundBufferStaticSourceInstance(source, buffer_);
	}
}

#endif // HAVE_OGG
