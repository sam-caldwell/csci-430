// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/codegen/CodeGenerator.h"

using namespace gwbasic;

/*
 * Test Suite: CodeGen Without Semantics
 * Purpose: Force CodeGenerator to collect decls/vars internally to cover
 *          collectStmtVars/collectExprVars paths.
 */
TEST(CodeGenCore, GenerateWithoutSemanticsCollectsVars) {
    const auto src =
        "10 LET A=2\n"
        "20 IF A>1 THEN 30\n"
        "30 PRINT A\n"
        "40 END\n";
    std::istringstream iss(src);
    Lexer lex(iss);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();
    CodeGenerator gen;
    // Do NOT call setSemantics; ensure internal collection runs
    std::string ir = gen.generate(prog);
    EXPECT_NE(ir.find("@.fmt_num"), std::string::npos);
}
