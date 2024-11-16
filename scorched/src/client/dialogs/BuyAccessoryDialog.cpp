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

#include <dialogs/BuyAccessoryDialog.h>
#include <dialogs/GiftMoneyDialog.h>
#include <GLW/GLWWindowManager.h>
#include <GLW/GLWTextButton.h>
#include <GLW/GLWIcon.h>
#include <GLEXT/GLViewPort.h>
#include <client/ClientState.h>
#include <client/ScorchedClient.h>
#include <graph/OptionsDisplay.h>
#include <common/OptionsScorched.h>
#include <common/OptionsTransient.h>
#include <common/Defines.h>
#include <coms/ComsMessageSender.h>
#include <coms/ComsBuyAccessoryMessage.h>
#include <weapons/AccessoryStore.h>
#include <stdio.h>

BuyAccessoryDialog *BuyAccessoryDialog::instance_ = 0;

BuyAccessoryDialog *BuyAccessoryDialog::instance()
{
	if (!instance_)
	{
		instance_ = new BuyAccessoryDialog;
	}
	return instance_;
}

BuyAccessoryDialog::BuyAccessoryDialog() : 
	GLWWindow("Buy", 10.0f, 10.0f, 465.0f, 300.0f, eHideName,
		"Allows the current player to buy and sell\n"
		"weapons and other accessories."),
	firstDrawTime_(true), sellTab_(0), flag_(0),
	tankInfo_(*BuyAccessoryDialogTankInfo::instance())
{
	okId_ = addWidget(new GLWTextButton(LANG_RESOURCE("OK", "Ok"), 400, 10, 55, this, 
		GLWButton::ButtonFlagOk | GLWButton::ButtonFlagCenterX))->getId();

	if (!ScorchedClient::instance()->getOptionsGame().getTutorial()[0])
	{
		giftId_ = addWidget(new GLWTextButton(LANG_RESOURCE("GIFT", "Gift"), 320, 10, 70, this, 
			GLWButton::ButtonFlagCenterX))->getId();
	}

	topPanel_ = (GLWPanel *)
		addWidget(new GLWPanel(10, 265, 450, 50));

	defaultTab_ = (GLWCheckBoxText *) 
		addWidget(new GLWCheckBoxText(18.0f, 230.0f, LANG_RESOURCE("DEFAULT_TAB","Default Tab"), false, 3.0f));
	defaultTab_->getCheckBox().setW(12);
	defaultTab_->getCheckBox().setH(12);
	defaultTab_->getLabel().setSize(10);
	defaultTab_->getCheckBox().setHandler(this);

	addWidget(new GLWLabel(15, 9, LANG_RESOURCE("SORT_LABEL", "Sort by:")));

	sortDropDown_ = (GLWDropDownText *) addWidget(new GLWDropDownText(100, 9, 100));

	sortDropDown_->addText(LANG_RESOURCE("SORT_NOTHING", "Nothing"), "Nothing");
	sortDropDown_->addText(LANG_RESOURCE("SORT_NAME", "Name"), "Name");
	sortDropDown_->addText(LANG_RESOURCE("SORT_PRICE", "Price"), "Price");
	sortDropDown_->setName("Sort");
	sortDropDown_->setHandler(this);
}

BuyAccessoryDialog::~BuyAccessoryDialog()
{
}

void BuyAccessoryDialog::draw()
{
	if (sellTab_ && firstDrawTime_)
	{
		firstDrawTime_ = false;
		float screenHeight = (float) GLViewPort::getHeight();
		float addition = 0;
		if (screenHeight > 340) addition = screenHeight - 340;
		if (addition > 200) addition = 200;

		setH(300 + addition);
		topPanel_->setY(240 + addition);
		defaultTab_->setY(190 + addition);

		std::list<GLWPanel::GLWPanelEntry>::iterator itor;
		for (itor = getWidgets().begin();
			itor != getWidgets().end();
			++itor)
		{
			GLWPanel::GLWPanelEntry &entry = (*itor);
			if (entry.widget->getMetaClassId() == sellTab_->getMetaClassId())
			{
				GLWTab *tab = (GLWTab *) entry.widget;
				tab->setH(165 + addition);
			}
		}

		needCentered_ = true;
	}

	GLWWindow::draw();

	LANG_RESOURCE_VAR(BUY, "BUY", "Buy");
	LANG_RESOURCE_VAR(SELL, "SELL", "Sell");
	Vector red(0.7f, 0.0f, 0.0f);
	Vector green(0.0f, 0.4f, 0.0f);
	GLWFont::instance()->getGameFont()->draw(
		green, 12.0f, x_ + 260.0f, y_ + topPanel_->getY() - 50.0f, 0.0f, BUY);
	GLWFont::instance()->getGameFont()->draw(
		red, 12.0f, x_ + 360.0f, y_ + topPanel_->getY() - 50.0f, 0.0f, SELL);
}

