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

#include <GLW/GLWOptionEntry.h>
#include <GLW/GLWLabel.h>
#include <GLW/GLWPanel.h>
#include <GLW/GLWCheckBox.h>
#include <GLW/GLWDropDownText.h>
#include <GLW/GLWTextBox.h>
#include <engine/ModDirs.h>
#include <common/Defines.h>
#include <common/ToolTip.h>

GLWOptionEntry::GLWOptionEntry(GLWidget *control, OptionEntry *entry) :
	control_(control), entry_(entry)
{
}

GLWOptionEntry::~GLWOptionEntry()
{
}

void GLWOptionEntry::createEntry(
	std::list<GLWOptionEntry> &controls, 
	GLWPanel *parent, OptionEntry &entry)
{
	std::string descriptionName;
	descriptionName.append(entry.getName()).append("_description");

	GLWLabel *staticText = new GLWLabel(0.0f, 0.0f, 
		LANG_RESOURCE(entry.getName(), entry.getName()));
	staticText->setToolTip(new ToolTip(ToolTip::ToolTipHelp, 
		LANG_RESOURCE(entry.getName(), entry.getName()), 
		LANG_RESOURCE(descriptionName, entry.getDescription())));
	parent->addWidget(staticText, 0, GLWPanel::AlignRight | 
		GLWPanel::SpaceLeft | GLWPanel::SpaceTop, 10.0f);

	GLWidget *control = 0;
	if (0 == strcmp(entry.getName(), "Mod"))
	{
		control = new GLWDropDownText(0.0f, 0.0f, 170.0f);
		ModDirs modDirs;
		modDirs.loadModDirs();
		std::list<ModInfo>::iterator itor;
		for (itor = modDirs.getDirs().begin();
			itor != modDirs.getDirs().end();
			++itor)
		{
			ModInfo &info = (*itor);
			((GLWDropDownText *) control)->addText(
				LANG_RESOURCE(info.getName(), info.getName()), info.getName());
		}
	}
	else
	switch (entry.getEntryType())
	{
	case OptionEntry::OptionEntryStringType:
		{
			control = new GLWTextBox(0.0f, 0.0f, 170.0f);
		}
		break;
	case OptionEntry::OptionEntryBoundedIntType:
		{
			control = new GLWDropDownText(0.0f, 0.0f, 170.0f);
			OptionEntryBoundedInt &boundedInt = (OptionEntryBoundedInt &) entry;
			for (int i=boundedInt.getMinValue(); 
				i<=boundedInt.getMaxValue(); 
				i+=boundedInt.getStepValue())
			{
				std::string value = S3D::formatStringBuffer("%i", i);
				((GLWDropDownText *) control)->addText(LANG_STRING(value), value);
			}
		}
		break;
	case OptionEntry::OptionEntryEnumType:
		{
			control = new GLWDropDownText(0.0f, 0.0f, 170.0f);

			OptionEntryEnum &optionEntryEnum = (OptionEntryEnum &) entry;
			OptionEntryEnum::EnumEntry *enums = optionEntryEnum.getEnums();
			for (OptionEntryEnum::EnumEntry *current = enums; current->description[0]; current++)
			{
				((GLWDropDownText *) control)->addEntry(
					GLWSelectorEntry(LANG_RESOURCE(current->description, current->description), 
						0, false, 0, (void *) current->value, current->description));
			}
		}
		break;
	case OptionEntry::OptionEntryStringEnumType:
		{
			control = new GLWDropDownText(0.0f, 0.0f, 170.0f);

			OptionEntryStringEnum &optionEntryStringEnum = (OptionEntryStringEnum &) entry;
			OptionEntryStringEnum::EnumEntry *enums = optionEntryStringEnum.getEnums();
			for (OptionEntryStringEnum::EnumEntry *current = enums; current->value[0]; current++)
			{
				((GLWDropDownText *) control)->addText(LANG_RESOURCE(current->value, current->value), 
					current->value);
			}
		}
		break;
	case OptionEntry::OptionEntryBoolType:
		{
			control = new GLWCheckBox();
		}
		break;
	default:
		S3D::dialogExit("GLWOptionEntry::createEntry",
			S3D::formatStringBuffer("Unhandled OptionEntry type %s:%i",
			entry.getName(), entry.getEntryType()));
	}

	control->setToolTip(new ToolTip(ToolTip::ToolTipHelp, 
		LANG_RESOURCE(entry.getName(), entry.getName()), 
		LANG_RESOURCE(descriptionName, entry.getDescription())));
	parent->addWidget(control, 0, 
		GLWPanel::SpaceRight | GLWPanel::SpaceLeft | GLWPanel::SpaceTop, 10.0f);

	controls.push_back(GLWOptionEntry(control, &entry));
}

