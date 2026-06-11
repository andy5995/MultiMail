# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

MultiMail is an offline mail packet reader (Blue Wave, QWK/QWKE, OMEN, SOUP,
OPX) with a curses interface. C++ but deliberately old-school: hand-maintained
Makefiles, compile-time configuration via `config.h`, heavy platform `#ifdef`
use, and broad portability (Unix/Linux, DOS, OS/2, Windows, macOS; ncurses,
SysV curses, or PDCurses). This checkout is `andy5995/MultiMail`, a fork of
upstream `wmcbrine/MultiMail`.

## Build / run

The primary (Unix/Linux/macOS/MSYS2/MSVC) build is **meson**:

```
meson setup builddir                 # debug by default
meson compile -C builddir            # -> builddir/mm
./builddir/mm                        # run (reads ~/.mmailrc; packets in ~/mmail)
meson install -C builddir            # installs mm + mm.1 under prefix
meson setup builddir --buildtype=release -Dstrip=true   # stripped release
```

- **Unit tests live in `tests/`** (pure backend helpers — no curses, no globals;
  harness in `tests/test.h`). The `tests` option defaults on, so
  `meson test -C builddir` runs them. Each test links the backend `.cc` it covers
  (sources listed in `tests/meson.build`). A test that branches on a platform
  macro like `DOSNAMES` **must `#include "../config.h"`**, or it silently compiles
  the wrong platform's expectations while the linked code does the right thing —
  exactly what broke the `misc` test on MSVC/MSYS2.
- CI (`.github/workflows/`) builds on Linux, macOS, MSYS2 (ncurses), and MSVC
  (PDCurses via vcpkg). Beyond `meson test`, the cross-platform bar is still
  "compiles clean everywhere."
- Curses is found via `dependency('curses')` with `find_library` fallbacks
  (ncursesw/ncurses/pdcurses). It's a **frontend-only** dependency — nothing in
  `mmail/` includes curses.
- `config.h` is **not generated** — it stays hand-maintained for the legacy
  makefiles. meson only flips three feature toggles by predefining guard macros:
  `-Dvanity_plate=false` → `-DNO_VANITY_PLATE`, `-Dshadows=false` →
  `-DNO_USE_SHADOWS`, `-Dtar_kludge=false` → `-DNO_TAR_KLUDGE`. See
  `meson_options.txt` and the guarded `#define`s in `config.h`. Run
  `meson fmt -eir` after editing `meson.build`/`meson_options.txt`.
- **Adding/removing a `.cc` means editing `meson.build`'s `mmail_src`/
  `interfac_src` lists AND the legacy makefiles' object lists (`Makefile.bcc`,
  `Makefile.vc`, `Makefile.wcc`, and `tclist`).** That source-list duplication is
  the main upkeep cost of keeping the legacy makefiles around.
