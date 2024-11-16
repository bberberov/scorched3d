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

#include <dialogs/AutoDefenseDialog.h>
#include <weapons/Accessory.h>
#include <GLW/GLWFlag.h>
#include <GLW/GLWTextButton.h>
#include <GLW/GLWWindowManager.h>
#include <tankgraph/TankKeyboardControlUtil.h>
#include <target/TargetContainer.h>
#include <target/TargetShield.h>
#include <target/TargetParachute.h>
#include <common/OptionsTransient.h>
#include <common/Defines.h>
#include <coms/ComsPlayedMoveMessage.h>
#include <coms/ComsMessageSender.h>
#include <client/ClientState.h>
#include <client/ScorchedClient.h>
#include <lang/LangResource.h>

AutoDefenseDialog *AutoDefenseDialog::instance()
{
	static AutoDefenseDialog instance;
	return &instance;
}

AutoDefenseDialog::AutoDefenseDialog() :
	GLWWindow("Auto Defense", 10.0f, 10.0f, 440.0f, 280.0f, 0,
		"Allows the current player to raise and\n"
		"lower defenses before the round starts"),
	tankInfo_(*BuyAccessoryDialogTankInfo::instance())
{
	needCentered_ = true;

	topPanel_ = (GLWPanel *)
		addWidget(new GLWPanel(10, 245, 420, 50),
		0, SpaceLeft | SpaceRight | SpaceTop, 10.0f);

	ddpara_ = (GLWDropDownText *) addWidget(new GLWDropDownText(120, 170, 420),
		0, SpaceLeft | SpaceRight | SpaceTop, 10.0f);
	ddpara_->setHandler(this);
	ddpara_->setToolTip(new ToolTip(ToolTip::ToolTipHelp, 
		LANG_RESOURCE("ENABLE_PARACHUTES", "Enable Parachutes"),
		LANG_RESOURCE("ENABLE_PARACHUTES_TOOLTIP", "Choose to enable parachutes before the\n"
		"beginning of the next round.")));
	ddshields_ = (GLWDropDownText *) addWidget(new GLWDropDownText(120, 200, 420),
		0, SpaceLeft | SpaceRight | SpaceTop, 10.0f);
	ddshields_->setToolTip(new ToolTip(ToolTip::ToolTipHelp, 
		LANG_RESOURCE("CHOOSE_SHIELDS", "Choose Shields"),
		LANG_RESOURCE("CHOOSE_SHIELDS_TOOLTIP", "Choose the shield to use at the beginning\n"
		"of the next round.")));
	ddshields_->setHandler(this);

	GLWPanel *buttonPanel = new GLWPanel(0.0f, 0.0f, 0.0f, 0.0f, false, false);
	GLWButton *cancelButton = new GLWTextButton(LANG_RESOURCE("CANCEL", "Cancel"), 95, 10, 105, this, 
		GLWButton::ButtonFlagCancel | GLWButton::ButtonFlagCenterX);
	cancelId_ = cancelButton->getId();
	buttonPanel->addWidget(cancelButton, 0, SpaceRight, 10.0f);
	GLWButton *okButton = new GLWTextButton(LANG_RESOURCE("OK", "Ok"), 235, 10, 55, this, 
		GLWButton::ButtonFlagOk | GLWButton::ButtonFlagCenterX);
	okId_ = okButton->getId();
	buttonPanel->addWidget(okButton);
	buttonPanel->setLayout(GLWPanel::LayoutHorizontal);
	addWidget(buttonPanel, 0, SpaceAll | AlignRight, 10.0f);

	setLayout(GLWPanel::LayoutVerticle);
	layout();
}

AutoDefenseDialog::~AutoDefenseDialog()
{

}

void AutoDefenseDialog::windowInit(const unsigned state)
{
	if (tankInfo_.tankAccessories.getAutoDefense().haveDefense())
	{
		displayCurrent();
	}
	else
	{
		finished();
	}
}

void AutoDefenseDialog::buttonDown(unsigned int butid)
{
	if (butid == okId_)
	{
		// Set Parachutes on/off
		if (ddpara_->getCurrentPosition() == 0)
		{
			TankKeyboardControlUtil::parachutesUpDown(tankInfo_.tankId, 0);
		}
		else
		{
			std::list<Accessory *> &paras =
				tankInfo_.tankAccessories.getAllAccessoriesByType(
					AccessoryPart::AccessoryParachute);
			std::list<Accessory *>::iterator parasItor = paras.begin();
			for (int i=1; i<ddpara_->getCurrentPosition() && parasItor != paras.end(); i++) ++parasItor;
			
			if (parasItor != paras.end())
			{
				TankKeyboardControlUtil::parachutesUpDown(tankInfo_.tankId, 
					(*parasItor)->getAccessoryId());
			}
		}

		// Set shields on/off
		if (ddshields_->getCurrentPosition() == 0)
		{
			TankKeyboardControlUtil::shieldsUpDown(tankInfo_.tankId, 0);
		}
		else
		{
			std::list<Accessory *> &shields =
				tankInfo_.tankAccessories.getAllAccessoriesByType(
					AccessoryPart::AccessoryShield);
			std::list<Accessory *>::iterator shieldsItor = shields.begin();
			for (int i=1; i<ddshields_->getCurrentPosition() && shieldsItor != shields.end(); i++) ++shieldsItor;
			
			if (shieldsItor != shields.end())
			{
				TankKeyboardControlUtil::shieldsUpDown(tankInfo_.tankId,
					(*shieldsItor)->getAccessoryId());
			}
		}

		finished();
	}
	else if (butid == cancelId_)
	{
		finished();
	}
}

