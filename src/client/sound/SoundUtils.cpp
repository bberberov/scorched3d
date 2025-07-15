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

#include <sound/SoundUtils.h>
#include <sound/Sound.h>

void SoundUtils::playRelativeSound(unsigned int priority, 
	SoundBuffer *buffer)
{
	VirtualSoundSource *source = new VirtualSoundSource(priority, false, true);
	source->setRelative();
	source->play(buffer);
}

void SoundUtils::playAbsoluteSound(unsigned int priority, 
	SoundBuffer *buffer, Vector &position)
{
	VirtualSoundSource *source = new VirtualSoundSource(priority, false, true);
	source->setPosition(position);
	source->play(buffer);
}
