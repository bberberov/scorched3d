///////////////////////////////////////////////////////////////////////////////
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

#include <weapons/EconomyFreeMarket.h>
#include <weapons/AccessoryStore.h>
#include <server/ScorchedServer.h>
#include <common/Defines.h>
#include <common/OptionsScorched.h>
#include <common/Logger.h>
#include <XML/XMLFile.h>
#include <XML/XMLParser.h>
#include <tank/Tank.h>
#include <tank/TankScore.h>
#include <tanket/TanketAccessories.h>
#include <stdlib.h>

REGISTER_CLASS_SOURCE(EconomyFreeMarket);

static std::string getEconomyFileName()
{
	return S3D::getSettingsFile(S3D::formatStringBuffer("freemarket-%s-%i.xml",
		ScorchedServer::instance()->getOptionsGame().getMod(),
		ScorchedServer::instance()->getOptionsGame().getPortNo()));
}

static bool validAccessory(Accessory *accessory)
{
	return 
		accessory->getStartingNumber() != -1 &&
		accessory->getMaximumNumber() != 0 &&
		!accessory->getAIOnly() &&
		!accessory->getBotOnly();
}

EconomyFreeMarket::EconomyFreeMarket()
{
}

EconomyFreeMarket::~EconomyFreeMarket()
{
}

bool EconomyFreeMarket::loadPrices()
{
	// Parse the file containing the last used prices
	XMLFile file;
	if (!file.readFile(getEconomyFileName()))
	{
		S3D::dialogMessage("EconomyFreeMarket", S3D::formatStringBuffer(
			"Failed to parse freemarket.xml file : %s",
			file.getParserError()));
		return false;
	}

	// If there are entries in the file
	accessoryPrice_.clear();
	XMLNode *rootnode = file.getRootNode();
	if (rootnode)
	{
		std::list<XMLNode *> &nodes = rootnode->getChildren();
		std::list<XMLNode *>::iterator itor;
		for (itor = nodes.begin();
			itor != nodes.end();
			++itor)
		{
			XMLNode *node = *itor;
			std::string name;
			float price;

			if (!node->getNamedChild("name", name)) return false;
			if (!node->getNamedChild("buyprice", price)) return false;

			Accessory *accessory = ScorchedServer::instance()->getAccessoryStore().
				findByPrimaryAccessoryName(name.c_str());
			if (accessory)
			{	
				// Check that this accessory is still valid
				// (just in case the file has been changed)
				if (validAccessory(accessory))
				{
					// Set the actual accessory price (based on the last used market prices)
					accessoryPrice_[accessory->getAccessoryId()] = price;
				}
			}
		}
	}
	calculatePrices();

	return true;	
}

bool EconomyFreeMarket::savePrices()
{
	FileLines file;
	file.addLine("<prices source=\"Scorched3D\">");
	std::map<unsigned int, float>::iterator itor;
	for (itor = accessoryPrice_.begin();
		itor != accessoryPrice_.end();
		++itor)
	{
		unsigned int accessoryId = itor->first;
		float price = itor->second;

		Accessory *accessory = 
			ScorchedServer::instance()->getAccessoryStore().findByAccessoryId(accessoryId);
		if (accessory && validAccessory(accessory))
		{
			std::string cleanName;
			std::string dirtyName(accessory->getName());
			XMLNode::removeSpecialChars(dirtyName, cleanName);
			file.addLine("  <accessory>");
			file.addLine(S3D::formatStringBuffer("    <!-- %s, original Price %i -->", 
				cleanName.c_str(), accessory->getOriginalPrice()));
			file.addLine(S3D::formatStringBuffer("    <name>%s</name>", cleanName.c_str()));
			file.addLine(S3D::formatStringBuffer("    <buyprice>%.4f</buyprice>", price));
			file.addLine("  </accessory>");
		}
	}
	file.addLine("</prices>");

	if (!file.writeFile(getEconomyFileName())) return false;

	return true;
}

void EconomyFreeMarket::calculatePrices()
{
	std::map<unsigned int, float>::iterator itor;
	for (itor = accessoryPrice_.begin();
		itor != accessoryPrice_.end();
		++itor)
	{
		unsigned int accessoryId = itor->first;
		float price = itor->second;

		Accessory *accessory = 
			ScorchedServer::instance()->getAccessoryStore().findByAccessoryId(accessoryId);
		if (accessory && validAccessory(accessory))
		{
			setPrice(accessory, int(price));
		}
	}
}

