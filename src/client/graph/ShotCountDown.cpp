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

#include <graph/ShotCountDown.h>
#include <client/ScorchedClient.h>
#include <client/ClientState.h>
#include <common/OptionsScorched.h>
#include <common/DefinesString.h>
#include <graph/OptionsDisplay.h>
#include <GLEXT/GLViewPort.h>
#include <GLW/GLWFont.h>
#include <GLW/GLWidget.h>
#include <GLW/GLWColors.h>
#include <GLW/GLWToolTip.h>
#include <GLEXT/GLTexture.h>
#include <sound/Sound.h>
#include <sound/SoundUtils.h>
#include <target/TargetContainer.h>
#include <tank/TankAvatar.h>
#include <lang/LangResource.h>

ShotCountDown *ShotCountDown::instance_ = 0;

ShotCountDown *ShotCountDown::instance()
{
	if (!instance_)
	{
		instance_ = new ShotCountDown;
	}
	return instance_;
}

ShotCountDown::ShotCountDown() : 
	GameStateI("ShotCountDown"),
	moveTip_(ToolTipResource(ToolTip::ToolTipHelp,
		"ROUND_TIMER", "Round Timer",
		"ROUND_TIMER_TOOLTIP",
		"Remaining time for the current player to make a move."))
{
	move.show_ = false;
	round.show_ = false;
}

ShotCountDown::~ShotCountDown()
{
}

void ShotCountDown::showMoveTime(fixed timer, TimerType type, unsigned int playerId)
{
	move.show_ = true;
	move.timer_ = timer;
	move.type_ = type;
	move.playerId_ = playerId;
	if (move.timer_ < 0) move.timer_ = 0;
}

void ShotCountDown::showRoundTime(fixed timer)
{
	round.show_ = true;
	round.timer_ = timer;
	if (round.timer_ < 0) round.timer_ = 0;
}

void ShotCountDown::simulateTime(fixed simTime)
{
	if (!OptionsDisplay::instance()->getNoCountDownSound() &&
		move.show_)
	{
		if (move.timer_ > 0 && move.timer_.asInt() <= 6)
		{
			if ((move.timer_ - simTime) <= 0)
			{
				CACHE_SOUND(sound, S3D::getModFile("data/wav/misc/beep2.wav"));
				SoundUtils::playRelativeSound(VirtualSoundPriority::eText, sound);
			}
			else if ((move.timer_.asInt() > (move.timer_ - simTime).asInt()))
			{
				CACHE_SOUND(sound, S3D::getModFile("data/wav/misc/beep.wav"));
				SoundUtils::playRelativeSound(VirtualSoundPriority::eText, sound);
			}
		}
	}

	round.timer_ -= simTime;
	move.timer_ -= simTime;
}

void ShotCountDown::hideMoveTime()
{
	move.show_ = false;
}

void ShotCountDown::hideRoundTime()
{
	round.show_ = false;
}

void ShotCountDown::draw(const unsigned currentstate)
{
	if (move.show_) 
	{
		drawMove();
	}
}

void ShotCountDown::drawMove()
{
	float wHeight = (float) GLViewPort::getHeight();
	float wWidth = (float) GLViewPort::getWidth();

	GLState state(GLState::BLEND_ON | GLState::TEXTURE_ON | GLState::DEPTH_OFF); 

	static Vector yellow(0.85f, 0.85f, 0.3f);
	static Vector darkYellow(0.7f, 0.7f, 0.2f);
	static Vector red(0.7f, 0.0f, 0.0f);

	std::string str = "-";
	Vector *fontColor = &darkYellow;

	if (move.timer_ > 0)
	{
		if (move.timer_ <= 5)
		{
			fontColor = &red;

			str = S3D::formatStringBuffer("%.1f", 
				move.timer_.asFloat());		
		}
		else
		{
			fontColor = &yellow;

			int timeLeft = move.timer_.asInt();
			div_t split = div(timeLeft, 60);
			str = S3D::formatStringBuffer("%02i:%02i", 
				split.quot,
				split.rem);
		}
	}
	else
	{
		return;
	}

	float width = GLWFont::instance()->getGameFont()->getWidth(20, str);	

	GLWFont::instance()->getGameShadowFont()->draw(
		GLWColors::black, 20, (wWidth/2.0f) - (width / 2) - 2,
		wHeight - 25.0f + 2, 0.0f, 
		str);
	GLWFont::instance()->getGameFont()->draw(
		*fontColor, 20, (wWidth/2.0f) - (width / 2),
		wHeight - 25.0f, 0.0f, 
		str);

	GLWToolTip::instance()->addToolTip(&moveTip_,
		(wWidth/2.0f) - (width / 2), wHeight - 25.0f, width, 20.0f);
}

bool ShotCountDown::getRoundTime(std::string &str)
{
	if (!round.show_) return false;

	str = "-";
	if (round.timer_ > 0)
	{
		if (round.timer_ <= 5)
		{
			str = S3D::formatStringBuffer("%.1f", 
				round.timer_.asFloat());		
		}
		else
		{
			int timeLeft = round.timer_.asInt();
			div_t split = div(timeLeft, 60);
			str = S3D::formatStringBuffer("%02i:%02i", 
				split.quot,
				split.rem);
		}
	}
	return true;
}
