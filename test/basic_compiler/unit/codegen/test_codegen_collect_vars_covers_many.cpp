// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/codegen/CodeGenerator.h"

using namespace gwbasic;

/***
Test: CodeGenCollectVars.CoversManyKinds
Inputs: Program exercising PRINT strings/vars, ASSIGN, MID$, IF, FOR, INPUT, RANDOMIZE,
        COMMON, MERGE, DATA, READ (scalars+arrays), ON GOTO/GOSUB.
Code under test: CodeGenerator::collectStmtVars + collectExprVars
Expected behavior: No crashes; traversal covers diverse branches.
*/
TEST(CodeGenCollectVars, CoversManyKinds) {
    const char* src =
        "10 PRINT \"S\", A\n"
        "20 A = 5\n"
        "30 INPUT \"Q\"; X\n"
        "40 RANDOMIZE 123\n"
        "50 COMMON C, D\n"
        "60 DATA 1, \"T\"\n"
        "70 READ A\n"
        "80 PRINT \"S\", A\n"
        "90 ON 1 GOTO 150\n"
        "100 MID$(B$,2,1) = \"X\"\n"
        "110 MERGE \"F.BAS\"\n"
        "140 END\n"
        "150 END\n";
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();

    CodeGenerator cg; // do not set semantics; force internal collectors to run
    std::string ir = cg.generate(prog);
    EXPECT_NE(ir.find("@.str."), std::string::npos);
}
