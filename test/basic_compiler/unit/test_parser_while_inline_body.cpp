// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/ast/WhileStmt.h"
#include "basic_compiler/ast/PrintStmt.h"

using namespace gwbasic;

TEST(Parser, WhileInlineBody) {
    std::string src = "10 WHILE I < 3 : PRINT I : WEND\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto [lines] = p.parseProgram();
    ASSERT_EQ(lines.size(), 1u);
    ASSERT_EQ(lines[0].statements.size(), 1u);
    auto* ws = dynamic_cast<WhileStmt*>(lines[0].statements[0].get());
    ASSERT_NE(ws, nullptr);
    ASSERT_EQ(ws->body.size(), 1u);
    ASSERT_NE(dynamic_cast<PrintStmt*>(ws->body[0].get()), nullptr);
}

