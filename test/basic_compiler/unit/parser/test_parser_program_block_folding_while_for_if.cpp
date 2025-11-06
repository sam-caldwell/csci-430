// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/WhileStmt.h"
#include "basic_compiler/ast/IfBlockStmt.h"
#include "basic_compiler/ast/ForStmt.h"

using namespace gwbasic;

/*
Test: Parser.ParseProgramBlockFoldingWhileForIf
Inputs: WHILE with nested IF block containing a FOR
Code under test: Parser::parseProgram stack handling for WhileK
Expected behavior: AST nests While → IfBlock → For correctly
*/
TEST(Parser, ParseProgram_BlockFolding_While_For_If) {
    std::string src =
        "10 WHILE 1\n"
        "20 IF 1 THEN\n"
        "30 FOR I=1 TO 1\n"
        "40 NEXT\n"
        "50 END IF\n"
        "60 WEND\n";
    Lexer lx(src); auto toks = lx.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();
    ASSERT_EQ(prog.lines.size(), 1u);
    auto* ws = dynamic_cast<WhileStmt*>(prog.lines[0].statements[0].get());
    ASSERT_NE(ws, nullptr);
    ASSERT_EQ(ws->body.size(), 1u);
    auto* ib = dynamic_cast<IfBlockStmt*>(ws->body[0].get());
    ASSERT_NE(ib, nullptr);
    ASSERT_EQ(ib->thenBody.size(), 1u);
    auto* fs = dynamic_cast<ForStmt*>(ib->thenBody[0].get());
    ASSERT_NE(fs, nullptr);
}