void AutoDefenseDialog::displayCurrent()
{
	GLWWindowManager::instance()->showWindow(getId());

	// Put information at the top of the dialog
	topPanel_->clear();
	topPanel_->addWidget(new GLWFlag(tankInfo_.tankColor, 5, 15, 60));
	topPanel_->addWidget(new GLWLabel(75, 10, tankInfo_.tankName));
	topPanel_->addWidget(new GLWLabel(260, 20, 
		LANG_STRING(S3D::formatMoney(tankInfo_.tankMoney))));
	topPanel_->addWidget(new GLWLabel(260, 0,
		LANG_RESOURCE_2("ROUND_OF", "Round {0} of {1}",
		S3D::formatStringBuffer("%i", ScorchedClient::instance()->getOptionsTransient().getCurrentRoundNo()),
		S3D::formatStringBuffer("%i", ScorchedClient::instance()->getOptionsGame().getNoRounds()))));

	// Put shields info
	static ToolTip shieldsOffTip(ToolTip::ToolTipHelp, 
		LANG_RESOURCE("SHIELDS_OFF", "Shields Off"),
		LANG_RESOURCE("SHIELDS_OFF_TOOLTIP", "Turns off shields."));
	ddshields_->clear();
	std::list<Accessory *>::iterator shieldsItor;
	std::list<Accessory *> &shields =
		tankInfo_.tankAccessories.getAllAccessoriesByType(
			AccessoryPart::AccessoryShield);
	ddshields_->addEntry(GLWSelectorEntry(LANG_RESOURCE("SHIELDS_OFF", "Shields Off"), &shieldsOffTip));
	for (shieldsItor = shields.begin();
		shieldsItor != shields.end();
		++shieldsItor)
	{
		Accessory *shield = (*shieldsItor);
		ddshields_->addEntry(GLWSelectorEntry(
			tankInfo_.tankAccessories.getAccessoryAndCountString(shield),
			&shield->getToolTip(), 0, &shield->getTexture()));
	}

	// Put paras info
	static ToolTip parachutesOffTip(ToolTip::ToolTipHelp, 
		LANG_RESOURCE("PARACHUTES_OFF", "Parachutes Off"),
		LANG_RESOURCE("PARACHUTES_OFF_TOOLTIP", "Turns off parachutes."));
	ddpara_->clear();
	std::list<Accessory *>::iterator parachutesItor;
	std::list<Accessory *> &parachutes =
		tankInfo_.tankAccessories.getAllAccessoriesByType(
			AccessoryPart::AccessoryParachute);
	ddpara_->addEntry(GLWSelectorEntry(LANG_RESOURCE("PARACHUTES_OFF", "Parachutes Off"), &parachutesOffTip));
	for (parachutesItor = parachutes.begin();
		parachutesItor != parachutes.end();
		++parachutesItor)
	{
		Accessory *parachute = (*parachutesItor);
		ddpara_->addEntry(GLWSelectorEntry(
			tankInfo_.tankAccessories.getAccessoryAndCountString(parachute),
			&parachute->getToolTip(), 0, &parachute->getTexture()));
	}

	// Set the currently shown items
	ddshields_->setCurrentText(LANG_RESOURCE("SHIELDS_OFF", "Shields Off"));

	// Set the currently shown items
	ddpara_->setCurrentText(LANG_RESOURCE("PARACHUTES_OFF", "Parachutes Off"));
}

void AutoDefenseDialog::select(unsigned int id, 
							   const int pos, 
							   GLWSelectorEntry value)
{
	// Nothing to do as we don't actualy set the status
	// until the ok button is pressed
}

void AutoDefenseDialog::finished()
{
	// send message saying we are finished with shot
	ComsPlayedMoveMessage comsMessage(
		tankInfo_.tankId, 
		tankInfo_.tankMoveId,
		ComsPlayedMoveMessage::eFinishedBuy);
	ComsMessageSender::sendToServer(comsMessage);

	GLWWindowManager::instance()->hideWindow(getId());
	ScorchedClient::instance()->getGameState().stimulate(ClientState::StimWait);
}
