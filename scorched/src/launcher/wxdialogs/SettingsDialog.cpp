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

#include <wxdialogs/DisplayDialog.h>
#include <wxdialogs/MainDialog.h>
#include <wxdialogs/OptionEntrySetter.h>
#include <landscapedef/LandscapeDefinitionsBase.h>
#include <tankai/TankAINames.h>
#include <common/OptionsGame.h>
#include <common/Defines.h>
#include <wx/wx.h>
#include <wx/image.h>
#include <wx/utils.h>
#include <wx/notebook.h>
#include <wx/textctrl.h>

extern char scorched3dAppName[128];

class SettingsFrame: public wxDialog
{
public:
	SettingsFrame(bool server, OptionsGame &context);

	virtual bool TransferDataToWindow();
	virtual bool TransferDataFromWindow();

protected:
	OptionsGame &context_;
	wxNotebook *book_;
	wxPanel *mainPanel_;
	wxPanel *moneyPanel_;
	wxPanel *scorePanel_;
	wxPanel *envPanel_;
	wxPanel *weaponsPanel_;
	wxPanel *landPanel_;
	wxPanel *playersPanel_;
	wxPanel *motdPanel_;

	std::list<OptionEntrySetter> setters_;

	void setupPlayers();
	void onMaxPlayerChange(wxCommandEvent &event);
	void onSelectAll(wxCommandEvent &event);
	void onDeselectAll(wxCommandEvent &event);

private:
	DECLARE_EVENT_TABLE()

	LandscapeDefinitionsBase landscapeDefinitions;

	wxTextCtrl *IDC_MOTD_CTRL;
	wxButton *IDC_SELECTALL_CTRL;
	wxButton *IDC_DESELECTALL_CTRL;
	wxCheckBox *IDC_CYCLEMAPS_CTRL;
	wxCheckBox **landscapes;
	wxComboBox *IDC_SERVER_MIN_PLAYERS_CTRL;
	wxComboBox *IDC_SERVER_MAX_PLAYERS_CTRL;
	wxComboBox *IDC_SERVER_REMOVEBOT_PLAYERS_CTRL;
	wxComboBox **IDC_COMBO_PTYPE_CTRL;
	wxTextCtrl *IDC_EDIT3_CTRL;
	wxCheckBox *IDC_SERVER_RESIDUAL_CTRL;

	void createMainPanel(bool server);
	void createMoneyPanel();
	void createScorePanel();
	void createEnvPanel();
	void createMotdPanel();
	void createWeaponsPanel();
	void createLandPanel();
	void createPlayersPanel();
};

enum
{
	IDC_SERVER_MAX_PLAYERS = 500,
	IDC_SELECTALL,
	IDC_DESELECTALL
};

BEGIN_EVENT_TABLE(SettingsFrame, wxDialog)
	EVT_TEXT(IDC_SERVER_MAX_PLAYERS,  SettingsFrame::onMaxPlayerChange)
	EVT_BUTTON(IDC_SELECTALL,  SettingsFrame::onSelectAll)
	EVT_BUTTON(IDC_DESELECTALL,  SettingsFrame::onDeselectAll)
END_EVENT_TABLE()

