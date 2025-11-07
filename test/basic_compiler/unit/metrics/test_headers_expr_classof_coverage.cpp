// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <memory>

#include "basic_compiler/ast/Expr.h"
#include "basic_compiler/ast/NumberExpr.h"
#include "basic_compiler/ast/StringExpr.h"
#include "basic_compiler/ast/VarExpr.h"
#include "basic_compiler/ast/UnaryExpr.h"
#include "basic_compiler/ast/BinaryExpr.h"
#include "basic_compiler/ast/ForStmt.h"

using namespace gwbasic;

/***
 * Test: Headers.Expr_Classof_Coverage
 * Purpose: Exercise Expr::classof for several expression kinds and ensure non-expr kinds return false.
 * Components Under Test: AST classof implementations.
 * Expected Behavior: Expr kinds return true; non-Expr kind returns false.
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
