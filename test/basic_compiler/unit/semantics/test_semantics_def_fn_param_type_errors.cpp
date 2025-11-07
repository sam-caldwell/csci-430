// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/*
 * Test: SemanticsDefFn.ParamTypeErrors
 * Purpose: Validate that DEF FN enforces parameter type at call sites.
 */
/***
Test: SemanticsDefFn.ParamTypeErrors
Inputs: Parsed AST (from BASIC snippet) and default environment
Code under test: Semantics analyzer (type/arity/domain checks)
Expected behavior: Valid programs accepted; invalid ones produce expected semantic errors
*/
TEST(SemanticsDefFn, ParamTypeErrors) {
    // String param expected, numeric provided
    const char* src1 =
        "10 DEF FNA$(S$) = S$ + \"!\"\n"
        "20 PRINT FNA$(1)\n"
        "30 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src1); (void)ir; }, SemanticError);

    // Numeric param expected, string provided
    const char* src2 =
        "10 DEF FND(X) = X + 1\n"
        "20 PRINT FND(\"A\")\n"
        "30 END\n";
    EXPECT_THROW({ auto ir = Compiler::compileString(src2); (void)ir; }, SemanticError);
}
