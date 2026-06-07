# Each test is a standalone executable linked only against the specific
# self-contained source under test, then registered with meson's test().
# Source file lists that reach outside this directory are passed in from
# the top-level meson.build (meson forbids files() from escaping the subdir).

test(
    'misc',
    executable(
        'test_misc',
        files('misc_test.cc') + mm_misc_src,
        include_directories: include_directories('..'),
        override_options: cpp98,
    ),
)

test(
    'qwkhdr',
    executable(
        'test_qwkhdr',
        files('qwkhdr_test.cc') + mm_qwkhdr_src + mm_misc_src,
        include_directories: include_directories('..'),
        override_options: cpp98,
    ),
)

test(
    'qheader',
    executable(
        'test_qheader',
        files('qheader_test.cc') + mm_qheader_src + mm_misc_src,
        include_directories: include_directories('..'),
        override_options: cpp98,
    ),
)
