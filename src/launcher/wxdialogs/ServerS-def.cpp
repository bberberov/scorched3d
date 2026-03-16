////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2003, 2008, 2010, 2026
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

enum
{
	IDC_SERVER_NAME = 1,
	IDC_PUBLISH,
	IDC_USEPNP,
	IDC_PUBLISHIP,
	IDC_BUTTON_SETTINGS,
	IDC_PUBLISHAUTO
};

static wxComboBox*   IDC_SERVER_MOD_CTRL            = nullptr;
static wxTextCtrl*   IDC_SERVER_NAME_CTRL           = nullptr;
static wxTextCtrl*   IDC_SERVER_PORT_CTRL           = nullptr;
static wxTextCtrl*   IDC_SERVERMANAGEMENT_PORT_CTRL = nullptr;
static wxCheckBox*   IDC_PUBLISH_CTRL               = nullptr;
static wxCheckBox*   IDC_USEUPNP_CTRL               = nullptr;
static wxCheckBox*   IDC_ALLOWSAME_CTRL             = nullptr;
static wxCheckBox*   IDC_ALLOWSAMEID_CTRL           = nullptr;
static wxCheckBox*   IDC_LOGTOFILE_CTRL             = nullptr;
static wxTextCtrl*   IDC_PUBLISHIP_CTRL             = nullptr;
static wxButton*     IDOK_PUBLISHAUTO_CTRL          = nullptr;
static wxButton*     IDOK_CTRL                      = nullptr;
static wxButton*     IDC_BUTTON_SETTINGS_CTRL       = nullptr;
static wxButton*     IDCANCEL_CTRL                  = nullptr;
static wxStaticText* IDC_PUBLISHIP_CTRL_TEXT        = nullptr;