void GLWOptionEntry::updateControls(
	std::list<GLWOptionEntry> &controls)
{
	std::list<GLWOptionEntry>::iterator itor;
	for (itor = controls.begin();
		itor != controls.end();
		++itor)
	{
		GLWOptionEntry &entrySetter = *itor;

		if (0 == strcmp(entrySetter.getEntry()->getName(), "Mod"))
		{
			GLWDropDownText *control = (GLWDropDownText *) entrySetter.getControl();
			control->setCurrentText(LANG_RESOURCE(entrySetter.getEntry()->getValueAsString(),
				entrySetter.getEntry()->getValueAsString()));			
		}
		else
		switch (entrySetter.getEntry()->getEntryType())
		{
		case OptionEntry::OptionEntryStringType:
			{
				GLWTextBox *control = (GLWTextBox *) entrySetter.getControl();
				control->setText(LANG_STRING(entrySetter.getEntry()->getValueAsString()));
			}
			break;
		case OptionEntry::OptionEntryBoundedIntType:
			{
				GLWDropDownText *control = (GLWDropDownText *) entrySetter.getControl();
				control->setCurrentText(LANG_STRING(entrySetter.getEntry()->getValueAsString()));
			}
			break;
		case OptionEntry::OptionEntryEnumType:
		case OptionEntry::OptionEntryStringEnumType:
			{
				GLWDropDownText *control = (GLWDropDownText *) entrySetter.getControl();
				control->setCurrentText(LANG_RESOURCE(entrySetter.getEntry()->getValueAsString(),
					entrySetter.getEntry()->getValueAsString()));
			}
			break;
		case OptionEntry::OptionEntryBoolType:
			{
				OptionEntryBool *boolEntry = (OptionEntryBool *) entrySetter.getEntry();
				GLWCheckBox *control = (GLWCheckBox *) entrySetter.getControl();
				control->setState(boolEntry->getValue());
			}
			break;
		}
	}
}

void GLWOptionEntry::updateEntries(
	std::list<GLWOptionEntry> &controls)
{
	std::list<GLWOptionEntry>::iterator itor;
	for (itor = controls.begin();
		itor != controls.end();
		++itor)
	{
		GLWOptionEntry &entrySetter = *itor;

		if (0 == strcmp(entrySetter.getEntry()->getName(), "Mod"))
		{
			GLWDropDownText *control = (GLWDropDownText *) entrySetter.getControl();
			entrySetter.getEntry()->setValueFromString(control->getCurrentDataText());		
		}
		else
		switch (entrySetter.getEntry()->getEntryType())
		{
		case OptionEntry::OptionEntryStringType:
			{
				GLWTextBox *control = (GLWTextBox *) entrySetter.getControl();
				entrySetter.getEntry()->setValueFromString(control->getText().c_str());	
			}
			break;
		case OptionEntry::OptionEntryBoundedIntType:
		case OptionEntry::OptionEntryEnumType:
		case OptionEntry::OptionEntryStringEnumType:
			{
				GLWDropDownText *control = (GLWDropDownText *) entrySetter.getControl();
				entrySetter.getEntry()->setValueFromString(control->getCurrentDataText());
			}
			break;
		case OptionEntry::OptionEntryBoolType:
			{
				OptionEntryBool *boolEntry = (OptionEntryBool *) entrySetter.getEntry();
				GLWCheckBox *control = (GLWCheckBox *) entrySetter.getControl();
				boolEntry->setValue(control->getState());
			}
			break;
		}
	}
}
