// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Lexer.h"
#include "basic_compiler/Parser.h"
#include "basic_compiler/opt/AstOptimizer.h"
#include "basic_compiler/ast/IfStmt.h"
#include "basic_compiler/ast/Line.h"
#include "basic_compiler/compiler/Metrics.h"

using namespace gwbasic;

/***
 * Test: OptimizerIf.AnalyzeOnly_NoRewrite
 * Purpose: When Metrics::analyze_only is set, IF constant folding should
 *          not rewrite or remove the IF statement.
 */
TEST(OptimizerIf, AnalyzeOnly_NoRewrite) {
    Metrics m; m.setAnalyzeOnly(true);
    auto* prev = gMetrics; gMetrics = &m;
    const char* src = "10 IF 1 THEN 100\n";
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();
    AstOptimizer::optimize(prog);
    gMetrics = prev;
    ASSERT_FALSE(prog.lines.empty());
    const auto it = std::find_if(prog.lines.begin(), prog.lines.end(), [](const Line& L){return L.number==10;});
    ASSERT_NE(it, prog.lines.end());
    const auto& l10 = *it;
    ASSERT_EQ(l10.statements.size(), 1u);
    EXPECT_NE(dynamic_cast<IfStmt*>(l10.statements[0].get()), nullptr);
}
