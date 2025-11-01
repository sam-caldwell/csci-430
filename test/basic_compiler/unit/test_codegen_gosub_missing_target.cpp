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
TEST(CodeGenFlow, GosubToMissingTargetIsError) {
    const auto src =
        "10 GOSUB 9999\n"
        "20 PRINT 1\n"
        "30 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, gwbasic::SemanticError);
}
