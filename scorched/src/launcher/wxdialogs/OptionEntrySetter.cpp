////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2004
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
//    You should have received a copy of the GNU General Public License
//    along with Scorched3D; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

#include <wxdialogs/OptionEntrySetter.h>
#include <wxdialogs/MainDialog.h>
#include <common/Defines.h>

OptionEntrySetter::OptionEntrySetter(wxControl *control, OptionEntry &entry) :
	control_(control), entry_(entry)
{
}

OptionEntrySetter::~OptionEntrySetter()
{
}

OptionEntrySetter OptionEntrySetterUtil::createOtherSetter(
	wxWindow *parent, wxSizer *sizer, OptionEntry &entry)
{
	wxStaticText *staticText = 0;
	sizer->Add(staticText = new wxStaticText(parent, -1,
		wxString(entry.getName(), wxConvUTF8)), 
		0, wxALIGN_RIGHT | wxRIGHT, 10);
	staticText->SetToolTip(
		wxString(entry.getDescription(), wxConvUTF8));

	wxControl *control = 0;
	switch (entry.getEntryType())
	{
	case OptionEntry::OptionEntryStringType:
		{
			sizer->Add(control = 
				new wxTextCtrl(parent, -1,
				wxT(""),
				wxDefaultPosition, wxDefaultSize,
				wxTE_PASSWORD), 
				0, wxALIGN_LEFT);
			control->SetToolTip(
				wxString(entry.getDescription(), wxConvUTF8));
		}
		break;
	case OptionEntry::OptionEntryBoundedIntType:
		{
			sizer->Add(control = 
				new wxComboBox(parent, -1,
				wxT(""),
				wxDefaultPosition, wxSize(160, -1),
				0, 0, 0), 
				0, wxALIGN_LEFT);
			control->SetToolTip(
				wxString(entry.getDescription(), wxConvUTF8));

			OptionEntryBoundedInt &boundedInt = (OptionEntryBoundedInt &) entry;
			for (int i=boundedInt.getMinValue(); 
				i<=boundedInt.getMaxValue(); 
				i+=boundedInt.getStepValue())
			{
				((wxComboBox *) control)->Append(
					convertString(S3D::formatStringBuffer("%i", i)), 
					(void *) i);
			}
		}
		break;
	case OptionEntry::OptionEntryEnumType:
		{
			sizer->Add(control = 
				new wxComboBox(parent, -1,
				wxT(""),
				wxDefaultPosition, wxSize(160, -1),
				0, 0, wxCB_READONLY), 
				0, wxALIGN_LEFT);
			control->SetToolTip(
				wxString(entry.getDescription(), wxConvUTF8));

			OptionEntryEnum &optionEntryEnum = (OptionEntryEnum &) entry;
			OptionEntryEnum::EnumEntry *enums = optionEntryEnum.getEnums();
			for (OptionEntryEnum::EnumEntry *current = enums; current->description[0]; current++)
			{
				((wxComboBox *) control)->Append(
					wxString(current->description, wxConvUTF8), 
					(void *) current->value);
			}
		}
		break;
	case OptionEntry::OptionEntryStringEnumType:
		{
			sizer->Add(control = 
				new wxComboBox(parent, -1,
				wxT(""),
				wxDefaultPosition, wxSize(160, -1),
				0, 0, wxCB_READONLY), 
				0, wxALIGN_LEFT);
			control->SetToolTip(
				wxString(entry.getDescription(), wxConvUTF8));

			OptionEntryStringEnum &optionEntryStringEnum = (OptionEntryStringEnum &) entry;
			OptionEntryStringEnum::EnumEntry *enums = optionEntryStringEnum.getEnums();
			for (OptionEntryStringEnum::EnumEntry *current = enums; current->value[0]; current++)
			{
				((wxComboBox *) control)->Append(
					wxString(current->value, wxConvUTF8));
			}
		}
		break;
	case OptionEntry::OptionEntryBoolType:
		{
			sizer->Add(control = 
				new wxCheckBox(parent, -1,
					wxT(""),
					wxDefaultPosition, wxDefaultSize),
					0, wxALIGN_LEFT);
			control->SetToolTip(
				wxString(entry.getDescription(), wxConvUTF8));
		}
		break;
	default:
		S3D::dialogExit("createOtherSetter",
			S3D::formatStringBuffer("Unhandled OptionEntry type %s:%i",
			entry.getName(), entry.getEntryType()));
	}

	return OptionEntrySetter(control, entry);
}

void OptionEntrySetterUtil::updateControls(
	std::list<OptionEntrySetter> &controls)
{
	std::list<OptionEntrySetter>::iterator itor;
	for (itor = controls.begin();
		itor != controls.end();
		++itor)
	{
		OptionEntrySetter &entrySetter = *itor;
		switch (entrySetter.getEntry().getEntryType())
		{
		case OptionEntry::OptionEntryStringType:
			{
				wxTextCtrl *control = (wxTextCtrl *) entrySetter.getControl();
				control->SetValue(wxString(entrySetter.getEntry().getValueAsString(), wxConvUTF8));
			}
			break;
		case OptionEntry::OptionEntryBoundedIntType:
		case OptionEntry::OptionEntryEnumType:
		case OptionEntry::OptionEntryStringEnumType:
			{
				wxComboBox *control = (wxComboBox *) entrySetter.getControl();
				control->SetValue(wxString(entrySetter.getEntry().getValueAsString(), wxConvUTF8));
			}
			break;
		case OptionEntry::OptionEntryBoolType:
			{
				OptionEntryBool &boolEntry = (OptionEntryBool &) entrySetter.getEntry();
				wxCheckBox *control = (wxCheckBox *) entrySetter.getControl();
				control->SetValue(boolEntry.getValue());
			}
			break;
		}
	}
}

void OptionEntrySetterUtil::updateEntries(
	std::list<OptionEntrySetter> &controls)
{
	std::list<OptionEntrySetter>::iterator itor;
	for (itor = controls.begin();
		itor != controls.end();
		++itor)
	{
		OptionEntrySetter &entrySetter = *itor;
		switch (entrySetter.getEntry().getEntryType())
		{
		case OptionEntry::OptionEntryStringType:
			{
				wxTextCtrl *control = (wxTextCtrl *) entrySetter.getControl();
				entrySetter.getEntry().setValueFromString(
					std::string(control->GetValue().mb_str(wxConvUTF8)));
			}
			break;
		case OptionEntry::OptionEntryBoundedIntType:
		case OptionEntry::OptionEntryEnumType:
		case OptionEntry::OptionEntryStringEnumType:
			{
				wxComboBox *control = (wxComboBox *) entrySetter.getControl();
				entrySetter.getEntry().setValueFromString(
					std::string(control->GetValue().mb_str(wxConvUTF8)));
			}
			break;
		case OptionEntry::OptionEntryBoolType:
			{
				OptionEntryBool &boolEntry = (OptionEntryBool &) entrySetter.getEntry();
				wxCheckBox *control = (wxCheckBox *) entrySetter.getControl();
				boolEntry.setValue(control->GetValue());
			}
			break;
		}
	}
}