void SettingsFrame::createMainPanel(bool server)
{
	mainPanel_ = new wxPanel(book_, -1);
	book_->AddPage(mainPanel_, wxT("Main"));
	wxSizer *mainPanelSizer = new wxBoxSizer(wxVERTICAL);
	wxSizer *sizer = new wxFlexGridSizer(2, 2);
	mainPanelSizer->Add(sizer, 0, wxALL | wxALIGN_CENTER, 10);

	setters_.push_back(
		OptionEntrySetterUtil::createOtherSetter(
			mainPanel_, sizer, context_.getTeamsEntry()));
	setters_.push_back(
		OptionEntrySetterUtil::createOtherSetter(
			mainPanel_, sizer, context_.getTeamBallanceEntry()));
	setters_.push_back(
		OptionEntrySetterUtil::createOtherSetter(
			mainPanel_, sizer, context_.getNoRoundsEntry()));
	setters_.push_back(
		OptionEntrySetterUtil::createOtherSetter(
			mainPanel_, sizer, context_.getNoTurnsEntry()));
	setters_.push_back(
		OptionEntrySetterUtil::createOtherSetter(
			mainPanel_, sizer, context_.getTurnTypeEntry()));	
	setters_.push_back(
		OptionEntrySetterUtil::createOtherSetter(
			mainPanel_, sizer, context_.getStartTimeEntry()));	
	setters_.push_back(
		OptionEntrySetterUtil::createOtherSetter(
			mainPanel_, sizer, context_.getShotTimeEntry()));	
	setters_.push_back(
		OptionEntrySetterUtil::createOtherSetter(
			mainPanel_, sizer, context_.getRemoveTimeEntry()));	
	setters_.push_back(
		OptionEntrySetterUtil::createOtherSetter(
			mainPanel_, sizer, context_.getAIShotTimeEntry()));	
	setters_.push_back(
		OptionEntrySetterUtil::createOtherSetter(
			mainPanel_, sizer, context_.getBuyingTimeEntry()));	
	setters_.push_back(
		OptionEntrySetterUtil::createOtherSetter(
			mainPanel_, sizer, context_.getRoundTimeEntry()));	
	setters_.push_back(
		OptionEntrySetterUtil::createOtherSetter(
			mainPanel_, sizer, context_.getModDownloadSpeedEntry()));	
	setters_.push_back(
		OptionEntrySetterUtil::createOtherSetter(
			mainPanel_, sizer, context_.getResignModeEntry()));
	setters_.push_back(
		OptionEntrySetterUtil::createOtherSetter(
			mainPanel_, sizer, context_.getPlayerLivesEntry()));

	if (server) // Server only
	{
		setters_.push_back(
			OptionEntrySetterUtil::createOtherSetter(
				mainPanel_, sizer, context_.getServerPasswordEntry()));	
	}

	mainPanel_->SetAutoLayout(TRUE);
	mainPanel_->SetSizer(mainPanelSizer);
}

void SettingsFrame::createMoneyPanel()
{
	moneyPanel_ = new wxPanel(book_, -1);
	wxSizer *ecoPanelSizer = new wxBoxSizer(wxVERTICAL);
	wxSizer *sizer = new wxFlexGridSizer(2, 2);
	ecoPanelSizer->Add(sizer, 0, wxALL | wxALIGN_CENTER, 10);

	setters_.push_back(
		OptionEntrySetterUtil::createOtherSetter(
			moneyPanel_, sizer, context_.getBuyOnRoundEntry()));
	setters_.push_back(
		OptionEntrySetterUtil::createOtherSetter(
			moneyPanel_, sizer, context_.getEconomyEntry()));
	setters_.push_back(
		OptionEntrySetterUtil::createOtherSetter(
			moneyPanel_, sizer, context_.getMoneyWonPerAssistPointEntry()));
	setters_.push_back(
		OptionEntrySetterUtil::createOtherSetter(
			moneyPanel_, sizer, context_.getMoneyWonPerKillPointEntry()));
	setters_.push_back(
		OptionEntrySetterUtil::createOtherSetter(
			moneyPanel_, sizer, context_.getMoneyWonPerMultiKillPointEntry()));
	setters_.push_back(
		OptionEntrySetterUtil::createOtherSetter(
			moneyPanel_, sizer, context_.getMoneyWonPerHitPointEntry()));
	setters_.push_back(
		OptionEntrySetterUtil::createOtherSetter(
			moneyPanel_, sizer, context_.getStartMoneyEntry()));
	setters_.push_back(
		OptionEntrySetterUtil::createOtherSetter(
			moneyPanel_, sizer, context_.getMoneyWonForRoundEntry()));
	setters_.push_back(
		OptionEntrySetterUtil::createOtherSetter(
			moneyPanel_, sizer, context_.getMoneyPerRoundEntry()));
	setters_.push_back(
		OptionEntrySetterUtil::createOtherSetter(
			moneyPanel_, sizer, context_.getInterestEntry()));
	setters_.push_back(
		OptionEntrySetterUtil::createOtherSetter(
			moneyPanel_, sizer, context_.getMoneyWonForLivesEntry()));
	setters_.push_back(
		OptionEntrySetterUtil::createOtherSetter(
			moneyPanel_, sizer, context_.getMoneyPerHealthPointEntry()));

	book_->AddPage(moneyPanel_, wxT("Money"));
	moneyPanel_->SetAutoLayout(TRUE);
	moneyPanel_->SetSizer(ecoPanelSizer);
}

