// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/DeleteStmt.h"

using namespace gwbasic;

/***
Test: Parser.DELETE_Variants
Inputs: DELETE, DELETE 100, DELETE 100-, DELETE 100-200
Code under test: Parser::parseDelete
Expected behavior: start/end and dot flags parsed as expected
*/
TEST(Parser, DELETE_Variants) {
    const char* src =
        "10 DELETE\n"
        "20 DELETE 100\n"
        "30 DELETE 100-\n"
        "40 DELETE 100-200\n";
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();
    ASSERT_EQ(prog.lines.size(), 4u);

    auto* d1 = dynamic_cast<DeleteStmt*>(prog.lines[0].statements[0].get());
    ASSERT_NE(d1, nullptr);
    EXPECT_FALSE(d1->startLine.has_value());
    EXPECT_FALSE(d1->endLine.has_value());

    auto* d2 = dynamic_cast<DeleteStmt*>(prog.lines[1].statements[0].get());
    ASSERT_NE(d2, nullptr);
    EXPECT_EQ(d2->startLine.value_or(-1), 100);
    EXPECT_EQ(d2->endLine.value_or(-1), 100);

    auto* d3 = dynamic_cast<DeleteStmt*>(prog.lines[2].statements[0].get());
    ASSERT_NE(d3, nullptr);
    EXPECT_EQ(d3->startLine.value_or(-1), 100);
    EXPECT_FALSE(d3->endLine.has_value());

    auto* d4 = dynamic_cast<DeleteStmt*>(prog.lines[3].statements[0].get());
    ASSERT_NE(d4, nullptr);
    EXPECT_EQ(d4->startLine.value_or(-1), 100);
    EXPECT_EQ(d4->endLine.value_or(-1), 200);

    // Only four inputs; no additional assertions
}
