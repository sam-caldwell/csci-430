// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/FilesStmt.h"

using namespace gwbasic;

/***
 * Test: Parser.Files_NoArgs
 * Purpose: FILES without arguments parses into FilesStmt with null device/pattern.
 */
TEST(Parser, Files_NoArgs) {
    const char* src = "10 FILES\n";
    Lexer lx(src);
    auto toks = lx.tokenize();
    Parser p(std::move(toks));
    Program prog = p.parseProgram();
    ASSERT_EQ(prog.lines.size(), 1u);
    ASSERT_EQ(prog.lines[0].statements.size(), 1u);
    auto* fs = dyn_cast<FilesStmt>(prog.lines[0].statements[0].get());
    ASSERT_NE(fs, nullptr);
    EXPECT_EQ(fs->device.get(), nullptr);
    EXPECT_EQ(fs->pattern.get(), nullptr);
}

/***
 * Test: Parser.Files_DeviceAndPattern
 * Purpose: FILES with device and pattern parses both expressions.
 */
TEST(Parser, Files_DeviceAndPattern) {
    const char* src = "10 FILES \"LPT1:\", \"*.*\"\n";
    Lexer lx(src);
    auto toks = lx.tokenize();
    Parser p(std::move(toks));
    Program prog = p.parseProgram();
    ASSERT_EQ(prog.lines.size(), 1u);
    auto* fs = dyn_cast<FilesStmt>(prog.lines[0].statements[0].get());
    ASSERT_NE(fs, nullptr);
    ASSERT_NE(fs->device.get(), nullptr);
    ASSERT_NE(fs->pattern.get(), nullptr);
}

