# MultiMail

[![Linux](https://github.com/andy5995/MultiMail/actions/workflows/linux.yml/badge.svg)](https://github.com/andy5995/MultiMail/actions/workflows/linux.yml)
[![macOS](https://github.com/andy5995/MultiMail/actions/workflows/macos.yml/badge.svg)](https://github.com/andy5995/MultiMail/actions/workflows/macos.yml)
[![MSVC](https://github.com/andy5995/MultiMail/actions/workflows/msvc.yml/badge.svg)](https://github.com/andy5995/MultiMail/actions/workflows/msvc.yml)
[![MSYS2](https://github.com/andy5995/MultiMail/actions/workflows/msys2.yml/badge.svg)](https://github.com/andy5995/MultiMail/actions/workflows/msys2.yml)
[![DOS](https://github.com/andy5995/MultiMail/actions/workflows/dos.yml/badge.svg)](https://github.com/andy5995/MultiMail/actions/workflows/dos.yml)

MultiMail is an offline mail packet reader with a curses-based interface,
for Unix / Linux, macOS, Windows, and other systems. It reads and replies
to messages in the Blue Wave, QWK, OMEN, SOUP, and OPX formats.

> This is a fork of MultiMail by [@andy5995][maintainer]. The original was
> written by Kolossvary Tamas and Toth Istvan and maintained for many years
> by [William McBrine][wmcbrine]; see [Credits](#credits). This fork adds a
> Meson build, a unit-test suite, and a number of fixes and features on top
> of the 0.52 base.

MultiMail is free software, distributed under the [GNU General Public
License][gpl], version 3 or later.


## Features

Additions in this fork (see [HISTORY.md] for the full list):

* **Meson build system** for Unix/Linux, macOS, and Windows (MSYS2 with
  ncurses, MSVC with PDCurses), with per-platform CI.
* **Full-length QWK headers** via Synchronet's HEADERS.DAT -- both read
  (To/From/Subject beyond the 25-character MESSAGES.DAT limit) and written
  back into reply packets.
* **Configurable date format** in the reading view (the `DateFormat`
  keyword, a strftime string; defaults to your locale).
* **External viewer** -- the `L` key opens the current message in a pager
  or editor of your choice (the `Viewer` keyword), so links can be clicked
  and text selected.
* A **unit-test suite** (`meson test`), also run under
  AddressSanitizer/UBSan, plus several memory-safety fixes.


## Downloads

Prebuilt binaries, when available, are on the [releases page][releases].
To build from source instead, see below.


## Building

You'll need a C++ compiler, Meson, Ninja, and a curses library (ncurses,
SysV curses, or PDCurses 3.6+). From the top of the source tree:

    meson setup builddir
    meson compile -C builddir

The binary is `builddir/mm`. To run the tests:

    meson test -C builddir

To install under the prefix (default `/usr/local`):

    meson install -C builddir

See [INSTALL.md] for build options, the 16-bit DOS/OS2 makefiles, and
PDCurses/XCurses notes. At run time you'll also need an archiver such as
InfoZip to unpack packets and pack replies.


## Usage

See the [man page][MANUAL] for usage, and edit `~/.mmailrc` (or `mmail.rc`
on Windows) to configure the reader. Example color schemes are in the
`colors` directory; select one with the `ColorFile` keyword.


## Contributing

Bug reports, suggestions, and pull requests are welcome on the
[GitHub project][github]. Use the [issue tracker][issues] for problems and
feature requests.


## Credits

MultiMail was originally developed under Linux by Kolossvary Tamas and
Toth Istvan. John Zero maintained versions 0.2 through 0.6; from version
0.7, [William McBrine][wmcbrine] was the maintainer, and his releases are
the basis for this fork.

Additional code has been contributed by Peter Krefting, Mark D. Rejhon,
Ingo Brueckl, Robert Vukovic, and Frederic Cambus. The QWK HEADERS.DAT
work in this fork was done with input from Rob Swindell (Synchronet).

Earlier bug reports and suggestions are noted in the [HISTORY.md] file.


[gpl]: LEGAL.md
[HISTORY.md]: HISTORY.md
[INSTALL.md]: INSTALL.md
[MANUAL]: MANUAL.md

[maintainer]: https://github.com/andy5995
[github]: https://github.com/andy5995/MultiMail
[issues]: https://github.com/andy5995/MultiMail/issues
[releases]: https://github.com/andy5995/MultiMail/releases/latest
[wmcbrine]: https://wmcbrine.com/MultiMail/