void SettingsFrame::createScorePanel()
{
	scorePanel_ = new wxPanel(book_, -1);
	wxSizer *ecoPanelSizer = new wxBoxSizer(wxVERTICAL);
	wxSizer *sizer = new wxFlexGridSizer(2, 2);
	ecoPanelSizer->Add(sizer, 0, wxALL | wxALIGN_CENTER, 10);

	setters_.push_back(
		OptionEntrySetterUtil::createOtherSetter(
			scorePanel_, sizer, context_.getScoreWonForRoundEntry()));
	setters_.push_back(
		OptionEntrySetterUtil::createOtherSetter(
			scorePanel_, sizer, context_.getScoreWonForLivesEntry()));
	setters_.push_back(
		OptionEntrySetterUtil::createOtherSetter(
			scorePanel_, sizer, context_.getScorePerKillEntry()));
	setters_.push_back(
		OptionEntrySetterUtil::createOtherSetter(
			scorePanel_, sizer, context_.getScorePerAssistEntry()));
	setters_.push_back(
		OptionEntrySetterUtil::createOtherSetter(
			scorePanel_, sizer, context_.getScorePerMoneyEntry()));
	setters_.push_back(
		OptionEntrySetterUtil::createOtherSetter(
			scorePanel_, sizer, context_.getScorePerResignEntry()));
	setters_.push_back(
		OptionEntrySetterUtil::createOtherSetter(
			scorePanel_, sizer, context_.getScorePerSpectateEntry()));

	book_->AddPage(scorePanel_, wxT("Score"));
	scorePanel_->SetAutoLayout(TRUE);
	scorePanel_->SetSizer(ecoPanelSizer);
}

void SettingsFrame::createWeaponsPanel()
{
	weaponsPanel_ = new wxPanel(book_, -1);
	wxSizer *ecoPanelSizer = new wxBoxSizer(wxVERTICAL);
	wxSizer *sizer = new wxFlexGridSizer(2, 2);
	ecoPanelSizer->Add(sizer, 0, wxALL | wxALIGN_CENTER, 10);

	setters_.push_back(
		OptionEntrySetterUtil::createOtherSetter(
			weaponsPanel_, sizer, context_.getWeapScaleEntry()));
	setters_.push_back(
		OptionEntrySetterUtil::createOtherSetter(
			weaponsPanel_, sizer, context_.getWeaponSpeedEntry()));
	setters_.push_back(
		OptionEntrySetterUtil::createOtherSetter(
			weaponsPanel_, sizer, context_.getStartArmsLevelEntry()));
	setters_.push_back(
		OptionEntrySetterUtil::createOtherSetter(
			weaponsPanel_, sizer, context_.getEndArmsLevelEntry()));
	setters_.push_back(
		OptionEntrySetterUtil::createOtherSetter(
			weaponsPanel_, sizer, context_.getMovementRestrictionEntry()));
	setters_.push_back(
		OptionEntrySetterUtil::createOtherSetter(
			weaponsPanel_, sizer, context_.getGiveAllWeaponsEntry()));
	setters_.push_back(
		OptionEntrySetterUtil::createOtherSetter(
			weaponsPanel_, sizer, context_.getResidualPlayersEntry()));
	setters_.push_back(
		OptionEntrySetterUtil::createOtherSetter(
			weaponsPanel_, sizer, context_.getDelayedDefenseActivationEntry()));

	book_->AddPage(weaponsPanel_, wxT("Weapons"));
	weaponsPanel_->SetAutoLayout(TRUE);
	weaponsPanel_->SetSizer(ecoPanelSizer);
}

