// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
 * Test: SemanticsMathMore.TypeErrorsOnStringArgs
 * Purpose: Validate that math intrinsics like INT reject string arguments.
 * Components Under Test: Compiler::compileString; Semantics (intrinsic argument type checks)
 * Expected Behavior: Compilation throws SemanticError for INT("A").
 */
/*
Test: SemanticsMathMore.TypeErrorsOnStringArgs
Inputs: Parsed AST (from BASIC snippet) and default environment
Code under test: Semantics analyzer (type/arity/domain checks)
Expected behavior: Valid programs accepted; invalid ones produce expected semantic errors
*/
TEST(SemanticsMathMore, TypeErrorsOnStringArgs) {
    const auto src =
        "10 PRINT INT(\"A\")\n"
        "20 END\n";
    EXPECT_THROW({ const auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}
