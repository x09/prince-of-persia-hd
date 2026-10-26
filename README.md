# PoPHD — Prince of Persia HD for Linux

<img width="1280" height="800" alt="Prince of Persia HD" src="https://github.com/user-attachments/assets/df854895-e0b6-4ed2-a6aa-c49b163af539" />

This project began as an attempt to analyse Jordan Mechner's original
[Prince of Persia source code for the Apple II](https://github.com/jmechner/Prince-of-Persia-Apple-II)
and rewrite the game for Linux using GCC, Make, SDL2 and SDL2_image.

After translating the assembly code and getting the original game running on
Linux, the Apple II implementation was set aside. 

| 5% of the workflow | 90% of the workflow |
| :---: | :---: |
| <img width="40%" height="40%" alt="Apple II PoP" src="https://github.com/user-attachments/assets/f0be88ce-9561-4746-804c-9e752126bcda" /> |<img width="40%" height="40%" alt="Apple II PoP" src="https://github.com/user-attachments/assets/36dfd912-3869-42a0-a091-cf8301119ae1" /> |

Development moved to [SDLPoP](https://github.com/NagyD/SDLPoP), the native C/SDL port based on the
disassembly of the DOS version. This became the foundation of the current game.
The earlier Apple II work remains part of the project's history; the release
source archive contains the maintained DOS-based implementation.

The new version received HD artwork for the characters and environments,
a replacement opening presentation and soundtrack, and improvements to the
rendering of animation, traps and scenery. Development and artwork were
AI-assisted. The familiar platforming, sword fighting and puzzles remain,
with a few hidden abilities added for the hero :)


| Old graphics | HD |
| :---: | :---: |
| <img width="1156" height="715" alt="изображение" src="https://github.com/user-attachments/assets/2444443b-3973-4f59-9191-67107fc50530" /> | <img width="1152" height="714" alt="изображение" src="https://github.com/user-attachments/assets/cbaaa06f-feab-47b7-9b5d-9fa9bfcd18b6" /> |

<img width="1440" height="780" alt="prince" src="https://github.com/user-attachments/assets/62f7c8ad-4cea-433b-9db7-6174fd475593" />

<img width="1000" height="320" alt="characters" src="https://github.com/user-attachments/assets/cbce5826-d8ca-4490-994e-a85eca37a0ff" />


Project: [github.com/x09/prince-of-persia-hd](https://github.com/x09/prince-of-persia-hd).
[Русская версия](README-ru.md).

## Stack and dependencies

PoPHD is a native Linux application written in C (C99), built with GCC and GNU
Make. SDL2 handles the window, input and audio; SDL2_image loads PNG artwork.
Building requires GCC, Make, pkg-config, and the SDL2/SDL2_image development
packages. On ALT Linux the package names are:

```text
gcc make pkg-config libSDL2-devel libSDL2_image-devel
```

Ready-to-use game assets are included; a normal build does not require image
generation tools or network downloads.

## Build and run

From the project or extracted source archive directory:

```sh
make -j4
./dos/pophd
```

HD graphics are enabled by default. To select a graphics mode explicitly:

```sh
./dos/pophd --hd
./dos/pophd --dos
```

F10 switches between HD and original graphics during an HD session.
Arrow keys move the hero; Shift performs actions and sword attacks;
Esc opens the menu; Alt+Enter toggles fullscreen; Ctrl+Q quits.
The upstream control reference is in `dos/README.SDLPoP.md`.

Basic resource/level checks and the full automated campaign check:

```sh
make check
make campaign-full-check
```

The campaign check covers the ending and compares DOS/HD simulation traces;
it does not replace visual playtesting of every possible situation.

## Installation and RPM

To stage an installation without writing to system directories:

```sh
make install DESTDIR=/tmp/pophd-stage prefix=/usr
```

For a system installation, run `make install prefix=/usr` with administrator
privileges. The application is called PoPHD in the desktop menu; the installed
command is `pophd`. User settings and saves are stored in
`~/.prince-of-persia-hd`.

The `prince-of-persia-hd-1.0.tar.gz` source archive and the accompanying
`prince-of-persia-hd.spec` provide the sources and packaging instructions for
ALT Linux RPM builds. Both this README and its Russian translation are included
in the archive and installed with the package documentation.

## Credits and licenses

Original game: Jordan Mechner and the original Prince of Persia team.
Current engine foundation: David Nagy and SDLPoP contributors.
SDLPoP's GPL-3.0-or-later license and copyright notices are preserved.
Original game assets retain their respective rights and are not relicensed by
the engine's GPL. See `dos/COPYING`, `dos/README.SDLPoP.md` and
`packaging/ASSET_NOTICES.md` for licenses and attribution.
