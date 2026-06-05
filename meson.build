project(
    'multimail',
    'cpp',
    version: '0.52',
    license: 'GPL-3.0-or-later',
    meson_version: '>= 0.59.0',
    default_options: [
        'warning_level=3',
        'cpp_std=c++17',
    ],
)

cc = meson.get_compiler('cpp')
is_msvc = cc.get_id() == 'msvc'

# config.h is hand-maintained (platform/compiler detection); meson only
# flips the three feature toggles via -D guards. See config.h.
toggles = [
    ['vanity_plate', 'NO_VANITY_PLATE'],
    ['shadows', 'NO_USE_SHADOWS'],
    ['tar_kludge', 'NO_TAR_KLUDGE'],
]

foreach t : toggles
    if not get_option(t[0])
        add_project_arguments('-D' + t[1], language: 'cpp')
    endif
endforeach

if is_msvc
    add_project_arguments(
        '-D_CRT_SECURE_NO_WARNINGS',
        language: 'cpp',
    )
else
    extra_flags = [
        '-fno-common',
        '-fstack-protector-strong',
        '-Wformat-security',
        '-Wstrict-overflow=2',
        '-Werror=odr',
        '-Werror=lto-type-mismatch',
        '-Werror=strict-aliasing',
        '-Wno-deprecated-declarations',
        # Deliberate cascading fall-throughs in the key-dispatch switches.
        '-Wno-implicit-fallthrough',
    ]
    add_project_arguments(
        cc.get_supported_arguments(extra_flags),
        language: 'cpp',
    )
endif

# curses: ncurses/SysV curses on Unix/macOS/MSYS2, PDCurses on MSVC.
curses_dep = dependency('curses', required: false)
if not curses_dep.found()
    foreach name : ['ncursesw', 'ncurses', 'pdcurses', 'curses']
        curses_dep = cc.find_library(name, required: false)
        if curses_dep.found()
            break
        endif
    endforeach
endif
if not curses_dep.found()
    error('No curses library found (need ncurses or PDCurses)')
endif

deps = [curses_dep]

# PDCurses' Win32 console port pulls in these; harmless if unused.
if is_msvc
    foreach winlib : ['user32', 'advapi32', 'gdi32', 'winmm']
        wl = cc.find_library(winlib, required: false)
        if wl.found()
            deps += wl
        endif
    endforeach
endif

mmail_src = files(
    'mmail/area.cc',
    'mmail/bw.cc',
    'mmail/compress.cc',
    'mmail/filelist.cc',
    'mmail/letter.cc',
    'mmail/misc.cc',
    'mmail/mmail.cc',
    'mmail/netadd.cc',
    'mmail/omen.cc',
    'mmail/opx.cc',
    'mmail/pktbase.cc',
    'mmail/qwk.cc',
    'mmail/read.cc',
    'mmail/resource.cc',
    'mmail/soup.cc',
)

interfac_src = files(
    'interfac/addrbook.cc',
    'interfac/ansiview.cc',
    'interfac/arealist.cc',
    'interfac/basic.cc',
    'interfac/help.cc',
    'interfac/interfac.cc',
    'interfac/isoconv.cc',
    'interfac/letterl.cc',
    'interfac/letterw.cc',
    'interfac/lettpost.cc',
    'interfac/main.cc',
    'interfac/mmcolor.cc',
    'interfac/mysystem.cc',
    'interfac/packet.cc',
    'interfac/tagline.cc',
)

executable(
    'mm',
    mmail_src + interfac_src,
    include_directories: include_directories('.'),
    dependencies: deps,
    install: true,
)

# mm.1 is committed and current; regenerate from MANUAL.md only when the
# (optional) md2man-roff tool is present.
md2man = find_program('md2man-roff', required: false)
if md2man.found()
    custom_target(
        'mm.1',
        input: 'MANUAL.md',
        output: 'mm.1',
        command: [md2man, '@INPUT@'],
        capture: true,
        install: true,
        install_dir: join_paths(get_option('mandir'), 'man1'),
    )
else
    install_man('mm.1')
endif
