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

#include <sound/Sound.h>
#include <sound/SoundBufferFactory.h>
#include <sound/SoundBufferEmpty.h>
#include <sound/SoundBufferWav.h>
#include <sound/SoundBufferOV.h>
#include <common/Defines.h>

SoundBuffer *SoundBufferFactory::createBuffer(const char *fileName)
{
	if (Sound::instance()->getInit())
	{
		int len = (int) strlen(fileName);
		if (len >= 3)
		{
			if (0 == strcmp(&fileName[len-3], "wav"))
			{
				return new SoundBufferWav(fileName);
			}
			else if (0 == strcmp(&fileName[len-3], "ogg"))
			{
#ifdef HAVE_OGG
				return new SoundBufferOV(fileName);
#else
				return new SoundBufferEmpty(fileName);
#endif // HAVE_OGG			
			}
			else
			{
				S3D::dialogExit("Scorched3D",
					S3D::formatStringBuffer("Error: Unknown sound file type \"%s\"",
					fileName));
			}
		}
	}

	return new SoundBufferEmpty(fileName);
}