void SettingsFrame::createEnvPanel()
{
	envPanel_ = new wxPanel(book_, -1);
	wxSizer *envPanelSizer = new wxBoxSizer(wxVERTICAL);
	wxSizer *sizer = new wxFlexGridSizer(2, 2);
	envPanelSizer->Add(sizer, 0, wxALL | wxALIGN_CENTER, 10);

	setters_.push_back(
		OptionEntrySetterUtil::createOtherSetter(
			envPanel_, sizer, context_.getWindForceEntry()));
	setters_.push_back(
		OptionEntrySetterUtil::createOtherSetter(
			envPanel_, sizer, context_.getWindTypeEntry()));
	setters_.push_back(
		OptionEntrySetterUtil::createOtherSetter(
			envPanel_, sizer, context_.getWallTypeEntry()));
	setters_.push_back(
		OptionEntrySetterUtil::createOtherSetter(
			envPanel_, sizer, context_.getMinFallingDistanceEntry()));
	setters_.push_back(
		OptionEntrySetterUtil::createOtherSetter(
			envPanel_, sizer, context_.getMaxClimbingDistanceEntry()));
	setters_.push_back(
		OptionEntrySetterUtil::createOtherSetter(
			envPanel_, sizer, context_.getTankFallingDamageEntry()));

	book_->AddPage(envPanel_, wxT("Env"));
	envPanel_->SetAutoLayout(TRUE);
	envPanel_->SetSizer(envPanelSizer);
}

void SettingsFrame::createMotdPanel()
{
	motdPanel_ = new wxPanel(book_, -1);
	wxSizer *motdPanelSizer = new wxBoxSizer(wxVERTICAL);

	IDC_MOTD_CTRL = new wxTextCtrl(motdPanel_, -1, wxT(""), 
		wxDefaultPosition, wxSize(380, 100), wxTE_MULTILINE);
	motdPanelSizer->Add(IDC_MOTD_CTRL, 1, wxGROW | wxALL, 10);

	book_->AddPage(motdPanel_, wxT("MOTD"));
	motdPanel_->SetAutoLayout(TRUE);
	motdPanel_->SetSizer(motdPanelSizer);
}

void SettingsFrame::createLandPanel()
{
	landPanel_ = new wxPanel(book_, -1);
	wxSizer *landPanelSizer = new wxBoxSizer(wxVERTICAL);

	landscapes = new wxCheckBox*[landscapeDefinitions.getAllLandscapes().size()];

	wxScrolledWindow *scrolledWindow = new wxScrolledWindow(landPanel_, -1, 
		wxDefaultPosition, wxSize(225, 200));

	wxSizer *sizer = new wxFlexGridSizer(3, 3);
	int i = 0;
	std::list<LandscapeDefinitionsEntry> &defns =
		landscapeDefinitions.getAllLandscapes();
	std::list<LandscapeDefinitionsEntry>::iterator itor;
	for (itor = defns.begin();
		 itor != defns.end();
		 ++itor, i++)
	{
		LandscapeDefinitionsEntry &dfn = *itor;
		wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);

		std::string fileName = S3D::getModFile(S3D::formatStringBuffer("data/landscapes/%s", dfn.picture.c_str()));
		if (!::wxFileExists(convertString(fileName)))
		{
			fileName = S3D::getModFile("data/landscapes/picture-none.bmp");
		}

		wxImage image;
		if (image.LoadFile(convertString(fileName), wxBITMAP_TYPE_BMP))
		{
			wxBitmap bitmap(image);
			wxStaticBitmap *staticBmp = new wxStaticBitmap(scrolledWindow, -1, bitmap);
			staticBmp->SetToolTip(wxString(dfn.description.c_str(), wxConvUTF8));
			boxSizer->Add(staticBmp, 0, wxALL, 2);
		}

		landscapes[i] = new wxCheckBox(scrolledWindow, -1, wxString(dfn.name.c_str(), wxConvUTF8));
		boxSizer->Add(landscapes[i]);

		sizer->Add(boxSizer, 0, wxALL, 5);
	}

	scrolledWindow->SetAutoLayout(TRUE);
	scrolledWindow->SetSizer(sizer);
	wxSize minSize = sizer->CalcMin();
	scrolledWindow->SetScrollbars(10, 10, 
		(minSize.GetWidth() + 10) / 10, (minSize.GetHeight() + 10) / 10);
	landPanelSizer->Add(scrolledWindow, 1, wxGROW | wxALL, 10);

	IDC_CYCLEMAPS_CTRL = new wxCheckBox(landPanel_, -1, wxT("Linearly cycle maps"));
	landPanelSizer->Add(IDC_CYCLEMAPS_CTRL, 0, wxALIGN_CENTER);

	wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	IDC_SELECTALL_CTRL = new wxButton(landPanel_, IDC_SELECTALL, wxT("Select All"));
	IDC_DESELECTALL_CTRL = new wxButton(landPanel_, IDC_DESELECTALL, wxT("Deselect All"));
	buttonSizer->Add(IDC_SELECTALL_CTRL, 0, wxALL, 10);
	buttonSizer->Add(IDC_DESELECTALL_CTRL, 0, wxALL, 10);
	landPanelSizer->Add(buttonSizer, 0, wxALIGN_CENTER);

	book_->AddPage(landPanel_, wxT("Land"));
	landPanel_->SetAutoLayout(TRUE);
