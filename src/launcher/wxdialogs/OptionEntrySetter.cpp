////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2004, 2025, 2026
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

#include "OptionEntrySetter.hpp"
#include <wxdialogs/MainDialog.hpp>
#include <common/Defines.hpp>

OptionEntrySetter::OptionEntrySetter( wxControl* control, OptionEntry& entry ) : control_( control ), entry_( entry ) {}

OptionEntrySetter::~OptionEntrySetter() {}

wxControl* OptionEntrySetter::getControl()
{
	return control_;
}

OptionEntry& OptionEntrySetter::getEntry()
{
	return entry_;
}

OptionEntrySetter OptionEntrySetterUtil::createOtherSetter( wxWindow* parent, wxSizer* sizer, OptionEntry& entry )
{
	wxStaticText* staticText = new wxStaticText( parent, -1, wxString( entry.getName(), wxConvUTF8 ) );
	staticText->SetToolTip( wxString( entry.getDescription(), wxConvUTF8 ) );
	sizer->Add( staticText, 0, wxALIGN_RIGHT | wxRIGHT, 10 );

	wxControl* control = nullptr;
	switch ( entry.getEntryType() )
	{
		case OptionEntry::OptionEntryStringType:
		{
			control = new wxTextCtrl( parent, -1, wxT( "" ), wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD );
			control->SetToolTip( wxString( entry.getDescription(), wxConvUTF8 ) );
			sizer->Add( control, 0, wxALIGN_LEFT );
		}
		break;
		case OptionEntry::OptionEntryBoundedIntType:
		{
			control = new wxComboBox( parent, -1, wxT( "" ), wxDefaultPosition, wxSize( 160, -1 ), 0, nullptr, 0 );
			control->SetToolTip( wxString( entry.getDescription(), wxConvUTF8 ) );
			sizer->Add( control, 0, wxALIGN_LEFT );

			OptionEntryBoundedInt& boundedInt = (OptionEntryBoundedInt&)entry;
			for ( int i = boundedInt.getMinValue(); i <= boundedInt.getMaxValue(); i += boundedInt.getStepValue() )
			{
				( (wxComboBox*)control )->Append(
					convertString( S3D::formatStringBuffer( "%i", i ) ),
					(void*)(long)i  // NOTE: pass value directly as void*
				);
			}
		}
		break;
		case OptionEntry::OptionEntryEnumType:
		{
			control = new wxComboBox(
				parent,
				-1,
				wxT( "" ),
				wxDefaultPosition,
				wxSize( 160, -1 ),
				0,
				nullptr,
				wxCB_READONLY
			);
			control->SetToolTip( wxString( entry.getDescription(), wxConvUTF8 ) );
			sizer->Add( control, 0, wxALIGN_LEFT );

			OptionEntryEnum&            optionEntryEnum = (OptionEntryEnum&)entry;
			OptionEntryEnum::EnumEntry* enums           = optionEntryEnum.getEnums();
			for ( OptionEntryEnum::EnumEntry* current = enums; current->description[0]; current++ )
			{
				( (wxComboBox*)control )->Append(
					wxString( current->description, wxConvUTF8 ),
					(void*)(long)( current->value )  // NOTE: pass value directly as void*
				);
			}
		}
		break;
		case OptionEntry::OptionEntryStringEnumType:
		{
			control = new wxComboBox(
				parent,
				-1,
				wxT( "" ),
				wxDefaultPosition,
				wxSize( 160, -1 ),
				0,
				nullptr,
				wxCB_READONLY
			);
			control->SetToolTip( wxString( entry.getDescription(), wxConvUTF8 ) );
			sizer->Add( control, 0, wxALIGN_LEFT );

			OptionEntryStringEnum&            optionEntryStringEnum = (OptionEntryStringEnum&)entry;
			OptionEntryStringEnum::EnumEntry* enums                 = optionEntryStringEnum.getEnums();
			for ( OptionEntryStringEnum::EnumEntry* current = enums; current->value[0]; current++ )
			{
				( (wxComboBox*)control )->Append( wxString( current->value, wxConvUTF8 ) );
			}
		}
		break;
		case OptionEntry::OptionEntryBoolType:
		{
			control = new wxCheckBox( parent, -1, wxT( "" ), wxDefaultPosition, wxDefaultSize );
			control->SetToolTip( wxString( entry.getDescription(), wxConvUTF8 ) );
			sizer->Add( control, 0, wxALIGN_LEFT );
		}
		break;
		default:
			S3D::dialogExit(
				"createOtherSetter",
				S3D::formatStringBuffer( "Unhandled OptionEntry type %s:%i", entry.getName(), entry.getEntryType() )
			);
	}

	return OptionEntrySetter( control, entry );
}

