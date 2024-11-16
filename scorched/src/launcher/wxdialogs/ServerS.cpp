////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2003
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

#include "ServerS-def.cpp"

static void createControls(wxWindow *parent,
						   wxSizer *sizer)
{
	wxStaticBox *servernameBox = 
		new wxStaticBox(parent, -1, wxT("Server Settings"));
	wxStaticBoxSizer *servernameSizer = 
		new wxStaticBoxSizer(servernameBox, wxVERTICAL);
	wxFlexGridSizer *servernameSizer2 = new wxFlexGridSizer(4, 2, 5, 5);
	IDC_SERVER_NAME_CTRL = 
		new wxTextCtrl(parent, IDC_SERVER_NAME,
		wxT(""),
		wxDefaultPosition, wxSize((int) 226.5, -1));
	servernameSizer2->Add(new wxStaticText(parent, -1, wxT("Server Name :")), 
						  0, wxALIGN_CENTER_VERTICAL);
	servernameSizer2->Add(IDC_SERVER_NAME_CTRL);
	IDC_SERVER_PORT_CTRL = 
		new wxTextCtrl(parent, -1,
		wxT(""),
		wxDefaultPosition, wxSize((int) 72, -1));
	servernameSizer2->Add(new wxStaticText(parent, -1, wxT("Port Number :")),
						  0, wxALIGN_CENTER_VERTICAL);
	servernameSizer2->Add(IDC_SERVER_PORT_CTRL);		
	IDC_SERVERMANAGEMENT_PORT_CTRL = new wxTextCtrl(parent, -1,
		wxT(""),
		wxDefaultPosition, wxSize((int) 72, -1));
	servernameSizer2->Add(new wxStaticText(parent, -1, wxT("Management Port Number :")),
						  0, wxALIGN_CENTER_VERTICAL);
	servernameSizer2->Add(IDC_SERVERMANAGEMENT_PORT_CTRL);		
	servernameSizer->Add(servernameSizer2);
	IDC_PUBLISHIP_CTRL_TEXT = new wxStaticText(parent, -1, wxT("Published IP :"));
	servernameSizer2->Add(IDC_PUBLISHIP_CTRL_TEXT,
						  0, wxALIGN_CENTER_VERTICAL);

	wxSizer *hori = new wxBoxSizer(wxHORIZONTAL);
	IDC_PUBLISHIP_CTRL = 
		new wxTextCtrl(parent, IDC_PUBLISHIP,
		wxT(""),
		wxDefaultPosition, wxSize((int) 150, -1));
	hori->Add(IDC_PUBLISHIP_CTRL);
	IDOK_PUBLISHAUTO_CTRL =
		new wxButton(parent, IDC_PUBLISHAUTO,
		wxT("AutoDetect"));
	hori->Add(IDOK_PUBLISHAUTO_CTRL);
	servernameSizer2->Add(hori);		

	IDC_PUBLISH_CTRL = 
		new wxCheckBox(parent, IDC_PUBLISH,
		wxT("Allow ALL other internet users to see and use this server"));	
	servernameSizer->Add(IDC_PUBLISH_CTRL, 0, wxTOP, 5);

	IDC_USEUPNP_CTRL = 
		new wxCheckBox(parent, IDC_USEPNP,
		wxT("Try to automaticaly add external port forwarding rules using UPnP"));	
	servernameSizer->Add(IDC_USEUPNP_CTRL, 0, wxTOP, 5);

	IDC_ALLOWSAME_CTRL = 
		new wxCheckBox(parent, -1,
		wxT("Allow multiple clients from same machine to use this server"));	
	servernameSizer->Add(IDC_ALLOWSAME_CTRL, 0, wxTOP, 5);
	IDC_ALLOWSAMEID_CTRL = 
		new wxCheckBox(parent, -1,
		wxT("Allow multiple clients with the same unique id to use this server"));	
	servernameSizer->Add(IDC_ALLOWSAMEID_CTRL, 0, wxTOP, 5);
	IDC_LOGTOFILE_CTRL = 
		new wxCheckBox(parent, -1,
		wxT("Log to file"));	
	servernameSizer->Add(IDC_LOGTOFILE_CTRL, 0, wxTOP, 5);

	sizer->Add(servernameSizer, 0, wxALL, 5);
	
	wxStaticBox *modBox = 
		new wxStaticBox(parent, -1, wxT("Mod Settings"));
	wxStaticBoxSizer *modSizer = 
		new wxStaticBoxSizer(modBox, wxHORIZONTAL);
	modSizer->Add(new wxStaticText(parent, -1,
		wxT("Use Mod :")), 0, wxALL, 5);
	IDC_SERVER_MOD_CTRL = 
		new wxComboBox(parent, -1,
		wxT(""),
		wxDefaultPosition, wxDefaultSize,
		0, 0, wxCB_READONLY);
	modSizer->Add(IDC_SERVER_MOD_CTRL, 0, wxALL, 5);
	sizer->Add(modSizer, 0, wxGROW | wxALIGN_RIGHT | wxALL, 5);

	wxStaticBox *advBox = 
		new wxStaticBox(parent, -1, wxT("Advanced Settings"));
	wxStaticBoxSizer *advSizer = 
		new wxStaticBoxSizer(advBox, wxVERTICAL);
	IDC_BUTTON_SETTINGS_CTRL = 
		new wxButton(parent, IDC_BUTTON_SETTINGS,
		wxT("Edit Advanced Settings"));
	advSizer->Add(IDC_BUTTON_SETTINGS_CTRL, 0, wxALL, 5);
	sizer->Add(advSizer, 0, wxGROW | wxALL, 5);

	// Ok and cancel boxes
	wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	IDCANCEL_CTRL = new wxButton(parent, wxID_CANCEL, wxT("Cancel"));
	IDOK_CTRL = new wxButton(parent, wxID_OK, wxT("Start Server"));
	buttonSizer->Add(IDCANCEL_CTRL, 0, wxALL, 10);
	buttonSizer->Add(IDOK_CTRL, 0, wxALL, 10);
	sizer->Add(buttonSizer, 0, wxALIGN_RIGHT);
}

