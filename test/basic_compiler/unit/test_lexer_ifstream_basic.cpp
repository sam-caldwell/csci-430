// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <string>
#include "basic_compiler/Lexer.h"

using namespace gwbasic;

/*
 * Test Suite: Lexer Ifstream Basic
 * Purpose: Directly exercise Lexer(std::ifstream) by reading a real file
 *          from build/tmp and verifying a few representative tokens.
 */
TEST(Lexer, IfstreamReadsProgram) {
    // Arrange: ensure build/tmp exists and write a small BASIC program
    // Use ../tmp relative to the build dir so files land under build/tmp
    std::filesystem::path tmpdir = std::filesystem::path("..") / "tmp" / "lexer_ifstream";
    std::filesystem::create_directories(tmpdir);
    std::filesystem::path srcPath = tmpdir / "program.bas";

    const std::string program =
        "10 LET A=1\r\n"
        "20 PRINT \"OK\"\n"
        "30 END\n";

    {
        std::ofstream out(srcPath);
        ASSERT_TRUE(out.good());
        out << program;
    }

    // Act: open file stream and lex
    std::ifstream in(srcPath);
    ASSERT_TRUE(in.good());
    Lexer lex(in);
    auto toks = lex.tokenize();

    // Assert: sanity-check a few key tokens and structure
    ASSERT_GE(toks.size(), 8u);
    EXPECT_EQ(toks.front().type, TokenType::Integer); // 10

    bool sawLet = false, sawPrint = false, sawString = false, sawNewline = false;
    for (const auto &t : toks) {
        if (t.type == TokenType::KwLet)   sawLet = true;
        if (t.type == TokenType::KwPrint) sawPrint = true;
        if (t.type == TokenType::String && t.lexeme == "OK") sawString = true;
        if (t.type == TokenType::NewLine) sawNewline = true;
    }
    EXPECT_TRUE(sawLet);
    EXPECT_TRUE(sawPrint);
    EXPECT_TRUE(sawString);
    EXPECT_TRUE(sawNewline);

    EXPECT_EQ(toks.back().type, TokenType::EndOfFile);
}