void BuyAccessoryDialog::addPlayerName()
{
	float flagOffset = 0.0f;
	if (flag_) flagOffset = flag_->getOffset();
	topPanel_->clear();

	flag_ = (GLWFlag *) topPanel_->addWidget(new GLWFlag(tankInfo_.tankColor, 5, 15, 60));
	flag_->setOffset(flagOffset);
	topPanel_->addWidget(new GLWLabel(75, 10, tankInfo_.tankName));
	topPanel_->addWidget(new GLWLabel(260, 20, 
		LANG_STRING(S3D::formatMoney(tankInfo_.tankMoney))));
	topPanel_->addWidget(new GLWLabel(260, 0, 
		LANG_RESOURCE_2("ROUND_OF", "Round {0} of {1}",
		S3D::formatStringBuffer("%i", ScorchedClient::instance()->getOptionsTransient().getCurrentRoundNo()),
		S3D::formatStringBuffer("%i", ScorchedClient::instance()->getOptionsGame().getNoRounds()))));
}

void BuyAccessoryDialog::addTabs()
{
	if (sellTab_) return;

	std::set<std::string> &groupNames =
		ScorchedClient::instance()->
			getAccessoryStore().getTabGroupNames();
	std::set<std::string>::reverse_iterator itor;
	for (itor = groupNames.rbegin();
		itor != groupNames.rend();
		++itor)
	{
		std::string name = (*itor);
		GLWTab *tab = (GLWTab *)
			addWidget(new GLWTab(name, LANG_RESOURCE(name, name), 10, 40, 450, 160));
		buyTabs_[name] = tab;
		tab->setHandler(this);
	}
	sellTab_ = (GLWTab *)
		addWidget(new GLWTab("Inv", LANG_RESOURCE("INVENTORY_TAB", "Inv"), 10, 40, 450, 160));
	sellTab_->setHandler(this);

	favouritesTab_ = (GLWTab *)
		addWidget(new GLWTab("Fav", LANG_RESOURCE("FAVOURITES_TAB", "Fav"), 10, 40, 450, 160));
	favouritesTab_->setHandler(this);
}

void BuyAccessoryDialog::playerRefresh()
{
	addPlayerName();
	addPlayerWeapons();
}

void BuyAccessoryDialog::playerRefreshKeepPos()
{
	if (!sellTab_) return;

	std::map<std::string, int> scrollPositions;
	std::list<GLWPanel::GLWPanelEntry>::iterator itor;
	for (itor = getWidgets().begin();
		itor != getWidgets().end();
		++itor)
	{
		GLWPanel::GLWPanelEntry &entry = (*itor);
		if (entry.widget->getMetaClassId() == sellTab_->getMetaClassId())
		{
			GLWTab *tab = (GLWTab *) entry.widget;
			scrollPositions[tab->getName()] = tab->getScrollBar().getCurrent();
		}
	}

	addPlayerName();
	addPlayerWeapons();

	for (itor = getWidgets().begin();
		itor != getWidgets().end();
		++itor)
	{
		GLWPanel::GLWPanelEntry &entry = (*itor);
		if (entry.widget->getMetaClassId() == sellTab_->getMetaClassId())
		{
			GLWTab *tab = (GLWTab *) entry.widget;
			tab->getScrollBar().setCurrent(scrollPositions[tab->getName()]);
		}
	}
}

void BuyAccessoryDialog::addPlayerWeapons()
{
	std::list<GLWPanel::GLWPanelEntry>::iterator itor;
	for (itor = getWidgets().begin();
		itor != getWidgets().end();
		++itor)
	{
		GLWPanel::GLWPanelEntry &entry = (*itor);
		if (entry.widget->getMetaClassId() == sellTab_->getMetaClassId())
		{
			GLWTab *tab = (GLWTab *) entry.widget;
			tab->clear();
		}
	}

	addPlayerFavorites();
	addPlayerWeaponsSell();

	std::set<std::string> &groupNames =
		ScorchedClient::instance()->
			getAccessoryStore().getTabGroupNames();
	std::set<std::string>::iterator groupItor;
	for (groupItor = groupNames.begin();
		groupItor != groupNames.end();
		++groupItor)
	{
		addPlayerWeaponsBuy(buyTabs_[*groupItor], (*groupItor).c_str());
	}
}

