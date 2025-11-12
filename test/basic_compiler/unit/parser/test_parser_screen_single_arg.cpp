// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/ScreenStmt.h"

using namespace gwbasic;

static const ScreenStmt* firstScreen_single(const std::string& src) {
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
 * Test: ParserScreen.SingleArg
 * Purpose: Ensure SCREEN with one argument parses (argument specifics may be stubbed).
 * Components Under Test: Parser::parse SCREEN.
 * Expected Behavior: Non-null ScreenStmt is returned.
 */
TEST(ParserScreen, SingleArg) {
    auto* sc = firstScreen_single("10 SCREEN 1\n");
    // Parser recognizes SCREEN statement; argument support may be stubbed
    ASSERT_NE(sc, nullptr);
}
