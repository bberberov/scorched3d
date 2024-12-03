![Scorched3D](.images/logo_overlay.png "Scorched3D")

Scorched3D incorporates lively, fully destructible 3D landscapes that include animated jets, naval vessels, water and even birds, with detailed tanks and projectiles and stunning visual effects.

<a href='.images/6700.jpg'>
<img src='.images/6700.jpg' alt='Trees from version 36'            width=160 height=120 />
</a>
<a href='.images/6702.jpg'>
<img src='.images/6702.jpg' alt='Spectator view from version 37.2' width=160 height=120 />
</a>
<a href='.images/6704.jpg'>
<img src='.images/6704.jpg' alt='Some action from version 37.2'    width=160 height=120 />
</a>
<a href='.images/8943.jpg'>
<img src='.images/8943.jpg' alt='Lightning from version 38'        width=160 height=120 />
</a>

A simple turn-based artillery game and also a real-time strategy game in which players can counter each others' weapons with other creative accessories, shields and tactics. Test your skill in timing, aiming and judgement of distance, move around on the landscape, or outsmart your opponent economically.

## Features

- Realisticly rendered 3D landscape environments
- Real-time or turn-based artillery combat
- Online or offline, single or multi-player play
- Mod support for various gaming styles
- Tournament and stats championships

## Gameplay

Learning how to play is easy, just point the turret, adjust power and trajectory, select a weapon – and shoot!

Manage your inventory of weapons with the prize money you win during each round of combat. Learn the basic controls using the tutorial mode, or set up your own game by customising dozens of economy and environmental conditions to your liking, or simply play online immediately at one of the various public servers.

You can play with up to twenty-four other players at a time, mixing computer players with humans on both randomly generated landscapes and user-designed maps. Scorched3D comes bundled with Apocalypse mod, which offers enhanced levels of weaponry.

Scorched3D is available for Microsoft Windows, Linux and Mac operating systems.

## Requirements

### Build

- [pkg-config](https://www.freedesktop.org/wiki/Software/pkg-config/) >= 0.29 or [pkgconf](http://pkgconf.org/)
- [OpenAL](https://www.openal-soft.org/) >= 1.22.0
- [Freealut](https://github.com/vancegroup/freealut)
- [FFTW3](https://www.fftw.org/)
- [Freetype](https://freetype.org/)
- OpenGL, GLEW
- [OGG](https://www.xiph.org/ogg/) and [Vorbis](https://xiph.org/vorbis/)
- SDL 1.2.x series, SDL_net
- [wxWidgets](https://www.wxwidgets.org/)
- [expat](https://libexpat.github.io/)
- [zlib](http://zlib.net/)
- [libpng](http://www.libpng.org/)

### Optional

- `xdg-utils` for opening web links
- MySQL or PostgreSQL

## Building

### Linux

- Run `createAMMakefile.pl` from its directory to create the necesary `Makefile.am` from the Visual Studio project files
  ```
  cd scripts
  perl createAMMakefile.pl
  cd ..
  ```
- Reconfigure with `autoreconf -fiv`
- Configure with `./configure ...`
- Build with `make ...`
- Install with `make install ...`

### Windows

TODO

### macOS

TODO

## License

License under the terms of the [GNU General Public License v2.0 or later](https://www.gnu.org/licenses/old-licenses/gpl-2.0.html)
___

## Original README

Scorched3D : Copyright 2009 Gavin Camp

For Scorched3D updates and news visit http://www.scorched3d.co.uk

For Scorched3D documentation visit http://www.scorched3d.co.uk/wiki

For information on how to compile Scorched3D visit http://www.scorched3d.co.uk/wiki/index.php/Scorched3D_Development

For Scorched3D license information see the documentation directory.

For Apocalypse mod updates and news visit http://apochq.handwired.net
