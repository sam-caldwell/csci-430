// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
 * Test: Semantics.GotoMissingTargetErrors
 * Purpose: Validate that GOTO to a non-existent line number is rejected.
 * Components Under Test: Compiler::compileString; Semantics (control-flow resolution)
 * Expected Behavior: Compilation throws SemanticError when the target line (9999) does not exist.
 */
TEST(Semantics, GotoMissingTargetErrors) {
    const auto src =
        "10 GOTO 9999\n"
        "20 END\n";
    EXPECT_THROW({ const auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}
