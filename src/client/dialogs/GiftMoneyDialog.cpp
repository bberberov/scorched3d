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

#include <dialogs/GiftMoneyDialog.hpp>
#include <graph/OptionsDisplay.hpp>
#include <client/ScorchedClient.hpp>
#include <target/TargetContainer.hpp>
#include <tank/Tank.hpp>
#include <tank/TankScore.hpp>
#include <tank/TankState.hpp>
#include <GLW/GLWTextButton.hpp>
#include <GLW/GLWWindowManager.hpp>
#include <coms/ComsGiftMoneyMessage.hpp>
#include <coms/ComsMessageSender.hpp>

GiftMoneyDialog *GiftMoneyDialog::instance_ = nullptr;

GiftMoneyDialog *GiftMoneyDialog::instance()
{
	if (nullptr == instance_)
	{
		instance_ = new GiftMoneyDialog();
	}
	return instance_;
}

GiftMoneyDialog::GiftMoneyDialog() :
	GLWWindow("Gift", 10.0f, 10.0f, 300.0f, 70.0f, eSmallTitle | eHideName,
		"Send money to other team players"),
	tankInfo_(*BuyAccessoryDialogTankInfo::instance())
{
	needCentered_ = true;

	GLWPanel* mainPanel = new GLWPanel( 0.0f, 0.0f, 0.0f, 0.0f, false, false );
	mainPanel->addWidget( new GLWLabel( 0.0f, 0.0f, LANG_RESOURCE( "GIFT", "Gift" ) ), nullptr, SpaceRight, 10.0f );
	money_ = (GLWDropDownText*)mainPanel->addWidget(
		new GLWDropDownText( 0.0f, 0.0f, 150.0f ), nullptr, SpaceRight, 10.0f
	);
	mainPanel->addWidget( new GLWLabel( 0.0f, 0.0f, LANG_RESOURCE( "TO", "to" ) ), nullptr, SpaceRight, 10.0f );
	players_ = (GLWDropDownText*)mainPanel->addWidget( new GLWDropDownText( 0.0f, 0.0f, 150.0f ) );
	mainPanel->setLayout( GLWPanel::LayoutHorizontal );
	addWidget( mainPanel, nullptr, SpaceLeft | SpaceRight | SpaceTop, 10.0f );

	GLWPanel* buttonPanel   = new GLWPanel( 0.0f, 0.0f, 0.0f, 0.0f, false, false );
	GLWButton* cancelButton = new GLWTextButton(
		LANG_RESOURCE( "CANCEL", "Cancel" ),
		95,
		10,
		105,
		this,
		GLWButton::ButtonFlagCancel | GLWButton::ButtonFlagCenterX
	);
	GLWButton* okButton = new GLWTextButton(
		LANG_RESOURCE( "OK", "Ok" ),
		235,
		10,
		55,
		this,
		GLWButton::ButtonFlagOk | GLWButton::ButtonFlagCenterX
	);
	cancelId_ = cancelButton->getId();
	okId_     = okButton->getId();
	buttonPanel->addWidget( cancelButton, nullptr, SpaceRight, 10.0f );
	buttonPanel->addWidget( okButton );
	buttonPanel->setLayout( GLWPanel::LayoutHorizontal );
	addWidget( buttonPanel, nullptr, SpaceAll, 10.0f );

	setLayout(GLWPanel::LayoutVerticle);
	layout();
}

GiftMoneyDialog::~GiftMoneyDialog()
{
}

void GiftMoneyDialog::display()
{
	GLWWindow::display();

	players_->clear();
	money_->clear();

	// Get the current playing tank
	Tank* currentTank = ScorchedClient::instance()->getTargetContainer().getCurrentTank();
	if ( ! currentTank )
	{
		GLWWindowManager::instance()->hideWindow( getId() );
		return;
	}

	// Add available amounts of money
	const int amounts[] = { 1000, 2500, 5000, 10000, 15000, 20000, 25000, 50000, 100000 };
	for ( unsigned int i = 0; i < sizeof( amounts ) / sizeof( int ); ++i )
	{
		int amount = amounts[i];
		if ( amount <= tankInfo_.tankMoney )
		{
			money_->addEntry( GLWSelectorEntry(
				LANG_RESOURCE( S3D::formatStringBuffer( "%i_DOLLARS", amount ), S3D::formatMoney( amount ) ),
				nullptr,
				false,
				nullptr,
				(void*)(long)amount  // NOTE: pass value directly as void*
			) );
		}
	}

	// Add all tanks in the same team as the current
	std::map< unsigned int, Tank* >&          tanks = ScorchedClient::instance()->getTargetContainer().getTanks();
	std::map< unsigned int, Tank* >::iterator itor;
	for ( itor = tanks.begin(); itor != tanks.end(); ++itor )
	{
		Tank* tank = itor->second;
		if ( tank->getTeam() == currentTank->getTeam() && tank != currentTank && tank->getState().getTankPlaying() )
		{
			players_->addEntry( GLWSelectorEntry(
				tank->getTargetName(),
				nullptr,
				false,
				nullptr,
				(void*)(unsigned long)( tank->getPlayerId() )  // NOTE: pass value directly as void*
			) );
		}
	}
}

void GiftMoneyDialog::buttonDown( unsigned int id )
{
	if ( id == okId_ )
	{
		if ( money_->getCurrentEntry() && players_->getCurrentEntry() )
		{
			// NOTE: read values from void* directly
			int          money    = (int)(long)( money_->getCurrentEntry()->getUserData() );
			unsigned int playerId = (unsigned int)(unsigned long)( players_->getCurrentEntry()->getUserData() );

			ComsGiftMoneyMessage message( tankInfo_.tankId, playerId, money );
			ComsMessageSender::sendToServer( message );
		}
	}
	GLWWindowManager::instance()->hideWindow( getId() );
}
