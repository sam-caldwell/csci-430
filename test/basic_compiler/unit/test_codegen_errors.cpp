// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <memory>
#include <string>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/codegen/CodeGenerator.h"
#include "basic_compiler/ast/Program.h"

using namespace gwbasic;

namespace {
    struct [[maybe_unused]] DummyStmt final : Stmt {};
    struct [[maybe_unused]] DummyExpr final : Expr {};
}
/*
 * Test Suite: CodeGen Errors
 * Purpose: Ensure invalid or unsupported AST shapes are rejected by codegen
 *          and reported via CodeGenError.
 * Components Under Test: CodeGenerator emitLineBlock, emitSubroutineInline,
 *          emitFor, emitExpr.
 * Expected Behavior: Throws on IF with non-comparison condition; unsupported
 *          statements in line/GOSUB/FOR bodies; unknown expression in PRINT.
 */
TEST(CodeGenErrors, IfConditionMustBeComparison) {
    const auto src =
        "10 IF 1 + 2 THEN 20\n"
        "20 END\n";
    // Use Compiler facade to build AST via parser
    EXPECT_THROW({
        const std::string ir = Compiler::compileString(src);
        (void)ir;
    }, CodeGenError);
}

// Single-test-per-file policy: additional cases moved to dedicated files.
