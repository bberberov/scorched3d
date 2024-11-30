Name:           scorched3d
Version:        44
Release:        1
License:        GPL
Group:          Amusements/Games/3D/Other
Summary:        A 3D version of the classic DOS game Scorched Earth
URL:            http://www.scorched3d.co.uk

Source0:        http://unc.dl.sourceforge.net/sourceforge/scorched3d/Scorched3D-%{version}-src.tar.gz

BuildRoot:      %{_tmppath}/%{name}-buildroot

BuildRequires:  SDL_net SDL_net-devel SDL SDL-devel >= 1.2.5
BuildRequires:  fftw-devel libjpeg-devel libpng-devel glew-devel
BuildRequires:  freetype freetype-devel expat-devel
BuildRequires:  libogg libogg-devel libvorbis libvorbis-devel libjpeg-devel
BuildRequires:  wxGTK wxGTK-devel >= 2.4.0
BuildRequires:  openal-soft openal-soft-devel freealut freealut-devel
BuildRequires:  autoconf automake libtool

Requires:       SDL, SDL_net, freetype, libogg, wxGTK, openal-soft, freealut

%description
Scorched3D is a cross-platform 3D remake of the popular 2D artillery game
Scorched Earth. Scorched3D can be played against the computer, other players,
and remotely across the Internet or LAN.

%prep
%setup -q -n scorched


%build
# sh ./autogen.sh

%configure --datadir=%{_datadir}/scorched3d \
	--program-prefix=%{?_program_prefix:%{_program_prefix}}

make -j2 %{?_smp_mflags}


%install
rm -rf $RPM_BUILD_ROOT

%makeinstall datadir=$RPM_BUILD_ROOT%{_datadir}/scorched3d


%clean
rm -rf $RPM_BUILD_ROOT


%files
%defattr(664,root,games,755)
%doc documentation/*
%dir %{_datadir}/scorched3d
%{_datadir}/scorched3d/data
%{_datadir}/scorched3d/documentation
%attr(755,root,root) %{_bindir}/scorched3d
%attr(755,root,root) %{_bindir}/scorched3dc
%attr(755,root,root) %{_bindir}/scorched3ds


%changelog
* Fri Apr 2 2004 Edward Rudd 
- Fixed data dir location and permissions
* Sun Dec 7 2003 Gavin Camp gcamp@scorched3d.co.uk
- First build.
