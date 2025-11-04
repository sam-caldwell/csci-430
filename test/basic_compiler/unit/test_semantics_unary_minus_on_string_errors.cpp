// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include <string>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
 * Test: Semantics.UnaryMinusOnStringErrors
 * Purpose: Validate that applying unary minus to a string is rejected by semantics.
 * Components Under Test: Compiler::compileString; Semantics (type checking)
 * Expected Behavior: Compilation throws SemanticError for expression -"X".
 */
/*
Test: Semantics.UnaryMinusOnStringErrors
Inputs: Parsed AST (from BASIC snippet) and default environment
Code under test: Semantics analyzer (type/arity/domain checks)
Expected behavior: Valid programs accepted; invalid ones produce expected semantic errors
*/
TEST(Semantics, UnaryMinusOnStringErrors) {
    const auto src =
        "10 LET A = -\"X\"\n"
        "20 END\n";
    EXPECT_THROW({ const auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}
