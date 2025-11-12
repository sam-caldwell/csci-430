// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/lexer/Lexer.h"
#include "../../../../include/basic_compiler/parser/Parser.h"
#include "basic_compiler/opt/AstOptimizer.h"
#include "basic_compiler/compiler/Metrics.h"
#include "basic_compiler/ast/Program.h"
#include "basic_compiler/ast/Line.h"
#include "basic_compiler/ast/IfStmt.h"
#include "basic_compiler/ast/GotoStmt.h"
#include "basic_compiler/ast/ForStmt.h"

using namespace gwbasic;

/***
 * Test: Optimizer.Stmt_Rewrites_IfAndFor
 * Purpose: Verify IF constant folding to GOTO/removal and FOR STEP 1 elision.
 */
TEST(Optimizer, Stmt_Rewrites_IfAndFor) {
    const std::string src =
        "10 IF 1 THEN 100\n"
        "15 IF 0 THEN 200\n"
        "20 FOR I=1 TO 3 STEP 1: PRINT I: NEXT I\n";
    Lexer lex(src);
    auto toks = lex.tokenize();
    Parser p(std::move(toks));
    auto prog = p.parseProgram();

    Metrics m; // ensure we count step elided but not analyze-only
    auto* prev = gMetrics; gMetrics = &m;
    AstOptimizer::optimize(prog);
    gMetrics = prev;

    // After optimization: line 10 -> GOTO 100; line 15 removed; FOR step reset
    auto it10 = std::find_if(prog.lines.begin(), prog.lines.end(), [](const Line& L){ return L.number==10; });
    ASSERT_NE(it10, prog.lines.end());
    // ReSharper disable once CppUseStructuredBinding
    const auto& l10 = *it10;
    ASSERT_EQ(l10.statements.size(), 1u);
    EXPECT_NE(dynamic_cast<GotoStmt*>(l10.statements[0].get()), nullptr);

    auto it15 = std::find_if(prog.lines.begin(), prog.lines.end(), [](const Line& L){ return L.number==15; });
    ASSERT_NE(it15, prog.lines.end());
    // The IF 0 should be optimized away; allow it to be present if analyze-only was on, but we set it off.
    if (const auto&[number, statements] = *it15; !statements.empty()) {
        EXPECT_EQ(dynamic_cast<IfStmt*>(statements[0].get()), nullptr);
    }

    auto it20 = std::find_if(prog.lines.begin(), prog.lines.end(), [](const Line& L){ return L.number==20; });
    ASSERT_NE(it20, prog.lines.end());
    // ReSharper disable once CppUseStructuredBinding
    const auto& l20 = *it20;
    ASSERT_FALSE(l20.statements.empty());
    auto* fs = dynamic_cast<ForStmt*>(l20.statements[0].get());
    ASSERT_NE(fs, nullptr);
    EXPECT_EQ(fs->step, nullptr);
}
