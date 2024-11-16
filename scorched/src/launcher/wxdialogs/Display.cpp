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

#include "Display-def.cpp"

static void createMainControls(wxWindow *parent, wxSizer *sizer)
{
	{
	// Display settings
	wxStaticBox *displayBox = new wxStaticBox(parent, -1, wxT("&Display"));
	wxStaticBoxSizer *displaySizer = new wxStaticBoxSizer(displayBox, wxVERTICAL);
	wxFlexGridSizer *displaySizer2 = new wxFlexGridSizer(2, 3, 5, 5);
	wxStaticText *resText = new wxStaticText(parent, -1, wxT("&Resolution :"));
	IDC_DISPLAY_CTRL = 
		new wxComboBox(parent, -1,
		wxT(""),
		wxDefaultPosition, wxSize(315, -1),
		0, 0, wxCB_READONLY);
	IDC_FULLSCREEN_CTRL = 
		new wxCheckBox(parent, -1, wxT("Full Screen"));
	IDC_MORERES_CTRL =
		new wxCheckBox(parent, ID_MORERES, wxT("More Resolutions"));
	displaySizer2->Add(resText, 0, wxALIGN_CENTER_VERTICAL);
	displaySizer2->Add(IDC_DISPLAY_CTRL, 0);
	displaySizer2->Add(IDC_FULLSCREEN_CTRL, 0, wxALIGN_CENTRE_VERTICAL);
	wxStaticText *brightText = new wxStaticText(parent, -1, wxT("&Brightness :"));
	IDC_SLIDER1_CTRL = 
		new wxSlider(parent, -1,
		0,0,0,
		wxDefaultPosition, wxSize(315, -1),
		wxSL_HORIZONTAL | wxSL_AUTOTICKS);
	displaySizer2->Add(brightText, 0, wxALIGN_CENTER_VERTICAL);
	displaySizer2->Add(IDC_SLIDER1_CTRL, 0);
	displaySizer2->Add(IDC_MORERES_CTRL, 0, wxALIGN_CENTRE_VERTICAL);
	displaySizer->Add(displaySizer2, 0);

	wxFlexGridSizer *displaySizer3 = new wxFlexGridSizer(1, 7, 5, 5);

	displaySizer3->Add(new wxStaticText(parent, -1, wxT("&UI/HUD Scale :")));
	IDC_TINYDIALOGS_CTRL = new wxRadioButton(parent, -1, wxT("Tiny"), 
		wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	IDC_SMALLDIALOGS_CTRL = new wxRadioButton(parent, -1, wxT("Small"));
	IDC_MEDIUMDIALOGS_CTRL = new wxRadioButton(parent, -1, wxT("Medium"));
	IDC_LARGEDIALOGS_CTRL = new wxRadioButton(parent, -1, wxT("Large"));
        wxStaticText *frameLimitText = new wxStaticText(parent, -1, wxT("     Limit Framerate :"));
	IDC_FRAMELIMIT_CTRL = 
		new wxTextCtrl(parent, -1, wxString(), wxDefaultPosition, wxSize(60, -1));
	displaySizer3->Add(IDC_TINYDIALOGS_CTRL);
	displaySizer3->Add(IDC_SMALLDIALOGS_CTRL);
	displaySizer3->Add(IDC_MEDIUMDIALOGS_CTRL);
	displaySizer3->Add(IDC_LARGEDIALOGS_CTRL);
	displaySizer3->Add(frameLimitText, -1, wxALIGN_CENTRE_VERTICAL);
	displaySizer3->Add(IDC_FRAMELIMIT_CTRL);
	displaySizer->Add(displaySizer3, 0, wxTOP | wxBOTTOM, 10);
	sizer->Add(displaySizer, 0, wxGROW | wxLEFT | wxRIGHT | wxTOP, 5);

	wxStaticText *aliasText = new wxStaticText(parent, -1, wxT("Anti Aliasing :"));
	displaySizer2->Add(aliasText, 0);
	IDC_ANTIALIAS_CTRL = new wxComboBox(parent, -1,
		wxT(""),
		wxDefaultPosition, wxSize(60, -1),
		0, 0, wxCB_READONLY);
	displaySizer2->Add(IDC_ANTIALIAS_CTRL, 0);
	IDC_FOCUSPAUSE_CTRL =
		new wxCheckBox(parent, -1, wxT("Pause graphics when not focused"));
	displaySizer2->Add(IDC_FOCUSPAUSE_CTRL, 0);

	}

	{
	// Sound settings
	wxStaticBox *soundBox = new wxStaticBox(parent, -1, wxT("&Sound"));
	wxStaticBoxSizer *soundSizerMain = new wxStaticBoxSizer(soundBox, wxVERTICAL);
	wxFlexGridSizer *soundSizer1 = new wxFlexGridSizer(4, 2);
	wxStaticText *volumeText = new wxStaticText(parent, -1, wxT("Sound Volume :"));
	wxStaticText *musicVolumeText = new wxStaticText(parent, -1, wxT("Music Volume :"));
	wxStaticText *ambientVolumeText = new wxStaticText(parent, -1, wxT("Ambient Volume :"));
	IDC_VOLUME_CTRL = 
		new wxSlider(parent, -1,
		0,0,0,
		wxDefaultPosition, wxSize(315, -1),
		wxSL_HORIZONTAL | wxSL_AUTOTICKS);
	IDC_SOUNDCHANNELS_CTRL = 
		new wxComboBox(parent, -1,
			wxT(""),
			wxDefaultPosition, wxSize(55, -1),
			0, 0, wxCB_READONLY);
	IDC_MUSICVOLUME_CTRL = 
		new wxSlider(parent, -1,
		0,0,0,
		wxDefaultPosition, wxSize(315, -1),
		wxSL_HORIZONTAL | wxSL_AUTOTICKS);
	IDC_AMBIENTVOLUME_CTRL = 
		new wxSlider(parent, -1,
		0,0,0,
		wxDefaultPosition, wxSize(315, -1),
		wxSL_HORIZONTAL | wxSL_AUTOTICKS);
	IDC_NOSOUND_CTRL = 
		new wxCheckBox(parent, -1, wxT("No Sound"));
	IDC_NOMUSIC_CTRL = 
		new wxCheckBox(parent, -1, wxT("No Music"));
	soundSizer1->Add(volumeText, 0, wxRIGHT, 10);
	soundSizer1->Add(IDC_VOLUME_CTRL, 0, wxRIGHT, 10);
	soundSizer1->Add(IDC_SOUNDCHANNELS_CTRL, 0, wxRIGHT, 5);
	soundSizer1->Add(new wxStaticText(parent, -1, wxT("Channels")), 0, wxALIGN_CENTRE_VERTICAL);
	soundSizer1->Add(ambientVolumeText, 0, wxRIGHT, 10);
	soundSizer1->Add(IDC_AMBIENTVOLUME_CTRL, 0, wxRIGHT, 10);
	soundSizer1->Add(IDC_NOSOUND_CTRL, 0, wxRIGHT, 0);
	soundSizer1->Add(new wxStaticText(parent, -1, wxT("")), 0, wxRIGHT, 0);
	soundSizer1->Add(musicVolumeText, 0, wxRIGHT, 10);
	soundSizer1->Add(IDC_MUSICVOLUME_CTRL, 0, wxRIGHT, 10);
	soundSizer1->Add(IDC_NOMUSIC_CTRL, 0, wxRIGHT, 0);
	soundSizerMain->Add(soundSizer1, 0, wxGROW | wxTOP, 5);
	sizer->Add(soundSizerMain, 0, wxGROW | wxLEFT | wxRIGHT | wxTOP, 5);
	}

	{
	// Load settings
	wxStaticBox *speedBox = new wxStaticBox(parent, -1, 
		wxT("Game Detail Options"));
	wxStaticBoxSizer *speedSizer = new wxStaticBoxSizer(speedBox, wxHORIZONTAL);

	wxGridSizer *loadSizer = new wxFlexGridSizer(2, 2);
	IDC_LOADULTRA_CTRL = new wxButton(parent,  ID_LOADULTRA, wxT("&Ultra Detail"), wxDefaultPosition, wxSize(120,-1));
	IDC_LOADDEFAULTS_CTRL = new wxButton(parent, ID_LOADDEFAULTS, wxT("&Normal Detail"), wxDefaultPosition, wxSize(120,-1));
	IDC_LOADMEDIUM_CTRL = new wxButton(parent, ID_LOADMEDIUM, wxT("Faster Detail"), wxDefaultPosition, wxSize(120,-1));
	IDC_LOADFASTEST_CTRL = new wxButton(parent, ID_LOADFASTEST, wxT("Fastest Detail"), wxDefaultPosition, wxSize(120,-1));
	IDC_LOADSAFE_CTRL = new wxButton(parent, ID_LOADSAFE, wxT("Safe Mode"), wxDefaultPosition, wxSize(120,-1));
	loadSizer->Add(IDC_LOADULTRA_CTRL, 0, wxALL, 2);
	loadSizer->Add(new wxStaticText(parent, -1, wxT("Use ultra detail settings for Scorched3D. (For V FAST machines)")),
		0, wxALL, 2);
	loadSizer->Add(IDC_LOADDEFAULTS_CTRL, 0, wxALL, 2);
	loadSizer->Add(new wxStaticText(parent, -1, wxT("Use normal detail settings for Scorched3D. (For FAST machines)")),
		0, wxALL, 2);
	loadSizer->Add(IDC_LOADMEDIUM_CTRL, 0, wxALL, 2);
	loadSizer->Add(new wxStaticText(parent, -1, wxT("Use faster detail settings for Scorched3D. (For MEDIUM machines)\n"
		"Note: This will result in a slightly reduced graphical experience.")),
		0, wxALL, 2);
	loadSizer->Add(IDC_LOADFASTEST_CTRL, 0, wxALL, 2);
	loadSizer->Add(new wxStaticText(parent, -1, wxT("Use fastest detail settings for Scorched3D. (For SLOW machines)\n"
		"Note: This will result in a greatly reduced graphical and audio experience.")),
		0, wxALL, 2);
	loadSizer->Add(IDC_LOADSAFE_CTRL, 0, wxALL, 2);
	loadSizer->Add(new wxStaticText(parent, -1, wxT("Use safest settings for Scorched3D.\n"
		"Note: This will result in a greatly reduced overall experience but may prevent crashes.")),
		0, wxALL, 2);

	speedSizer->Add(loadSizer, 0, wxGROW);
	sizer->Add(speedSizer, 0, wxGROW | wxTOP | wxALIGN_CENTER, 5);
	}
}

static void createOtherControls(wxWindow *parent, wxSizer *sizer)
{
	{
	// Misc switches 
	wxStaticBox *miscBox = new wxStaticBox(parent, -1, 
		wxT("Misc. Options"));
	wxStaticBoxSizer *miscSizer = new wxStaticBoxSizer(miscBox, wxHORIZONTAL);
	wxGridSizer *miscSizer2 = new wxGridSizer(3, 3, 10, 10);
	IDC_INVERT_CTRL = 
		new wxCheckBox(parent, -1, wxT("Invert key elevation"));
	miscSizer2->Add(IDC_INVERT_CTRL, 0);
	IDC_INVERTMOUSE_CTRL = 
		new wxCheckBox(parent, -1, wxT("Invert mouse elevation"));
	miscSizer2->Add(IDC_INVERTMOUSE_CTRL, 0);
	//IDC_SWAPYAXIS_CTRL = 
	//	new wxCheckBox(parent, -1, wxT("Invert mouse y axis (OS X)"));
	//miscSizer2->Add(IDC_SWAPYAXIS_CTRL, 0);
	IDC_TIMER_CTRL = 
		new wxCheckBox(parent, -1, wxT("Show frames per second"));
	miscSizer2->Add(IDC_TIMER_CTRL, 0);
	IDC_SMOUSE_CTRL = 
		new wxCheckBox(parent, -1, wxT("Software Mouse"));
	miscSizer2->Add(IDC_SMOUSE_CTRL, 0);
	IDC_VALIDATESERVER_CTRL = 
		new wxCheckBox(parent, -1, wxT("Validate Server Ip"));
	miscSizer2->Add(IDC_VALIDATESERVER_CTRL, 0);
	IDC_SIDESCROLL_CTRL = 
		new wxCheckBox(parent, -1, wxT("Side Scrolling"));
	miscSizer2->Add(IDC_SIDESCROLL_CTRL, 0);
	IDC_PLAYERCAMERA_CTRL = 
		new wxCheckBox(parent, -1, wxT("Remember camera pos"));
	miscSizer2->Add(IDC_PLAYERCAMERA_CTRL, 0);
	IDC_LOGGING_CTRL = 
		new wxCheckBox(parent, -1, wxT("Client Logging"));
	miscSizer2->Add(IDC_LOGGING_CTRL, 0);
	miscSizer->Add(miscSizer2, 0, wxGROW);
	sizer->Add(miscSizer, 0, wxGROW | wxLEFT | wxRIGHT | wxTOP, 5);
	}
}

static void createSpeedControls(wxWindow *parent, wxSizer *sizer)
{
	// Texture sizes (small med large)
	wxStaticBox *textureBox = new wxStaticBox(parent, -1, wxT("Level of Detail settings"));
	wxStaticBoxSizer *textureSizer = new wxStaticBoxSizer(textureBox, wxVERTICAL);
	wxGridSizer *textureSizer2 = new wxGridSizer(3, 4, 10, 10);
	wxStaticText *texSizeText = new wxStaticText(parent, -1, wxT("Texture Sizes :"));
	textureSizer2->Add(texSizeText, 0, wxALIGN_CENTER_VERTICAL);
	IDC_SMALLTEX_CTRL = new wxRadioButton(parent, -1, wxT("Small (3dfx/faster)"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	textureSizer2->Add(IDC_SMALLTEX_CTRL, 0);
	IDC_MEDIUMTEX_CTRL = new wxRadioButton(parent, -1, wxT("Medium"));
	textureSizer2->Add(IDC_MEDIUMTEX_CTRL, 0);
	IDC_LARGETEX_CTRL = new wxRadioButton(parent, -1, wxT("Large (slower)"));
	textureSizer2->Add(IDC_LARGETEX_CTRL, 0);
	wxStaticText *tankSizeText = new wxStaticText(parent, -1, wxT("Tank Detail :"));
	textureSizer2->Add(tankSizeText, 0, wxALIGN_CENTER_VERTICAL);
	IDC_LOWTANK_CTRL = new wxRadioButton(parent, -1, wxT("Low (faster)"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	textureSizer2->Add(IDC_LOWTANK_CTRL, 0);
	IDC_MEDIUMTANK_CTRL = new wxRadioButton(parent, -1, wxT("Medium"));
	textureSizer2->Add(IDC_MEDIUMTANK_CTRL, 0);
	IDC_HIGHTANK_CTRL = new wxRadioButton(parent, -1, wxT("Max (slower)"));
	textureSizer2->Add(IDC_HIGHTANK_CTRL, 0);
	wxStaticText *effectSizeText = new wxStaticText(parent, -1, wxT("Effects Detail :"));
	textureSizer2->Add(effectSizeText, 0, wxALIGN_CENTER_VERTICAL);
	IDC_LOWEFFECTS_CTRL = new wxRadioButton(parent, -1, wxT("Low (faster)"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	textureSizer2->Add(IDC_LOWEFFECTS_CTRL, 0);
	IDC_MEDIUMEFFECTS_CTRL = new wxRadioButton(parent, -1, wxT("Medium"));
	textureSizer2->Add(IDC_MEDIUMEFFECTS_CTRL, 0);
	IDC_HIGHEFFECTS_CTRL = new wxRadioButton(parent, -1, wxT("High (slower)"));
	textureSizer2->Add(IDC_HIGHEFFECTS_CTRL, 0);
	textureSizer->Add(textureSizer2, 0, wxGROW);
	
	wxFlexGridSizer *textureSizer3 = new wxFlexGridSizer(1, 2, 10, 10);
	textureSizer3->Add(new wxStaticText(parent, -1, wxT("Land Error :")));
	IDC_LANDLOD_CTRL = 
	new wxSlider(parent, -1,
				 0,0,0,
				 wxDefaultPosition, wxSize(315, -1),
				 wxSL_HORIZONTAL | wxSL_AUTOTICKS);
	textureSizer3->Add(IDC_LANDLOD_CTRL, 0, wxGROW);
	textureSizer->Add(textureSizer3, 0, wxGROW | wxTOP, 10);
	
	sizer->Add(textureSizer, 0, wxGROW | wxLEFT | wxRIGHT | wxTOP, 5);
	
	// Detail switches 
	{
		wxStaticBox *waterDetailBox = new wxStaticBox(parent, -1, 
													  wxT("Graphics Detail"));
		wxStaticBoxSizer *waterDetailSizer = new wxStaticBoxSizer(waterDetailBox, wxHORIZONTAL);
		wxGridSizer *waterDetailSizer2 = new wxGridSizer(3, 1, 10, 10);
		
		IDC_NOWATERREF_CTRL = 
		new wxCheckBox(parent, -1, wxT("Don't draw water reflections - Large Framerate Improvement"));
		waterDetailSizer2->Add(IDC_NOWATERREF_CTRL, 0);
		IDC_NOWATERMOVEMENT_CTRL = 
		new wxCheckBox(parent, -1, wxT("Don't draw water movement - Large Framerate Improvement"));
		waterDetailSizer2->Add(IDC_NOWATERMOVEMENT_CTRL, 0);
		IDC_NOWATER_CTRL = 
		new wxCheckBox(parent, -1, wxT("Don't draw water - Large Framerate Improvement"));
		waterDetailSizer2->Add(IDC_NOWATER_CTRL, 0);
		IDC_NOWATERWAVES_CTRL = 
		new wxCheckBox(parent, -1, wxT("Don't draw water breakers - Small Loading Speed Improvement"));
		waterDetailSizer2->Add(IDC_NOWATERWAVES_CTRL, 0);
		IDC_NOWATERLOD_CTRL = 
		new wxCheckBox(parent, -1, wxT("Draw simplistic water - Medium Framerate Improvement"));
		waterDetailSizer2->Add(IDC_NOWATERLOD_CTRL, 0);
		IDC_NOSHADERS_CTRL = 
		new wxCheckBox(parent, -1, wxT("No GL shaders - Large Framerate Improvement"));
		waterDetailSizer2->Add(IDC_NOSHADERS_CTRL, 0);
		IDC_SIMPLEWATERSHADERS_CTRL = 
		new wxCheckBox(parent, -1, wxT("Simple water shaders - Medium Framerate Improvement"));
		waterDetailSizer2->Add(IDC_SIMPLEWATERSHADERS_CTRL, 0);
		IDC_NOOBJECTSHADOWS_CTRL = 
		new wxCheckBox(parent, -1, wxT("No GL shadows for objects - Medium Framerate Improvement"));
		waterDetailSizer2->Add(IDC_NOOBJECTSHADOWS_CTRL, 0);
		IDC_NOOBJECTREFLECTIONS_CTRL = 
		new wxCheckBox(parent, -1, wxT("No water reflections for objects - Medium Framerate Improvement"));
		waterDetailSizer2->Add(IDC_NOOBJECTREFLECTIONS_CTRL, 0);
		IDC_NOPARTICLEREFLECTIONS_CTRL = 
		new wxCheckBox(parent, -1, wxT("No water reflections for particles - Small Framerate Improvement"));
		waterDetailSizer2->Add(IDC_NOPARTICLEREFLECTIONS_CTRL, 0);
		IDC_NOSHADOWS_CTRL = 
		new wxCheckBox(parent, -1, wxT("No GL shadows - Large Framerate Improvement"));
		waterDetailSizer2->Add(IDC_NOSHADOWS_CTRL, 0);
		
		waterDetailSizer->Add(waterDetailSizer2, 0, wxGROW);
		sizer->Add(waterDetailSizer, 0, wxGROW | wxLEFT | wxRIGHT | wxTOP, 5);
	}
}

static void createTroubleControls(wxWindow *parent, wxSizer *sizer)
{
	// Graphics Settings
	{
		wxStaticBox *glDetailBox = new wxStaticBox(parent, -1, 
			wxT("Compatibility Settings"));
		wxStaticBoxSizer *glDetailSizer = new wxStaticBoxSizer(glDetailBox, wxHORIZONTAL);
		wxGridSizer *glDetailSizer2 = new wxGridSizer(3, 3, 10, 10);

		IDC_NOEXT_CTRL = 
			new wxCheckBox(parent, -1, wxT("No GL Extensions"));
		glDetailSizer2->Add(IDC_NOEXT_CTRL, 0);
		IDC_NOMULTITEX_CTRL = 
			new wxCheckBox(parent, -1, wxT("No multi texture"));
		glDetailSizer2->Add(IDC_NOMULTITEX_CTRL, 0);
		IDC_NOLANDSCAPESCORCH_CTRL = 
			new wxCheckBox(parent, -1, wxT("No TexSubImaging"));
		glDetailSizer2->Add(IDC_NOLANDSCAPESCORCH_CTRL, 0);
		IDC_NOENVCOMBINE_CTRL = 
			new wxCheckBox(parent, -1, wxT("No combined textures"));
		glDetailSizer2->Add(IDC_NOENVCOMBINE_CTRL, 0);
		IDC_NOCUBEMAP_CTRL = 
			new wxCheckBox(parent, -1, wxT("No cube map"));
		glDetailSizer2->Add(IDC_NOCUBEMAP_CTRL, 0);
		IDC_NOSPHEREMAP_CTRL = 
			new wxCheckBox(parent, -1, wxT("No sphere map"));
		glDetailSizer2->Add(IDC_NOSPHEREMAP_CTRL, 0);
		IDC_NOMIPMAPS_CTRL = 
			new wxCheckBox(parent, -1, wxT("No HW mip maps"));
		glDetailSizer2->Add(IDC_NOMIPMAPS_CTRL, 0);
		IDC_NOVBO_CTRL = 
			new wxCheckBox(parent, -1, wxT("No VBO"));
		glDetailSizer2->Add(IDC_NOVBO_CTRL, 0);

		glDetailSizer->Add(glDetailSizer2, 0, wxGROW);
		sizer->Add(glDetailSizer, 0, wxGROW | wxLEFT | wxRIGHT | wxTOP, 5);
	}

	{
		wxStaticBox *miscDetailBox = new wxStaticBox(parent, -1, 
			wxT("Misc Detail"));
		wxStaticBoxSizer *miscDetailSizer = new wxStaticBoxSizer(miscDetailBox, wxHORIZONTAL);
		wxGridSizer *miscDetailSizer2 = new wxGridSizer(3, 3, 10, 10);

		IDC_NODETAILTEX_CTRL = 
			new wxCheckBox(parent, -1, wxT("Don't use detail textures"));
		miscDetailSizer2->Add(IDC_NODETAILTEX_CTRL, 0);
		IDC_NOSURROUND_CTRL = 
			new wxCheckBox(parent, -1, wxT("Don't draw surround"));
		miscDetailSizer2->Add(IDC_NOSURROUND_CTRL, 0);
		IDC_SINGLESKYLAYER_CTRL = 
			new wxCheckBox(parent, -1, wxT("Single sky layer"));
		miscDetailSizer2->Add(IDC_SINGLESKYLAYER_CTRL, 0);
		IDC_NOSKYANI_CTRL = 
			new wxCheckBox(parent, -1, wxT("Don't animate sky"));
		miscDetailSizer2->Add(IDC_NOSKYANI_CTRL, 0);
		IDC_NOSKINS_CTRL = 
			new wxCheckBox(parent, -1, wxT("No model skins"));
		miscDetailSizer2->Add(IDC_NOSKINS_CTRL, 0);
		IDC_NODYNAMICLIGHT_CTRL = 
			new wxCheckBox(parent, -1, wxT("No model dynamic lighting"));
		miscDetailSizer2->Add(IDC_NODYNAMICLIGHT_CTRL, 0);
		IDC_NOPRECIPITATION_CTRL = 
			new wxCheckBox(parent, -1, wxT("No precipitation"));
		miscDetailSizer2->Add(IDC_NOPRECIPITATION_CTRL, 0);
		IDC_NODEPTHSORT_CTRL = 
			new wxCheckBox(parent, -1, wxT("No sprite depth sorting"));
		miscDetailSizer2->Add(IDC_NODEPTHSORT_CTRL, 0);
		IDC_NOBACKDROP_CTRL = 
			new wxCheckBox(parent, -1, wxT("No progress backdrop"));
		miscDetailSizer2->Add(IDC_NOBACKDROP_CTRL, 0);

		miscDetailSizer->Add(miscDetailSizer2, 0, wxGROW);
		sizer->Add(miscDetailSizer, 0, wxGROW | wxLEFT | wxRIGHT | wxTOP, 5);
	}
}

static void createIdentControls(wxWindow *parent, wxSizer *sizer)
{
	{
	// User name edit box
	wxStaticBox *userNameBox = new wxStaticBox(parent, -1, 
		wxT("Online User Name"));
	wxStaticBoxSizer *userNameSizer = new wxStaticBoxSizer(userNameBox, wxVERTICAL);
	IDC_USERNAME_CTRL = new wxTextCtrl(parent, -1, wxString(), wxDefaultPosition, wxSize(300, -1));
	userNameSizer->Add(IDC_USERNAME_CTRL, 0, wxALIGN_CENTER);
	sizer->Add(userNameSizer, 0, wxGROW | wxLEFT | wxRIGHT | wxTOP, 5);
	}

	{
	// TankModel edit box
	wxStaticBox *tankModelBox = new wxStaticBox(parent, -1, 
		wxT("Online Tank Model"));
	wxStaticBoxSizer *tankModelSizer = new wxStaticBoxSizer(tankModelBox, wxVERTICAL);
	IDC_TANKMODEL_CTRL = new wxTextCtrl(parent, -1, wxString(), wxDefaultPosition, wxSize(300, -1));
	tankModelSizer->Add(IDC_TANKMODEL_CTRL, 0, wxALIGN_CENTER);
	sizer->Add(tankModelSizer, 0, wxGROW | wxLEFT | wxRIGHT | wxTOP, 5);
	}

	// User id edit box
	wxStaticBox *userBox = new wxStaticBox(parent, -1, 
		wxT("User ID (Uniquely identifies this player for stats, not generated from any user information.)"));
	wxStaticBoxSizer *userSizer = new wxStaticBoxSizer(userBox, wxVERTICAL);
	IDC_USERID_CTRL = new wxGrid(parent, -1, wxDefaultPosition, wxDefaultSize);
	IDC_USERID_CTRL->CreateGrid(0, 3);
	IDC_USERID_CTRL->SetColLabelValue(0, wxT("Published Ip"));
	IDC_USERID_CTRL->SetColLabelValue(1, wxT("Current Ip"));
	IDC_USERID_CTRL->SetColLabelValue(2, wxT("Unique Id"));
	IDC_USERID_CTRL->SetColLabelSize(20);
	IDC_USERID_CTRL->SetRowLabelSize(0);

	userSizer->Add(IDC_USERID_CTRL, 1, wxALIGN_CENTER | wxGROW);
	IDC_HOSTDESC_CTRL = new wxTextCtrl(parent, -1, wxString(), wxDefaultPosition, wxSize(300, -1), wxTE_READONLY);
	userSizer->Add(IDC_HOSTDESC_CTRL, 0, wxALIGN_CENTER);
	sizer->Add(userSizer, 1, wxGROW | wxLEFT | wxRIGHT | wxTOP, 5);
}

static void refreshIdentControls()
{
	UniqueIdStore idStore;
	idStore.loadStore();

	if (int(idStore.getIds().size()) > IDC_USERID_CTRL->GetNumberRows())
	{
		IDC_USERID_CTRL->AppendRows(
			int(idStore.getIds().size()) - IDC_USERID_CTRL->GetNumberRows());
	}

	// User id edit box
	int pos = 0;
	IDC_USERID_CTRL->ClearGrid();
	std::list<UniqueIdStore::Entry>::iterator itor;
	for (itor = idStore.getIds().begin();
		itor != idStore.getIds().end();
		++itor, pos++)
	{
		UniqueIdStore::Entry &entry = *itor;
		IDC_USERID_CTRL->SetCellValue(pos, 0, wxString(entry.published.c_str(), wxConvUTF8));
		IDC_USERID_CTRL->SetCellValue(pos, 1, wxString(NetInterface::getIpName(entry.ip), wxConvUTF8));
		IDC_USERID_CTRL->SetCellValue(pos, 2, wxString(entry.id.c_str(), wxConvUTF8));
		IDC_USERID_CTRL->SetReadOnly(pos, 0);
		IDC_USERID_CTRL->SetReadOnly(pos, 1);
	}
	IDC_USERID_CTRL->EnableEditing(true);
	IDC_USERID_CTRL->AutoSizeColumns(true);
	IDC_USERID_CTRL->ForceRefresh();
}

class KeyButtonData : public wxObjectRefData
{
public:
	KeyButtonData(const char *key, unsigned int position);
	virtual ~KeyButtonData();

	std::string key_;
	unsigned int position_;
};

KeyButtonData::KeyButtonData(const char *key, unsigned int position) :
	wxObjectRefData(),
	key_(key), position_(position)
{
}

KeyButtonData::~KeyButtonData()
{
}

static std::list<wxButton *> keyboardKeyList;

static void createKeysControls(wxWindow *parent, wxSizer *topsizer)
{
	wxScrolledWindow *scrolledWindow = new wxScrolledWindow(parent, -1, 
		wxDefaultPosition, wxSize(480, 250));
	wxSizer *sizer = new wxFlexGridSizer(5, 1);
	
	keyboardKeyList.clear();
	if (!Keyboard::instance()->loadKeyFile())
	{
		S3D::dialogExit("Keyboard", "Failed to process keyboad file keys.xml");
	}

	int lastGroup = 0;
	std::list<std::string> &keys = 
		Keyboard::instance()->getKeyList();
	std::list<std::string>::iterator itor;
	for (itor = keys.begin();
		itor != keys.end();
		++itor)
	{
		KeyboardKey *key = Keyboard::instance()->getKey((*itor).c_str());

		// Add a spacer line
		if (key->getGroup() != lastGroup)
		{
			lastGroup = key->getGroup();
			for (unsigned int i=0; i<5; i++)
			{
				sizer->Add(new wxStaticText(
					scrolledWindow, -1, wxString("", wxConvUTF8)), 0, wxALIGN_LEFT);
			}
		}

		// Add the key name
		wxStaticText *text = new wxStaticText(
			scrolledWindow, -1, wxString(key->getTitle(), wxConvUTF8));
		text->SetToolTip(wxString(key->getDescription(), wxConvUTF8));
		sizer->Add(text, 0, wxALIGN_LEFT);

		// Add the keys
		for (unsigned int i=0; i<4; i++)
		{
			wxButton *button = new wxButton(scrolledWindow, ID_KEY, wxT(""), 
				wxDefaultPosition, wxSize(120, -1));
			button->SetRefData(new KeyButtonData(key->getName(), i));
			button->SetToolTip(wxString(key->getDescription(), wxConvUTF8));
			sizer->Add(button, 0, wxLEFT | wxALIGN_CENTER, 5);
			keyboardKeyList.push_back(button);
		}
	}
	
	// Setup the scrolled area size
	scrolledWindow->SetAutoLayout(TRUE);
	scrolledWindow->SetSizer(sizer);
	wxSize minSize = sizer->CalcMin();
	scrolledWindow->SetScrollbars(10, 10, 
		(minSize.GetWidth() + 10) / 10, (minSize.GetHeight() + 10) / 10);
	topsizer->Add(scrolledWindow, 1, wxGROW | wxALL | wxALIGN_CENTER, 2);

	// Add the load default keys button
	IDC_LOADKEYDEFAULTS_CTRL = new wxButton(parent, ID_KEYDEFAULTS, wxT("Load Default Keys"));
	topsizer->Add(IDC_LOADKEYDEFAULTS_CTRL, 0, wxCENTER | wxALL, 2);
}

static wxListBox *modbox = 0;
static void updateModList()
{
	modbox->Clear();
	ModDirs dirs;
	if (dirs.loadModDirs())
	{
		std::list<ModInfo>::iterator itor;
		for (itor = dirs.getDirs().begin();
			itor != dirs.getDirs().end();
			++itor)
		{
			ModInfo &info = (*itor);
			modbox->Append(wxString(info.getName(), wxConvUTF8));
		}
	}
}

static void createModsControls(wxWindow *parent, wxSizer *topsizer)
{
	wxStaticBox *modsBox = new wxStaticBox(parent, -1, wxT("Mods"));
	wxStaticBoxSizer *modsSizer = new wxStaticBoxSizer(modsBox, wxVERTICAL);

	modbox = new wxListBox(parent, -1, 
		wxDefaultPosition, wxSize(150, 200), 0, 0, wxLB_SINGLE);
	updateModList();
	modsSizer->Add(modbox, 0, wxALIGN_CENTER | wxALL, 5);

	wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	wxButton *exportBut = new wxButton(parent, ID_EXPORT, wxT("Export"));
	wxButton *importBut = new wxButton(parent, ID_IMPORT, wxT("Import"));
	buttonSizer->Add(exportBut, 0, wxRIGHT, 5);
	buttonSizer->Add(importBut, 0, wxLEFT, 5);
	modsSizer->Add(buttonSizer, 0, wxALIGN_CENTER);
	
	topsizer->Add(modsSizer, 0, wxGROW);	
}

