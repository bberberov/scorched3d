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

static wxCheckBox *IDC_NOENVCOMBINE_CTRL = 0;
static wxCheckBox *IDC_NOOBJECTSHADOWS_CTRL = 0;
static wxCheckBox *IDC_NOSHADOWS_CTRL = 0;
static wxCheckBox *IDC_NOSHADERS_CTRL = 0;
static wxCheckBox *IDC_SIMPLEWATERSHADERS_CTRL = 0;
static wxCheckBox *IDC_NOCUBEMAP_CTRL = 0;
static wxCheckBox *IDC_NOSPHEREMAP_CTRL = 0;
static wxCheckBox *IDC_NOMIPMAPS_CTRL = 0;
static wxCheckBox *IDC_NOEXT_CTRL = 0;
static wxCheckBox *IDC_NOPARTICLEREFLECTIONS_CTRL = 0;
static wxCheckBox *IDC_NOOBJECTREFLECTIONS_CTRL = 0;
static wxCheckBox *IDC_NOSOUND_CTRL = 0;
static wxCheckBox *IDC_NOMUSIC_CTRL = 0;
static wxComboBox *IDC_SOUNDCHANNELS_CTRL = 0;
static wxCheckBox *IDC_NOWATER_CTRL = 0;
static wxCheckBox *IDC_NOWATERREF_CTRL = 0;
static wxCheckBox *IDC_NOSURROUND_CTRL = 0;
static wxCheckBox *IDC_NOVBO_CTRL = 0;
static wxCheckBox *IDC_NOPRECIPITATION_CTRL = 0;
static wxCheckBox *IDC_NOMULTITEX_CTRL = 0;
static wxCheckBox *IDC_INVERT_CTRL = 0;
static wxCheckBox *IDC_TIMER_CTRL = 0;
static wxCheckBox *IDC_NODETAILTEX_CTRL = 0;
static wxCheckBox *IDC_NOLANDSCAPESCORCH_CTRL = 0;
static wxCheckBox *IDC_NODEPTHSORT_CTRL = 0;
static wxCheckBox *IDC_NOBACKDROP_CTRL = 0;
static wxCheckBox *IDC_VALIDATESERVER_CTRL = 0;
static wxRadioButton *IDC_SMALLTEX_CTRL = 0;
static wxRadioButton *IDC_MEDIUMTEX_CTRL = 0;
static wxRadioButton *IDC_LARGETEX_CTRL = 0;
static wxRadioButton *IDC_LOWTANK_CTRL = 0;
static wxRadioButton *IDC_MEDIUMTANK_CTRL = 0;
static wxRadioButton *IDC_HIGHTANK_CTRL = 0;
static wxRadioButton *IDC_LOWEFFECTS_CTRL = 0;
static wxRadioButton *IDC_MEDIUMEFFECTS_CTRL = 0;
static wxRadioButton *IDC_HIGHEFFECTS_CTRL = 0;
static wxRadioButton *IDC_TINYDIALOGS_CTRL = 0;
static wxRadioButton *IDC_SMALLDIALOGS_CTRL = 0;
static wxRadioButton *IDC_MEDIUMDIALOGS_CTRL = 0;
static wxRadioButton *IDC_LARGEDIALOGS_CTRL = 0;
static wxCheckBox *IDC_NOSKINS_CTRL = 0;
static wxCheckBox *IDC_NODYNAMICLIGHT_CTRL = 0;
static wxCheckBox *IDC_FULLSCREEN_CTRL = 0;
static wxCheckBox *IDC_MORERES_CTRL = 0;
static wxCheckBox *IDC_LOGGING_CTRL = 0;
static wxComboBox *IDC_DISPLAY_CTRL = 0;
static wxComboBox *IDC_ANTIALIAS_CTRL = 0;
static wxCheckBox *IDC_FOCUSPAUSE_CTRL = 0;
static wxTextCtrl *IDC_FRAMELIMIT_CTRL = 0;
static wxCheckBox *IDC_SIDESCROLL_CTRL = 0;
static wxCheckBox *IDC_PLAYERCAMERA_CTRL = 0;
static wxSlider *IDC_SLIDER1_CTRL = 0;
static wxSlider *IDC_VOLUME_CTRL = 0;
static wxSlider *IDC_AMBIENTVOLUME_CTRL = 0;
static wxSlider *IDC_MUSICVOLUME_CTRL = 0;
static wxSlider *IDC_LANDLOD_CTRL = 0;
static wxCheckBox *IDC_SINGLESKYLAYER_CTRL = 0;
static wxCheckBox *IDC_NOSKYANI_CTRL = 0;
static wxCheckBox *IDC_NOWATERMOVEMENT_CTRL = 0;
static wxCheckBox *IDC_NOWATERWAVES_CTRL = 0;
static wxCheckBox *IDC_NOWATERLOD_CTRL = 0;
static wxCheckBox *IDC_INVERTMOUSE_CTRL = 0;
static wxCheckBox *IDC_SMOUSE_CTRL = 0;
static wxTextCtrl *IDC_USERNAME_CTRL = 0;
static wxTextCtrl *IDC_TANKMODEL_CTRL = 0;
static wxGrid *IDC_USERID_CTRL = 0;
static wxTextCtrl *IDC_HOSTDESC_CTRL = 0;

static wxButton *IDC_LOADULTRA_CTRL = 0;
static wxButton *IDC_LOADDEFAULTS_CTRL = 0;
static wxButton *IDC_LOADKEYDEFAULTS_CTRL = 0;
static wxButton *IDC_LOADSAFE_CTRL = 0;
static wxButton *IDC_LOADFASTEST_CTRL = 0;
static wxButton *IDC_LOADMEDIUM_CTRL = 0;

static wxButton *IDOK_CTRL = 0;
static wxButton *IDCANCEL_CTRL = 0;
