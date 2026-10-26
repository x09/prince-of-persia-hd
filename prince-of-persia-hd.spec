Name: prince-of-persia-hd
Version: 1.0
Release: alt3

Summary: PoPHD - classic Prince of Persia with HD artwork
# GPL covers the engine. Original game data retain their original copyright;
# see ASSET_NOTICES.md. This is not an assertion of GPL licensing for all data.
License: GPL-3.0-or-later AND LicenseRef-Original-Game-Data AND Bitstream-Vera AND MIT
Group: Games/Arcade
Url: https://github.com/x09/prince-of-persia-hd
Source: %name-%version.tar.gz

BuildRequires: gcc make pkg-config libSDL2-devel libSDL2_image-devel
BuildRequires: desktop-file-utils grep coreutils

%description
PoPHD is an unofficial HD fork of the native SDLPoP (https://github.com/NagyD/SDLPoP) engine.
It preserves the classic game simulation and adds new HD artwork, soundtrack and fonts.
The package includes game data; see ASSET_NOTICES.md for attribution.

%description -l ru_RU.UTF-8
PoPHD — неофициальная HD-версия Prince of Persia на основе SDLPoP (https://github.com/NagyD/SDLPoP).
Классическая игровая механика дополнена новой HD-графикой, саундтреком и шрифтами.

%prep
%setup

%build
%make_build -C dos CFLAGS="%optflags -std=c99 -Wall"

%install
%makeinstall_std prefix=%_prefix bindir=%_bindir datadir=%_datadir docdir=%_docdir/%name-%version

%check
desktop-file-validate packaging/pophd.desktop
%make_build -C dos check campaign-full-check persiality-check CFLAGS="%optflags -std=c99 -Wall"
%make_build -C dos hd-loose-check hd-floor-overlay-check hd-chomper-check CFLAGS="%optflags -std=c99 -Wall"
%make_build -C dos persiality-special-check CFLAGS="%optflags -std=c99 -Wall"

%files
%_bindir/pophd
%_datadir/prince-of-persia-hd/
%_desktopdir/pophd.desktop
%_iconsdir/hicolor/256x256/apps/pophd.png
%_docdir/%name-%version/

%changelog
* Fri Oct 23 2026  Anton Shevtsov <shevtsov.anton@gmail.com>  1.0-alt3
- Include English and Russian project history and build instructions in the documentation.
- Include the PoPHD project URL in version output.
- Ensure build checks use source-tree HD assets instead of an installed version.
- Add HD-only Persiality for guards, Skeleton and Jaffar while preserving story events.
- Fix secret code input, spell animation ghosting and letterbox flash artifacts.
- Add painted HD dungeon columns, exit details and steel chompers with realistic blood.
- Fix column joints, pedestal shapes and their alignment with the floor.
- Fix climbing occlusion and wall artifacts after loose floors fall.

* Wed Oct 21 2026 Anton Shevtsov <shevtsov.anton@gmail.com> 1.0-alt2
- Replace scratched dungeon material with a quiet matte stone texture.
- Use grep for build checks instead of ripgrep.

* Wed Oct 21 2026 Anton Shevtsov <shevtsov.anton@gmail.com> 1.0-alt1
- Initial source release with HD launcher, soundtrack, artwork and desktop integration.
- Include full campaign regression and preserve upstream attribution.
