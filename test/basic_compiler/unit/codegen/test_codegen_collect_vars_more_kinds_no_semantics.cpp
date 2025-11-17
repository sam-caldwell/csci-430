// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/codegen/CodeGenerator.h"

using namespace gwbasic;

/***
Test: CodeGenCore.CollectsVars_MoreKinds_NoSemantics
Inputs: Adds INPUT and multi-item PRINT to broaden collector coverage.
Code under test: CodeGenerator::generate (without pre-set semantics)
Expected behavior: IR is produced and contains constructs exercised.
*/
TEST(CodeGenCore, CollectsVars_MoreKinds_NoSemantics) {
    const auto src =
        "10 COMMON A, B$, C%\n"
        "20 DATA 1, \"X\"\n"
        "30 READ A\n"
        "40 RANDOMIZE 42\n"
        "50 ON 1 GOTO 100,200\n"
        "55 PRINT \"S\", A\n"
        "56 INPUT \"Q\"; C%\n"
        "57 ON 1 GOSUB 300,400\n"
        "58 MID$(B$,2,1)=\"Z\"\n"
        "90 PRINT \"DONE\"\n"
        "100 END\n"
        "200 END\n"
        "300 RETURN\n"
        "400 RETURN\n";

    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();
    CodeGenerator gen; // no semantics set
    const std::string ir = gen.generate(prog);
    ASSERT_NE(ir.find("@.str."), std::string::npos);       // DATA literal
    ASSERT_NE(ir.find("switch i32"), std::string::npos);   // ON ...
    ASSERT_NE(ir.find("printf"), std::string::npos);       // PRINT lowering
}
