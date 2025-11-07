// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/codegen/CodeGenerator.h"

using namespace gwbasic;

/***
 * Test: CodeGenCore.CollectsVarsBroadWithoutSemantics
 * Purpose: Ensure collectStmtVars/collectExprVars paths are exercised when semantics
 *          are not provided. Uses DATA/READ, RANDOMIZE, COMMON, ON GOTO/GOSUB, MID$.
 * Components Under Test: CodeGenerator::collectStmtVars/collectExprVars via generate()
 * Expected Behavior: Generation succeeds and produces IR with relevant constructs present.
 */
TEST(CodeGenCore, CollectsVarsBroadWithoutSemantics) {
    const auto src =
        "10 COMMON A, B$, C%\n"
        "20 DATA 1, \"X\"\n"
        "30 READ A, B$\n"
        "40 RANDOMIZE 42\n"
        "50 ON 1 GOTO 100,200\n"
        "60 ON 1 GOSUB 300,400\n"
        "70 MID$(B$,2)=\"Z\"\n"
        "80 IF A > 0 THEN 90\n"
        "90 PRINT \"DONE\"\n"
        "100 END\n"
        "300 RETURN\n"
        "400 RETURN\n";

    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();
    CodeGenerator gen; // no semantics set
    std::string ir = gen.generate(prog);
    // Presence checks for various constructs recorded by collectors
    ASSERT_NE(ir.find("@.str."), std::string::npos);          // DATA string literal interned
    // RANDOMIZE plumbing present (either RNG state or srand48/drand48 usage)
    bool hasRng = ir.find("@gwb_last_rnd") != std::string::npos
               || ir.find("@srand48") != std::string::npos
               || ir.find("@drand48") != std::string::npos;
    ASSERT_TRUE(hasRng);
    ASSERT_NE(ir.find("switch i32"), std::string::npos);      // ON ... lowers to switch
    ASSERT_NE(ir.find("@strncpy"), std::string::npos);        // MID$ lowering
}
