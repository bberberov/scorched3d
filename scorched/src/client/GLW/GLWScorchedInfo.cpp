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

#include <GLW/GLWScorchedInfo.h>
#include <GLW/GLWFont.h>
#include <GLW/GLWTranslate.h>
#include <GLW/GLWWindView.h>
#include <GLW/GLWColors.h>
#include <client/ScorchedClient.h>
#include <tankgraph/TargetRendererImplTank.h>
#include <target/TargetContainer.h>
#include <tank/TankState.h>
#include <tank/TankAvatar.h>
#include <tank/TankScore.h>
#include <tank/TankShotHistory.h>
#include <tanket/TanketShotInfo.h>
#include <tanket/TanketAccessories.h>
#include <target/TargetParachute.h>
#include <target/TargetLife.h>
#include <target/TargetShield.h>
#include <common/OptionsTransient.h>
#include <common/Defines.h>
#include <engine/Simulator.h>
#include <weapons/AccessoryStore.h>
#include <lang/CachedValueString.h>
#include <lang/LangResource.h>

REGISTER_CLASS_SOURCE(GLWHudCondition);

GLWHudCondition::GLWHudCondition()
{
}

GLWHudCondition::~GLWHudCondition()
{
}

bool GLWHudCondition::getResult(GLWidget *widget)
{
	return TargetRendererImplTankHUD::drawText();
}

REGISTER_CLASS_SOURCE(GLWScorchedInfo);

GLWScorchedInfo::GLWScorchedInfo(float x, float y, float w, float h) : 
	GLWidget(x, y, w, h), infoType_(eNone), noCenter_(false)
{

}

GLWScorchedInfo::~GLWScorchedInfo()
{

}

