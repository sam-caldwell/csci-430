// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/*
 * Test: SemanticsDefFn.ArityError
 * Purpose: Ensure calling DEF FN with wrong arity fails.
 */
/***
Test: SemanticsDefFn.ArityError
Inputs: Parsed AST (from BASIC snippet) and default environment
Code under test: Semantics analyzer (type/arity/domain checks)
Expected behavior: Valid programs accepted; invalid ones produce expected semantic errors
*/
TEST(SemanticsDefFn, ArityError) {
    const char* src =
        "10 DEF FNA(X) = X\n"
        "20 PRINT FNA(1,2)\n"
        "30 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}
