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

#include <GLW/GLWChannelViewTextRenderer.h>
#include <GLW/GLWChannelView.h>
#include <GLW/GLWTranslate.h>
#include <target/TargetContainer.h>
#include <tank/TankAvatar.h>
#include <tankgraph/TargetRendererImplTank.h>
#include <client/ScorchedClient.h>
#include <lang/LangResource.h>
#include <weapons/AccessoryStore.h>

GLWChannelViewTextRenderer::GLWChannelViewTextRenderer()
{
}

GLWChannelViewTextRenderer::~GLWChannelViewTextRenderer()
{
}

bool GLWChannelViewTextRenderer::drawCharacter(
	unsigned int character,
	int charPosition, Vector &position, 
	GLFont2dStorage::CharEntry &charEntry, Vector4 &color)
{
	ChannelTextEntry *textEntry = getEntry(charPosition);
	if (!textEntry) return true;

	// Set the appropriate tool tip
	if (GLWToolTip::instance()->addToolTip(
		&toolTip_,
		GLWTranslate::getPosX() + position[0], 
		GLWTranslate::getPosY() + position[1],
		12.0f, 18.0f))
	{
		switch (textEntry->type)
		{
		case ePlayerEntry:
			{
				TargetRendererImplTank *renderer = 0;
				Tank *tank = ScorchedClient::instance()->getTargetContainer().
					getTankById(textEntry->data);
				if (tank) renderer = (TargetRendererImplTank *) tank->getRenderer();
				if (renderer)
				{
					GLWToolTip::instance()->addToolTip(
						&renderer->getTips()->tankTip,
						GLWTranslate::getPosX() + position[0], 
						GLWTranslate::getPosY() + position[1],
						12.0f, 18.0f);
				}
				else
				{
					toolTip_.setText(ToolTip::ToolTipInfo, 
						LANG_RESOURCE("PLAYER_DISCONECTED", "Player disconnected"),
						textEntry->part);
				}
			}
			break;
		case eChannelEntry:
			toolTip_.setText(ToolTip::ToolTipHelp, 
				LANG_RESOURCE("CHANNEL", "Channel"),
				textEntry->part);		
			break;
		case eWeaponEntry:
			{
				Accessory *accessory = ScorchedClient::instance()->getAccessoryStore().
					findByAccessoryId(textEntry->data);
				if (accessory)
				{
					GLWToolTip::instance()->addToolTip(
						&accessory->getToolTip(),
						GLWTranslate::getPosX() + position[0], 
						GLWTranslate::getPosY() + position[1],
						12.0f, 18.0f);
				}
				else
				{
					toolTip_.setText(ToolTip::ToolTipHelp, 
						LANG_RESOURCE("WEAPON", "Weapon"),
						textEntry->part);	
				}
			}
			break;
		case eAdminEntry:
			toolTip_.setText(ToolTip::ToolTipHelp, 
				LANG_RESOURCE("ADMIN", "Admin"),
				textEntry->part);	
			break;
		}
	}

	// Render any player avatars
	if (textEntry->type == ePlayerEntry &&
		character == 3473 &&
		textEntry->data)
	{
		glColor4f(1.0f, 1.0f, 1.0f, color[3]);

		{
			Tank *tank = ScorchedClient::instance()->getTargetContainer().
				getTankById(textEntry->data);
			if (tank) tank->getAvatar().getTexture()->draw(true);
			else TankAvatar::getDefaultTexture()->draw(true);
		}
			
		glPushMatrix();
			glTranslatef((float) charEntry.left, 0.0f, 0.0f);
			glTranslatef(0.0f, (float) charEntry.rows, 0.0f);
			glBegin(GL_QUADS);
				glTexCoord2f(0.0f,1.0f); glVertex2f(0.0f,(float)charEntry.height);
				glTexCoord2f(0.0f,0.0f); glVertex2f(0.0f,0.0f);
				glTexCoord2f(1.0f,0.0f); glVertex2f((float)charEntry.width,0.0f);
				glTexCoord2f(1.0f,1.0f); glVertex2f((float)charEntry.width,(float)charEntry.height);
			glEnd();
		glPopMatrix();
		glTranslatef((float)charEntry.advances ,0.0f ,0.0f);
		return false;
	}

	// Set the appropriate text colours etc.
	switch (textEntry->type)
	{
	case eChannelEntry:
		break;
	default:
		color[0] = textEntry->color[0];
		color[1] = textEntry->color[1];
		color[2] = textEntry->color[2];
		break;
	}

	return true;
}