- Legacy cross-compiler makefiles (kept; meson can't drive these toolchains):
  `Makefile.bcc` (Borland/Turbo C++, DOS/Win), `Makefile.wcc` (Watcom/wmake).
  `Makefile.vc` (MSVC/nmake) is redundant with the meson MSVC path and could be
  retired. They share the `depend` file (`make dep` regenerates it).
- `mm.1` is committed and current; meson installs it as-is and only regenerates
  from `MANUAL.md` if `md2man-roff` is installed. Edit `MANUAL.md`, not `mm.1`.

## Architecture

Two layers, each its own directory, joined only through the abstract interfaces
in `mmail/mmail.h`:

- **`mmail/` — backend / data model.** Format-agnostic mail model plus the
  per-format packet drivers. Knows nothing about curses.
- **`interfac/` — curses UI.** All screen handling. `main()` lives in
  `interfac/main.cc`.

Three global singletons wire it together (defined in `interfac/main.cc`):
`mm` (the `mmail` facade), `ui` (the `Interface`), and `error`.

### Backend model (`mmail/mmail.h`)

`mmail` is the central facade: it owns the resource/config object, the open
packet driver, the reply driver, the area/letter lists, and the read-status
tracker. The data model is a hierarchy of list/header/body classes —
`area_list` → `area_header`, `letter_list` → `letter_header` → `letter_body`,
plus `file_list`/`file_header` for filesystem browsing.

**Packet driver design (the key abstraction):**
- `specific_driver` — pure-virtual interface every readable format implements
  (enumerate areas, enumerate letters, fetch a body, expose BBS metadata).
- `pktbase` (`mmail/pktbase.{h,cc}`) — shared implementation of the common
  packet logic. The concrete reader drivers inherit from it:
  `bw` (Blue Wave), `qwk`, `omen`, `soup`, `opx`. Each `*.cc`/`*.h` pair handles
  one wire format; format quirks belong here, not in the UI.
- `reply_driver` extends `specific_driver` with reply-writing operations;
  `pktreply` is its shared base, and each format has a matching reply class.
- `mmail::detect_and_open()` sniffs an unknown packet and instantiates the right
  driver — that's where format autodetection lives.
- `read_class` (concrete: `main_read_class`, `reply_read_class`) tracks
  read/replied/marked/saved status per letter, persisted across sessions.

Adding or changing a format means working through these interfaces: implement
the `specific_driver`/`reply_driver` virtuals (usually by subclassing
`pktbase`/`pktreply`), then register detection in `detect_and_open()`.

### Interface (`interfac/interfac.h`)

`Interface` (`ui`) is a state machine over `enum statetype` (packetlist,
arealist, letterlist, letter, address, tagwin, ansiwin, help screens). Most
screens derive from the `ListWindow` base, which provides scrolling, search,
and filtering; concrete windows (`PacketListWindow`, `AreaListWindow`,
`LetterListWindow`, `AddressBook`, `TaglineWindow`, …) implement the
`oneLine`/`oneSearch`/`extrakeys` virtuals. `LetterWindow` and `AnsiWindow`
(ANSI/Avatar art viewer) are the heavier non-list screens. `Win` / `ShadowedWin`
/ `InfoWin` are thin curses-window wrappers used everywhere.

Other interface pieces: `mmcolor.*` (color-scheme engine; schemes are the
`colors/*.col` files, selected via the `ColorFile` rc keyword), `isoconv.*`
(charset/ISO conversion), `mysystem.*` (OS abstraction: paths, spawning the
external (de)compressor and editor), `packet.cc` (UI side of opening packets).

### Configuration — two separate things, don't conflate

- **`config.h`** — *compile-time*. Version numbers, feature toggles
  (`USE_SHADOWS`, `VANITY_PLATE`, `TAR_KLUDGE`), and the large block of
  per-compiler/per-OS `#define`s. Hand-edited; there is no `configure` step.
- **`resource` class (`mmail/resource.{h,cc}`)** — *runtime*. Parses
  `~/.mmailrc` (`mmail.rc` on DOS/OS2/Win). The `enum`s at the top of
  `resource.h` are the canonical list of every rc keyword (strings then ints);
  adding a setting means extending those enums plus the parallel name/default
  tables in `resource.cc`. `ColorClass` and `resource` both derive from
  `baseconfig`, the shared config-file parser.

## Conventions

- Match the surrounding style: `.cc`/`.h` pairs, `extern "C"` blocks around C
  headers, manual memory management (raw `new`/`delete`, no STL containers or
  smart pointers), C-string APIs throughout. This is intentional for portability
  to ancient toolchains (16-bit DOS, Turbo C++) — don't modernize toward STL or
  C++11 idioms without reason.
- Guard anything compiler- or OS-specific behind the existing `config.h` macros
  rather than introducing new ad-hoc `#ifdef`s.
- The release build runs `-Wall -pedantic`; keep new code warning-clean.
- Don't restyle existing code when editing it. This codebase deliberately uses
  *both* `else` forms: a braced block cuddles (`} else {`), while a single-
  statement (unbraced) `if` puts `else` on its own line. Match whichever the
  lines you're touching already use; never brace a single statement or move an
  `else` just to make an edit fit.
- Changes affecting users get a one-line entry; `HISTORY.md` is the running
  changelog.
