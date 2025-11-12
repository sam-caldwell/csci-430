// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/codegen/CodeGenerator.h"

using namespace gwbasic;

/***
 * Test: CodeGenIfBlock.GotoGosubReturnEnd
 * Purpose: Exercise additional emitIfBlock branches for GOTO/GOSUB/RETURN/END in both THEN and ELSE.
 * Note: Bypass semantics to allow RETURN outside subroutine context.
 */
TEST(CodeGenIfBlock, GotoGosubReturnEnd) {
    const auto src =
        "10 IF 1<2 THEN GOTO 200: GOSUB 300: RETURN: END ELSE GOTO 400: GOSUB 500: RETURN: END\n"
        "200 END\n"
        "300 RETURN\n"
        "400 END\n"
        "500 RETURN\n";
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();
    CodeGenerator gen; // no semantics
    std::string ir = gen.generate(prog);
    ASSERT_NE(ir.find("_if_then"), std::string::npos);
    ASSERT_NE(ir.find("_if_else"), std::string::npos);
    // GOTO and GOSUB branches appear
    ASSERT_NE(ir.find("_gosub_entry"), std::string::npos);
    ASSERT_NE(ir.find(" br label %line"), std::string::npos);
}
