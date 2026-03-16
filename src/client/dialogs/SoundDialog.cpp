////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011, 2025, 2026
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

#include <dialogs/SoundDialog.hpp>
#include <graph/OptionsDisplay.hpp>
#include <sound/Sound.hpp>
#include <GLW/GLWTextButton.hpp>
#include <GLW/GLWWindowManager.hpp>
#include <GLW/GLWSlider.hpp>

SoundDialog *SoundDialog::instance_ = nullptr;

SoundDialog *SoundDialog::instance()
{
	if (nullptr == instance_)
	{
		instance_ = new SoundDialog();
	}
	return instance_;
}

SoundDialog::SoundDialog() :
	GLWWindow("Sound", 10.0f, 10.0f, 300.0f, 70.0f, eSmallTitle,
		"Allow the user to change sound settings")
{
	needCentered_ = true;

	{
		GLWPanel* soundVolumePanel = new GLWPanel( 0.0f, 0.0f, 0.0f, 0.0f, false, false );
		soundVolume_               = new GLWSlider( 0.0f, 0.0f, 300.0f );
		soundVolumePanel->addWidget(
			new GLWLabel( 0.0f, 0.0f, LANG_RESOURCE( "SOUND_VOLUME", "Sound Volume    " ) ),
			nullptr,
			SpaceRight,
			10.0f
		);
		soundVolumePanel->addWidget( soundVolume_, nullptr, AlignCenterLeftRight );
		soundVolumePanel->setLayout( GLWPanel::LayoutHorizontal );
		addWidget( soundVolumePanel, nullptr, SpaceLeft | SpaceRight | SpaceTop, 10.0f );
	}

	{
		GLWPanel* ambientSoundVolumePanel = new GLWPanel( 0.0f, 0.0f, 0.0f, 0.0f, false, false );
		ambientSoundVolume_               = new GLWSlider( 0.0f, 0.0f, 300.0f );
		ambientSoundVolumePanel->addWidget(
			new GLWLabel( 0.0f, 0.0f, LANG_RESOURCE( "AMBIENT_VOLUME", "Ambient Volume" ) ),
			nullptr,
			SpaceRight,
			10.0f
		);
		ambientSoundVolumePanel->addWidget( ambientSoundVolume_, nullptr, AlignCenterLeftRight );
		ambientSoundVolumePanel->setLayout( GLWPanel::LayoutHorizontal );
		addWidget( ambientSoundVolumePanel, nullptr, SpaceLeft | SpaceRight | SpaceTop, 10.0f );
	}

	{
		GLWPanel* musicVolumePanel = new GLWPanel( 0.0f, 0.0f, 0.0f, 0.0f, false, false );
		musicVolume_               = new GLWSlider( 0.0f, 0.0f, 300.0f );
		musicVolumePanel->addWidget(
			new GLWLabel( 0.0f, 0.0f, LANG_RESOURCE( "MUSIC_VOLUME", "Music Volume    " ) ),
			nullptr,
			SpaceRight,
			10.0f
		);
		musicVolumePanel->addWidget( musicVolume_, nullptr, AlignCenterLeftRight );
		musicVolumePanel->setLayout( GLWPanel::LayoutHorizontal );
		addWidget( musicVolumePanel, nullptr, SpaceLeft | SpaceRight | SpaceTop, 10.0f );
	}

	{
		GLWPanel* checkPanel = new GLWPanel( 0.0f, 0.0f, 0.0f, 0.0f, false, false );
		noSoundBox_          = new GLWCheckBoxText( 0.0f, 0.0f, LANG_RESOURCE( "NO_SOUND", "No Sound" ) );
		noMusicBox_          = new GLWCheckBoxText( 0.0f, 0.0f, LANG_RESOURCE( "NO_MUSIC", "No Music" ) );
		checkPanel->addWidget( noSoundBox_, nullptr, SpaceRight, 10.0f );
		checkPanel->addWidget( noMusicBox_ );
		checkPanel->setLayout( GLWPanel::LayoutHorizontal );
		addWidget( checkPanel, nullptr, SpaceLeft | SpaceRight | SpaceTop, 10.0f );
	}

	{
		GLWPanel* optionPanel = new GLWPanel( 0.0f, 0.0f, 0.0f, 0.0f, false, false );
		noCountDownSoundBox_  = new GLWCheckBoxText(
			0.0f,
			0.0f,
			LANG_RESOURCE( "NO_COUNT_DOWN_SOUND", "No Count Down Sound" )
		);
		noChannelTextSoundBox_ = new GLWCheckBoxText(
			0.0f,
			0.0f,
			LANG_RESOURCE( "NO_CHANNEL_TEXT_SOUND", "No Channel Text Sound" )
		);
		optionPanel->addWidget( noCountDownSoundBox_ );
		optionPanel->addWidget( noChannelTextSoundBox_, nullptr, SpaceTop, 10.0f );
		optionPanel->setLayout( GLWPanel::LayoutVerticle );
		addWidget( optionPanel, nullptr, SpaceLeft | SpaceRight | SpaceTop, 10.0f );
	}

	GLWPanel*  buttonPanel  = new GLWPanel( 0.0f, 0.0f, 0.0f, 0.0f, false, false );
	GLWButton* cancelButton = new GLWTextButton(
		LANG_RESOURCE( "CANCEL", "Cancel" ),
		95,
		10,
		105,
		this,
		GLWButton::ButtonFlagCancel | GLWButton::ButtonFlagCenterX
	);
	GLWButton* applyButton = new GLWTextButton(
		LANG_RESOURCE( "APPLY", "Apply" ),
		235,
		10,
		110,
		this,
		GLWButton::ButtonFlagCenterX
	);
	GLWButton* okButton = new GLWTextButton(
		LANG_RESOURCE( "OK", "Ok" ),
		475,
		10,
		55,
		this,
		GLWButton::ButtonFlagOk | GLWButton::ButtonFlagCenterX
	);
	cancelId_ = cancelButton->getId();
	applyId_  = applyButton->getId();
	okId_     = okButton->getId();
	buttonPanel->addWidget( cancelButton, nullptr, SpaceRight, 10.0f );
	buttonPanel->addWidget( applyButton, nullptr, SpaceRight, 10.0f );
	buttonPanel->addWidget( okButton );
	buttonPanel->setLayout( GLWPanel::LayoutHorizontal );
	addWidget( buttonPanel, nullptr, SpaceAll, 10.0f );

	setLayout( GLWPanel::LayoutVerticle );
	layout();
}

