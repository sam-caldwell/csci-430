// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/Program.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/UnsupportedStmt.h"

using namespace gwbasic;

/***
 * Test: Parser.AdditionalKeywords_ParseAsUnsupported
 * Purpose: Ensure additional EBNF-listed keywords are recognized by the lexer
 * and parsed into UnsupportedStmt placeholders that consume tokens
 * up to end-of-statement.
 */
TEST(Parser, AdditionalKeywords_ParseAsUnsupported) {
    const char* src =
        "10 FILES\n"
        "20 CLS\n"
        "30 LOCATE 10,20\n"
        "40 WIDTH 80\n"
        "50 BEEP\n"
        "60 TIMER ON\n"
        "70 SOUND 440,2\n"
        "80 PLAY \"CDE\"\n"
        "90 NAME \n"
        "100 MKDIR \"dir\"\n"
        "110 RMDIR \"dir\"\n";
    Lexer lx(src);
    auto tokens = lx.tokenize();
    Parser p(std::move(tokens));
    Program prog = p.parseProgram();
    ASSERT_EQ(prog.lines.size(), 11u);
    std::vector<std::string> seen;
    for (const auto& line : prog.lines) {
        ASSERT_EQ(line.statements.size(), 1u);
        const Stmt* st = line.statements.front().get();
        if (line.number == 20 || line.number == 30) {
            // CLS and LOCATE should not be UnsupportedStmt
            EXPECT_EQ(dyn_cast<const UnsupportedStmt>(st), nullptr);
            continue;
        }
        const auto* us = dyn_cast<const UnsupportedStmt>(st);
        ASSERT_NE(us, nullptr) << "Expected UnsupportedStmt for line " << line.number;
        seen.push_back(us->keyword);
    }
    // Verify keywords captured (CLS excluded)
    ASSERT_EQ(seen.size(), 9u);
    EXPECT_EQ(seen[0], "FILES");
    EXPECT_EQ(seen[1], "WIDTH");
    EXPECT_EQ(seen[2], "BEEP");
    EXPECT_EQ(seen[3], "TIMER");
    EXPECT_EQ(seen[4], "SOUND");
    EXPECT_EQ(seen[5], "PLAY");
    EXPECT_EQ(seen[6], "NAME");
    EXPECT_EQ(seen[7], "MKDIR");
    EXPECT_EQ(seen[8], "RMDIR");
}