#if wxCHECK_VERSION(2,6,0)
	landPanel_->SetSizer(landPanelSizer);
#else
#if wxCHECK_VERSION(2,5,0)
#else
	landPanel_->SetSizer(landPanelSizer);
#endif
#endif
}

void SettingsFrame::createPlayersPanel()
{
	IDC_COMBO_PTYPE_CTRL = new wxComboBox*[24];

	playersPanel_ = new wxPanel(book_, -1);
	wxSizer *playersPanelSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	playersPanelSizer->Add(buttonSizer, 0, wxALIGN_CENTER | wxTOP, 10);

	buttonSizer->Add(new wxStaticText(playersPanel_, -1,
		wxT("Min Players :")), 0, wxALIGN_CENTER | wxRIGHT | wxLEFT, 5);
	buttonSizer->Add(IDC_SERVER_MIN_PLAYERS_CTRL = 
		new wxComboBox(playersPanel_, -1,
		wxT(""),
		wxDefaultPosition, wxSize(70, -1),
		0, 0, wxCB_READONLY), 0, wxALIGN_CENTER);
	buttonSizer->Add(new wxStaticText(playersPanel_, -1,
		wxT("Max Players :")), 0, wxALIGN_CENTER | wxRIGHT | wxLEFT, 5);
	buttonSizer->Add(IDC_SERVER_MAX_PLAYERS_CTRL = 
		new wxComboBox(playersPanel_, IDC_SERVER_MAX_PLAYERS,
		wxT(""),
		wxDefaultPosition, wxSize(70, -1),
		0, 0, wxCB_READONLY), 0, wxALIGN_CENTER);	

	wxBoxSizer *buttonSizer2 = new wxBoxSizer(wxHORIZONTAL);
	playersPanelSizer->Add(buttonSizer2, 0, wxALIGN_CENTER | wxTOP, 10);
	buttonSizer2->Add(new wxStaticText(playersPanel_, -1,
		wxT("Remove bots after players :")), 0, wxALIGN_CENTER | wxRIGHT | wxLEFT, 5);
	buttonSizer2->Add(IDC_SERVER_REMOVEBOT_PLAYERS_CTRL = 
		new wxComboBox(playersPanel_, -1,
		wxT(""),
		wxDefaultPosition, wxDefaultSize,
		0, 0, wxCB_READONLY), 0, wxALIGN_CENTER);

	IDC_SERVER_RESIDUAL_CTRL = new wxCheckBox(playersPanel_, -1, 
		wxT("Players are persistent for game"));
	playersPanelSizer->Add(IDC_SERVER_RESIDUAL_CTRL, 0, wxALIGN_CENTER | wxTOP, 10);
		
	wxSizer *sizer = new wxGridSizer(3, 3);
	playersPanelSizer->Add(sizer, 0, wxALL | wxALIGN_CENTER, 10);

	for (int i=0; i<24; i++)
	{
		char buffer[256];
		sprintf(buffer, "%i", (i+1));
	
		wxBoxSizer *playerSizer = new wxBoxSizer(wxHORIZONTAL);
		playerSizer->Add(new wxStaticText(playersPanel_, -1, wxString(buffer, wxConvUTF8)));
		playerSizer->Add(IDC_COMBO_PTYPE_CTRL[i] = 
			new wxComboBox(playersPanel_, -1,
			wxT(""),
			wxDefaultPosition, wxSize(70, -1),
			0, 0, wxCB_READONLY));
		sizer->Add(playerSizer, 0, wxALIGN_RIGHT | wxALL, 2);
	}

	wxBoxSizer *botSizer = new wxBoxSizer(wxHORIZONTAL);
	playersPanelSizer->Add(botSizer, 0, wxALIGN_CENTER | wxALL, 10);
	botSizer->Add(new wxStaticText(playersPanel_, -1,
		wxT("Bot Name Prefix :")), 0, wxALIGN_CENTER | wxRIGHT | wxLEFT, 5);
	botSizer->Add(IDC_EDIT3_CTRL = 
		new wxTextCtrl(playersPanel_, -1,
		wxT(""),
		wxDefaultPosition, wxSize(100, -1)), 
		0, wxALIGN_CENTER);

	book_->AddPage(playersPanel_, wxT("Players"));
	playersPanel_->SetAutoLayout(TRUE);
	playersPanel_->SetSizer(playersPanelSizer);
}