SoundDialog::~SoundDialog()
{
}

void SoundDialog::display()
{
	GLWWindow::display();

	soundVolume_->setCurrent(
		float(OptionsDisplay::instance()->getSoundVolume()) / 1.28f);
	ambientSoundVolume_->setCurrent(
		float(OptionsDisplay::instance()->getAmbientSoundVolume()) / 1.28f);
	musicVolume_->setCurrent(
		float(OptionsDisplay::instance()->getMusicVolume()) / 1.28f);
	noSoundBox_->getCheckBox().setState(
		OptionsDisplay::instance()->getNoSound());
	noMusicBox_->getCheckBox().setState(
		OptionsDisplay::instance()->getNoMusic());
	noCountDownSoundBox_->getCheckBox().setState(
		OptionsDisplay::instance()->getNoCountDownSound());
	noChannelTextSoundBox_->getCheckBox().setState(
		OptionsDisplay::instance()->getNoChannelTextSound());
}

void SoundDialog::buttonDown(unsigned int id)
{
	if (id == okId_ || id == applyId_)
	{
		OptionsDisplay::instance()->getNoSoundEntry().setValue(
			noSoundBox_->getCheckBox().getState());
		OptionsDisplay::instance()->getNoMusicEntry().setValue(
			noMusicBox_->getCheckBox().getState());
		OptionsDisplay::instance()->getNoCountDownSoundEntry().setValue(
			noCountDownSoundBox_->getCheckBox().getState());
		OptionsDisplay::instance()->getNoChannelTextSoundEntry().setValue(
			noChannelTextSoundBox_->getCheckBox().getState());

		int volume = int(soundVolume_->getCurrent() * 1.28f);
		OptionsDisplay::instance()->getSoundVolumeEntry().setValue(volume);
		Sound::instance()->getDefaultListener()->setGain(float(volume) / 128.0f);

		volume = int(musicVolume_->getCurrent() * 1.28f);
		OptionsDisplay::instance()->getMusicVolumeEntry().setValue(volume);

		volume = int(ambientSoundVolume_->getCurrent() * 1.28f);
		OptionsDisplay::instance()->getAmbientSoundVolumeEntry().setValue(volume);
	}

	if (id != applyId_)
	{
		// Only hide the window if Ok or Cancel are hit!
		GLWWindowManager::instance()->hideWindow(getId());
	}
}
