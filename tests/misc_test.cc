/*
 * MultiMail offline mail reader
 * Unit tests for mmail/misc.cc (pure helpers; no curses, no globals)

 Distributed under the GNU General Public License, version 3 or later. */

#include "test.h"
#include "../mmail/misc.h"

int main()
{
    unsigned char b[4];

    // Little-endian 16-bit: byte order and round-trip
    putshort(b, 0x1234);
    CHECK(b[0] == 0x34 && b[1] == 0x12);
    CHECK(getshort(b) == 0x1234u);
    putshort(b, 0xFFFF);
    CHECK(getshort(b) == 0xFFFFu);

    // Little-endian 32-bit
    putlong(b, 0x89ABCDEFUL);
    CHECK(b[0] == 0xEF && b[3] == 0x89);
    CHECK(getlong(b) == 0x89ABCDEFUL);

    // Big-endian 32-bit
    putblong(b, 0x89ABCDEFUL);
    CHECK(b[0] == 0x89 && b[3] == 0xEF);
    CHECK(getblong(b) == 0x89ABCDEFUL);

    // Same bytes, opposite endianness reads differently
    putlong(b, 0x00000001UL);
    CHECK(getblong(b) == 0x01000000UL);

    // strnzcpy: truncates to len, always NUL-terminates, returns end pointer
    char buf[16];
    char *end = strnzcpy(buf, "hello", 3);
    CHECK_STR(buf, "hel");
    CHECK(end == buf + 3 && *end == '\0');
    end = strnzcpy(buf, "hi", 10);
    CHECK_STR(buf, "hi");
    CHECK(end == buf + 2);

    // stripre: strips a leading, case-insensitive "re: ", repeatedly
    CHECK_STR(stripre("Re: hello"), "hello");
    CHECK_STR(stripre("RE: re: x"), "x");
    CHECK_STR(stripre("hello"), "hello");
    CHECK_STR(stripre("Reply"), "Reply");

    return mm_test_report();
}
