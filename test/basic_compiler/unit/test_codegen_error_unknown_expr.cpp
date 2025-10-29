// (c) 2025 Sam Caldwell. All Rights Reserved.
/*
 * Test Suite: CodeGen Error (unknown expression)
 * Purpose: Ensure printing an unknown expression kind triggers CodeGenError.
 * Components Under Test: CodeGenerator emitExpr.
 * Expected Behavior: generate() throws CodeGenError when encountering
 *          a PrintStmt with a non-supported Expr type.
 */
#include <gtest/gtest.h>
#include <memory>
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/Program.h"

using namespace gwbasic;

namespace { struct DummyExpr : Expr {}; }

TEST(CodeGenErrors, UnknownExpressionKind) {
    Program p;
    Line l; l.number = 10;
    auto ps = std::make_unique<PrintStmt>(std::make_unique<DummyExpr>());
    ps->pos = SourcePos{10, 1};
    l.statements.push_back(std::move(ps));
    p.lines.push_back(std::move(l));
    CodeGenerator gen;
    EXPECT_THROW({ (void)gen.generate(p); }, CodeGenError);
}