void GLWScorchedInfo::draw()
{
	LANG_RESOURCE_CONST_VAR(ON, "ON", "On");
	LANG_RESOURCE_CONST_VAR(OFF, "OFF", "Off");

	Vector *fontColor = &fontColor_;
	int mouseX = ScorchedClient::instance()->getGameState().getMouseX();
	int mouseY = ScorchedClient::instance()->getGameState().getMouseY();
	if (inBox((float) mouseX - GLWTranslate::getPosX(), 
		(float) mouseY - GLWTranslate::getPosY(), x_, y_, w_, h_))
	{
		fontColor = &selectedColor_;
	}

	GLWidget::draw();
	
	GLState state(GLState::TEXTURE_ON | GLState::DEPTH_OFF);
	
	// Items not relating to the current player
	switch(infoType_)
	{
		case eWind:
		{
			static WindDialogToolTip windTip;
			setToolTip(&windTip);

			static CachedValueString windSpeed;
			static float windWidth = 0.0f;
			if (windSpeed.hasChanged(ScorchedClient::instance()->
				getSimulator().getWind().getWindSpeed()))
			{
				if (windSpeed.cachedValue == 0)
				{
					windSpeed.cachedString = LANG_RESOURCE("NO_WIND", "No Wind");
				}
				else
				{
					windSpeed.cachedString = LANG_RESOURCE_1("WIND_FORCE", "Force {0}",
						S3D::formatStringBuffer("%.0f", windSpeed.cachedValue.asFloat()));
				}
				windWidth = GLWFont::instance()->
					getGameFont()->getWidth(fontSize_, windSpeed.cachedString);
			}

			float offSet = 0.0f;
			if (!noCenter_) 
			{
				offSet = w_ / 2.0f - (windWidth / 2.0f);
			}
			GLWFont::instance()->getGameShadowFont()->draw(
				GLWColors::black, fontSize_,
				x_ + offSet - 1.0f, y_ + 1.0f, 0.0f,
				windSpeed.cachedString);
			GLWFont::instance()->getGameFont()->draw(
				*fontColor, fontSize_,
				x_ + offSet, y_, 0.0f,
				windSpeed.cachedString);
		}
		break;
	}
	
	// Get the current tank and model
	Tank *current = 
 		ScorchedClient::instance()->getTargetContainer().getCurrentTank();
	if (!current ||
		current->getState().getState() != TankState::sNormal)
  	{
  		return;
	}
	TargetRendererImplTank *renderer = (TargetRendererImplTank *) 
		current->getRenderer();
	if (!renderer) return;

	// Items relating to the current player
	switch (infoType_)
	{
		case ePlayerName:
		{
			setToolTip(&renderer->getTips()->nameTip);

			float offSet = 0.0f;
			if (!noCenter_) 
			{
				float namewidth = GLWFont::instance()->getGameFont()->getWidth(
					fontSize_, current->getTargetName());
				offSet = w_ / 2.0f - (namewidth / 2.0f);
			}
			GLWFont::instance()->getGameShadowFont()->draw(
				GLWColors::black, fontSize_, x_ + offSet - 1,
				y_ + 1, 0.0f, 
				current->getTargetName());
			GLWFont::instance()->getGameFont()->draw(
				current->getColor(), fontSize_,
				x_ + offSet, y_, 0.0f,
				current->getTargetName());
		}
		break;
		case ePlayerIcon:
		{
			setToolTip(&renderer->getTips()->nameTip);
			GLState state(GLState::TEXTURE_ON | GLState::BLEND_ON);
			glColor3f((*fontColor)[0], (*fontColor)[1], (*fontColor)[2]);
			current->getAvatar().getTexture()->draw();
			glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 0.0f);
				glVertex2f(x_, y_);
				glTexCoord2f(1.0f, 0.0f);
				glVertex2f(x_ + w_, y_);
				glTexCoord2f(1.0f, 1.0f);
				glVertex2f(x_ + w_, y_ + h_);
				glTexCoord2f(0.0f, 1.0f);
				glVertex2f(x_, y_ + h_);
			glEnd();
		}
		break;
		case ePlayerRank:
			setToolTip(&renderer->getTips()->rankTip);
			if (current->getScore().getRank() > -1)
			{
				GLWFont::instance()->getGameFont()->draw(
					current->getColor(), fontSize_,
					x_, y_, 0.0f,
					S3D::formatStringBuffer("%i", current->getScore().getRank()));
			}
		break;
		case eAutoDefenseCount:
			setToolTip(&renderer->getTips()->autodTip);
			GLWFont::instance()->getGameFont()->draw(
				*fontColor, fontSize_,
				x_, y_, 0.0f,
				(current->getAccessories().getAutoDefense().haveDefense()?ON:OFF));
		break;
		case eParachuteCount:
			setToolTip(&renderer->getTips()->paraTip);
			if (!current->getParachute().getCurrentParachute())
			{
				GLWFont::instance()->getGameFont()->draw(
					*fontColor, fontSize_,
					x_, y_, 0.0f,
					OFF);
			}
			else
			{
				static CachedValueString paraCount;
				if (paraCount.hasChanged(
					current->getAccessories().getAccessoryCount(
					current->getParachute().getCurrentParachute())))
				{
					paraCount.cachedString = 
						current->getAccessories().getAccessoryCountString(
							current->getParachute().getCurrentParachute());
				}
				GLWFont::instance()->getGameFont()->draw(
					*fontColor, fontSize_,
					x_, y_, 0.0f,
					paraCount.cachedString);
			}
		break;
		case eHealthCount:
			{
				setToolTip(&renderer->getTips()->healthTip);

				static CachedValueString health;
				if (health.hasChanged(current->getLife().getLife()))
				{
					health.cachedString = LANG_STRING(S3D::formatStringBuffer("%.0f", 
						health.cachedValue.asFloat()));
				}
				GLWFont::instance()->getGameFont()->draw(
					*fontColor, fontSize_,
					x_, y_, 0.0f,
					health.cachedString);
			}
		break;
		case eShieldCount:
			setToolTip(&renderer->getTips()->shieldTip);
			if (!current->getShield().getCurrentShield())
			{
				GLWFont::instance()->getGameFont()->draw(
					*fontColor, fontSize_,
					x_, y_, 0.0f,
					OFF);
			}
			else
			{
				static CachedValueString shieldPower;
				if (shieldPower.hasChanged(current->getShield().getShieldPower()))
				{
					shieldPower.cachedString = LANG_STRING(S3D::formatStringBuffer("%.0f", 
						shieldPower.cachedValue.asFloat()));
				}
				GLWFont::instance()->getGameFont()->draw(
					*fontColor, fontSize_,
					x_, y_, 0.0f,
					shieldPower.cachedString);
			}
		break;
		case eBatteryCount:
			{
				setToolTip(&renderer->getTips()->batteryTip);

				static CachedValueString batteryCount;
				if (batteryCount.hasChanged(current->getAccessories().getBatteries().getNoBatteries()))
				{
					if (batteryCount.cachedValue == -1) 
						batteryCount.cachedString = LANG_RESOURCE("INF", "In");
					else batteryCount.cachedString = 
						LANG_STRING(S3D::formatStringBuffer("%i",	batteryCount.cachedValue.asInt()));
				}
				GLWFont::instance()->getGameFont()->draw(
					*fontColor, fontSize_,
					x_, y_, 0.0f,
					batteryCount.cachedString);
			}
		break;
		case eFuelCount:
			{
			setToolTip(&renderer->getTips()->fuelTip);

			Accessory *weapon = current->getAccessories().getWeapons().getCurrent();
			if (!weapon ||
				weapon->getPositionSelect() != Accessory::ePositionSelectFuel)
			{
				GLWFont::instance()->getGameFont()->draw(
					*fontColor, fontSize_,
					x_, y_, 0.0f,
					OFF);
			}
			else
			{
				static CachedValueString weaponCount;
				if (weaponCount.hasChanged(current->getAccessories().getAccessoryCount(weapon)))
				{
					weaponCount.cachedString = 
						current->getAccessories().getAccessoryCountString(weapon);
				}
				GLWFont::instance()->getGameFont()->draw(
					*fontColor, fontSize_,
					x_, y_, 0.0f,
					weaponCount.cachedString);
			}
			}
		break;
		case eWeaponName:
		{
			Accessory *weapon = current->getAccessories().getWeapons().getCurrent();
			if (!weapon) return;

			setToolTip(&renderer->getTips()->weaponTip);
			float offSet = 0.0f;
			if (!noCenter_) 
			{
				float weaponWidth = (float) GLWFont::instance()->getGameFont()->
					getWidth(fontSize_, weapon->getStringName());
				offSet = w_ / 2.0f - (weaponWidth / 2.0f);
			}
			GLWFont::instance()->getGameFont()->draw(
				*fontColor, fontSize_,
				x_ + offSet, y_, 0.0f,
				weapon->getStringName());
		}
		break;
		case eWeaponCount:
		{
			Accessory *weapon = current->getAccessories().getWeapons().getCurrent();
			if (!weapon) return;

			setToolTip(&renderer->getTips()->weaponTip);
			static CachedValueString weaponCount;
			if (weaponCount.hasChanged(current->getAccessories().getAccessoryCount(weapon)))
			{
				weaponCount.cachedString = current->getAccessories().
					getAccessoryCountString(weapon);
			}
			GLWFont::instance()->getGameFont()->draw(
				*fontColor, fontSize_,
				x_, y_, 0.0f,
				weaponCount.cachedString);
		}
		break;
		case eWeaponIcon:
		{
			Accessory *weapon = current->getAccessories().getWeapons().getCurrent();
			if (!weapon) return;

			setToolTip(&renderer->getTips()->weaponTip);

			GLState state(GLState::TEXTURE_ON | GLState::BLEND_ON);
			glColor3f((*fontColor)[0], (*fontColor)[1], (*fontColor)[2]);
			weapon->getTexture().draw();
			glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 0.0f);
				glVertex2f(x_, y_);
				glTexCoord2f(1.0f, 0.0f);
				glVertex2f(x_ + w_, y_);
				glTexCoord2f(1.0f, 1.0f);
				glVertex2f(x_ + w_, y_ + h_);
				glTexCoord2f(0.0f, 1.0f);
				glVertex2f(x_, y_ + h_);
			glEnd();
		}
		break;
		case eRotation:
			{
				setToolTip(&renderer->getTips()->rotationTip);
				static CachedValueString rotationValue;
				if (rotationValue.hasChanged(current->getShotInfo().getRotationGunXY()))
				{
					rotationValue.cachedString =
						LANG_STRING(S3D::formatStringBuffer("%.1f",
							360.0f - rotationValue.cachedValue.asFloat()));
				}
			
				GLWFont::instance()->getGameFont()->draw(
					*fontColor, fontSize_,
					x_, y_, 0.0f,
					rotationValue.cachedString);
			}
		break;
		case eRotationDiff:
			{
				static CachedValueString rotationValue;
				if (rotationValue.hasChanged(current->getShotHistory().getRotationXYDiff()))
				{
					rotationValue.cachedString = 
						LANG_STRING(S3D::formatStringBuffer("%+.1f",
							rotationValue.cachedValue.asFloat()));
				}

				GLWFont::instance()->getGameFont()->draw(
					*fontColor, fontSize_,
					x_, y_, 0.0f,
					rotationValue.cachedString);
			}
		break;
		case eElevation:
			{
				setToolTip(&renderer->getTips()->elevationTip);

				static CachedValueString elevationValue;
				if (elevationValue.hasChanged(current->getShotInfo().getRotationGunYZ()))
				{
					elevationValue.cachedString =
						LANG_STRING(S3D::formatStringBuffer("%.1f",
							elevationValue.cachedValue.asFloat()));

				}
				GLWFont::instance()->getGameFont()->draw(
					*fontColor, fontSize_,
					x_, y_, 0.0f,
					elevationValue.cachedString);
			}
		break;
		case eElevationDiff:
			{
				static CachedValueString elevationValue;
				if (elevationValue.hasChanged(current->getShotHistory().getRotationYZDiff()))
				{
					elevationValue.cachedString =
						LANG_STRING(S3D::formatStringBuffer("%+.1f",
							elevationValue.cachedValue.asFloat()));
				}
				GLWFont::instance()->getGameFont()->draw(
					*fontColor, fontSize_,
					x_, y_, 0.0f,
					elevationValue.cachedString);
			}
		break;
		case ePower:
			{
				setToolTip(&renderer->getTips()->powerTip);

				static CachedValueString powerValue;
				if (powerValue.hasChanged(current->getShotInfo().getPower()))
				{
					powerValue.cachedString =
						LANG_STRING(S3D::formatStringBuffer("%.1f",
							powerValue.cachedValue.asFloat()));
				}
				GLWFont::instance()->getGameFont()->draw(
					*fontColor, fontSize_,
					x_, y_, 0.0f,
					powerValue.cachedString);
			}
		break;
		case ePowerDiff:
			{
				static CachedValueString powerValue;
				if (powerValue.hasChanged(current->getShotHistory().getPowerDiff()))
				{
					powerValue.cachedString =
						LANG_STRING(S3D::formatStringBuffer("%+.1f",
							powerValue.cachedValue.asFloat()));
				}
				GLWFont::instance()->getGameFont()->draw(
					*fontColor, fontSize_,
					x_, y_, 0.0f,
					powerValue.cachedString);
			}
		break;
	}
}