SettingsFrame::SettingsFrame(bool server, OptionsGame &context) :
	wxDialog(getMainDialog(), -1, wxString(scorched3dAppName,wxConvUTF8),
		wxDefaultPosition, wxDefaultSize),
	context_(context), playersPanel_(0)
{
#ifdef _WIN32
	// Set the frame's icon
	wxIcon icon(convertString(S3D::getDataFile("data/images/tank2.ico")), wxBITMAP_TYPE_ICO);
	SetIcon(icon);
#endif

	// Create the positioning sizer
	wxBoxSizer *topsizer = new wxBoxSizer(wxVERTICAL);

	// Create all the display controls
	book_ = new wxNotebook(this, -1);
#if wxCHECK_VERSION(2,6,0)
        wxBoxSizer *nbs = new wxBoxSizer(wxVERTICAL);
	nbs->Add(book_);
#else
        wxNotebookSizer *nbs = new wxNotebookSizer(book_);
#endif

	// Re-read just in case a new mod has been loaded
	DIALOG_ASSERT(landscapeDefinitions.readLandscapeDefinitions());

	createMainPanel(server);
	createMoneyPanel();
	createScorePanel();
	createEnvPanel();
	createWeaponsPanel();
	if (server) createPlayersPanel();
	createMotdPanel();
	createLandPanel();

	topsizer->Add(nbs, 0, wxALL, 10);

	// Ok and cancel boxes
	wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	wxButton *cancelButton = new wxButton(this, wxID_CANCEL, wxT("&Cancel"));
	wxButton *okButton = new wxButton(this, wxID_OK, wxT("&Ok"));
	buttonSizer->Add(cancelButton, 0, wxALL, 10);
	buttonSizer->Add(okButton, 0, wxALL, 10);
	topsizer->Add(buttonSizer, 0, wxALIGN_RIGHT);
	okButton->SetDefault();
		
	// use the sizer for layout
	SetSizer(topsizer); 
	topsizer->SetSizeHints(this); // set size hints to honour minimum size

	CentreOnScreen();
}

void SettingsFrame::onSelectAll(wxCommandEvent &event)
{
	std::list<LandscapeDefinitionsEntry> &defns =
		landscapeDefinitions.getAllLandscapes();
	std::list<LandscapeDefinitionsEntry>::iterator itor = 
		defns.begin();
	for (int i = 0; i<(int) defns.size(); i++, ++itor)
	{
		landscapes[i]->SetValue(true);
	}
}

