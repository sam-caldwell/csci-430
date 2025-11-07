// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#define private public
#include "basic_compiler/Lexer.h"
#undef private

using namespace gwbasic;

/***
 * Test: Lexer.EscapeForLog_CoversAllBranches
 * Purpose: Validate escaping behavior for log output across special/control characters.
 * Components Under Test: Lexer::escapeForLog
 * Expected Behavior: Produces C-style escapes for backslash, newline, carriage return,
 *                    tab, and quote; emits hex (\\x..) for control/DEL; preserves
 *                    printable characters.
 */
TEST(Lexer, EscapeForLog_CoversAllBranches) {
    // Include: backslash, newline, carriage return, tab, quote, control chars and printable
    std::string in;
    in.push_back('\\');
    in.push_back('\n');
    in.push_back('\r');
    in.push_back('\t');
    in.push_back('"');
    in.push_back('\a');        // control (<0x20)
    in.push_back('\x7F');      // DEL
    in.push_back('A');          // printable

    std::string out = Lexer::escapeForLog(in);
    // Expect C-style escapes for the first five and hex for control/DEL
    // We don't assert exact sequence beyond ensuring key substrings exist.
    EXPECT_NE(out.find("\\\\"), std::string::npos);
    EXPECT_NE(out.find("\\n"), std::string::npos);
    EXPECT_NE(out.find("\\r"), std::string::npos);
    EXPECT_NE(out.find("\\t"), std::string::npos);
    EXPECT_NE(out.find("\\\""), std::string::npos);
    EXPECT_NE(out.find("\\x"), std::string::npos);     // control/DEL as hex
    EXPECT_NE(out.find('A'), std::string::npos);
}
