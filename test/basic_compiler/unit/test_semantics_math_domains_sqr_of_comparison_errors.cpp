// (c) 2025 Sam Caldwell. All Rights Reserved.

#include <gtest/gtest.h>
#include "basic_compiler/Compiler.h"
#include "basic_compiler/semantics/SemanticError.h"

using namespace gwbasic;

/***
 * Test: SemanticsMathDomains.SqrOfComparisonErrors
 * Purpose: Validate type/domain checking rejects SQR applied to a boolean/comparison result.
 * Components Under Test: Compiler::compileString; Semantics (intrinsic domain/type checks)
 * Expected Behavior: Compilation throws SemanticError for SQR(2<3).
 */
/*
Test: SemanticsMathDomains.SqrOfComparisonErrors
Inputs: Parsed AST (from BASIC snippet) and default environment
Code under test: Semantics analyzer (type/arity/domain checks)
Expected behavior: Valid programs accepted; invalid ones produce expected semantic errors
*/
TEST(SemanticsMathDomains, SqrOfComparisonErrors) {
    const auto src =
        "10 PRINT SQR(2<3)\n"
        "20 END\n";
    EXPECT_THROW({ const auto ir = Compiler::compileString(src); (void)ir; }, SemanticError);
}
