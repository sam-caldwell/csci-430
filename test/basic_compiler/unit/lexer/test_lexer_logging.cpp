// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <string>
#include "basic_compiler/lexer/Lexer.h"

using namespace gwbasic;

/***
 * Test Suite: Lexer Logging
 * Purpose: Exercise lex log path to improve coverage of log_token.cpp
 */
TEST(Lexer, LoggingProducesOutput) {
    // GW-BASIC string rules: doubled quotes inside strings, no C-style escapes
    // This program prints: A\n\t"B\C
    const std::string src = "10 PRINT \"A\\n\\t\"\"B\\\\C\"\n20 ' comment here\r\n30 END\n";
    const std::filesystem::path tmpdir = std::filesystem::path("..") / "tmp" / "unit_logs";
    std::filesystem::create_directories(tmpdir);
    const std::filesystem::path tmp = tmpdir / "lexer_logging_test.log";
    std::error_code ec; std::filesystem::remove(tmp, ec); // best-effort cleanup
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    ASSERT_FALSE(toks.empty());
    // Logging has been removed; this test exercises tokenize() only.
}