void GLWScorchedInfo::mouseDown(int button, float x, float y, bool &skipRest)
{
	Tank *current = 
 		ScorchedClient::instance()->getTargetContainer().getCurrentTank();
	if (!current ||
		current->getState().getState() != TankState::sNormal)
  	{
  		return;
	}
	TargetRendererImplTank *renderer = (TargetRendererImplTank *) 
		current->getRenderer();
	if (!renderer) return;
	GLWTankTips *tankTips = renderer->getTips();

	if (inBox(x, y, x_, y_, w_, h_))
	{
		skipRest = true;

		switch (infoType_)
		{
			case ePlayerName:
			break;
			case ePlayerIcon:
			break;
			case ePlayerRank:
			break;
			case eAutoDefenseCount:
				tankTips->autodTip.showItems(GLWTranslate::getPosX() + x, 
					GLWTranslate::getPosY() + y);
			break;
			case eParachuteCount:
				tankTips->paraTip.showItems(GLWTranslate::getPosX() + x, 
					GLWTranslate::getPosY() + y);
			break;
			case eHealthCount:
			break;
			case eShieldCount:
				tankTips->shieldTip.showItems(GLWTranslate::getPosX() + x, 
					GLWTranslate::getPosY() + y);
			break;
			case eBatteryCount:
				tankTips->batteryTip.showItems(GLWTranslate::getPosX() + x, 
					GLWTranslate::getPosY() + y);
			break;
			case eFuelCount:
				tankTips->fuelTip.showItems(GLWTranslate::getPosX() + x, 
					GLWTranslate::getPosY() + y);
			break;
			case eWeaponName:
				tankTips->weaponTip.showItems(GLWTranslate::getPosX() + x, 
					GLWTranslate::getPosY() + y);
			break;
			case eWeaponCount:
				tankTips->weaponTip.showItems(GLWTranslate::getPosX() + x, 
					GLWTranslate::getPosY() + y);
			break;
			case eWeaponIcon:
				tankTips->weaponTip.showItems(GLWTranslate::getPosX() + x, 
					GLWTranslate::getPosY() + y);
			break;
			case eRotation:
				tankTips->undoMenu.showItems(GLWTranslate::getPosX() + x, 
					GLWTranslate::getPosY() + y);
			break;
			case eElevation:
				tankTips->undoMenu.showItems(GLWTranslate::getPosX() + x, 
					GLWTranslate::getPosY() + y);
			break;
			case ePower:
				tankTips->undoMenu.showItems(GLWTranslate::getPosX() + x, 
					GLWTranslate::getPosY() + y);
			break;
		}
	}
	else
	{
		GLWidget::mouseDown(button, x, y, skipRest);
	}
}

