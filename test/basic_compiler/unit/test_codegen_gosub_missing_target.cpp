// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"

using namespace gwbasic;
/*
 * Test Suite: CodeGen Control Flow (GOSUB missing target)
 * Purpose: Verify missing subroutine target causes immediate branch to cont.
 * Components Under Test: CodeGenerator emitSubroutineInline.
 * Expected Behavior: Entry label for subroutine and branch to continuation.
 */
/*
Test: CodeGenFlow.GosubToMissingTargetIsError
Inputs: AST (and semantic info) from BASIC snippet
Code under test: LLVM IR code generator
Expected behavior: Emits expected IR calls/ops; unsupported cases are reported
*/
TEST(CodeGenFlow, GosubToMissingTargetIsError) {
    const auto src =
        "10 GOSUB 9999\n"
        "20 PRINT 1\n"
        "30 END\n";
    EXPECT_THROW({ const auto ir = Compiler::compileString(src); (void)ir; }, gwbasic::SemanticError);
}
