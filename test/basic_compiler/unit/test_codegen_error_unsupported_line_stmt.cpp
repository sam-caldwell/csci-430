// (c) 2025 Sam Caldwell. All Rights Reserved.
/*
 * Test Suite: CodeGen Error (unsupported stmt in line)
 * Purpose: Ensure an unsupported Stmt in a line triggers CodeGenError.
 * Components Under Test: CodeGenerator emitLineBlock.
 * Expected Behavior: generate() throws CodeGenError.
 */
#include <gtest/gtest.h>
#include <memory>
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/Program.h"

using namespace gwbasic;

namespace { struct DummyStmt : Stmt {}; }

TEST(CodeGenErrors, UnsupportedStatementInLineBlock) {
    Program p;
    Line l; l.number = 10;
    l.statements.emplace_back(std::make_unique<DummyStmt>());
    p.lines.push_back(std::move(l));
    CodeGenerator gen;
    EXPECT_THROW({ (void)gen.generate(p); }, CodeGenError);
}