void OptionEntrySetterUtil::updateControls( std::list< OptionEntrySetter >& controls )
{
	std::list< OptionEntrySetter >::iterator itor;
	for ( itor = controls.begin(); itor != controls.end(); ++itor )
	{
		OptionEntrySetter& entrySetter = *itor;
		switch ( entrySetter.getEntry().getEntryType() )
		{
			case OptionEntry::OptionEntryStringType:
			{
				wxTextCtrl* control = (wxTextCtrl*)entrySetter.getControl();
				control->SetValue( wxString( entrySetter.getEntry().getValueAsString(), wxConvUTF8 ) );
			}
			break;
			case OptionEntry::OptionEntryBoundedIntType:
			case OptionEntry::OptionEntryEnumType:
			case OptionEntry::OptionEntryStringEnumType:
			{
				wxComboBox* control = (wxComboBox*)entrySetter.getControl();
				control->SetValue( wxString( entrySetter.getEntry().getValueAsString(), wxConvUTF8 ) );
			}
			break;
			case OptionEntry::OptionEntryBoolType:
			{
				OptionEntryBool& boolEntry = (OptionEntryBool&)entrySetter.getEntry();
				wxCheckBox*      control   = (wxCheckBox*)entrySetter.getControl();
				control->SetValue( boolEntry.getValue() );
			}
			break;
			default:
				S3D::dialogExit(
					"updateControls",
					S3D::formatStringBuffer(
						"Unhandled OptionEntry type %s:%i",
						entrySetter.getEntry().getName(),
						entrySetter.getEntry().getEntryType()
					)
				);
		}
	}
}

void OptionEntrySetterUtil::updateEntries( std::list< OptionEntrySetter >& controls )
{
	std::list< OptionEntrySetter >::iterator itor;
	for ( itor = controls.begin(); itor != controls.end(); ++itor )
	{
		OptionEntrySetter& entrySetter = *itor;
		switch ( entrySetter.getEntry().getEntryType() )
		{
			case OptionEntry::OptionEntryStringType:
			{
				wxTextCtrl* control = (wxTextCtrl*)entrySetter.getControl();
				entrySetter.getEntry().setValueFromString( std::string( control->GetValue().mb_str( wxConvUTF8 ) ) );
			}
			break;
			case OptionEntry::OptionEntryBoundedIntType:
			case OptionEntry::OptionEntryEnumType:
			case OptionEntry::OptionEntryStringEnumType:
			{
				wxComboBox* control = (wxComboBox*)entrySetter.getControl();
				entrySetter.getEntry().setValueFromString( std::string( control->GetValue().mb_str( wxConvUTF8 ) ) );
			}
			break;
			case OptionEntry::OptionEntryBoolType:
			{
				OptionEntryBool& boolEntry = (OptionEntryBool&)entrySetter.getEntry();
				wxCheckBox*      control   = (wxCheckBox*)entrySetter.getControl();
				boolEntry.setValue( control->GetValue() );
			}
			break;
			default:
				S3D::dialogExit(
					"updateEntries",
					S3D::formatStringBuffer(
						"Unhandled OptionEntry type %s:%i",
						entrySetter.getEntry().getName(),
						entrySetter.getEntry().getEntryType()
					)
				);
		}
	}
}
