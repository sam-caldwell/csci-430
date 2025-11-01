// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include "basic_compiler/Lexer.h"

using namespace gwbasic;

/*
 * Test Suite: Lexer Ifstream Empty File
 * Purpose: Ensure Lexer(std::ifstream) handles an empty file and still
 *          produces a terminal EndOfFile token.
 */
TEST(Lexer, IfstreamHandlesEmptyFile) {
    // Arrange: create an empty file under build/tmp
    // Use ../tmp relative to the build dir so files land under build/tmp
    std::filesystem::path tmpdir = std::filesystem::path("..") / "tmp" / "lexer_ifstream";
    std::filesystem::create_directories(tmpdir);
    std::filesystem::path srcPath = tmpdir / "empty.bas";

    {
        std::ofstream out(srcPath, std::ios::trunc);
        ASSERT_TRUE(out.good());
    }

    // Act: open file stream and lex
    std::ifstream in(srcPath);
    ASSERT_TRUE(in.good());
    Lexer lex(in);
    auto toks = lex.tokenize();

    // Assert: Only EndOfFile should be present
    ASSERT_EQ(toks.size(), 1u);
    EXPECT_EQ(toks[0].type, TokenType::EndOfFile);
}
