// (c) 2025 Sam Caldwell. All Rights Reserved.
/*
 * Test Suite: CodeGen Error (unsupported in FOR body)
 * Purpose: Ensure unsupported statements inside FOR body trigger CodeGenError.
 * Components Under Test: CodeGenerator emitFor.
 * Expected Behavior: generate() throws CodeGenError.
 */
#include <gtest/gtest.h>
#include <memory>
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/Program.h"

using namespace gwbasic;

namespace { struct DummyStmt : Stmt {}; }

TEST(CodeGenErrors, UnsupportedStatementInForBody) {
    Program p;
    Line l; l.number = 10;
    auto fs = std::make_unique<ForStmt>(
        std::string("I"),
        std::make_unique<NumberExpr>(1.0),
        std::make_unique<NumberExpr>(3.0),
        std::unique_ptr<Expr>{}
    );
    fs->pos = SourcePos{10, 1};
    fs->body.emplace_back(std::make_unique<DummyStmt>());
    l.statements.push_back(std::move(fs));
    p.lines.push_back(std::move(l));
    CodeGenerator gen;
    EXPECT_THROW({ (void)gen.generate(p); }, CodeGenError);
}