void SettingsFrame::onDeselectAll(wxCommandEvent &event)
{
	std::list<LandscapeDefinitionsEntry> &defns =
		landscapeDefinitions.getAllLandscapes();
	std::list<LandscapeDefinitionsEntry>::iterator itor =
		defns.begin();  
	for (int i = 0; i<(int) defns.size(); i++, ++itor)
	{           
		landscapes[i]->SetValue(false);
	}
}

void SettingsFrame::onMaxPlayerChange(wxCommandEvent &event)
{
	setupPlayers();
}

void SettingsFrame::setupPlayers()
{
	int maxPlayers = 10;
	sscanf(IDC_SERVER_MAX_PLAYERS_CTRL->GetValue().mb_str(wxConvUTF8), "%i", &maxPlayers);
	context_.getNoMaxPlayersEntry().setValue(maxPlayers);

	for (int i=0; i<24; i++)
	{
		IDC_COMBO_PTYPE_CTRL[i]->Enable((i < context_.getNoMaxPlayers()));
	}
}

bool SettingsFrame::TransferDataToWindow()
{
	// Player tab
	if (playersPanel_)
	{
		IDC_EDIT3_CTRL->
			SetValue(wxString(context_.getBotNamePrefix(), wxConvUTF8));
		IDC_EDIT3_CTRL->SetToolTip(
			wxString("The text prefixed onto any player that is a bot.", wxConvUTF8));

		// Min max players are rounds combos
		char buffer[25];
		int i;
		for (i=24; i>=0; i--)
		{
			char string[20];
			snprintf(string, 20, "%i", i);

			if (i > 1)
			{
				IDC_SERVER_MIN_PLAYERS_CTRL->Append(wxString(string, wxConvUTF8));
				IDC_SERVER_REMOVEBOT_PLAYERS_CTRL->Append(wxString(string, wxConvUTF8));
			}
			IDC_SERVER_MAX_PLAYERS_CTRL->Append(wxString(string, wxConvUTF8));
		}
		IDC_SERVER_REMOVEBOT_PLAYERS_CTRL->Append(wxT("0"));

		snprintf(buffer, 25, "%i", context_.getNoMinPlayers());
		IDC_SERVER_MIN_PLAYERS_CTRL->SetValue(wxString(buffer, wxConvUTF8));
		IDC_SERVER_MIN_PLAYERS_CTRL->SetToolTip(
			wxString("The number of players that must be on the server before a game starts.", wxConvUTF8));

		snprintf(buffer, 25, "%i", context_.getNoMaxPlayers());
		IDC_SERVER_MAX_PLAYERS_CTRL->SetValue(wxString(buffer, wxConvUTF8));
		IDC_SERVER_MAX_PLAYERS_CTRL->SetToolTip(
			wxString("The maximum number of players that can be on the server.", wxConvUTF8));

		snprintf(buffer, 25, "%i", context_.getRemoveBotsAtPlayers());
		IDC_SERVER_REMOVEBOT_PLAYERS_CTRL->SetValue(wxString(buffer, wxConvUTF8));
		IDC_SERVER_REMOVEBOT_PLAYERS_CTRL->SetToolTip(
			wxString("The number of players to allow before remvoing bots.", wxConvUTF8));

		IDC_SERVER_RESIDUAL_CTRL->SetValue(context_.getResidualPlayers());
		IDC_SERVER_RESIDUAL_CTRL->SetToolTip(
			wxString("Players re-connect with the same money and weapons.", wxConvUTF8));

		// Reload the AIs in case a new mod has been loaded

		std::list<std::string> ais;
		TankAINames::loadAIs(ais);
		for (int i=0; i<24; i++)
		{
			std::list<std::string>::iterator itor;
			for (itor = ais.begin();
				itor != ais.end();
				++itor)
			{
				IDC_COMBO_PTYPE_CTRL[i]->Append(
					wxString((*itor).c_str(), wxConvUTF8));
			}
			IDC_COMBO_PTYPE_CTRL[i]->SetValue(
				wxString(context_.getPlayerType(i).getValue(), wxConvUTF8));
		}
		setupPlayers();
	}

	// Land 
	{
		std::list<LandscapeDefinitionsEntry> &defns =
			landscapeDefinitions.getAllLandscapes();
		std::list<LandscapeDefinitionsEntry>::iterator itor = 
			defns.begin();
		for (int i = 0; i<(int) defns.size(); i++, ++itor)
		{
			landscapes[i]->SetValue(
				landscapeDefinitions.landscapeEnabled(
					context_,
					(*itor).name.c_str()));
		}
		IDC_CYCLEMAPS_CTRL->SetValue(context_.getCycleMaps());
		IDC_CYCLEMAPS_CTRL->SetToolTip(
			wxString(context_.getCycleMapsEntry().getDescription(), wxConvUTF8));
	}

	// MOTD
	{
		IDC_MOTD_CTRL->SetValue(
			wxString(context_.getMOTD(), wxConvUTF8));
		IDC_MOTD_CTRL->SetToolTip(
			wxString("The Message Of The Day.", wxConvUTF8));
	}

	OptionEntrySetterUtil::updateControls(setters_);

	return true;
}