void EconomyFreeMarket::accessoryBought(Tank *tank, 
		const char *accessoryName)
{
	// Check if we track bot (ai) purchases
	if (tank->getDestinationId() == 0)
	{
		if (!ScorchedServer::instance()->getOptionsGame().getFreeMarketTracksBotPurchases())
		{
			return;
		}
	}

	// Find the bought accessory
	Accessory *boughtAccessory = 
		ScorchedServer::instance()->getAccessoryStore().
		findByPrimaryAccessoryName(accessoryName);
	DIALOG_ASSERT(boughtAccessory);

	// Find the list of accessories that this player could have bought
	// This list comprises of weapons that are similar in price to the one
	// that has been bought
	std::list<Accessory *> possibleAccessories;
	{
		std::list<Accessory *> weapons = 
			ScorchedServer::instance()->getAccessoryStore().
			getAllAccessories();
		std::list<Accessory *>::iterator itor;
		for (itor = weapons.begin();
			itor != weapons.end();
			++itor)
		{
			Accessory *accessory = *itor;

			int accessoryPrice = 
				accessory->getOriginalPrice() / MAX(accessory->getBundle(), 1); 
			int boughtAccessoryPrice = 
				boughtAccessory->getOriginalPrice() / MAX(accessory->getBundle(), 1); 
			if (accessoryPrice >= int(float(boughtAccessoryPrice) * 0.33f) && 
				accessoryPrice <= int(float(boughtAccessoryPrice) * 3.33f) && 
				tank->getAccessories().accessoryAllowed(accessory, accessory->getBundle()) && 
				validAccessory(accessory)) 
			{ 
				possibleAccessories.push_back(accessory); 
			}
		}
	}

	// Nothing to do if we can only buy 1 item
	if (possibleAccessories.size() <= 1) return;

	// How much should each accessory get (on average)
	float moneyShouldAquire = float(boughtAccessory->getPrice()) / float(possibleAccessories.size());
	
	// Alter prices
	{
		std::list<Accessory *>::iterator itor;
		for (itor = possibleAccessories.begin();
			itor != possibleAccessories.end();
			++itor)
		{
			Accessory *accessory = (*itor);

			// Figure out how much money was spent on this weapon
			float moneyDidAquire = 0.0f;
			if (accessory == boughtAccessory) moneyDidAquire = 
				float(boughtAccessory->getPrice());

			// Figure out if this is more or less money than on average
			// should be spent on this weapon
			float adjustment = 
				float(ScorchedServer::instance()->getOptionsGame().
				getFreeMarketAdjustment());
			float priceDiff = 0.0f;
			if (moneyDidAquire < moneyShouldAquire)
			{
				// This weapon was not bought, decrease its price
				priceDiff = -adjustment / float(possibleAccessories.size());
				//priceDiff /= 2;
			}
			else if (moneyDidAquire >= moneyShouldAquire)
			{
				// This weapon was bought, increase its price
				priceDiff = adjustment * (float(possibleAccessories.size()) - 1.0f)
					/ float(possibleAccessories.size());
			}

			// Update the price difference for this weapon
			std::map<unsigned int, float>::iterator findItor = 
				accessoryPrice_.find(accessory->getAccessoryId());
			if (findItor == accessoryPrice_.end()) 
			{
				accessoryPrice_[accessory->getAccessoryId()] = 
					float(accessory->getOriginalPrice()) + priceDiff;
			}
			else 
			{
				accessoryPrice_[accessory->getAccessoryId()] += 
					priceDiff;
			}
		}
	}
}

void EconomyFreeMarket::setPrice(Accessory *accessory, int price)
{
	float limit = float(accessory->getFreeMarketLimits()) / 100.0f;
	limit *= ScorchedServer::instance()->getOptionsGame().getFreeMarketLimits();
	if (limit == 0) // No limits accessory should stay at fixed price
	{
		price = accessory->getOriginalPrice();
	}
	else // Make sure price is within limits
	{
		// Make suse price does not get greater than 1.5X the original price
		if (price > int(float(accessory->getOriginalPrice()) * limit))
			price = int(float(accessory->getOriginalPrice()) * limit);
		// Make sure price does not get lower than 1.5X the original price
		else if (price < int(float(accessory->getOriginalPrice()) / limit))
			price = int(float(accessory->getOriginalPrice()) / limit);
	}
	price = (price / 10) * 10; // Round to 10

	// Set Buy Price
	accessory->setPrice(price);

	// Set sell price to 0.8X the buy price
	int selPrice = int(float(accessory->getPrice()) /
		float(accessory->getBundle()) * 0.8f);
	selPrice = (selPrice / 10) * 10; // Round to 10
	accessory->setSellPrice(selPrice);
}

void EconomyFreeMarket::accessorySold(Tank *tank, 
		const char *accessoryName)
{
	// Do nothing (yet) on a sold item
}

