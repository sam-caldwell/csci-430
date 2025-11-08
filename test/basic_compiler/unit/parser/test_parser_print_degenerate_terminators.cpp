// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/PrintStmt.h"

using namespace gwbasic;

/***
Test: Parser.Print_Degenerate_Terminators
Inputs: PRINT ; and PRINT , (no items)
Code under test: Parser::parsePrint
Expected behavior: No items; trail is Semicolon/Comma respectively.
*/
TEST(Parser, Print_Degenerate_Terminators) {
    {
        const std::string src = "10 PRINT ;\n";
        std::istringstream iss(src);
        Lexer lex(iss);
        auto toks = lex.tokenize();
        Parser p(std::move(toks));
        auto prog = p.parseProgram();
        ASSERT_EQ(prog.lines.size(), 1u);
        auto* ps = dynamic_cast<PrintStmt*>(prog.lines[0].statements[0].get());
        ASSERT_NE(ps, nullptr);
        EXPECT_EQ(ps->value, nullptr);
        EXPECT_EQ(ps->more.size(), 0u);
        EXPECT_EQ(ps->trail, PrintStmt::Terminator::Semicolon);
    }
    {
        const std::string src = "10 PRINT ,\n";
        std::istringstream iss(src);
        Lexer lex(iss);
        auto toks = lex.tokenize();
        Parser p(std::move(toks));
        auto prog = p.parseProgram();
        ASSERT_EQ(prog.lines.size(), 1u);
        auto* ps = dynamic_cast<PrintStmt*>(prog.lines[0].statements[0].get());
        ASSERT_NE(ps, nullptr);
        EXPECT_EQ(ps->value, nullptr);
        EXPECT_EQ(ps->more.size(), 0u);
        EXPECT_EQ(ps->trail, PrintStmt::Terminator::Comma);
    }
}

