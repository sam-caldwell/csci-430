// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/ast/RTTI.h"
#include "basic_compiler/ast/EraseStmt.h"

using namespace gwbasic;

/***
 * Test: Unit.Parser_ERASE_ParsesIdentifiers
 * Purpose: Ensure ERASE parses a comma-separated list of identifiers.
 * Components: Lexer, Parser
 * Expected: One EraseStmt with the correct names.
 */
/*
Test: Unit.Parser_ERASE_ParsesIdentifiers
Inputs: "10 ERASE A,B$\n"
Code under test: Parser::parseErase via parseStatement
Expected behavior: Program with line containing EraseStmt{names={"A","B$"}}
*/
TEST(Unit, Parser_ERASE_ParsesIdentifiers) {
    std::string src = "10 ERASE A,B$\n";
    Lexer lx(src);
    auto toks = lx.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();
    ASSERT_EQ(prog.lines.size(), 1u);
    const auto& line = prog.lines[0];
    ASSERT_EQ(line.statements.size(), 1u);
    auto* er = dyn_cast<EraseStmt>(line.statements[0].get());
    ASSERT_NE(er, nullptr);
    ASSERT_EQ(er->names.size(), 2u);
    EXPECT_EQ(er->names[0], "A");
    EXPECT_EQ(er->names[1], "B$");
}