bool SettingsFrame::TransferDataFromWindow()
{
	// Players
	if (playersPanel_)
	{
		context_.getBotNamePrefixEntry().setValue(
			std::string(IDC_EDIT3_CTRL->GetValue().mb_str(wxConvUTF8)));

		// Read min + max players
		int minPlayers = 2;
		sscanf(IDC_SERVER_MIN_PLAYERS_CTRL->GetValue().mb_str(wxConvUTF8), 
			"%i", &minPlayers);
		int maxPlayers = 10;
		sscanf(IDC_SERVER_MAX_PLAYERS_CTRL->GetValue().mb_str(wxConvUTF8), 
			"%i", &maxPlayers);
		int maxBotPlayers = 10;
		sscanf(IDC_SERVER_REMOVEBOT_PLAYERS_CTRL->GetValue().mb_str(wxConvUTF8), 
			"%i", &maxBotPlayers);

		context_.getRemoveBotsAtPlayersEntry().setValue(maxBotPlayers);
		context_.getNoMinPlayersEntry().setValue(minPlayers);
		context_.getNoMaxPlayersEntry().setValue(maxPlayers);

		context_.getResidualPlayersEntry().setValue(
				IDC_SERVER_RESIDUAL_CTRL->GetValue());

		for (int i=0; i<24; i++)
		{
			context_.getPlayerType(i).setValue( 
				std::string(IDC_COMBO_PTYPE_CTRL[i]->GetValue().mb_str(wxConvUTF8)));
		}
	}

	// Land
	{
		std::string landscapesString;
		std::list<LandscapeDefinitionsEntry> &defns =
			landscapeDefinitions.getAllLandscapes();
		std::list<LandscapeDefinitionsEntry>::iterator itor = 
			defns.begin();
		for (int i = 0; i<(int) defns.size(); i++, ++itor)
		{
			if (landscapes[i]->GetValue())
			{
				if (!landscapesString.empty()) landscapesString += ":";
				landscapesString += (*itor).name.c_str();
			}
		}
		context_.getLandscapesEntry().setValue(landscapesString.c_str());
		context_.getCycleMapsEntry().setValue(
			IDC_CYCLEMAPS_CTRL->GetValue());
	}

	// MOTD
	{
		context_.getMOTDEntry().setValue(
			std::string(IDC_MOTD_CTRL->GetValue().mb_str(wxConvUTF8)));
	}

	OptionEntrySetterUtil::updateEntries(setters_);

	return true;
}

bool showSettingsDialog(bool server, OptionsGame &context)
{
	// Set the current mod
	std::string modValue = S3D::getDataFileMod();
	S3D::setDataFileMod(context.getMod());

	// Show the settings
	SettingsFrame frame(server, context);
	bool result = (frame.ShowModal() == wxID_OK);

	// Reset the mod
	S3D::setDataFileMod(modValue.c_str());

	return result;
}
