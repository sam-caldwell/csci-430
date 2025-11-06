// (c) 2025 Sam Caldwell. All Rights Reserved.
/**
Test: Unit.Metrics_Collection_SimpleProgram
Inputs: Enable metrics and compile a simple program string
Code under test: Compiler::compileString; Metrics hooks (lexer/AST/codegen)
Expected behavior: Metrics reflect non-zero tokens, expected AST shape, and IR instruction count > 0
*/
#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/compiler/Metrics.h"

using namespace gwbasic;

TEST(Unit, Metrics_Collection_SimpleProgram) {
    Metrics m; gMetrics = &m;
    std::string src = R"(10 LET A = 1 + 2 * 3
20 PRINT A
30 END
)";
    std::string ir = Compiler::compileString(src);
    (void)ir;
    gMetrics = nullptr;

    // Lexer produced some tokens
    EXPECT_GT(m.lexer.token_count, 0u);

    // Parsed AST snapshot matches program shape
    EXPECT_EQ(m.ast_parsed.lines, 3u);
    EXPECT_EQ(m.ast_parsed.statements, 3u);
    EXPECT_GE(m.ast_parsed.expressions, 1u);
    EXPECT_GE(m.ast_parsed.max_expr_depth, 1u);

    // Codegen emitted non-zero amount of IR
    EXPECT_GT(m.codegen.ir_instructions, 0u);

    // Optimizer analysis should observe some constant folding on 1+2*3
    EXPECT_GE(m.semantics_opt.const_folds, 1u);
}

