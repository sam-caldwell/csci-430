// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/ScreenStmt.h"

using namespace gwbasic;

static const ScreenStmt* firstScreen_noargs(const std::string& src) {
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    Program prog = p.parseProgram();
    if (prog.lines.size() != 1u) return nullptr;
    if (prog.lines[0].statements.size() != 1u) return nullptr;
    auto* st = dyn_cast<ScreenStmt>(prog.lines[0].statements[0].get());
    return st;
}

/***
 * Test: ParserScreen.NoArgs
 * Purpose: Ensure SCREEN without arguments parses and all optionals are null.
 * Components Under Test: Parser::parse SCREEN statement.
 * Expected Behavior: ScreenStmt fields mode/colorSwitch/aPage/vPage are null.
 */
TEST(ParserScreen, NoArgs) {
    auto* sc = firstScreen_noargs("10 SCREEN\n");
    ASSERT_NE(sc, nullptr);
    EXPECT_EQ(sc->mode, nullptr);
    EXPECT_EQ(sc->colorSwitch, nullptr);
    EXPECT_EQ(sc->aPage, nullptr);
    EXPECT_EQ(sc->vPage, nullptr);
}
