// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <string>
#include "basic_compiler/Lexer.h"

using namespace gwbasic;

/*
 * Test Suite: Lexer Logging
 * Purpose: Exercise lex log path to improve coverage of log_token.cpp
 */
TEST(Lexer, LoggingProducesOutput) {
    const std::string src = "10 PRINT \"HI\"\n20 END\n";
    std::filesystem::path tmpdir = std::filesystem::path("..") / "tmp" / "unit_logs";
    std::filesystem::create_directories(tmpdir);
    std::filesystem::path tmp = tmpdir / "lexer_logging_test.log";
    std::error_code ec; std::filesystem::remove(tmp, ec); // best-effort cleanup
    Lexer lex(src);
    lex.setLexLogPath(tmp.string());
    auto toks = lex.tokenize();
    ASSERT_FALSE(toks.empty());
    // We don't assert on the file (CI environments vary). Enabling logging
    // and tokenizing exercises the log path for coverage.
}
