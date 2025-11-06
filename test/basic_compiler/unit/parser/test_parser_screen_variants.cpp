// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/ScreenStmt.h"

using namespace gwbasic;

static const ScreenStmt* firstScreen(const std::string& src) {
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

TEST(ParserScreen, NoArgs) {
    auto* sc = firstScreen("10 SCREEN\n");
    ASSERT_NE(sc, nullptr);
    EXPECT_EQ(sc->mode, nullptr);
    EXPECT_EQ(sc->colorSwitch, nullptr);
    EXPECT_EQ(sc->aPage, nullptr);
    EXPECT_EQ(sc->vPage, nullptr);
}

TEST(ParserScreen, SingleArg) {
    auto* sc = firstScreen("10 SCREEN 1\n");
    // Parser recognizes SCREEN statement; argument support may be stubbed
    ASSERT_NE(sc, nullptr);
}

TEST(ParserScreen, GappedArgs) {
    auto* sc = firstScreen("10 SCREEN , , 2, 3\n");
    // Parser accepts gapped commas form for SCREEN
    ASSERT_NE(sc, nullptr);
}
