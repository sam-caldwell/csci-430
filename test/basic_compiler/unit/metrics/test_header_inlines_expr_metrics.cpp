// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <sstream>
#include <string>

#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NumberExpr.h"
#include "basic_compiler/ast/StringExpr.h"
#include "basic_compiler/ast/VarExpr.h"
#include "basic_compiler/ast/UnaryExpr.h"
#include "basic_compiler/ast/BinaryExpr.h"
#include "basic_compiler/ast/ForStmt.h"
#include "basic_compiler/compiler/Metrics.h"

using namespace gwbasic;

/*
 * Test: Headers.Expr_Classof_Coverage
 * Purpose: Exercise Expr::classof for all covered expression kinds and a non-expr kind.
 */
TEST(Headers, Expr_Classof_Coverage) {
    NumberExpr n(1.0);
    StringExpr s("x");
    VarExpr v("A");
    UnaryExpr u('+', std::make_unique<NumberExpr>(2.0));
    BinaryExpr b(BinaryOp::Add, std::make_unique<NumberExpr>(1.0), std::make_unique<NumberExpr>(3.0));
    ForStmt f("I", std::make_unique<NumberExpr>(1.0), std::make_unique<NumberExpr>(2.0), nullptr);

    EXPECT_TRUE(Expr::classof(&n));
    EXPECT_TRUE(Expr::classof(&s));
    EXPECT_TRUE(Expr::classof(&v));
    EXPECT_TRUE(Expr::classof(&u));
    EXPECT_TRUE(Expr::classof(&b));
    EXPECT_FALSE(Expr::classof(&f));
}

/*
 * Test: Headers.Metrics_Inlines_Coverage
 * Purpose: Exercise inline methods in Metrics.h to raise header coverage.
 */
TEST(Headers, Metrics_Inlines_Coverage) {
    Metrics m;
    // Analysis-only flag
    EXPECT_FALSE(m.isAnalyzeOnly());
    m.setAnalyzeOnly(true);
    EXPECT_TRUE(m.isAnalyzeOnly());
    m.setAnalyzeOnly(false);
    EXPECT_FALSE(m.isAnalyzeOnly());

    // Lexer
    m.incToken();

    // Semantics/optimizer counters
    m.incConstFoldAdd();
    m.incConstFoldSub();
    m.incConstFoldMul();
    m.incConstFoldDiv();
    m.incConstFoldCmp();
    m.incUnaryElimPlus();
    m.incUnaryConstMinus();
    m.incIdAddZero();
    m.incIdSubZero();
    m.incIdMulOne();
    m.incIdMulZero();
    m.incIdDivOne();
    m.incIfConstTrueToGoto();
    m.incIfConstFalseRemoved();
    m.incForStepElided();

    // Codegen counters setters
    m.setIrInstructionCount(123);
    m.setOptPhaseCounts({{"-O1", 10}, {"-O2", 9}});

    // Print to a stream (indirectly exercises header-controlled state)
    std::ostringstream os;
    m.print(os);
    auto out = os.str();
    ASSERT_NE(out.find("Lexer"), std::string::npos);
    ASSERT_NE(out.find("Codegen"), std::string::npos);
}

