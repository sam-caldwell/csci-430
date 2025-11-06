// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
 * Test: SemanticsMathExt.ArityAndTypeChecks
 * Purpose: Validate arity and type checking for extended math intrinsics (e.g., RND, CINT).
 * Components Under Test: Compiler::compileString; Semantics (intrinsic arity/type checks)
 * Expected Behavior: Throws SemanticError for RND() with no args and for CINT("A") with string arg.
 */
/*
Test: SemanticsMathExt.ArityAndTypeChecks
Inputs: Parsed AST (from BASIC snippet) and default environment
Code under test: Semantics analyzer (type/arity/domain checks)
Expected behavior: Valid programs accepted; invalid ones produce expected semantic errors
*/
TEST(SemanticsMathExt, ArityAndTypeChecks) {
    const auto src1 =
        "10 PRINT RND()\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src1); (void)ir; }, SemanticError);

    const auto src2 =
        "10 PRINT CINT(\"A\")\n"
        "20 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src2); (void)ir; }, SemanticError);
}
