/*
 * MultiMail offline mail reader
 * Unit tests for mmail/qwkhdr.cc (Synchronet HEADERS.DAT parser)

 Distributed under the GNU General Public License, version 3 or later. */

#include "test.h"
#include "../mmail/qwkhdr.h"

int main()
{
    // A two-message HEADERS.DAT, as Synchronet's msgtoqwk.cc writes it:
    // hex-offset sections, RFC-style "Key: Value" plus ini-style "Key = Value",
    // CRLF line endings, and values that themselves contain ':' and '='.
    const char *sample =
        "[1a80]\r\n"
        "Utf8 = true\r\n"
        "Format = flowed\r\n"
        "Message-ID: <100:200/300@vert>\r\n"
        "WhenWritten:  Fri Jun 05 12:34:56 2026 0000\r\n"
        "Sender: Digital Man\r\n"
        "Subject: longer than the 25-char MESSAGES.DAT field = yes\r\n"
        "Recipient: All\r\n"
        "Conference: 12\r\n"
        "\r\n"
        "[2b00]\r\n"
        "Sender: Rob Swindell\r\n"
        "Subject: second message\r\n";

    qwkHeaders h;
    h.parse(sample);

    // Sections are keyed by hex offset into MESSAGES.DAT
    CHECK(h.has(0x1a80));
    CHECK(h.has(0x2b00));
    CHECK(!h.has(0x9999));

    // Full (untruncated) RFC-style ':' fields
    CHECK_STR(h.get(0x1a80, "Sender"), "Digital Man");
    CHECK_STR(h.get(0x1a80, "Recipient"), "All");
    CHECK_STR(h.get(0x1a80, "Conference"), "12");

    // ini-style '=' fields
    CHECK_STR(h.get(0x1a80, "Utf8"), "true");
    CHECK_STR(h.get(0x1a80, "Format"), "flowed");

    // A value containing ':' must split only on the first (key) separator
    CHECK_STR(h.get(0x1a80, "Message-ID"), "<100:200/300@vert>");
    CHECK_STR(h.get(0x1a80, "WhenWritten"), "Fri Jun 05 12:34:56 2026 0000");

    // A value containing '=' after the ':' separator stays intact
    CHECK_STR(h.get(0x1a80, "Subject"),
              "longer than the 25-char MESSAGES.DAT field = yes");

    // Keys are case-insensitive
    CHECK_STR(h.get(0x1a80, "sender"), "Digital Man");
    CHECK_STR(h.get(0x1a80, "MESSAGE-ID"), "<100:200/300@vert>");

    // The second section is independent
    CHECK_STR(h.get(0x2b00, "Sender"), "Rob Swindell");
    CHECK_STR(h.get(0x2b00, "Subject"), "second message");

    // Absent keys / offsets return 0
    CHECK(h.get(0x1a80, "Nonexistent") == 0);
    CHECK(h.get(0x9999, "Sender") == 0);

    // Parsing empty / null input is harmless
    qwkHeaders empty;
    empty.parse("");
    CHECK(!empty.has(0x1a80));
    empty.parse(0);

    return mm_test_report();
}
