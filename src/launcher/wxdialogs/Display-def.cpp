////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2003, 2008-2011, 2024-2026
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
	ID_LOADDEFAULTS = 60,
	ID_LOADULTRA,
	ID_LOADFASTEST,
	ID_LOADSAFE,
	ID_LOADMEDIUM,
	ID_MORERES,
	ID_KEYDEFAULTS,
	ID_IMPORT,
	ID_EXPORT,
	ID_NOTEBOOK,
	ID_PANEL_IDENT,
	ID_KEY = 200
};

static wxCheckBox*    IDC_NOENVCOMBINE_CTRL          = nullptr;
static wxCheckBox*    IDC_NOOBJECTSHADOWS_CTRL       = nullptr;
static wxCheckBox*    IDC_NOSHADOWS_CTRL             = nullptr;
static wxCheckBox*    IDC_NOSHADERS_CTRL             = nullptr;
static wxCheckBox*    IDC_SIMPLEWATERSHADERS_CTRL    = nullptr;
static wxCheckBox*    IDC_NOCUBEMAP_CTRL             = nullptr;
static wxCheckBox*    IDC_NOSPHEREMAP_CTRL           = nullptr;
static wxCheckBox*    IDC_NOMIPMAPS_CTRL             = nullptr;
static wxCheckBox*    IDC_NOEXT_CTRL                 = nullptr;
static wxCheckBox*    IDC_NOPARTICLEREFLECTIONS_CTRL = nullptr;
static wxCheckBox*    IDC_NOOBJECTREFLECTIONS_CTRL   = nullptr;
static wxCheckBox*    IDC_NOSOUND_CTRL               = nullptr;
static wxCheckBox*    IDC_NOMUSIC_CTRL               = nullptr;
static wxComboBox*    IDC_SOUNDCHANNELS_CTRL         = nullptr;
static wxCheckBox*    IDC_NOWATER_CTRL               = nullptr;
static wxCheckBox*    IDC_NOWATERREF_CTRL            = nullptr;
static wxCheckBox*    IDC_NOSURROUND_CTRL            = nullptr;
static wxCheckBox*    IDC_NOVBO_CTRL                 = nullptr;
static wxCheckBox*    IDC_NOPRECIPITATION_CTRL       = nullptr;
static wxCheckBox*    IDC_NOMULTITEX_CTRL            = nullptr;
static wxCheckBox*    IDC_INVERT_CTRL                = nullptr;
static wxCheckBox*    IDC_TIMER_CTRL                 = nullptr;
static wxCheckBox*    IDC_NODETAILTEX_CTRL           = nullptr;
static wxCheckBox*    IDC_NOLANDSCAPESCORCH_CTRL     = nullptr;
static wxCheckBox*    IDC_NODEPTHSORT_CTRL           = nullptr;
static wxCheckBox*    IDC_NOBACKDROP_CTRL            = nullptr;
static wxCheckBox*    IDC_VALIDATESERVER_CTRL        = nullptr;
static wxRadioButton* IDC_SMALLTEX_CTRL              = nullptr;
static wxRadioButton* IDC_MEDIUMTEX_CTRL             = nullptr;
static wxRadioButton* IDC_LARGETEX_CTRL              = nullptr;
static wxRadioButton* IDC_LOWTANK_CTRL               = nullptr;
static wxRadioButton* IDC_MEDIUMTANK_CTRL            = nullptr;
static wxRadioButton* IDC_HIGHTANK_CTRL              = nullptr;
static wxRadioButton* IDC_LOWEFFECTS_CTRL            = nullptr;
static wxRadioButton* IDC_MEDIUMEFFECTS_CTRL         = nullptr;
static wxRadioButton* IDC_HIGHEFFECTS_CTRL           = nullptr;
static wxRadioButton* IDC_TINYDIALOGS_CTRL           = nullptr;
static wxRadioButton* IDC_SMALLDIALOGS_CTRL          = nullptr;
static wxRadioButton* IDC_MEDIUMDIALOGS_CTRL         = nullptr;
static wxRadioButton* IDC_LARGEDIALOGS_CTRL          = nullptr;
static wxCheckBox*    IDC_NOSKINS_CTRL               = nullptr;
static wxCheckBox*    IDC_NODYNAMICLIGHT_CTRL        = nullptr;
static wxCheckBox*    IDC_FULLSCREEN_CTRL            = nullptr;
static wxCheckBox*    IDC_MORERES_CTRL               = nullptr;
static wxCheckBox*    IDC_LOGGING_CTRL               = nullptr;
static wxComboBox*    IDC_DISPLAY_CTRL               = nullptr;
static wxComboBox*    IDC_ANTIALIAS_CTRL             = nullptr;
static wxCheckBox*    IDC_FOCUSPAUSE_CTRL            = nullptr;
static wxTextCtrl*    IDC_FRAMELIMIT_CTRL            = nullptr;
static wxCheckBox*    IDC_SIDESCROLL_CTRL            = nullptr;
static wxCheckBox*    IDC_PLAYERCAMERA_CTRL          = nullptr;
static wxSlider*      IDC_SLIDER1_CTRL               = nullptr;
static wxSlider*      IDC_VOLUME_CTRL                = nullptr;
static wxSlider*      IDC_AMBIENTVOLUME_CTRL         = nullptr;
static wxSlider*      IDC_MUSICVOLUME_CTRL           = nullptr;
static wxSlider*      IDC_LANDLOD_CTRL               = nullptr;
static wxCheckBox*    IDC_SINGLESKYLAYER_CTRL        = nullptr;
static wxCheckBox*    IDC_NOSKYANI_CTRL              = nullptr;
static wxCheckBox*    IDC_NOWATERMOVEMENT_CTRL       = nullptr;
static wxCheckBox*    IDC_NOWATERWAVES_CTRL          = nullptr;
static wxCheckBox*    IDC_NOWATERLOD_CTRL            = nullptr;
static wxCheckBox*    IDC_INVERTMOUSE_CTRL           = nullptr;
static wxCheckBox*    IDC_SMOUSE_CTRL                = nullptr;
static wxTextCtrl*    IDC_USERNAME_CTRL              = nullptr;
static wxTextCtrl*    IDC_TANKMODEL_CTRL             = nullptr;
static wxGrid*        IDC_USERID_CTRL                = nullptr;
static wxTextCtrl*    IDC_HOSTDESC_CTRL              = nullptr;

static wxButton* IDC_LOADULTRA_CTRL       = nullptr;
static wxButton* IDC_LOADDEFAULTS_CTRL    = nullptr;
static wxButton* IDC_LOADKEYDEFAULTS_CTRL = nullptr;
static wxButton* IDC_LOADSAFE_CTRL        = nullptr;
static wxButton* IDC_LOADFASTEST_CTRL     = nullptr;
static wxButton* IDC_LOADMEDIUM_CTRL      = nullptr;

static wxButton* IDOK_CTRL     = nullptr;
static wxButton* IDCANCEL_CTRL = nullptr;
