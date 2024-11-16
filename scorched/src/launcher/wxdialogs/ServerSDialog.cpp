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

#include <wxdialogs/MainDialog.h>
#include <wxdialogs/SettingsDialog.h>
#include <scorched/ScorchedParams.h>
#include <common/Defines.h>
#include <common/OptionsGame.h>
#include <engine/ModDirs.h>
#include <wx/wx.h>
#include <wx/utils.h>
#include <wx/dir.h>
#include "ServerS.cpp"

extern char scorched3dAppName[128];

class ServerSFrame: public wxDialog
{
public:
	ServerSFrame(OptionsGame &options);

	virtual bool TransferDataToWindow();
	virtual bool TransferDataFromWindow();

	void onSettingsButton(wxCommandEvent &event);
	void onPublishAutoButton(wxCommandEvent &event);

private:
	DECLARE_EVENT_TABLE()

	OptionsGame &options_;
};

BEGIN_EVENT_TABLE(ServerSFrame, wxDialog)
    EVT_BUTTON(IDC_BUTTON_SETTINGS,  ServerSFrame::onSettingsButton)
	EVT_BUTTON(IDC_PUBLISHAUTO,  ServerSFrame::onPublishAutoButton)
END_EVENT_TABLE()

ServerSFrame::ServerSFrame(OptionsGame &options) :
	wxDialog(getMainDialog(), -1, wxString(scorched3dAppName, wxConvUTF8),
			 wxDefaultPosition, wxDefaultSize),
	 options_(options)
{
#ifdef _WIN32
	// Set the frame's icon
	wxIcon icon(convertString(S3D::getDataFile("data/images/tank2.ico")), wxBITMAP_TYPE_ICO);
	SetIcon(icon);
#endif

	// Create the positioning sizer
	wxBoxSizer *topsizer = new wxBoxSizer(wxVERTICAL);

	// Create all the display controlls
	createControls(this, topsizer);
	IDOK_CTRL->SetDefault();

	// use the sizer for layout
	SetSizer(topsizer); 
	topsizer->SetSizeHints(this); // set size hints to honour minimum size

	CentreOnScreen();
}

void ServerSFrame::onPublishAutoButton(wxCommandEvent &event)
{
	IDC_PUBLISHIP_CTRL->SetValue(wxT("AutoDetect"));
}

void ServerSFrame::onSettingsButton(wxCommandEvent &event)
{
	TransferDataFromWindow();
	// Don't save until the whole options have been choosen
	showSettingsDialog(true, options_);
}

