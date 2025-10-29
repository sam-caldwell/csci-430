// (c) 2025 Sam Caldwell. All Rights Reserved.
/*
 * Test Suite: CodeGen Error (unsupported in GOSUB body)
 * Purpose: Ensure unsupported statements encountered during inlined GOSUB
 *          emission trigger CodeGenError.
 * Components Under Test: CodeGenerator emitSubroutineInline.
 * Expected Behavior: generate() throws CodeGenError.
 */
#include <gtest/gtest.h>
#include <memory>
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/Program.h"

using namespace gwbasic;

namespace { struct DummyStmt : Stmt {}; }

TEST(CodeGenErrors, UnsupportedStatementInGosubBody) {
    Program p;
    {
        Line l; l.number = 10;
        auto gs = std::make_unique<GosubStmt>(100);
        gs->pos = SourcePos{10, 1};
        l.statements.push_back(std::move(gs));
        p.lines.push_back(std::move(l));
    }
    {
        Line l; l.number = 100;
        l.statements.emplace_back(std::make_unique<DummyStmt>());
        p.lines.push_back(std::move(l));
    }
    CodeGenerator gen;
    EXPECT_THROW({ (void)gen.generate(p); }, CodeGenError);
}

