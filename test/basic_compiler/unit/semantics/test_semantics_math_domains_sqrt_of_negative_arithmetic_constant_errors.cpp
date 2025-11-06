// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
 * Test: SemanticsMathDomains.SqrtOfNegativeArithmeticConstantErrors
 * Purpose: Validate domain checking rejects SQR of a negative arithmetic constant expression.
 * Components Under Test: Compiler::compileString; Semantics (intrinsic domain checks)
 * Expected Behavior: Compilation throws SemanticError for SQR(-5+3).
 */
/*
Test: SemanticsMathDomains.SqrtOfNegativeArithmeticConstantErrors
Inputs: Parsed AST (from BASIC snippet) and default environment
Code under test: Semantics analyzer (type/arity/domain checks)
Expected behavior: Valid programs accepted; invalid ones produce expected semantic errors
*/
TEST(SemanticsMathDomains, SqrtOfNegativeArithmeticConstantErrors) {
    const auto src =
        "10 PRINT SQR(-5+3)\n"
        "20 END\n";
    EXPECT_THROW({ const auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}
