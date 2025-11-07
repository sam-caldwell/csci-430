// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/IfBlockStmt.h"
#include "basic_compiler/ast/ForStmt.h"
#include "basic_compiler/ast/WhileStmt.h"
#include "basic_compiler/ast/PrintStmt.h"

using namespace gwbasic;

/***
Test: Parser.ParseProgramBlockFoldingNested
Inputs: Nested IF → FOR → WHILE bodies with ELSE
Code under test: Parser::parseProgram restructuring of nested blocks
Expected behavior: AST nests bodies correctly and folds markers
*/
TEST(Parser, ParseProgram_BlockFolding_Nested) {
    std::string src =
        "10 IF 1 THEN\n"
        "20 FOR I=1 TO 2\n"
        "30 WHILE 0\n"
        "40 PRINT 1\n"
        "50 WEND\n"
        "60 NEXT\n"
        "70 ELSE\n"
        "80 PRINT 2\n"
        "90 END IF\n";
    Lexer lx(src); auto toks = lx.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();
    ASSERT_EQ(prog.lines.size(), 1u);
    auto* ib = dynamic_cast<IfBlockStmt*>(prog.lines[0].statements[0].get());
    ASSERT_NE(ib, nullptr);
    // THEN: single FOR with nested WHILE and PRINT inside
    ASSERT_EQ(ib->thenBody.size(), 1u);
    auto* fs = dynamic_cast<ForStmt*>(ib->thenBody[0].get());
    ASSERT_NE(fs, nullptr);
    ASSERT_EQ(fs->body.size(), 1u);
    auto* ws = dynamic_cast<WhileStmt*>(fs->body[0].get());
    ASSERT_NE(ws, nullptr);
    ASSERT_EQ(ws->body.size(), 1u);
    ASSERT_NE(dynamic_cast<PrintStmt*>(ws->body[0].get()), nullptr);
    // ELSE: one PRINT statement
    ASSERT_EQ(ib->elseBody.size(), 1u);
    ASSERT_NE(dynamic_cast<PrintStmt*>(ib->elseBody[0].get()), nullptr);
}
