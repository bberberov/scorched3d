////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011, 2025
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

#include <sound/VirtualSoundSource.h>
#include <sound/PlayingSoundSource.h>
#include <sound/SoundSource.h>
#include <sound/Sound.h>

VirtualSoundSource::VirtualSoundSource( unsigned int priority, bool looping, bool managed) :
	priority_(priority),
	playingSource_(nullptr),
	buffer_(nullptr),
	gain_(1.0f),
	refDist_(75.0f),
	rolloff_(1.0f),
	relative_(false),
	looping_(looping),
	managed_(managed)
{
	if (managed_) Sound::instance()->addManaged(this);
}

VirtualSoundSource::~VirtualSoundSource()
{
	stop();
	playingSource_ = nullptr;
}

void VirtualSoundSource::play(SoundBuffer *buffer)
{
	stop();

	buffer_ = buffer;
	Sound::instance()->addPlaying(this);
}

void VirtualSoundSource::actualPlay()
{
	playingSource_->getActualSource()->setGain(gain_);
	playingSource_->getActualSource()->setRolloff(rolloff_);
	playingSource_->getActualSource()->setReferenceDistance(refDist_);
	playingSource_->getActualSource()->setRelative(relative_);
	playingSource_->getActualSource()->setPosition(position_);
	playingSource_->getActualSource()->setVelocity(velocity_);
	playingSource_->getActualSource()->play(buffer_, looping_);
}

void VirtualSoundSource::stop()
{
	if (playingSource_)
	{
		Sound::instance()->removePlaying(this);
	}
}

void VirtualSoundSource::setPlayingSource(PlayingSoundSource *s)
{
	playingSource_ = s;
}

bool VirtualSoundSource::getPlaying()
{
	if (playingSource_ && playingSource_->getActualSource())
	{
		return playingSource_->getActualSource()->getPlaying();
	}
	return false;
}

void VirtualSoundSource::setRelative()
{
	relative_ = true;
	if (playingSource_ && playingSource_->getActualSource())
	{
		playingSource_->getActualSource()->setRelative(true);
	}
}

void VirtualSoundSource::setPosition(Vector &position)
{
	position_ = position;
	if (playingSource_ && playingSource_->getActualSource())
	{
		playingSource_->getActualSource()->setPosition(position);
	}
}

void VirtualSoundSource::setVelocity(Vector &velocity)
{
	velocity_ = velocity;
	if (playingSource_ && playingSource_->getActualSource())
	{
		playingSource_->getActualSource()->setVelocity(velocity);
	}
}

void VirtualSoundSource::setGain(float gain)
{
	gain_ = gain;
	if (playingSource_ && playingSource_->getActualSource())
	{
		playingSource_->getActualSource()->setGain(gain);
	}
}

void VirtualSoundSource::setReferenceDistance(float refDist)
{
	refDist_ = refDist;
	if (playingSource_ && playingSource_->getActualSource())
	{
		playingSource_->getActualSource()->setReferenceDistance(refDist);
	}
}

void VirtualSoundSource::setRolloff(float rolloff)
{
	rolloff_ = rolloff;
	if (playingSource_ && playingSource_->getActualSource())
	{
		playingSource_->getActualSource()->setRolloff(rolloff);
	}
}

void VirtualSoundSource::updateDistance(Vector &listener)
{
	Vector pos = getPosition();
	if (!getRelative())
	{
		pos -= listener;
	}
	distance_ = pos[0] * pos[0] + pos[1] * pos[1] + pos[2] * pos[2];
}
