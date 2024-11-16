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

#include <GLW/GLWScorchedInfoSpinner.h>
#include <GLW/GLWTranslate.h>
#include <GLW/GLWTankTip.h>
#include <GLEXT/GLState.h>
#include <client/ScorchedClient.h>
#include <target/TargetRenderer.h>
#include <image/ImageFactory.h>
#include <target/TargetContainer.h>
#include <tank/TankState.h>
#include <tanket/TanketShotInfo.h>
#include <target/TargetLife.h>
#include <tankgraph/TargetRendererImplTank.h>

REGISTER_CLASS_SOURCE(GLWScorchedInfoSpinner);

GLWScorchedInfoSpinner::GLWScorchedInfoSpinner(
	float x, float y, float w, float h) : 
	GLWidget(x, y, w, h), infoType_(eNone),
	filledColor_(1.0f, 1.0f, 1.0f),
	unfilledColor_(1.0f, 1.0f, 1.0f)
{

}

GLWScorchedInfoSpinner::~GLWScorchedInfoSpinner()
{

}

void GLWScorchedInfoSpinner::draw()
{
	GLWidget::draw();
	
	GLState state(GLState::TEXTURE_ON | GLState::BLEND_ON | GLState::DEPTH_OFF);
	
	// Get the current tank and model
	Tank *current = 
 		ScorchedClient::instance()->getTargetContainer().getCurrentTank();
	if (!current ||
		current->getState().getState() != TankState::sNormal)
  	{
  		return;
	}

	float maxValue = 100.0f;
	float minValue = 0.0f;
	float currentValue = 0.0f;

	// Items relating to the current player
	switch (infoType_)
	{
	case eNone:
		break;
	case eRotation:
		minValue = 0.0f;
		maxValue = 360.0f;
		currentValue = current->getShotInfo().getRotationGunXY().asFloat();
		break;
	case eElevation:
		minValue = 0.0f;
		maxValue = 90.0f;
		currentValue = current->getShotInfo().getRotationGunYZ().asFloat();
		break;
	case ePower:
		minValue = 0.0f;
		maxValue = current->getShotInfo().getMaxPower().asFloat();
		currentValue = current->getShotInfo().getPower().asFloat();
		break;
	}

	if (currentValue < minValue) currentValue = minValue;
	if (currentValue > maxValue) currentValue = maxValue;
	float range = maxValue - minValue;
	float percentage = (currentValue - minValue) / range;
	float degrees = percentage * PI * 2.0f;

	float x = x_ + halfWidth_;
	float y = y_ + halfHeight_;
	float dx = sinf(degrees);
	float dy = cosf(degrees);

	float posx = 0.0f, posy = 0.0f;
	float tx = 0.0f, ty = 0.0f;
	if (percentage < 0.125 || percentage > 0.875)
	{
		posx = dx * halfWidth_ / dy;
		posy = halfHeight_;
		tx = posx / w_ + 0.5f;
		ty = 1.0;
	} 
	else if (percentage < 0.375)
	{
		posx = halfWidth_;
		posy = dy * halfHeight_ / dx;
		tx = 1.0f;
		ty = posy / h_ + 0.5f;
	}
	else if (percentage < 0.625)
	{
		posx = -dx * halfWidth_ / dy;
		posy = -halfHeight_;
		tx = posx / w_ + 0.5f;
		ty = 0.0f;
	}
	else 
	{
		posx = -halfWidth_;
		posy = -dy * halfHeight_ / dx;
		tx = 0.0f;
		ty = posy / h_ + 0.5f;
	}

	glColor3f(filledColor_[0], filledColor_[1], filledColor_[2]);
	filledTex_.draw();
	glBegin(GL_TRIANGLE_FAN);
		glTexCoord2f(0.5f, 0.5f);
		glVertex2f(x, y);
		glTexCoord2f(tx, ty);
		glVertex2f(x + posx, y + posy);
		if (percentage > 0.875)
		{
			glTexCoord2f(0.0f, 1.0f);
			glVertex2f(x - halfWidth_, y + halfHeight_);
		}
		if (percentage > 0.625)
		{
			glTexCoord2f(0.0f, 0.0f);
			glVertex2f(x - halfWidth_, y - halfHeight_);
		}
		if (percentage > 0.375)
		{
			glTexCoord2f(1.0f, 0.0f);
			glVertex2f(x + halfWidth_, y - halfHeight_);
		}
		if (percentage > 0.125)
		{
			glTexCoord2f(1.0f, 1.0f);
			glVertex2f(x + halfWidth_, y + halfHeight_);
		} 
		glTexCoord2f(0.5f, 1.0f);
		glVertex2f(x, y + halfHeight_);
	glEnd();

	glColor3f(unfilledColor_[0], unfilledColor_[1], unfilledColor_[2]);
	unfilledTex_.draw();
	glBegin(GL_TRIANGLE_FAN);
		glTexCoord2f(0.5f, 0.5f);
		glVertex2f(x, y);
		glTexCoord2f(0.5f, 1.0f);
		glVertex2f(x, y + halfHeight_);
		if (percentage < 0.875)
		{
			glTexCoord2f(0.0f, 1.0f);
			glVertex2f(x - halfWidth_, y + halfHeight_);
		}
		if (percentage < 0.625)
		{
			glTexCoord2f(0.0f, 0.0f);
			glVertex2f(x - halfWidth_, y - halfHeight_);
		}
		if (percentage < 0.375)
		{
			glTexCoord2f(1.0f, 0.0f);
			glVertex2f(x + halfWidth_, y - halfHeight_);
		}
		if (percentage < 0.125)
		{
			glTexCoord2f(1.0f, 1.0f);
			glVertex2f(x + halfWidth_, y + halfHeight_);
		} 
		glTexCoord2f(tx, ty);
		glVertex2f(x + posx, y + posy);
	glEnd();
}

void GLWScorchedInfoSpinner::mouseDown(int button, float x, float y, bool &skipRest)
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
			default:
				break;
		}
	}
	else
	{
		GLWidget::mouseDown(button, x, y, skipRest);
	}
}

bool GLWScorchedInfoSpinner::initFromXML(XMLNode *node)
{
	if (!GLWidget::initFromXML(node)) return false;

	halfHeight_ = h_ / 2.0f;
	halfWidth_ = w_ /2.0f;

	// Colors
	node->getNamedChild("filledcolor", filledColor_, false);
	node->getNamedChild("unfilledcolor", unfilledColor_, false);

	// Images
	std::string filledImageName, unfilledImageName;
	if (!node->getNamedChild("filledimage", filledImageName)) return false;
	if (!node->getNamedChild("unfilledimage", unfilledImageName)) return false;

	filledTex_.setImageID(ImageID(S3D::eModLocation, filledImageName));
	unfilledTex_.setImageID(ImageID(S3D::eModLocation, unfilledImageName));

	// Type node
	std::string type;
	if (!node->getNamedChild("type", type)) return false;
	if (type == "rotation") infoType_ = eRotation;
	else if (type == "elevation") infoType_ = eElevation;
	else if (type == "power") infoType_ = ePower;
	else
	{
		S3D::dialogMessage("GLWScorchedInfoSpinner", S3D::formatStringBuffer(
			"Unknown info type \"%s\"",
			type.c_str()));
		return false;
	}
	
	return true;
}
