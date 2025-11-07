// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include <algorithm>
#include "basic_compiler/Lexer.h"

using namespace gwbasic;

/***
Test: Lexer.EscapeForLog_EmbeddedNul_Safe
Purpose: Ensure embedded NUL bytes are preserved and rendered as hex (\\x00)
         so logs are not truncated by C-string printing.
Security: Guards against log truncation/injection via NUL bytes.
*/
TEST(Lexer, EscapeForLog_EmbeddedNul_Safe) {
    const char raw[] = {'a', '\0', 'b'}; // size 3
    std::string in(raw, sizeof raw);
    const std::string out = LexerAccessorForTests::escapeForLog(in);
    EXPECT_EQ(out, std::string("a\\x00b"));
    // No raw NUL remains in output
    EXPECT_EQ(out.find('\0'), std::string::npos);
}

/***
Test: Lexer.EscapeForLog_SingleLine_EscapesCRLF
Purpose: Ensure CR/LF are escaped so logs remain single-line and resilient
         against injection/control attacks.
*/
TEST(Lexer, EscapeForLog_SingleLine_EscapesCRLF) {
    const std::string in = std::string("foo\r\nbar\n");
    const std::string out = LexerAccessorForTests::escapeForLog(in);
    // Must not contain raw CR/LF; must contain literal sequences
    EXPECT_EQ(out.find('\n'), std::string::npos);
    EXPECT_EQ(out.find('\r'), std::string::npos);
    EXPECT_NE(out.find("\\r"), std::string::npos);
    EXPECT_NE(out.find("\\n"), std::string::npos);
}

/***
Test: Lexer.EscapeForLog_LongInput_NoOverflow
Purpose: Large input comprised of backslashes doubles in size and preserves
         structure, indicating no overflow or truncation.
*/
TEST(Lexer, EscapeForLog_LongInput_NoOverflow) {
    const size_t N = 4096; // modest but large enough for capacity growth
    std::string in(N, '\\');
    const std::string out = LexerAccessorForTests::escapeForLog(in);
    ASSERT_EQ(out.size(), N * 2);
    // All characters in out should be backslashes in pairs
    EXPECT_TRUE(std::all_of(out.begin(), out.end(), [](char c){ return c == '\\'; }));
}

/***
Test: Lexer.EscapeForLog_HighBitBytes_Preserved
Purpose: Validate that bytes >= 0x80 (e.g., UTF-8 sequences) are preserved.
Note: This behavior may be acceptable for logging, but consider escaping
      non-ASCII if logs must be strictly ASCII-only.
*/
TEST(Lexer, EscapeForLog_HighBitBytes_Preserved) {
    std::string in;
    in.push_back(static_cast<char>(0xC3)); // UTF-8 'é' first byte
    in.push_back(static_cast<char>(0xA9)); // UTF-8 'é' second byte
    in.push_back(static_cast<char>(0xFF)); // 0xFF byte
    const std::string out = LexerAccessorForTests::escapeForLog(in);
    EXPECT_EQ(out, in);
}