bool ServerSFrame::TransferDataToWindow()
{
	IDC_SERVER_PORT_CTRL->SetValue(
		convertString(S3D::formatStringBuffer("%i", options_.getPortNo())));
	IDC_SERVER_PORT_CTRL->SetToolTip(
		convertString(options_.getPortNoEntry().getDescription()));
	IDC_SERVERMANAGEMENT_PORT_CTRL->SetValue(
		convertString(S3D::formatStringBuffer("%i", options_.getManagementPortNo())));
	IDC_SERVERMANAGEMENT_PORT_CTRL->SetToolTip(
		convertString(options_.getManagementPortNoEntry().getDescription()));
	IDC_SERVER_NAME_CTRL->SetValue(
		convertString(options_.getServerName()));
	IDC_SERVER_NAME_CTRL->SetToolTip(
		convertString(options_.getServerNameEntry().getDescription()));
	IDC_PUBLISH_CTRL->SetValue(options_.getPublishServer());
	IDC_PUBLISH_CTRL->SetToolTip(
		convertString(options_.getPublishServerEntry().getDescription()));
	IDC_USEUPNP_CTRL->SetValue(options_.getUseUPnP());
	IDC_USEUPNP_CTRL->SetToolTip(
		convertString(options_.getUseUPnPEntry().getDescription()));
	IDC_PUBLISHIP_CTRL->SetValue(
		convertString(options_.getPublishAddress()));
	IDC_PUBLISHIP_CTRL->SetToolTip(
		convertString(options_.getPublishAddressEntry().getDescription()));
	IDC_ALLOWSAME_CTRL->SetValue(options_.getAllowSameIP());
	IDC_ALLOWSAME_CTRL->SetToolTip(
		convertString(options_.getAllowSameIPEntry().getDescription()));
	IDC_ALLOWSAMEID_CTRL->SetValue(options_.getAllowSameUniqueId());
	IDC_ALLOWSAMEID_CTRL->SetToolTip(
		convertString(options_.getAllowSameUniqueIdEntry().getDescription()));
	IDC_LOGTOFILE_CTRL->SetValue(0 == strcmp("file", options_.getServerFileLogger()));
	IDC_LOGTOFILE_CTRL->SetToolTip(
		convertString(options_.getServerFileLoggerEntry().getDescription()));

	ModDirs modDirs;
	if (!modDirs.loadModDirs()) S3D::dialogExit("ModFiles", "Failed to load mod files");
	std::list<ModInfo>::iterator itor;
	for (itor = modDirs.getDirs().begin();
		itor != modDirs.getDirs().end();
		++itor)
	{
		ModInfo &info = *itor;
		IDC_SERVER_MOD_CTRL->Append(convertString(info.getName()));
	}
	if (IDC_SERVER_MOD_CTRL->FindString(convertString(options_.getMod())) != -1)
		IDC_SERVER_MOD_CTRL->SetValue(convertString(options_.getMod()));
	else 
		IDC_SERVER_MOD_CTRL->SetValue(wxT("none"));
	IDC_SERVER_MOD_CTRL->SetToolTip(
		convertString("The Scorched3D mod to use for this game."));

	return true;
}

bool ServerSFrame::TransferDataFromWindow()
{
	options_.getPortNoEntry().setValue(atoi(IDC_SERVER_PORT_CTRL->GetValue().mb_str(wxConvUTF8)));
	options_.getManagementPortNoEntry().setValue(atoi(IDC_SERVERMANAGEMENT_PORT_CTRL->GetValue().mb_str(wxConvUTF8)));
	options_.getServerNameEntry().setValue(std::string(IDC_SERVER_NAME_CTRL->GetValue().mb_str(wxConvUTF8)));
	options_.getPublishServerEntry().setValue(IDC_PUBLISH_CTRL->GetValue());
	options_.getPublishAddressEntry().setValue(std::string(IDC_PUBLISHIP_CTRL->GetValue().mb_str(wxConvUTF8)));
	options_.getUseUPnPEntry().setValue(IDC_USEUPNP_CTRL->GetValue());
	options_.getAllowSameIPEntry().setValue(IDC_ALLOWSAME_CTRL->GetValue());
	options_.getAllowSameUniqueIdEntry().setValue(IDC_ALLOWSAMEID_CTRL->GetValue());
	options_.getModEntry().setValue(std::string(IDC_SERVER_MOD_CTRL->GetValue().mb_str(wxConvUTF8)));
	options_.getServerFileLoggerEntry().setValue((IDC_LOGTOFILE_CTRL->GetValue()?"file":"none"));
	return true;
}

bool showServerSDialog()
{
	OptionsGame tmpOptions;
	std::string serverFileSrc = S3D::getDataFile("data/server.xml");
	std::string serverFileDest = S3D::getSettingsFile("server.xml");
	if (S3D::fileExists(serverFileDest.c_str()))
	{
		tmpOptions.readOptionsFromFile((char *) serverFileDest.c_str());
	}
	else
	{
		tmpOptions.readOptionsFromFile((char *) serverFileSrc.c_str());
	}

	ServerSFrame frame(tmpOptions);
	if (frame.ShowModal() == wxID_OK)
	{
		tmpOptions.writeOptionsToFile((char *) serverFileDest.c_str(), 
			ScorchedParams::instance()->getWriteFullOptions());
		std::string fileName = S3D::formatStringBuffer("-startserver \"%s\"", serverFileDest.c_str());
		runScorched3D(fileName.c_str(), true);
		return true;
	}
	return false;
}