void BuyAccessoryDialog::addPlayerFavorites()
{
	float height = 10.0f;
	std::list<Accessory *> acessories = 
		ScorchedClient::instance()->
			getAccessoryStore().getAllAccessories(
				OptionsDisplay::instance()->getAccessorySortKey());
	std::list<Accessory *>::reverse_iterator itor;
	for (itor = acessories.rbegin();
		itor != acessories.rend();
		++itor)
	{
		Accessory *current = *itor;
		if (favorites_.find(current->getName()) != favorites_.end())
		{
			if (addAccessory(favouritesTab_, height, current)) height += 24.0f;
		}
	}
}

void BuyAccessoryDialog::addPlayerWeaponsBuy(GLWTab *tab, const char *group)
{
	std::list<Accessory *> weapons = ScorchedClient::instance()->
		getAccessoryStore().getAllAccessoriesByTabGroup(
			group,
			OptionsDisplay::instance()->getAccessorySortKey());

	float height = 10.0f;
	std::list<Accessory *>::reverse_iterator itor2;
	for (itor2 = weapons.rbegin();
		itor2 != weapons.rend();
		++itor2)
	{
		Accessory *current = (*itor2);
		if (addAccessory(tab, height, current)) height += 24.0f;
	}
}

void BuyAccessoryDialog::addPlayerWeaponsSell()
{
	float height = 10.0f;
	std::list<Accessory *> tankAccessories;
	tankInfo_.tankAccessories.getAllAccessories(
		tankAccessories);
	ScorchedClient::instance()->getAccessoryStore().sortList(tankAccessories,
		OptionsDisplay::instance()->getAccessorySortKey());
	std::list<Accessory *>::reverse_iterator itor;
	for (itor = tankAccessories.rbegin();
		itor != tankAccessories.rend();
		++itor)
	{
		Accessory *current = *itor;
		if (addAccessory(sellTab_, height, current)) height += 24.0f;
	}
}

bool BuyAccessoryDialog::addAccessory(
	GLWTab *tab, float height, Accessory *current)
{
	if (!tankInfo_.tankAccessories.accessoryAllowed(current, 0)) return false;
	if (current->getNoBuy()) return false;

	int currentNumber = 
		tankInfo_.tankAccessories.getAccessoryCount(current);

	// Panel
	GLWPanel *newPanel = (GLWPanel *)
		tab->addWidget(new GLWPanel(5.0f, (float) height, 415.0f, 22.0f, true));
	newPanel->setToolTip(&current->getToolTip());

	// Favorites checkbox
	GLWCheckBox *sortBox = (GLWCheckBox *) newPanel->addWidget(new GLWCheckBox(2, 4, false));
	sortBox->setHandler(this);
	sortBox->setW(14);
	sortBox->setH(14);
	sortBox->setToolTip(new ToolTip(ToolTip::ToolTipHelp, 
		LANG_RESOURCE("FAVOURITE_WEAPON", "Favorite Weapon"),
		LANG_RESOURCE("FAVOURITE_WEAPON_TOOLTIP", "Set/unset this weapon as a favorite.\n"
		"Favorite weapons will show in the\n"
		"favorites tab.")));
	sortBox->setState(favorites_.find(current->getName()) != favorites_.end());
	favMap_[sortBox->getId()] = current;
	
	// Others
	newPanel->addWidget(new GLWLabel(20, 0, 
		tankInfo_.tankAccessories.getAccessoryCountString(current), 12.0f));
	newPanel->addWidget(new GLWIcon(45, 4, 16, 16, current->getTexture().getImageID()));
	newPanel->addWidget(new GLWLabel(65, 0, LANG_RESOURCE(current->getName(), current->getName()), 12.0f));

	// Buy Button
	std::string price = S3D::formatMoney(current->getPrice());
	if (tankInfo_.tankAccessories.accessoryAllowed(current, current->getBundle()) && 
		current->getPrice() <= tankInfo_.tankMoney)
	{
		GLWTextButton *button = (GLWTextButton *)
			newPanel->addWidget(new GLWTextButton(
				LANG_STRING(S3D::formatStringBuffer("%s/%i",
					price.c_str(), current->getBundle())), 
					210, 2, 100, this, 
			GLWButton::ButtonFlagCenterX, 12.0f));
		button->setColor(Vector(0.0f, 0.4f, 0.0f));
		button->setToolTip(new ToolTip(ToolTip::ToolTipHelp, 
			LANG_RESOURCE("BUY", "Buy"), 
			LANG_RESOURCE_3("BUY_TOOLTIP", "Buy {0} {1}(s) for {2}",
				S3D::formatStringBuffer("%i", current->getBundle()),
				current->getName(),
				price)));
		button->setH(button->getH() - 2.0f);
		buyMap_[button->getId()] = current;
	}
	else
	{
		GLWLabel *label = (GLWLabel *)
			newPanel->addWidget(new GLWLabel(
				260, 0, 
				LANG_STRING(S3D::formatStringBuffer("%s/%i",
					price.c_str(), current->getBundle())), 12.0f));
		label->setX(label->getX() - label->getW() / 2);
		label->setColor(Vector(0.4f, 0.4f, 0.4f));
	}

	// Sell Button
	if (currentNumber > 0)
	{
		std::string sellprice = S3D::formatMoney(current->getSellPrice());
		GLWTextButton *button = (GLWTextButton *)
			newPanel->addWidget(new GLWTextButton(
				LANG_STRING(S3D::formatStringBuffer("%s/%i",
					sellprice.c_str(), 1)), 
					312, 2, 100, this,
			GLWButton::ButtonFlagCenterX, 12.0f));
		button->setColor(Vector(0.7f, 0.0f, 0.0f));
		button->setToolTip(new ToolTip(ToolTip::ToolTipHelp, 
			LANG_RESOURCE("SELL", "Sell"), 
			LANG_RESOURCE_2("SELL_TOOLTIP", "Sell 1 {0} for {1}",
				current->getName(),
				sellprice)));
		button->setH(button->getH() - 2.0f);
		sellMap_[button->getId()] = current;
	}

	return true;
}

