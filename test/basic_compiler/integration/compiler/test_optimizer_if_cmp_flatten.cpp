// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include <algorithm>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/opt/AstOptimizer.h"
#include "basic_compiler/compiler/Metrics.h"
#include "basic_compiler/ast/Program.h"
#include "basic_compiler/ast/Line.h"
#include "basic_compiler/ast/IfStmt.h"
#include "basic_compiler/ast/GotoStmt.h"

using namespace gwbasic;

/***
Test: Optimizer.IfCmp_Flattens_ToGotoOrRemoval
Inputs: Program with IF using constant-foldable comparisons
Code under test: AstOptimizer::optimize (IF constant rewrite via folded comparisons)
Expected behavior: IF 2>1 becomes GOTO; IF 1<0 removed.
*/
TEST(Optimizer, IfCmp_Flattens_ToGotoOrRemoval) {
    const std::string src =
        "10 IF 2>1 THEN 100\n"
        "15 IF 1<0 THEN 200\n"
        "20 PRINT 1\n";

    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();

    Metrics m; auto* prev = gMetrics; gMetrics = &m;
    AstOptimizer::optimize(prog);
    gMetrics = prev;

    // Line 10 should be rewritten to a GOTO
    auto it10 = std::find_if(prog.lines.begin(), prog.lines.end(), [](const Line& L){ return L.number==10; });
    ASSERT_NE(it10, prog.lines.end());
    ASSERT_EQ(it10->statements.size(), 1u);
    EXPECT_NE(dynamic_cast<GotoStmt*>(it10->statements[0].get()), nullptr);

    // Line 15 IF should be removed (no IfStmt should remain)
    auto it15 = std::find_if(prog.lines.begin(), prog.lines.end(), [](const Line& L){ return L.number==15; });
    ASSERT_NE(it15, prog.lines.end());
    if (!it15->statements.empty()) {
        EXPECT_EQ(dynamic_cast<IfStmt*>(it15->statements[0].get()), nullptr);
    }
}