bool GLWScorchedInfo::initFromXML(XMLNode *node)
{
	if (!GLWidget::initFromXML(node)) return false;

	// Type node
	XMLNode *typeNode = 0;
	if (!node->getNamedChild("type", typeNode)) return false;
	if (0 == strcmp(typeNode->getContent(), "wind")) infoType_ = eWind;
	else if (0 == strcmp(typeNode->getContent(), "playername")) infoType_ = ePlayerName;
	else if (0 == strcmp(typeNode->getContent(), "playericon")) infoType_ = ePlayerIcon;
	else if (0 == strcmp(typeNode->getContent(), "playerrank")) infoType_ = ePlayerRank;
	else if (0 == strcmp(typeNode->getContent(), "autodefensecount")) infoType_ = eAutoDefenseCount;
	else if (0 == strcmp(typeNode->getContent(), "parachutecount")) infoType_ = eParachuteCount;
	else if (0 == strcmp(typeNode->getContent(), "shieldcount")) infoType_ = eShieldCount;
	else if (0 == strcmp(typeNode->getContent(), "healthcount")) infoType_ = eHealthCount;
	else if (0 == strcmp(typeNode->getContent(), "fuelcount")) infoType_ = eFuelCount;
	else if (0 == strcmp(typeNode->getContent(), "batterycount")) infoType_ = eBatteryCount;
	else if (0 == strcmp(typeNode->getContent(), "weaponname")) infoType_ = eWeaponName;
	else if (0 == strcmp(typeNode->getContent(), "weaponcount")) infoType_ = eWeaponCount;
	else if (0 == strcmp(typeNode->getContent(), "weaponicon")) infoType_ = eWeaponIcon;
	else if (0 == strcmp(typeNode->getContent(), "rotation")) infoType_ = eRotation;
	else if (0 == strcmp(typeNode->getContent(), "elevation")) infoType_ = eElevation;
	else if (0 == strcmp(typeNode->getContent(), "power")) infoType_ = ePower;
	else if (0 == strcmp(typeNode->getContent(), "rotationdiff")) infoType_ = eRotationDiff;
	else if (0 == strcmp(typeNode->getContent(), "elevationdiff")) infoType_ = eElevationDiff;
	else if (0 == strcmp(typeNode->getContent(), "powerdiff")) infoType_ = ePowerDiff;
	else
	{
		S3D::dialogMessage("GLWScorchedInfo", S3D::formatStringBuffer(
			"Unknown info type \"%s\"",
			typeNode->getContent()));
		return false;
	}
	
	// Font Size
	if (!node->getNamedChild("fontsize", fontSize_)) return false;
	
	// Font Color
	if (!node->getNamedChild("fontcolorr", fontColor_[0])) return false;
	if (!node->getNamedChild("fontcolorg", fontColor_[1])) return false;
	if (!node->getNamedChild("fontcolorb", fontColor_[2])) return false;

	// Font Selected Size
	if (!node->getNamedChild("selfontcolorr", selectedColor_[0], false))  selectedColor_[0] = fontColor_[0];
	if (!node->getNamedChild("selfontcolorg", selectedColor_[1], false))  selectedColor_[1] = fontColor_[1];
	if (!node->getNamedChild("selfontcolorb", selectedColor_[2], false))  selectedColor_[2] = fontColor_[2];

	// No Center
	XMLNode *centerNode;
	if (node->getNamedChild("nocenter", centerNode, false))
	{
		noCenter_ = (0 == strcmp(centerNode->getContent(), "true"));
	}

	return true;
}