void BuyAccessoryDialog::display()
{
	addTabs();
	loadFavorites();

	sortDropDown_->setHandler(0);

	switch (OptionsDisplay::instance()->getAccessorySortKey())
	{
	case AccessoryStore::SortName:
		sortDropDown_->setCurrentText(LANG_RESOURCE("SORT_NAME", "Name"));
		break;

	case AccessoryStore::SortPrice:
		sortDropDown_->setCurrentText(LANG_RESOURCE("SORT_PRICE", "Price"));
		break;

	case AccessoryStore::SortNothing:
		sortDropDown_->setCurrentText(LANG_RESOURCE("SORT_NOTHING", "Nothing"));
		break;
	}

	sortDropDown_->setHandler(this);

	tankInfo_.set();

	if (buyTabs_.find("weapon") != buyTabs_.end())
	{
		buyTabs_["weapon"]->setDepressed();
	}
	const char *buyTab = OptionsDisplay::instance()->getBuyTab();
	std::list<GLWPanel::GLWPanelEntry>::iterator itor;
	for (itor = getWidgets().begin();
		itor != getWidgets().end();
		++itor)
	{
		GLWPanel::GLWPanelEntry &entry = (*itor);
		if (entry.widget->getMetaClassId() == sellTab_->getMetaClassId())
		{
			GLWTab *tab = (GLWTab *) entry.widget;
			if (0 == strcmp(buyTab, tab->getName()))
			{
				tab->setDepressed();
				break;
			}
		}
	}
	tabDown(0);
	
	playerRefresh();
}

void BuyAccessoryDialog::tabDown(unsigned int id)
{
	const char *buyTab = OptionsDisplay::instance()->getBuyTab();
	std::list<GLWPanel::GLWPanelEntry>::iterator itor;
	for (itor = getWidgets().begin();
		itor != getWidgets().end();
		++itor)
	{
		GLWPanel::GLWPanelEntry &entry = (*itor);
		if (entry.widget->getMetaClassId() == sellTab_->getMetaClassId())
		{
			GLWTab *tab = (GLWTab *) entry.widget;
			if (tab->getDepressed())
			{
				if (0 == strcmp(buyTab, tab->getName()))
				{
					defaultTab_->getCheckBox().setState(true);
				}
				else
				{
					defaultTab_->getCheckBox().setState(false);
				}
				break;
			}
		}
	}
}

void BuyAccessoryDialog::select(unsigned int id, const int pos, GLWSelectorEntry value)
{
	if (id == sortDropDown_->getId())
	{
		OptionsDisplay *display = OptionsDisplay::instance();
		const char *dataText = value.getDataText();

		if (strcmp(dataText, "Name") == 0)
			display->getAccessorySortKeyEntry().setValue(AccessoryStore::SortName);
		else if (strcmp(dataText, "Price") == 0)
			display->getAccessorySortKeyEntry().setValue(AccessoryStore::SortPrice);
		else
			display->getAccessorySortKeyEntry().setValue(AccessoryStore::SortNothing);

		playerRefreshKeepPos();
	}
}

