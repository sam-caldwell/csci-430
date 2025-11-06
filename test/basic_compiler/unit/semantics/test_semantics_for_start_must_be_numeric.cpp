// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
 * Test: Semantics.ForStartMustBeNumeric
 * Purpose: Validate that the FOR loop's start value must be numeric.
 * Components Under Test: Compiler::compileString; Semantics (type checking)
 * Expected Behavior: Compilation throws SemanticError when the start bound is a string (e.g., "A").
 */
/*
Test: Semantics.ForStartMustBeNumeric
Inputs: Parsed AST (from BASIC snippet) and default environment
Code under test: Semantics analyzer (type/arity/domain checks)
Expected behavior: Valid programs accepted; invalid ones produce expected semantic errors
*/
TEST(Semantics, ForStartMustBeNumeric) {
    const auto src =
        "10 FOR I=\"A\" TO 3:NEXT I\n"
        "20 END\n";
    EXPECT_THROW({ const auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}