void BuyAccessoryDialog::stateChange(bool state, unsigned int id)
{
	if (id == defaultTab_->getCheckBox().getId())
	{
		if (defaultTab_->getCheckBox().getState())
		{
			std::list<GLWPanel::GLWPanelEntry>::iterator itor;
			for (itor = getWidgets().begin();
				itor != getWidgets().end();
				++itor)
			{
				GLWPanel::GLWPanelEntry &entry = (*itor);
				if (entry.widget->getMetaClassId() == sellTab_->getMetaClassId())
				{
					GLWTab *tab = (GLWTab *) entry.widget;
					if (tab->getDepressed())
					{
						OptionsDisplay::instance()->getBuyTabEntry().setValue(tab->getName());
						break;
					}
				}
			}
		}
		else
		{
			OptionsDisplay::instance()->getBuyTabEntry().setValue("");
		}
	}
	else
	{
		// The favorites check box has been clicked
		std::map<unsigned int, Accessory *>::iterator findItor
			= favMap_.find(id);
		if (findItor != favMap_.end())
		{
			if (state)
			{
				favorites_.insert((*findItor).second->getName());
			}
			else
			{
				favorites_.erase((*findItor).second->getName());
			}

			playerRefreshKeepPos();
			favouritesTab_->calculateVisible();
			playerRefreshKeepPos();
		}
	}
}

void BuyAccessoryDialog::buttonDown(unsigned int id)
{
	if (id == okId_)
	{
		saveFavorites();
		ScorchedClient::instance()->getGameState().stimulate(ClientState::StimAutoDefense);
	}
	else if (id == giftId_)
	{
		GLWWindowManager::instance()->showWindow(
			GiftMoneyDialog::instance()->getId());
	}
	else
	{
		std::map<unsigned int, Accessory *>::iterator itor;
		itor = buyMap_.find(id);
		if (itor != buyMap_.end())
		{
			// Tell the server to add the accessory
			Accessory *acc = itor->second;
			ComsBuyAccessoryMessage buyMessage(tankInfo_.tankId, acc->getAccessoryId(), true);
			ComsMessageSender::sendToServer(buyMessage);

			// Add the accessory
			tankInfo_.tankAccessories.add(acc, acc->getBundle());
			tankInfo_.tankMoney -= acc->getPrice();
			if (tankInfo_.tankMoney < 0) tankInfo_.tankMoney = 0;

			// Refresh the window
			playerRefreshKeepPos();
		}
		else
		{
			itor = sellMap_.find(id);
			if (itor != sellMap_.end())
			{
				// Tell the server to add the accessory
				Accessory *acc = itor->second;
				ComsBuyAccessoryMessage buyMessage(tankInfo_.tankId, acc->getAccessoryId(), false);
				ComsMessageSender::sendToServer(buyMessage);

				// Add the accessory
				tankInfo_.tankAccessories.rm(acc, 1);
				tankInfo_.tankMoney += acc->getSellPrice();

				// Refresh the window
				playerRefreshKeepPos();
			}
		}
	}
}

void BuyAccessoryDialog::loadFavorites()
{
	std::string filename = 
		S3D::getSettingsFile(S3D::formatStringBuffer("weaponfavorites-%s.xml", 
			ScorchedClient::instance()->getOptionsGame().getMod()));

	favorites_.clear();
	XMLFile file;
	if (!file.readFile(filename))
	{
		S3D::dialogMessage("BuyAccessoryDialog", S3D::formatStringBuffer(
					  "Failed to parse \"%s\"\n%s", 
					  filename.c_str(),
					  file.getParserError()));
		return;
	}
	if (!file.getRootNode()) return; // Empty File

	std::string accessory;
	while (file.getRootNode()->getNamedChild("accessory", accessory, false))
	{
		favorites_.insert(accessory);
	}
}

void BuyAccessoryDialog::saveFavorites()
{
	std::string filename = 
		S3D::getSettingsFile(S3D::formatStringBuffer("weaponfavorites-%s.xml", 
			ScorchedClient::instance()->getOptionsGame().getMod()));

	XMLNode node("accessories");
	std::set<std::string>::iterator itor;
	for (itor = favorites_.begin();
		itor != favorites_.end();
		++itor)
	{
		std::string accessory = *itor;

		XMLNode *accessoryNode = 
			new XMLNode("accessory", accessory.c_str());
		node.addChild(accessoryNode);
	}
	node.writeToFile(filename);
}
